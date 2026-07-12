#pragma once

#include <cstdint>

// Pure CheckValidEquipment policy from charutils.

namespace checkequipmenthelpers
{

// Equip slot count iterated by CheckValidEquipment.
constexpr std::uint8_t EquipSlotCount = 16;

// SLOT_MAIN / SLOT_SUB pins (common/mmo.h style).
constexpr std::uint8_t SlotMain = 0;
constexpr std::uint8_t SlotSub  = 1;

// EffectiveLevelForGearReq mirrors DISABLE_GEAR_SCALING ? mLevel : jobLevel.
constexpr auto EffectiveLevelForGearReq(const bool disableGearScaling, const std::uint8_t mLevel, const std::uint8_t jobLevel) -> std::uint8_t
{
    return disableGearScaling ? mLevel : jobLevel;
}

// ShouldUnequipByLevel mirrors reqLvl > effectiveLevel.
constexpr auto ShouldUnequipByLevel(const std::uint8_t reqLvl, const std::uint8_t effectiveLevel) -> bool
{
    return reqLvl > effectiveLevel;
}

// IsSubSlot mirrors slotID == SLOT_SUB.
constexpr auto IsSubSlot(const std::uint8_t slotID) -> bool
{
    return slotID == SlotSub;
}

// ShouldUnequipInvalidSub mirrors sub non-shield without main, or without DW when skill != NONE.
// Host injects: isShield, hasMainEquip, hasDualWield, isGrip (skillType == SKILL_NONE).
constexpr auto ShouldUnequipInvalidSub(const bool isShield, const bool hasMainEquip, const bool hasDualWield, const bool isGrip) -> bool
{
    if (isShield)
    {
        return false;
    }
    // Unequip if no main weapon or a non-grip subslot without DW
    return !hasMainEquip || (!hasDualWield && !isGrip);
}

// JobBitForMainJob mirrors 1 << (mJob - 1).
constexpr auto JobBitForMainJob(const std::uint8_t mJob) -> std::uint32_t
{
    if (mJob == 0)
    {
        return 0;
    }
    return static_cast<std::uint32_t>(1u << (mJob - 1));
}

// SlotBitForEquipSlot mirrors 1 << slotID.
constexpr auto SlotBitForEquipSlot(const std::uint8_t slotID) -> std::uint32_t
{
    return static_cast<std::uint32_t>(1u << slotID);
}

// ShouldKeepEquipment mirrors (jobs & jobBit) && (equipSlotId & slotBit).
constexpr auto ShouldKeepEquipment(const std::uint32_t itemJobs, const std::uint32_t jobBit, const std::uint32_t itemEquipSlots, const std::uint32_t slotBit) -> bool
{
    return (itemJobs & jobBit) != 0 && (itemEquipSlots & slotBit) != 0;
}

// ShouldCheckUnarmedWeapon mirrors !main || !equipment || main is unarmed H2H sentinel.
// Host injects booleans.
constexpr auto ShouldCheckUnarmedWeapon(const bool hasMainEquip, const bool mainIsEquipment, const bool mainIsUnarmedH2H) -> bool
{
    return !hasMainEquip || !mainIsEquipment || mainIsUnarmedH2H;
}

// IsEquipmentSlotInRange mirrors slotID < 16 for the equip scan.
constexpr auto IsEquipmentSlotInRange(const std::uint8_t slotID) -> bool
{
    return slotID < EquipSlotCount;
}

// ShouldSkipNonEquipment mirrors null or not equipment type.
constexpr auto ShouldSkipNonEquipment(const bool hasItem, const bool isEquipment) -> bool
{
    return !hasItem || !isEquipment;
}

} // namespace checkequipmenthelpers
