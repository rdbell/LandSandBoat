#pragma once

#include <cstdint>

namespace unequippostswitcheffectshelpers
{
constexpr uint8_t InventoryLocationID = 0;

struct Plan
{
    bool    callItemUnequipScript{};
    bool    queueEquipChange{};
    uint8_t locationID{};
    uint8_t itemSlotID{};
    uint8_t equipSlotID{};
};

constexpr Plan PlanFor(const uint8_t equipSlotID)
{
    return {
        .callItemUnequipScript = true,
        .queueEquipChange      = true,
        .locationID            = InventoryLocationID,
        .equipSlotID           = equipSlotID,
    };
}
} // namespace unequippostswitcheffectshelpers
