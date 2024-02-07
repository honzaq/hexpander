#pragma once

#include <string>
#include "expander_recursive_intf.h"
//#include <memory>
#include <optional>

namespace hexpander
{

class expander_resolver_intf
{
public:
    virtual ~expander_resolver_intf() = default;

    /**
     * Allow expand `variables` 
     */
    virtual std::optional<std::wstring> expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const = 0;
    
    //virtual bool expand(std::shared_ptr<std::wstring> source, std::shared_ptr<std::wstring> out_value) = 0;
};

}  // namespace hexpander