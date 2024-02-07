#include "registry_resolver.h"
#include <regex>
#include <cassert>
#include <ranges>
#include <format>
#include "utils.h"

namespace hexpander
{

registry_resolver::registry_resolver(std::shared_ptr<registry_intf> sp_registry)
    : m_sp_registry(std::move(sp_registry))
{
}

std::optional<std::wstring> registry_resolver::expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const
{
    assert(expand_fn);

    // Regular expression to match %REG[root,key,value_name,wow]%, text inside [..] is returned as single text
    std::wregex const token_tegex(LR"(%REG\[(.*?)\]%)");

    //////////////////////////////////////////////////////////////////////////
    std::wstring_view found_source{source};

    std::wcmatch match;

    if (std::regex_search(found_source.data(), found_source.data() + found_source.size(), match, token_tegex))
    {
        // move "pointer" after match
        found_source = found_source.substr(match.position() + match.length());

        std::wstring token = match[0].str();
        std::wstring params_list = match[1].str();

        // split comma separated string
        std::wregex               params_regex{LR"([^,]+)"};
        std::vector<std::wstring> tokens{std::wsregex_token_iterator{std::begin(params_list), std::end(params_list), params_regex}, std::wsregex_token_iterator{}};

        // 3 parameters are mandatory
        if (tokens.size() < 3)
        {
            assert(false && "invalid number of %REG[...]% arguments, expect 4, 4th is optional");
            throw std::invalid_argument(to_utf8(std::format(L"invalid number of paramters for: {}", token)).c_str());
        }

        // Check if the matched token is in the variables map

        std::wstring root = expand_fn->expand_recursive(tokens[0], std::nullopt);
        std::wstring key = expand_fn->expand_recursive(tokens[1], std::nullopt);
        std::wstring value_name = expand_fn->expand_recursive(tokens[2], std::nullopt);
        // wow token is optional by default native
        std::wstring wow = L"native-wow";
        if (tokens.size() >= 4)
        {
            wow = expand_fn->expand_recursive(tokens[3], std::nullopt);
        }

        auto reg_value = m_sp_registry->read_reg_value(root + L"\\" + key, value_name, wow);
        std::wstring prefix, mid, suffix;
        if (match.prefix().length())
        {
            prefix = expand_fn->expand_recursive(match.prefix().str(), std::nullopt);
        }
        if (!reg_value.empty())
        {
            mid = std::move(reg_value);
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