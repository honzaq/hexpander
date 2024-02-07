#pragma once

#include "../src/registry_intf.h"

namespace hexpander_tests
{

class registry_fake : public hexpander::registry_intf
{
public:

    std::wstring m_read_reg_value_key;
    std::wstring m_read_reg_value_value_name;
    std::wstring m_read_reg_value_wow;
    std::wstring m_read_reg_value_rv;
    std::wstring read_reg_value(std::wstring_view key, std::wstring_view value_name, std::wstring_view wow) override
    {
        m_read_reg_value_key = key;
        m_read_reg_value_value_name = value_name;
        m_read_reg_value_wow = wow;
        return m_read_reg_value_rv;
    }
};

}  // namespace hexpander_tests