#pragma once

#include "style_update_capacity.h"

#include <array>
#include <cstdint>

// Pure snapshot/reset plan from charutils::SetStyleLock.

namespace stylelocktransitionhelpers
{

constexpr std::size_t SlotCount = 16;

struct EquipmentSlot
{
    bool          hasItem = false;
    std::uint16_t itemID  = 0;
};

struct Input
{
    bool                                  currentlyLocked = false;
    bool                                  requestedLocked = false;
    std::array<EquipmentSlot, SlotCount> equipment{};
};

struct Plan
{
    bool                             snapshotStyleItems = false;
    bool                             copyCurrentLook    = false;
    bool                             clearStyleItems    = false;
    bool                             notifyChange       = false;
    bool                             locked             = false;
    std::array<std::uint16_t, SlotCount> styleItems{};
};

constexpr auto PlanFor(const Input& input) -> Plan
{
    auto plan = Plan{
        .clearStyleItems = !input.requestedLocked,
        .notifyChange    = styleupdatehelpers::ShouldNotifyStyleLockChange(input.currentlyLocked, input.requestedLocked),
        .locked          = input.requestedLocked,
    };
    if (!styleupdatehelpers::ShouldApplyStyleLockSnapshot(input.requestedLocked))
    {
        return plan;
    }

    plan.snapshotStyleItems = true;
    plan.copyCurrentLook    = true;
    for (std::size_t slot = 0; slot < SlotCount; ++slot)
    {
        plan.styleItems[slot] = styleupdatehelpers::StyleItemFromEquip(input.equipment[slot].hasItem, input.equipment[slot].itemID);
    }
    return plan;
}

} // namespace stylelocktransitionhelpers
