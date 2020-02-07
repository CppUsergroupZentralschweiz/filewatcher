#pragma once

#include "ghc/filesystem.hpp"

#include <Poco/DirectoryWatcher.h>
#include <functional>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>

namespace cpp_ug {
namespace fs = ghc::filesystem;

class FileWatcher 
{
public:
    FileWatcher(fs::path const& dir_path, std::string const& files_filter);
    explicit FileWatcher(fs::path const& dir_path);

    using directory_event_handler_type = std::function<void(fs::path const&)>;

    void add_on_add_event_handler(directory_event_handler_type const& on_add_event_handler_fnc);
    void add_on_modified_event_handler(directory_event_handler_type const& on_modified_event_handler_fnc);
    void add_on_renamed_event_handler(directory_event_handler_type const& on_renamed_event_handler_fnc);

private:
    struct Delegate
    {
        Delegate(directory_event_handler_type delegate, std::regex filter);

        void invoke(Poco::DirectoryWatcher::DirectoryEvent const& event);

    private:
        directory_event_handler_type const m_delegate;
        std::regex const m_filter_regex;
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_triggered_map;
    };

    std::vector<Delegate> m_on_add_delegates{};
    std::vector<Delegate> m_on_modified_delegates{};
    std::vector<Delegate> m_on_renamed_delegates{};

    std::unique_ptr<Poco::DirectoryWatcher> m_dir_watcher;
    std::regex const m_filter_regex;
};

} // namespace cpp_ug
