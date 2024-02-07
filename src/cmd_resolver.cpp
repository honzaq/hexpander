#include "cmd_resolver.h"
#include <regex>
#include <cassert>
#include <ranges>
#include <format>
#include "utils.h"

namespace hexpander
{

cmd_resolver::cmd_resolver()
{}

void cmd_resolver::add_cmd_parameter(std::wstring_view cmd_param, std::wstring_view cmd_value)
{
    m_cmd_params.insert_or_assign(std::wstring(cmd_param), std::wstring(cmd_value));
}

std::optional<std::wstring> cmd_resolver::expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const
{
    assert(expand_fn);

    // Regular expression to match %CMD[cmdkeyname]%
    std::wregex const token_tegex(LR"(%CMD\[(.*?)\]%)");

    //////////////////////////////////////////////////////////////////////////
    std::wstring_view found_source{source};

    std::wcmatch match;

    if (std::regex_search(found_source.data(), found_source.data() + found_source.size(), match, token_tegex))
    {
        // move "pointer" after match
        found_source = found_source.substr(match.position() + match.length());

        std::wstring token = match[0].str();
        std::wstring cmd_param = match[1].str();

        std::wstring expanded_cmd_param = expand_fn->expand_recursive(cmd_param, std::nullopt);

        auto it = m_cmd_params.find(expanded_cmd_param);

        std::wstring prefix, mid, suffix;
        // prefix
        if (match.prefix().length())
        {
            prefix = expand_fn->expand_recursive(match.prefix().str(), std::nullopt);
        }
        // mid
        if (it != m_cmd_params.end() && !it->second.empty())
        {
            mid = expand_fn->expand_recursive(it->second, std::nullopt);
        }
        // suffix
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