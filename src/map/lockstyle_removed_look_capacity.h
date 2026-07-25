#pragma once

#include "equip_policy_capacity.h"

#include <cstdint>

// Pure snapshot-item gate and mask selection from UpdateRemovedSlotsLookForLockStyle.

namespace lockstyleremovedlookhelpers
{

struct Input
{
    std::uint16_t styleItemID      = 0;
    bool          itemFound        = false;
    std::uint32_t removeSlotLookID = 0;
    std::uint32_t removeSlotID     = 0;
};

struct Plan
{
    bool          applies           = false;
    std::uint32_t effectiveRemoveID = 0;
};

constexpr auto PlanFor(const Input& input) -> Plan
{
    if (input.styleItemID == 0 || !input.itemFound)
    {
        return {};
    }

    const auto effectiveRemoveID = equippolicyhelpers::PreferRemoveSlotLookID(input.removeSlotLookID, input.removeSlotID);
    return {
        .applies           = effectiveRemoveID > 0,
        .effectiveRemoveID = effectiveRemoveID,
    };
}

} // namespace lockstyleremovedlookhelpers
