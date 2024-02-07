#pragma once

#include <string>

namespace hexpander
{

// !!! NOTE: whole registry is just virtual and we will use it for unit-test 

class registry_intf
{
public:
    virtual ~registry_intf() = default;

    virtual std::wstring read_reg_value(std::wstring_view key, std::wstring_view value_name, std::wstring_view wow) = 0;
};

}  // namespace hexpander