#pragma once

#include <cstdint>

// Pure equip/unequip eligibility and EquipItem early-exit policy from charutils.

namespace equippolicyhelpers
{

// SLOTTYPE pins from battle_entity.h.
constexpr std::uint8_t SlotMain   = 0x00;
constexpr std::uint8_t SlotSub    = 0x01;
constexpr std::uint8_t SlotRanged = 0x02;
constexpr std::uint8_t SlotAmmo   = 0x03;
constexpr std::uint8_t SlotHead   = 0x04;
constexpr std::uint8_t SlotFeet   = 0x08;
constexpr std::uint8_t SlotEar1   = 0x0B;
constexpr std::uint8_t SlotEar2   = 0x0C;
constexpr std::uint8_t SlotRing1  = 0x0D;
constexpr std::uint8_t SlotRing2  = 0x0E;
constexpr std::uint8_t MaxEquipSlotID = 15;

// SKILL_NONE / SKILL_HAND_TO_HAND pins.
constexpr std::uint8_t SkillNone       = 0;
constexpr std::uint8_t SkillHandToHand = 1;

// IsEquipSlotIDValid mirrors equipSlotID <= 15.
constexpr auto IsEquipSlotIDValid(const std::uint8_t equipSlotID) -> bool
{
    return equipSlotID <= MaxEquipSlotID;
}

// HasSlotEquipped mirrors item present and is equipment.
constexpr auto HasSlotEquipped(const bool hasItem, const bool isEquipment) -> bool
{
    return hasItem && isEquipment;
}

// ShouldRejectNullChar mirrors PChar == nullptr.
constexpr auto ShouldRejectNullChar(const bool isNull) -> bool
{
    return isNull;
}

// ShouldRejectNullCharOrStorage mirrors PChar == nullptr || storage == nullptr.
constexpr auto ShouldRejectNullCharOrStorage(const bool charNull, const bool storageNull) -> bool
{
    return charNull || storageNull;
}

// IsAlreadyEquippedInSlot mirrors PItem && PItem == getEquip(equipSlot).
constexpr auto IsAlreadyEquippedInSlot(const bool hasItem, const bool sameAsEquipSlot) -> bool
{
    return hasItem && sameAsEquipSlot;
}

// IsUnequipRequest mirrors slotID == 0.
constexpr auto IsUnequipRequest(const std::uint8_t slotID) -> bool
{
    return slotID == 0;
}

// PairedSlotForDuplicateCheck returns the other dual slot for MAIN/SUB/EAR/RING, or 0xFF if none.
constexpr auto PairedSlotForDuplicateCheck(const std::uint8_t equipSlotID) -> std::uint8_t
{
    switch (equipSlotID)
    {
        case SlotMain:
            return SlotSub;
        case SlotSub:
            return SlotMain;
        case SlotEar1:
            return SlotEar2;
        case SlotEar2:
            return SlotEar1;
        case SlotRing1:
            return SlotRing2;
        case SlotRing2:
            return SlotRing1;
        default:
            return 0xFF;
    }
}

// ShouldSkipCrossSlotSameItem mirrors equip of same item already in paired dual slot.
// Only applies when slotID > 0 (not unequip). Host injects sameAsPaired.
constexpr auto ShouldSkipCrossSlotSameItem(const std::uint8_t slotID, const std::uint8_t equipSlotID, const bool sameAsPaired) -> bool
{
    if (slotID == 0)
    {
        return false;
    }
    if (PairedSlotForDuplicateCheck(equipSlotID) == 0xFF)
    {
        return false;
    }
    return sameAsPaired;
}

// ShouldBlockRangedEquipDuringRA mirrors ranged/ammo equip blocked in CRangeState.
// ammoBlockedWhenNoRanged: equipSlot is AMMO and no ranged equipped.
constexpr auto ShouldBlockRangedEquipDuringRA(const std::uint8_t equipSlotID, const bool hasRangedEquip, const bool inRangeState) -> bool
{
    if (!inRangeState)
    {
        return false;
    }
    if (equipSlotID == SlotRanged)
    {
        return true;
    }
    if (equipSlotID == SlotAmmo && !hasRangedEquip)
    {
        return true;
    }
    return false;
}

// ShouldRequire2HForGrip mirrors sub non-shield grip (skill NONE) without 2H main.
constexpr auto ShouldRequire2HForGrip(const bool isSubSlot, const bool isShield, const bool isWeapon, const std::uint8_t skillType, const bool mainIsTwoHanded) -> bool
{
    if (!isSubSlot || isShield || !isWeapon)
    {
        return false;
    }
    return skillType == SkillNone && !mainIsTwoHanded;
}

// ShouldRequireDualWield mirrors sub weapon with skill and no DW trait.
constexpr auto ShouldRequireDualWield(const bool isSubSlot, const bool isShield, const bool isWeapon, const std::uint8_t skillType, const bool hasDualWield) -> bool
{
    if (!isSubSlot || isShield || !isWeapon)
    {
        return false;
    }
    return skillType != SkillNone && !hasDualWield;
}

// ShouldBlockOffhandDualWield mirrors 2H main or missing main for DW offhand.
constexpr auto ShouldBlockOffhandDualWield(const bool isSubSlot, const bool isShield, const bool isWeapon, const std::uint8_t skillType, const bool hasMainWeapon, const bool mainIsTwoHanded) -> bool
{
    if (!isSubSlot || isShield || !isWeapon || skillType == SkillNone)
    {
        return false;
    }
    return mainIsTwoHanded || !hasMainWeapon;
}

// ShouldBlockSubWithH2HMain mirrors H2H main blocks non-shield sub equip path return.
// Called after grip/DW checks; original returns for any H2H main when in sub non-shield branch.
constexpr auto ShouldBlockSubWithH2HMain(const bool isSubSlot, const bool isShield, const bool mainIsH2H) -> bool
{
    return isSubSlot && !isShield && mainIsH2H;
}

// ShouldRemoveSubOnMainUnequip mirrors equipSlotID==0 && sub present non-shield after unequip main.
constexpr auto ShouldRemoveSubOnMainUnequip(const std::uint8_t equipSlotID, const bool hasSub, const bool subIsShield) -> bool
{
    return equipSlotID == SlotMain && hasSub && !subIsShield;
}

// EquipArmorEligibility pure form of EquipArmor rejection conditions.
// Returns true when equip is allowed.
constexpr auto IsEquipArmorEligible(const bool equipSlotBlocked,
                                    const bool jobAllowed,
                                    const bool superiorOK,
                                    const bool levelOK,
                                    const bool raceOK) -> bool
{
    return !equipSlotBlocked && jobAllowed && superiorOK && levelOK && raceOK;
}

// IsEquipSlotBlocked mirrors m_EquipBlock & (1 << equipSlotID).
constexpr auto IsEquipSlotBlocked(const std::uint32_t equipBlock, const std::uint8_t equipSlotID) -> bool
{
    return (equipBlock & (static_cast<std::uint32_t>(1u) << equipSlotID)) != 0;
}

// IsJobAllowedForItem mirrors getJobs() & (1 << (mJob-1)).
constexpr auto IsJobAllowedForItem(const std::uint32_t itemJobs, const std::uint8_t mJob) -> bool
{
    if (mJob == 0)
    {
        return false;
    }
    return (itemJobs & (static_cast<std::uint32_t>(1u) << (mJob - 1))) != 0;
}

// IsSuperiorLevelOK mirrors !(itemSuperior > superiorMod).
constexpr auto IsSuperiorLevelOK(const std::uint8_t itemSuperior, const std::int16_t superiorMod) -> bool
{
    return static_cast<std::int16_t>(itemSuperior) <= superiorMod;
}

// IsReqLevelOK mirrors reqLvl <= effectiveLevel (reuse EffectiveLevelForGearReq from checkequipment).
constexpr auto IsReqLevelOK(const std::uint8_t reqLvl, const std::uint8_t effectiveLevel) -> bool
{
    return reqLvl <= effectiveLevel;
}

// ShouldKeepSubOnMainTwoHandedSwap mirrors the negated condition that skips RemoveSub
// when swapping 2H→2H in same inventory slot continuity path.
// Host injects: sameInventorySlot, oldIsWeapon, newIsWeapon, oldTwoHanded, newTwoHanded.
constexpr auto ShouldRemoveSubOnMainEquip(const bool sameInventorySlot,
                                          const bool oldIsWeapon,
                                          const bool newIsWeapon,
                                          const bool oldTwoHanded,
                                          const bool newTwoHanded,
                                          const bool hasSubEquipment,
                                          const bool subIsShield) -> bool
{
    // Original: if (!(same && old weapon && new weapon && both 2H)) then maybe RemoveSub
    const bool preserveSub = sameInventorySlot && oldIsWeapon && newIsWeapon && oldTwoHanded && newTwoHanded;
    if (preserveSub)
    {
        return false;
    }
    return hasSubEquipment && !subIsShield;
}

// CanUseWeaponSkillByLevel mirrors GetSkill(type) >= skillLevel.
constexpr auto CanUseWeaponSkillByLevel(const std::uint16_t charSkill, const std::uint16_t wsSkillLevel) -> bool
{
    return charSkill >= wsSkillLevel;
}

// ShouldRejectNonPCTrait mirrors objtype != TYPE_PC.
constexpr auto ShouldRejectNonPCTrait(const bool isPC) -> bool
{
    return !isPC;
}

// PreferRemoveSlotLookID mirrors removeSlotLookID > 0 ? lookID : removeSlotID.
constexpr auto PreferRemoveSlotLookID(const std::uint32_t removeSlotLookID, const std::uint32_t removeSlotID) -> std::uint32_t
{
    return removeSlotLookID > 0 ? removeSlotLookID : removeSlotID;
}

// IsArmorLookSlot mirrors i >= SLOT_HEAD && i <= SLOT_FEET.
constexpr auto IsArmorLookSlot(const std::uint8_t slot) -> bool
{
    return slot >= SlotHead && slot <= SlotFeet;
}

// RemoveSubShouldUnequip mirrors sub present and is equipment.
constexpr auto RemoveSubShouldUnequip(const bool hasSub, const bool subIsEquipment) -> bool
{
    return hasSub && subIsEquipment;
}

// ShouldClearTPOnWeaponEquip mirrors main/ranged/sub and not wind/string instrument.
// skillType only consulted when item is weapon equipment.
constexpr auto ShouldClearTPOnWeaponEquip(const std::uint8_t equipSlotID, const bool hasItem, const bool isEquipment, const std::uint8_t skillType, const std::uint8_t skillString, const std::uint8_t skillWind) -> bool
{
    if (equipSlotID != SlotMain && equipSlotID != SlotRanged && equipSlotID != SlotSub)
    {
        return false;
    }
    if (!hasItem || !isEquipment)
    {
        return true;
    }
    return skillType != skillString && skillType != skillWind;
}

} // namespace equippolicyhelpers
