#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

// Pure UpdateItem quantity gates and style-lock policy from charutils.

namespace styleupdatehelpers
{

constexpr std::uint8_t  MaxJobType      = 24;
constexpr std::uint8_t  SlotLink1       = 0x10;
constexpr std::uint16_t MarvelousCheer  = 22283;
constexpr std::uint8_t  SkillWindInst   = 42;
constexpr std::uint16_t H2HSubLookOffset = 0x1000;

// --- UpdateItem pure gates ---

// ShouldRejectNullUpdateItem mirrors PItem == nullptr.
constexpr auto ShouldRejectNullUpdateItem(const bool hasItem) -> bool
{
    return !hasItem;
}

// ShouldRejectInvalidQuantity mirrors (quantity - reserve + delta) < 0.
constexpr auto ShouldRejectInvalidQuantity(const std::uint32_t quantity, const std::uint32_t reserve, const std::int32_t delta) -> bool
{
    return static_cast<std::int32_t>(quantity - reserve) + delta < 0;
}

// ShouldRejectBusyItemInUse mirrors item state busy path (same slot in use, !force).
constexpr auto ShouldRejectBusyItemInUse(const bool inItemState, const bool sameSlotAndLocation, const bool force) -> bool
{
    return inItemState && sameSlotAndLocation && !force;
}

// IsEquippedAmmoConsumption mirrors equipped ammo special case.
constexpr auto IsEquippedAmmoConsumption(const bool itemIsEquippedState, const bool isAmmoSlotItem) -> bool
{
    return itemIsEquippedState && isAmmoSlotItem;
}

// ShouldRejectBusyNonAmmo mirrors isBusy && !equippedAmmo && !force.
constexpr auto ShouldRejectBusyNonAmmo(const bool isBusy, const bool isEquippedAmmo, const bool force) -> bool
{
    return isBusy && !isEquippedAmmo && !force;
}

// CapQuantityToStack mirrors min(quantity+delta, stackSize) with non-negative base quantity assumed after prior checks.
constexpr auto CapQuantityToStack(const std::uint32_t quantity, const std::int32_t delta, const std::uint32_t stackSize) -> std::uint32_t
{
    const auto sum = static_cast<std::int64_t>(quantity) + static_cast<std::int64_t>(delta);
    auto       nq  = static_cast<std::uint32_t>(sum < 0 ? 0 : sum);
    if (nq > stackSize)
    {
        nq = stackSize;
    }
    return nq;
}

// ShouldKeepItemOnUpdate mirrors newQuantity > 0 || isCurrency.
constexpr auto ShouldKeepItemOnUpdate(const std::uint32_t newQuantity, const bool isCurrency) -> bool
{
    return newQuantity > 0 || isCurrency;
}

// ShouldDeleteItemOnUpdate mirrors newQuantity == 0 (and not currency path already handled).
constexpr auto ShouldDeleteItemOnUpdate(const std::uint32_t newQuantity) -> bool
{
    return newQuantity == 0;
}

// ShouldRefreshStyleOnDrop mirrors styleLocked && !HasItem remaining.
constexpr auto ShouldRefreshStyleOnDrop(const bool styleLocked, const bool stillHasItem) -> bool
{
    return styleLocked && !stillHasItem;
}

// --- Style lock pure policy ---

// CanEquipItemOnAnyJob: null item true; else any job 1..MAX-1 with job bit and reqLvl <= job level.
// Host iterates jobs and injects first match via this pure per-job check; or use AllJobsEligible below.
constexpr auto JobMeetsItemReqs(const std::uint32_t itemJobs, const std::uint8_t jobID, const std::uint8_t reqLvl, const std::uint8_t jobLevel) -> bool
{
    if (jobID == 0)
    {
        return false;
    }
    return (itemJobs & (static_cast<std::uint32_t>(1u) << (jobID - 1))) != 0 && reqLvl <= jobLevel;
}

// CanEquipItemOnAnyJobNullOK mirrors PItem == nullptr → true.
constexpr auto CanEquipItemOnAnyJobNullOK(const bool itemNull) -> bool
{
    return itemNull;
}

// CanEquipItemOnAnyJob mirrors the native null-item and job 1..MAX-1 scan.
// Superior-level eligibility remains intentionally deferred in the native path.
constexpr auto CanEquipItemOnAnyJob(const bool itemNull,
                                    const std::uint32_t itemJobs,
                                    const std::uint8_t reqLvl,
                                    const std::array<std::uint8_t, MaxJobType>& jobLevels) -> bool
{
    if (CanEquipItemOnAnyJobNullOK(itemNull))
    {
        return true;
    }

    for (std::uint8_t jobID = 1; jobID < MaxJobType; ++jobID)
    {
        if (JobMeetsItemReqs(itemJobs, jobID, reqLvl, jobLevels[jobID]))
        {
            return true;
        }
    }
    return false;
}

// HasValidStyleShieldCase mirrors both shields.
constexpr auto HasValidStyleShieldCase(const bool bothShields, const bool hasAppearanceItem, const bool canEquipAppearance) -> bool
{
    return bothShields && hasAppearanceItem && canEquipAppearance;
}

// HasValidStyleMarvelousCheer mirrors Marvelous Cheer + wind instrument equipped.
constexpr auto HasValidStyleMarvelousCheer(const bool appearanceIsMarvelousCheer, const bool equippedIsWeapon, const std::uint8_t equippedSkill, const bool hasAppearanceItem) -> bool
{
    return appearanceIsMarvelousCheer && equippedIsWeapon && equippedSkill == SkillWindInst && hasAppearanceItem;
}

// HasValidStyleMatchingSkill mirrors matching weapon skill types + has item + can equip.
constexpr auto HasValidStyleMatchingSkill(const bool bothWeapons, const std::uint8_t equippedSkill, const std::uint8_t appearanceSkill, const bool hasAppearanceItem, const bool canEquipAppearance) -> bool
{
    return bothWeapons && equippedSkill == appearanceSkill && hasAppearanceItem && canEquipAppearance;
}

// HasValidStyle combines shield / marvelous cheer / matching skill cases.
// When either item null → false (LSB if AItem && PItem).
constexpr auto HasValidStyle(const bool bothPresent,
                             const bool bothShields,
                             const bool hasAppearanceItem,
                             const bool canEquipAppearance,
                             const bool appearanceIsMarvelousCheer,
                             const bool equippedIsWeapon,
                             const std::uint8_t equippedSkill,
                             const bool bothWeapons,
                             const std::uint8_t appearanceSkill) -> bool
{
    if (!bothPresent)
    {
        return false;
    }
    if (bothShields)
    {
        return HasValidStyleShieldCase(true, hasAppearanceItem, canEquipAppearance);
    }
    if (HasValidStyleMarvelousCheer(appearanceIsMarvelousCheer, equippedIsWeapon, equippedSkill, hasAppearanceItem))
    {
        return true;
    }
    return HasValidStyleMatchingSkill(bothWeapons, equippedSkill, appearanceSkill, hasAppearanceItem, canEquipAppearance);
}

// ShouldApplyStyleLockSnapshot mirrors isStyleLocked true path.
constexpr auto ShouldApplyStyleLockSnapshot(const bool isStyleLocked) -> bool
{
    return isStyleLocked;
}

// StyleItemFromEquip mirrors PItem == nullptr ? 0 : getID().
constexpr auto StyleItemFromEquip(const bool hasEquip, const std::uint16_t itemID) -> std::uint16_t
{
    return hasEquip ? itemID : 0;
}

// ShouldNotifyStyleLockChange mirrors getStyleLocked() != isStyleLocked.
constexpr auto ShouldNotifyStyleLockChange(const bool currentlyLocked, const bool isStyleLocked) -> bool
{
    return currentlyLocked != isStyleLocked;
}

// ShouldSkipWeaponStyleUpdate mirrors !getStyleLocked().
constexpr auto ShouldSkipStyleUpdateWhenUnlocked(const bool styleLocked) -> bool
{
    return !styleLocked;
}

// AppearanceModelOrZero mirrors appearance ? getModelId() : 0.
constexpr auto AppearanceModelOrZero(const bool hasAppearance, const std::uint16_t modelID) -> std::uint16_t
{
    return hasAppearance ? modelID : 0;
}

// H2HSubLookModel mirrors appearanceModel + 0x1000.
constexpr auto H2HSubLookModel(const std::uint16_t appearanceModel) -> std::uint16_t
{
    return static_cast<std::uint16_t>(appearanceModel + H2HSubLookOffset);
}

// IsTwoHandedStyleSkill mirrors 2H weapon skills that reset sub look to look.sub.
constexpr auto IsTwoHandedStyleSkill(const std::uint8_t skillType) -> bool
{
    // SKILL_GREAT_SWORD=4, GREAT_AXE=6, SCYTHE=7, POLEARM=8, GREAT_KATANA=10, STAFF=12
    switch (skillType)
    {
        case 4:
        case 6:
        case 7:
        case 8:
        case 10:
        case 12:
            return true;
        default:
            return false;
    }
}

// ArmorStyleAppearanceModel: appearance && HasItem → model else 0.
constexpr auto ArmorStyleAppearanceModel(const bool hasAppearance, const bool stillHasItem, const std::uint16_t modelID) -> std::uint16_t
{
    if (hasAppearance && stillHasItem)
    {
        return modelID;
    }
    return 0;
}

// ShouldApplyArmorStyle mirrors canEquipItemOnAnyJob on appearance.
constexpr auto ShouldApplyArmorStyle(const bool canEquipAppearance) -> bool
{
    return canEquipAppearance;
}

// IsArmorStyleSlot mirrors HEAD..FEET.
constexpr auto IsArmorStyleSlot(const std::uint8_t equipSlotID) -> bool
{
    return equipSlotID >= 4 && equipSlotID <= 8; // SLOT_HEAD..SLOT_FEET
}

} // namespace styleupdatehelpers
