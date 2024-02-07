#include "light_expander.h"
#include <cassert>

namespace hexpander
{

std::wstring light_expander::expand(std::wstring_view source) const
{
    auto it = m_expands.find(source);
    if (it == m_expands.end())
    {
        return std::wstring(source);
    }
    return it->second;
}

std::optional<std::wstring> light_expander::expand_or_fail(std::wstring_view source) const
{
    assert(false && "not implemented");
    return std::nullopt;
}

void light_expander::add_expand(std::wstring_view source, std::wstring_view replacement)
{
    m_expands.insert_or_assign(std::wstring(source), std::wstring(replacement));
}

}  // namespace hexpander