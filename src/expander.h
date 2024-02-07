#pragma once

#include "expander_intf.h"
#include "expander_resolver_intf.h"
#include "expander_recursive_intf.h"
#include <memory>
#include <unordered_set>

namespace hexpander
{

class expander
    : public expander_intf
    , public expander_recursive_intf
{
public:
    // inherited via expander_intf
    std::wstring                expand(std::wstring_view source) const override;
    std::optional<std::wstring> expand_or_fail(std::wstring_view source) const override;
    //bool                        contains(std::wstring_view source) const override;

    // Inherited via expander_recursive_intf
    std::wstring expand_recursive(std::wstring_view source, std::optional<expanders_list> list) const override;

    void register_resolver(std::shared_ptr<expander_resolver_intf> sp_resolver);

private:
    expanders_list m_resolvers;

    
};

}  // namespace hexpander