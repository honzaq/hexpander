#pragma once

#include "expander_intf.h"
#include <map>

namespace hexpander
{

class light_expander : public expander_intf
{
public:
    /**
     * Expand source to replacement
     */
    std::wstring expand(std::wstring_view source) const override;

    /**
     * Not implemented for light expander
     */
    std::optional<std::wstring> expand_or_fail(std::wstring_view source) const override;

    /**
     * Add new expand replacement
     */
    void add_expand(std::wstring_view source, std::wstring_view replacement);

private:
    std::map<std::wstring, std::wstring, std::less<>> m_expands;
};

}  // namespace hexpander