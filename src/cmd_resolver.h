#pragma once

#include <memory>
#include <map>
#include "expander_resolver_intf.h"
#include "registry_intf.h"
#include "utils.h"

namespace hexpander
{

using cmd_params_map_t = std::map<std::wstring, std::wstring, nocase_comparator>;


class cmd_resolver : public expander_resolver_intf
{
public:
    cmd_resolver();

    // Inherited via expander_resolver_intf
    std::optional<std::wstring> expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const override;

    /**
     * @brief Add command line parameters
     */
    void add_cmd_parameter(std::wstring_view cmd_param, std::wstring_view cmd_value);

private:
    cmd_params_map_t m_cmd_params;
};

}  // namespace hexpander