#pragma once

#include <string>
#include <optional>
#include <memory>
#include <stack>
#include <optional>

namespace hexpander
{

// forward declaration
class expander_resolver_intf;

// define types
using expanders_list = std::stack<std::shared_ptr<expander_resolver_intf>>;

// define interface
class expander_recursive_intf
{
public:
    virtual ~expander_recursive_intf() = default;

    /**
     * Expand source `variable`
     */
    virtual std::wstring expand_recursive(std::wstring_view source, std::optional<expanders_list> list) const = 0;
};

}  // namespace hexpander