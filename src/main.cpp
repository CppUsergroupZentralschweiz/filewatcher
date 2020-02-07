#include "filewatcher.hpp"
#include "utility.hpp"

#include <lyra/lyra.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <mutex>

struct SignalHandler
{
    void set_shutdown_request()
    {
        std::lock_guard<std::mutex> const lock(sig_mutex);
        m_shutdown_request = true;
    }

    bool shutdown_requested()
    {
        std::lock_guard<std::mutex> const lock(sig_mutex);
        return m_shutdown_request;
    }

private:
    bool m_shutdown_request{};
    std::mutex sig_mutex;
};

SignalHandler& gSigHandler()
{
    static SignalHandler handler;
    return handler;
}

void signal_handler(int)
{
    gSigHandler().set_shutdown_request();
}


int main(int argc, const char** argv)
{

    bool show_help{};

    std::vector<std::string> paths;
    
    auto parser
		= lyra::help(show_help).description(
			"Filewatcher.")
		| lyra::arg(paths, "path to file | path to folder")("Which files or folders to watch.").required();

    auto result = parser.parse({ argc, argv });

	if (!result)
	{
		std::cerr << result.errorMessage() << "\n\n";
	}
	if (show_help or !result)
	{
		std::cout << parser << "\n";
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> watch_folders;
    std::vector<std::string> watch_files;

    for (auto const& path : paths) 
    {
        if(not cpp_ug::fs::exists(path)) {
            std::string const error_msg = path + std::string{" doesn't exist."};
            throw std::runtime_error(error_msg);
        }
        if(cpp_ug::fs::is_regular_file(path))
        {
            watch_files.emplace_back(path);
        } else {
            watch_folders.emplace_back(path);
        }
    }

    auto filewatchers = std::vector<cpp_ug::FileWatcher>{};

    for(auto const& _folder : watch_folders)
    {
        // Remove the last path separator, otherwise the fs::relative does not work correctly
        auto const folder = cpp_ug::rtrim(_folder, cpp_ug::fs::path::preferred_separator);
        try
        {
            filewatchers.emplace_back(cpp_ug::FileWatcher(folder));
            auto const add_cb = [folder](cpp_ug::fs::path const& filePath) {
                std::cout << filePath.string() << " was added to " << folder << "\n";
            };
            filewatchers.back().add_on_add_event_handler(add_cb);

            auto const modified_cb = [folder](cpp_ug::fs::path const& filePath) {
                std::cout << filePath.string() << " was modified in " << folder << "\n";
            };
            filewatchers.back().add_on_modified_event_handler(modified_cb);

            auto const renamed_cb = [](cpp_ug::fs::path const& filePath) {
                std::cout << filePath.string() << " was renamed \n";
            };
            filewatchers.back().add_on_renamed_event_handler(renamed_cb);
        }
        catch(cpp_ug::fs::filesystem_error const& ex)
        {
            std::cerr << "Error when creating file watcher for: " << _folder << " (" << ex.what() << ")\n";
        }
    }

    for(auto const& file : watch_files)
    {
        auto const path = cpp_ug::fs::path(file);
        auto const folder = path.parent_path();
        auto const filename = path.filename().string();
        try
        {
            filewatchers.emplace_back(cpp_ug::FileWatcher(folder, filename));
            
            auto const modified_cb = [](cpp_ug::fs::path const& filePath) {
                std::cout << filePath.string() << " was modified \n";
            };
            filewatchers.back().add_on_modified_event_handler(modified_cb);

            auto const renamed_cb = [](cpp_ug::fs::path const& filePath) {
                std::cout << filePath.string() << " was renamed \n";
            };
            filewatchers.back().add_on_renamed_event_handler(renamed_cb);
        }
        catch(cpp_ug::fs::filesystem_error const& ex)
        {
            std::cerr << "Error when creating file watcher for: " << file << " (" << ex.what() << ")\n";
        }
    }

    std::cout << "Watching for changes ....\n";
    std::cout << "Press Ctrl-C to exit.\n";
    while(not gSigHandler().shutdown_requested())
    {
 
    }

    exit(EXIT_SUCCESS);
}
