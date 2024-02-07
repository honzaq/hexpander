#include "expander.h"
#include "utils.h"
#include <regex>

namespace hexpander
{

std::wstring expander::expand(std::wstring_view source) const
{
    return expand_recursive(source, std::nullopt);
}

std::optional<std::wstring> expander::expand_or_fail(std::wstring_view source) const
{
    std::optional<bool> did_expand_fail = std::make_optional<bool>(false);
    auto const          result{L""};  // = expand_symbols(source, did_expand_fail);
    if (did_expand_fail.value_or(false))
    {
        return std::nullopt;
    }
    return result;
}


//bool expander::contains(std::wstring_view source) const
//{
//    return m_expands.contains(std::wstring(source));
//}

//std::wstring expander::expand_symbols(std::wstring_view source, std::optional<bool>& did_expand_fail) const
//{
//std::wstring result(source);
//expand_inside(result, did_expand_fail);
//    expand_escape(result);
//return result;
//}

void expander::register_resolver(std::shared_ptr<expander_resolver_intf> sp_resolver)
{
    m_resolvers.push(std::move(sp_resolver));
}

std::wstring expander::expand_recursive(std::wstring_view source, std::optional<expanders_list> list) const
{
    // 1. create list of expander (normal expand is on bottom)
    // 2. for each item in list (take from top tu down, each recursion remove one item)
    // 3. possible find: prefix %TOKEN% sufix
    // 4. expand(prefix)  -  new expander list(full)) -> again recursive
    // 5. expand(%TOKEN%) -  -||-
    // 6. expand(suffux)  -  -||-
    // 7. collect results

    // TODO: maybe second optional parameter "policy" EMPTY="%X%"->"" KEEP="y%X%y"->"y%X%y", current design is KEEP

    if (!list.has_value())
    {
        list = std::make_optional(m_resolvers);
    }
    if (list->empty())
    {
        return std::wstring(source);
    }
    if (source.empty())
    {
        return std::wstring(source);
    }

    std::optional<std::wstring> r;
    do
    {
        auto const resolver = list->top();
        list->pop();

        r = resolver->expand_by_resolver(r.value_or(std::wstring(source)), list, this);
        if (list->empty())
        {
            return r.value_or(std::wstring(source));
        }
        
    } while (true);

    return r.value_or(std::wstring(source));
}

}  // namespace hexpander