#pragma once

#include <cstdint>

// Pure armor target-slot appearance predicate from charutils::EquipArmor.

namespace equiparmortargetlookhelpers
{

constexpr std::uint8_t SlotHead = 4;
constexpr std::uint8_t SlotFeet = 8;

// ShouldSetArmorLook mirrors the HEAD through FEET EquipArmor target cases.
constexpr auto ShouldSetArmorLook(const std::uint8_t equipSlotID) -> bool
{
    return equipSlotID >= SlotHead && equipSlotID <= SlotFeet;
}

struct Plan
{
    bool          setArmorLook = false;
    std::uint8_t  slot         = 0;
    std::uint16_t modelID      = 0;
};

// PlanFor mirrors EquipArmor's armor target selection and model projection.
constexpr auto PlanFor(const std::uint8_t equipSlotID, const std::uint16_t modelID) -> Plan
{
    if (!ShouldSetArmorLook(equipSlotID))
    {
        return {};
    }
    return {
        .setArmorLook = true,
        .slot         = equipSlotID,
        .modelID      = modelID,
    };
}

} // namespace equiparmortargetlookhelpers
