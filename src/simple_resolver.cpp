#include "simple_resolver.h"
#include <regex>
#include <cassert>
#include <ranges>
#include <format>
#include "utils.h"

namespace hexpander
{

constexpr auto DEFAULT_LANG = L"en-us";

simple_resolver::simple_resolver()
{}

void simple_resolver::add_expand(std::wstring_view token, std::wstring_view value, std::wstring_view lang /*= {}*/)
{
    auto used_lang = lang.empty() ? DEFAULT_LANG : lang;
    auto it = m_expands.find(std::wstring(token));
    if (it != m_expands.end())
    {
        it->second.insert_or_assign(std::wstring(used_lang), std::wstring(value));
    }
    else
    {
        lang_items_t lang_item;
        lang_item.insert_or_assign(std::wstring(used_lang), std::wstring(value));
        m_expands.insert_or_assign(std::wstring(token), std::move(lang_item));
    }
}

std::optional<std::wstring> simple_resolver::expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const
{
    assert(expand_fn);

    // Regular expression to match tokens like "%var%"
    //std::wregex const token_tegex(LR"(%[A-Z0-9_,-]+%)");
    std::wregex const token_tegex(LR"(%[A-Z0-9_]+(?:,)?([a-z]{2}-[a-z]{2})?%)");

    std::wstring_view found_source{source};

    std::wcmatch match;

    if (std::regex_search(found_source.data(), found_source.data() + found_source.size(), match, token_tegex))
    {
        // move "pointer" after match
        found_source = found_source.substr(match.position() + match.length());

        // Check if the matched token is in the variables map
        std::wstring token = match[0].str();
        std::wstring lang = [&]() -> std::wstring {
            if (match[1].matched)
            {
                auto const & params_list = match[1].str();
                token.erase(token.size() - (params_list.size() + 2 /*,%*/), params_list.size() + 1);
                return match[1].str();
            }
            return DEFAULT_LANG;
        }();

        auto it = m_expands.find(token);

        std::wstring prefix, mid, suffix;
        if (match.prefix().length())
        {
            prefix = expand_fn->expand_recursive(match.prefix().str(), std::nullopt);
        }
        if (it != m_expands.end() && !it->second.empty())
        {
            auto lang_it = it->second.find(lang);
            if (lang_it != it->second.end())
            {
                mid = lang_it->second.empty() ? lang_it->second : expand_fn->expand_recursive(lang_it->second, std::nullopt);
            }
            else
            {
                mid = std::move(token);
            }
        }
        else
        {
            mid = std::move(token);
        }
        if (match.suffix().length())
        {
            suffix = expand_fn->expand_recursive(match.suffix().str(), std::nullopt);
        }
        return std::move(prefix) + std::move(mid) + std::move(suffix);
    }
    else if (list->empty())
    {
        return std::wstring(source);  // no found, we will use source token as output
    }
    return std::nullopt;
}

}  // namespace hexpander