#pragma once

#include <memory>
#include "expander_resolver_intf.h"
#include "registry_intf.h"

namespace hexpander
{

class registry_resolver : public expander_resolver_intf
{
public:
    registry_resolver(std::shared_ptr<registry_intf> sp_registry);

    // Inherited via expander_resolver_intf
    std::optional<std::wstring> expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const override;

private:
    std::shared_ptr<registry_intf> m_sp_registry;
};

}  // namespace hexpander