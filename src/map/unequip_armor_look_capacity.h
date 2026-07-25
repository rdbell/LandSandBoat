#pragma once

#include <cstdint>

// Pure armor look-clear plan from charutils::UnequipItem.

namespace unequiparmorlookhelpers
{

constexpr std::uint8_t SlotHead = 4;
constexpr std::uint8_t SlotFeet = 8;

struct Plan
{
    bool          setArmorLook = false;
    std::uint8_t  slot         = 0;
    std::uint16_t modelID      = 0;
};

// PlanFor mirrors the direct HEAD through FEET look clears.
constexpr auto PlanFor(const std::uint8_t equipSlotID) -> Plan
{
    if (equipSlotID < SlotHead || equipSlotID > SlotFeet)
    {
        return {};
    }
    return {
        .setArmorLook = true,
        .slot         = equipSlotID,
    };
}

} // namespace unequiparmorlookhelpers
