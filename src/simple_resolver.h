#pragma once

#include <memory>
#include <map>
#include "expander_resolver_intf.h"
#include "expander_feeder_intf.h"
#include "registry_intf.h"
#include "utils.h"

namespace hexpander
{

using lang_items_t = std::map<std::wstring, std::wstring, nocase_comparator>;
using tokens_t = std::map<std::wstring, lang_items_t, nocase_comparator>;

class simple_resolver 
    : public expander_resolver_intf
    , public expander_feeder_intf
{
public:
    simple_resolver();

    // inherited via expander_resolver_intf
    std::optional<std::wstring> expand_by_resolver(std::wstring_view source, std::optional<expanders_list> list, expander_recursive_intf const* expand_fn) const override;

    // inherited via expander_feeder_intf
    void add_expand(std::wstring_view token, std::wstring_view value, std::wstring_view lang = {}) override;

private:
    tokens_t m_expands;
};

}  // namespace hexpander