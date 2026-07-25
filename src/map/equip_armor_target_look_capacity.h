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

} // namespace equiparmortargetlookhelpers
