#include "test_style_update_1518.h"

#include "map/armor_style_update_capacity.h"
#include "map/style_update_capacity.h"
#include "map/weapon_style_update_capacity.h"

#include <iostream>

namespace
{
using styleupdatehelpers::ArmorStyleAppearanceModel;
using styleupdatehelpers::AppearanceModelOrZero;
using styleupdatehelpers::CanEquipItemOnAnyJobNullOK;
using styleupdatehelpers::CapQuantityToStack;
using styleupdatehelpers::H2HSubLookModel;
using styleupdatehelpers::HasValidStyle;
using styleupdatehelpers::IsArmorStyleSlot;
using styleupdatehelpers::IsEquippedAmmoConsumption;
using styleupdatehelpers::IsTwoHandedStyleSkill;
using styleupdatehelpers::JobMeetsItemReqs;
using styleupdatehelpers::ShouldApplyArmorStyle;
using styleupdatehelpers::ShouldApplyStyleLockSnapshot;
using styleupdatehelpers::ShouldDeleteItemOnUpdate;
using styleupdatehelpers::ShouldKeepItemOnUpdate;
using styleupdatehelpers::ShouldNotifyStyleLockChange;
using styleupdatehelpers::ShouldRefreshStyleOnDrop;
using styleupdatehelpers::ShouldRejectBusyItemInUse;
using styleupdatehelpers::ShouldRejectBusyNonAmmo;
using styleupdatehelpers::ShouldRejectInvalidQuantity;
using styleupdatehelpers::ShouldRejectNullUpdateItem;
using styleupdatehelpers::ShouldSkipStyleUpdateWhenUnlocked;
using styleupdatehelpers::StyleItemFromEquip;

auto Check() -> bool
{
    if (!ShouldRejectNullUpdateItem(false) || ShouldRejectNullUpdateItem(true))
    {
        return false;
    }
    if (!ShouldRejectInvalidQuantity(5, 0, -6) || ShouldRejectInvalidQuantity(5, 0, -5) || ShouldRejectInvalidQuantity(5, 2, -3))
    {
        return false;
    }
    if (!ShouldRejectBusyItemInUse(true, true, false) || ShouldRejectBusyItemInUse(true, true, true) || ShouldRejectBusyItemInUse(true, false, false))
    {
        return false;
    }
    if (!IsEquippedAmmoConsumption(true, true) || IsEquippedAmmoConsumption(true, false))
    {
        return false;
    }
    if (!ShouldRejectBusyNonAmmo(true, false, false) || ShouldRejectBusyNonAmmo(true, true, false) || ShouldRejectBusyNonAmmo(true, false, true))
    {
        return false;
    }
    if (CapQuantityToStack(10, 5, 12) != 12 || CapQuantityToStack(10, 1, 20) != 11)
    {
        return false;
    }
    if (!ShouldKeepItemOnUpdate(1, false) || !ShouldKeepItemOnUpdate(0, true) || ShouldKeepItemOnUpdate(0, false))
    {
        return false;
    }
    if (!ShouldDeleteItemOnUpdate(0) || ShouldDeleteItemOnUpdate(1))
    {
        return false;
    }
    if (!ShouldRefreshStyleOnDrop(true, false) || ShouldRefreshStyleOnDrop(true, true) || ShouldRefreshStyleOnDrop(false, false))
    {
        return false;
    }
    if (!CanEquipItemOnAnyJobNullOK(true) || CanEquipItemOnAnyJobNullOK(false))
    {
        return false;
    }
    if (!JobMeetsItemReqs(0x2, 2, 50, 75) || JobMeetsItemReqs(0x2, 2, 80, 75) || JobMeetsItemReqs(0x2, 1, 50, 75))
    {
        return false;
    }
    if (!HasValidStyle(true, true, true, true, false, false, 0, false, 0))
    {
        return false;
    }
    if (!HasValidStyle(true, false, true, false, true, true, 42, false, 0))
    {
        return false;
    }
    if (!HasValidStyle(true, false, true, true, false, true, 5, true, 5) || HasValidStyle(true, false, true, true, false, true, 5, true, 6))
    {
        return false;
    }
    if (HasValidStyle(false, true, true, true, false, false, 0, false, 0))
    {
        return false;
    }
    if (!ShouldApplyStyleLockSnapshot(true) || ShouldApplyStyleLockSnapshot(false))
    {
        return false;
    }
    if (StyleItemFromEquip(false, 100) != 0 || StyleItemFromEquip(true, 100) != 100)
    {
        return false;
    }
    if (!ShouldNotifyStyleLockChange(false, true) || ShouldNotifyStyleLockChange(true, true))
    {
        return false;
    }
    if (!ShouldSkipStyleUpdateWhenUnlocked(false) || ShouldSkipStyleUpdateWhenUnlocked(true))
    {
        return false;
    }
    if (AppearanceModelOrZero(false, 5) != 0 || AppearanceModelOrZero(true, 5) != 5)
    {
        return false;
    }
    if (H2HSubLookModel(1) != 0x1001)
    {
        return false;
    }
    if (!IsTwoHandedStyleSkill(4) || !IsTwoHandedStyleSkill(12) || IsTwoHandedStyleSkill(1))
    {
        return false;
    }
    if (ArmorStyleAppearanceModel(true, true, 9) != 9 || ArmorStyleAppearanceModel(true, false, 9) != 0)
    {
        return false;
    }
    if (!ShouldApplyArmorStyle(true) || ShouldApplyArmorStyle(false))
    {
        return false;
    }
    if (!IsArmorStyleSlot(4) || !IsArmorStyleSlot(8) || IsArmorStyleSlot(3))
    {
        return false;
    }
    if (armorstyleupdatehelpers::PlanFor({ .canEquipAppearance = true, .equipSlotID = 4 }).setMainLook ||
        armorstyleupdatehelpers::PlanFor({ .styleLocked = true, .equipSlotID = 4 }).setMainLook ||
        armorstyleupdatehelpers::PlanFor({ .styleLocked = true, .canEquipAppearance = true, .equipSlotID = 3 }).setMainLook)
    {
        return false;
    }
    const auto retainedArmorStyle = armorstyleupdatehelpers::PlanFor({
        .styleLocked        = true,
        .hasAppearance      = true,
        .stillHasAppearance = true,
        .canEquipAppearance = true,
        .modelID            = 0x1234,
        .equipSlotID        = 5,
    });
    const auto missingArmorStyle = armorstyleupdatehelpers::PlanFor({
        .styleLocked        = true,
        .canEquipAppearance = true,
        .modelID            = 0x1234,
        .equipSlotID        = 8,
    });
    if (!retainedArmorStyle.setMainLook || retainedArmorStyle.slot != 5 || retainedArmorStyle.modelID != 0x1234 ||
        !missingArmorStyle.setMainLook || missingArmorStyle.slot != 8 || missingArmorStyle.modelID != 0)
    {
        return false;
    }
    if (weaponstyleupdatehelpers::PlanFor({ .equipSlotID = weaponstyleupdatehelpers::SlotMain }).setMainLook)
    {
        return false;
    }
    const auto h2hWeaponStyle = weaponstyleupdatehelpers::PlanFor({
        .styleLocked          = true,
        .styleValid           = true,
        .incomingIsWeapon     = true,
        .incomingIsHandToHand = true,
        .equipSlotID          = weaponstyleupdatehelpers::SlotMain,
        .appearanceModel      = 0x1234,
        .currentMainModel     = 10,
        .currentSubModel      = 11,
    });
    const auto twoHandedWeaponStyle = weaponstyleupdatehelpers::PlanFor({
        .styleLocked         = true,
        .incomingIsWeapon    = true,
        .incomingIsTwoHanded = true,
        .equipSlotID         = weaponstyleupdatehelpers::SlotMain,
        .appearanceModel     = 0x1234,
        .currentMainModel    = 10,
        .currentSubModel     = 11,
    });
    const auto genericMainWeaponStyle = weaponstyleupdatehelpers::PlanFor({
        .styleLocked      = true,
        .styleValid       = true,
        .incomingIsWeapon = true,
        .equipSlotID      = weaponstyleupdatehelpers::SlotMain,
        .appearanceModel  = 0x1234,
    });
    const auto nilMainWeaponStyle = weaponstyleupdatehelpers::PlanFor({
        .styleLocked      = true,
        .incomingItemNil  = true,
        .equipSlotID      = weaponstyleupdatehelpers::SlotMain,
        .currentMainModel = 10,
        .currentSubModel  = 11,
    });
    const auto subWeaponStyle = weaponstyleupdatehelpers::PlanFor({
        .styleLocked     = true,
        .styleValid      = true,
        .equipSlotID     = weaponstyleupdatehelpers::SlotSub,
        .appearanceModel = 0x3333,
        .currentSubModel = 11,
    });
    const auto rangedWeaponStyle = weaponstyleupdatehelpers::PlanFor({
        .styleLocked        = true,
        .equipSlotID        = weaponstyleupdatehelpers::SlotRanged,
        .currentRangedModel = 12,
    });
    if (!h2hWeaponStyle.setMainLook || h2hWeaponStyle.mainModel != 0x1234 || !h2hWeaponStyle.setSubLook || h2hWeaponStyle.subModel != 0x2234 ||
        !twoHandedWeaponStyle.setMainLook || twoHandedWeaponStyle.mainModel != 10 || !twoHandedWeaponStyle.setSubLook || twoHandedWeaponStyle.subModel != 11 ||
        !genericMainWeaponStyle.setMainLook || genericMainWeaponStyle.mainModel != 0x1234 || genericMainWeaponStyle.setSubLook ||
        !nilMainWeaponStyle.setMainLook || nilMainWeaponStyle.mainModel != 10 || !nilMainWeaponStyle.setSubLook || nilMainWeaponStyle.subModel != 11 ||
        !subWeaponStyle.setSubLook || subWeaponStyle.subModel != 0x3333 ||
        !rangedWeaponStyle.setRangedLook || rangedWeaponStyle.rangedModel != 12)
    {
        return false;
    }
    return true;
}
} // namespace

auto runStyleUpdate1518SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "style update 1518 self-test failed\n";
    }
    return ok;
}
