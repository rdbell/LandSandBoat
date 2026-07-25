#pragma once

#include "keyitem_spell_capacity.h"

#include <cstddef>
#include <cstdint>

namespace keyitemtablebithelpers
{
struct Facts
{
    uint16_t    keyItemID{};
    std::size_t tablesSize{};
};

struct Plan
{
    uint16_t tableIndex{};
    uint16_t bitIndex{};
    bool     inRange{};
};

constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{
        .tableIndex = keyitemspellhelpers::KeyItemTableIndex(facts.keyItemID),
        .bitIndex   = keyitemspellhelpers::KeyItemBitIndex(facts.keyItemID),
    };
    plan.inRange = keyitemspellhelpers::KeyItemTableInRange(plan.tableIndex, facts.tablesSize);
    return plan;
}
} // namespace keyitemtablebithelpers
