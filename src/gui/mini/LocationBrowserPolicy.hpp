#pragma once
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>

namespace big::LocationBrowser
{
    inline std::string Trim(std::string_view value)
    {
        const auto first=value.find_first_not_of(" \t\r\n");
        if (first==std::string_view::npos) return {};
        const auto last=value.find_last_not_of(" \t\r\n");
        return std::string(value.substr(first,last-first+1));
    }
    inline bool ValidName(std::string_view value)
    {
        if (value.empty() || value.size()>64) return false;
        for (unsigned char c:value) if (c<32 || c==127 || c=='#') return false;
        return true;
    }
    inline bool Matches(std::string_view name,std::string_view filter)
    {
        std::string a(name),b(filter);
        const auto lower=[](unsigned char c){return static_cast<char>(std::tolower(c));};
        std::transform(a.begin(),a.end(),a.begin(),lower);
        std::transform(b.begin(),b.end(),b.begin(),lower);
        return a.find(b)!=std::string::npos;
    }
    template<class Location> bool Finite(const Location& location)
    { return std::isfinite(location.x) && std::isfinite(location.y) && std::isfinite(location.z); }
    template<class Location> std::vector<Location> Filter(const std::vector<Location>& category,std::string_view search)
    {
        std::vector<Location> result;
        for (const auto& location:category) if (Matches(location.name,search)) result.push_back(location);
        return result;
    }
}
