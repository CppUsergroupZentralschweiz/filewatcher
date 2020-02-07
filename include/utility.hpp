#pragma once

#include <memory>
#include <utility>

namespace cpp_ug {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T = const char*>
inline std::string rtrim(std::string const& s, T t = " \t\n\r\f\v")
{
    std::string ret(s);
    ret.erase(s.find_last_not_of(t) + 1);
    return ret;
}

inline std::string replace_all(std::string const& source, std::string const& search, std::string const& replace)
{
    std::string ret(source);
    for(auto pos = ret.find(search); pos != std::string::npos; pos = ret.find(search, pos + replace.size()))
    {
        ret.replace(pos, search.size(), replace);
    }
    return ret;
}

} // namespace cpp_ug