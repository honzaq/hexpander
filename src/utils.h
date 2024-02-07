#pragma once
#include <Windows.h>
#include <string>
#include <cwctype>
#include <locale>
#include <codecvt>
#include <ranges>
#include <algorithm>

namespace hexpander
{

inline std::wstring toUpperCase(const std::wstring_view input)
{
    std::wstring result(input);

    for (wchar_t& c : result)
    {
        c = std::towupper(c);
    }

    return result;
}

inline std::string to_utf8(std::wstring const& input)
{
    int                     res;
    char                    buf[0x400];
    char*                   pbuf = buf;
    std::unique_ptr<char[]> shared_pbuf;

    res = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, buf, sizeof(buf), NULL, NULL);
    if (0 == res && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        res = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, NULL, 0, NULL, NULL);

        shared_pbuf = std::move(std::unique_ptr<char[]>(new char[res]));

        pbuf = shared_pbuf.get();

        res = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, pbuf, res, NULL, NULL);
    }

    return std::string(pbuf);
}


inline bool is_char_equal_nocase(wchar_t a, wchar_t b)
{
    return std::tolower(static_cast<wchar_t>(a)) == std::tolower(static_cast<wchar_t>(b));
}

inline bool compare_nocase(std::wstring const& left, std::wstring const& right)
{
    return std::ranges::equal(left, right, is_char_equal_nocase);
}


struct nocase_comparator
{
    bool operator()(std::wstring const& left, std::wstring const& right) const
    {
       // return compare_nocase(left, right);
        return std::lexicographical_compare(
            left.begin(),
            left.end(),
            right.begin(),
            right.end(),
            [](wchar_t c1, wchar_t c2) { return std::tolower(c1) < std::tolower(c2); });
    }
};

struct CaseInsensitiveCompare
{
    bool operator()(const std::string& str1, const std::string& str2) const
    {
        // Use std::tolower to convert characters to lowercase
        return std::lexicographical_compare(
            str1.begin(),
            str1.end(),
            str2.begin(),
            str2.end(),
            [](char c1, char c2) { return std::tolower(c1) < std::tolower(c2); });
    }
};


}  // namespace hexpander