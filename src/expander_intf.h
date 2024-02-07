#pragma once

#include <string>
#include <optional>
#include <memory>

namespace hexpander
{

class expander_intf
{
public:
    virtual ~expander_intf() = default;

    /**
     * Expand source `variable`
     */
    virtual std::wstring expand(std::wstring_view source) const = 0;

    /**
     * Expand source `variable` but if at least one expand fail return std::nullopt
     */
    virtual std::optional<std::wstring> expand_or_fail(std::wstring_view source) const = 0;

    /**
     * Change is source is in expander
    */
    //virtual bool contains(std::wstring_view source) const = 0;

    /**
     * Expand source `variable as shared pointer
     */
    std::shared_ptr<std::wstring> expand_as_ptr(std::wstring_view source) const
    {
        return std::make_shared<std::wstring>(std::move(expand(source)));
    }

    /**
     * Expand optional source `variable` as optional
     */
    std::optional<std::wstring> expand_optional(const std::optional<std::wstring>& source) const
    {
        if (!source.has_value())
        {
            return source;
        }
        return std::make_optional<std::wstring>(expand(source.value()));
    }
};

}  // namespace hexpander