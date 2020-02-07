#include "filewatcher.hpp"
#include "utility.hpp"

#include <Poco/Delegate.h>
#include <Poco/Exception.h>
#include <iostream>

namespace cpp_ug {

FileWatcher::FileWatcher(fs::path const& dir_path, std::string const& files_filter)
    // Transform the given files filter in a valid regex, e.g.: *.txt -> .*\.txt
    : m_filter_regex(std::regex{replace_all(replace_all(files_filter, ".", "\\."), "*", ".*")})
{
    try
    {
        m_dir_watcher = cpp_ug::make_unique<Poco::DirectoryWatcher>(dir_path.string());
    }
    catch(Poco::FileNotFoundException const& ex)
    {
        throw fs::filesystem_error(ex.displayText(), fs::path(ex.name()), std::error_code(ex.code(), std::generic_category()));
    }
}

FileWatcher::FileWatcher(fs::path const& dir_path)
    : FileWatcher(dir_path, "*")
{
}

void FileWatcher::add_on_add_event_handler(directory_event_handler_type const& on_add_event_handler_fnc)
{
    m_on_add_delegates.emplace_back(Delegate{on_add_event_handler_fnc, m_filter_regex});
    m_dir_watcher->itemAdded += Poco::delegate(&m_on_add_delegates.back(), &Delegate::invoke);
}

void FileWatcher::add_on_modified_event_handler(directory_event_handler_type const& on_modified_event_handler_fnc)
{
    m_on_modified_delegates.emplace_back(Delegate{on_modified_event_handler_fnc, m_filter_regex});
    m_dir_watcher->itemModified += Poco::delegate(&m_on_modified_delegates.back(), &Delegate::invoke);
}

void FileWatcher::add_on_renamed_event_handler(directory_event_handler_type const& on_renamed_event_handler_fnc)
{
    m_on_renamed_delegates.emplace_back(Delegate{on_renamed_event_handler_fnc, m_filter_regex});
    m_dir_watcher->itemMovedTo += Poco::delegate(&m_on_renamed_delegates.back(), &Delegate::invoke);
}

FileWatcher::Delegate::Delegate(directory_event_handler_type delegate, std::regex filter)
    : m_delegate(std::move(delegate))
    , m_filter_regex(std::move(filter))
{}

void FileWatcher::Delegate::invoke(Poco::DirectoryWatcher::DirectoryEvent const& event)
{
    auto const path = fs::path(event.item.path());
    if(std::regex_match(path.filename().string(), m_filter_regex))
    {
        //----------------------------------------------------------------------------------
        // Use an "inhibit time" to avoid multiple triggers on the same modification
        // This is probably a bug in Poco::DirectoryWatcher implementation
        auto const time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_triggered_map[event.item.path()]).count();
        if(time_diff < 1000)
        {
            // std::cout << "Filtered event on " << event.item.path() << " (tdiff: " << time_diff << ")\n";
            return;
        }
        m_triggered_map[event.item.path()] = std::chrono::steady_clock::now();
        //----------------------------------------------------------------------------------

        if(m_delegate)
        {
            m_delegate(path);
        }
    }
}

} // namespace cpp_ug
