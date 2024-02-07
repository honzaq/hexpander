#pragma once

#include <string>

namespace hexpander
{

class expander_feeder_intf
{
public:
    virtual ~expander_feeder_intf() = default;

    /**
     * Register new expand variable
     * @param token       token for expansion, ex: %SOMETHING%
     * @param replacement text replacing token
     * @param lang        optional language for replacement, ex: en-us
     */
    virtual void add_expand(std::wstring_view token, std::wstring_view replacement, std::wstring_view lang = {}) = 0;
};

}  // namespace hexpander