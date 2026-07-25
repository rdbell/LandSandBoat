#include "test_equip_policy_1516.h"

#include "map/equip_item_finalize_capacity.h"
#include "map/equip_item_success_capacity.h"
#include "map/equip_armor_direct_restrictions_capacity.h"
#include "map/equip_armor_ammo_look_capacity.h"
#include "map/equip_armor_main_sub_capacity.h"
#include "map/equip_armor_ranged_compatibility_capacity.h"
#include "map/equip_armor_removed_look_capacity.h"
#include "map/equip_armor_reverse_restrictions_capacity.h"
#include "map/equip_armor_sub_capacity.h"
#include "map/equip_armor_target_look_capacity.h"
#include "map/equip_policy_capacity.h"
#include "map/lockstyle_removed_look_capacity.h"

#include <iostream>

namespace
{
using equippolicyhelpers::CanUseWeaponSkillByLevel;
using equippolicyhelpers::HasSlotEquipped;
using equippolicyhelpers::IsAlreadyEquippedInSlot;
using equippolicyhelpers::IsArmorLookSlot;
using equippolicyhelpers::IsEquipArmorEligible;
using equippolicyhelpers::IsEquipSlotBlocked;
using equippolicyhelpers::IsEquipSlotIDValid;
using equippolicyhelpers::IsJobAllowedForItem;
using equippolicyhelpers::IsReqLevelOK;
using equippolicyhelpers::IsSuperiorLevelOK;
using equippolicyhelpers::IsUnequipRequest;
using equippolicyhelpers::PairedSlotForDuplicateCheck;
using equippolicyhelpers::PreferRemoveSlotLookID;
using equippolicyhelpers::RemoveSubShouldUnequip;
using equippolicyhelpers::ShouldBlockOffhandDualWield;
using equippolicyhelpers::ShouldBlockRangedEquipDuringRA;
using equippolicyhelpers::ShouldBlockSubWithH2HMain;
using equippolicyhelpers::ShouldClearTPOnWeaponEquip;
using equippolicyhelpers::ShouldRejectNonPCTrait;
using equippolicyhelpers::ShouldRejectNullChar;
using equippolicyhelpers::ShouldRejectNullCharOrStorage;
using equippolicyhelpers::ShouldRemoveSubOnMainEquip;
using equippolicyhelpers::ShouldRemoveSubOnMainUnequip;
using equippolicyhelpers::ShouldRequire2HForGrip;
using equippolicyhelpers::ShouldRequireDualWield;
using equippolicyhelpers::ShouldSkipCrossSlotSameItem;
using equippolicyhelpers::SlotAmmo;
using equippolicyhelpers::SlotEar1;
using equippolicyhelpers::SlotEar2;
using equippolicyhelpers::SlotHead;
using equippolicyhelpers::SlotMain;
using equippolicyhelpers::SlotRanged;
using equippolicyhelpers::SlotSub;

auto Check() -> bool
{
    if (!IsEquipSlotIDValid(0) || !IsEquipSlotIDValid(15) || IsEquipSlotIDValid(16))
    {
        return false;
    }
    if (!HasSlotEquipped(true, true) || HasSlotEquipped(true, false) || HasSlotEquipped(false, true))
    {
        return false;
    }
    if (!ShouldRejectNullChar(true) || ShouldRejectNullChar(false))
    {
        return false;
    }
    if (!ShouldRejectNullCharOrStorage(true, false) || !ShouldRejectNullCharOrStorage(false, true) || ShouldRejectNullCharOrStorage(false, false))
    {
        return false;
    }
    if (!IsAlreadyEquippedInSlot(true, true) || IsAlreadyEquippedInSlot(true, false))
    {
        return false;
    }
    if (!IsUnequipRequest(0) || IsUnequipRequest(1))
    {
        return false;
    }
    if (PairedSlotForDuplicateCheck(SlotMain) != SlotSub || PairedSlotForDuplicateCheck(SlotEar1) != SlotEar2 ||
        PairedSlotForDuplicateCheck(SlotRanged) != 0xFF)
    {
        return false;
    }
    if (!ShouldSkipCrossSlotSameItem(1, SlotMain, true) || ShouldSkipCrossSlotSameItem(0, SlotMain, true) ||
        ShouldSkipCrossSlotSameItem(1, SlotMain, false) || ShouldSkipCrossSlotSameItem(1, SlotRanged, true))
    {
        return false;
    }
    if (!ShouldBlockRangedEquipDuringRA(SlotRanged, true, true) || !ShouldBlockRangedEquipDuringRA(SlotAmmo, false, true) ||
        ShouldBlockRangedEquipDuringRA(SlotAmmo, true, true) || ShouldBlockRangedEquipDuringRA(SlotRanged, true, false))
    {
        return false;
    }
    if (!ShouldRequire2HForGrip(true, false, true, 0, false) || ShouldRequire2HForGrip(true, false, true, 0, true) ||
        ShouldRequire2HForGrip(true, true, true, 0, false))
    {
        return false;
    }
    if (!ShouldRequireDualWield(true, false, true, 2, false) || ShouldRequireDualWield(true, false, true, 2, true) ||
        ShouldRequireDualWield(true, false, true, 0, false))
    {
        return false;
    }
    if (!ShouldBlockOffhandDualWield(true, false, true, 2, false, false) || !ShouldBlockOffhandDualWield(true, false, true, 2, true, true) ||
        ShouldBlockOffhandDualWield(true, false, true, 2, true, false))
    {
        return false;
    }
    if (!ShouldBlockSubWithH2HMain(true, false, true) || ShouldBlockSubWithH2HMain(true, true, true) || ShouldBlockSubWithH2HMain(true, false, false))
    {
        return false;
    }
    if (!ShouldRemoveSubOnMainUnequip(0, true, false) || ShouldRemoveSubOnMainUnequip(0, true, true) || ShouldRemoveSubOnMainUnequip(1, true, false))
    {
        return false;
    }
    if (!IsEquipSlotBlocked(0x4, 2) || IsEquipSlotBlocked(0x4, 1))
    {
        return false;
    }
    if (!IsJobAllowedForItem(0x2, 2) || IsJobAllowedForItem(0x2, 1) || IsJobAllowedForItem(0x2, 0))
    {
        return false;
    }
    if (!IsSuperiorLevelOK(1, 1) || IsSuperiorLevelOK(2, 1) || IsSuperiorLevelOK(1, -1))
    {
        return false;
    }
    if (!IsReqLevelOK(50, 50) || IsReqLevelOK(51, 50))
    {
        return false;
    }
    if (!IsEquipArmorEligible(false, true, true, true, true) || IsEquipArmorEligible(true, true, true, true, true) ||
        IsEquipArmorEligible(false, false, true, true, true))
    {
        return false;
    }
    if (!ShouldRemoveSubOnMainEquip(false, true, true, true, true, true, false) ||
        ShouldRemoveSubOnMainEquip(true, true, true, true, true, true, false) || ShouldRemoveSubOnMainEquip(false, true, true, true, true, true, true))
    {
        return false;
    }
    if (!CanUseWeaponSkillByLevel(100, 100) || CanUseWeaponSkillByLevel(99, 100))
    {
        return false;
    }
    if (!ShouldRejectNonPCTrait(false) || ShouldRejectNonPCTrait(true))
    {
        return false;
    }
    if (PreferRemoveSlotLookID(0x10, 0x20) != 0x10 || PreferRemoveSlotLookID(0, 0x20) != 0x20)
    {
        return false;
    }
    if (!IsArmorLookSlot(4) || !IsArmorLookSlot(8) || IsArmorLookSlot(3) || IsArmorLookSlot(9))
    {
        return false;
    }
    if (!RemoveSubShouldUnequip(true, true) || RemoveSubShouldUnequip(true, false))
    {
        return false;
    }
    if (!ShouldClearTPOnWeaponEquip(SlotMain, false, false, 0, 41, 42) || !ShouldClearTPOnWeaponEquip(SlotMain, true, true, 1, 41, 42) ||
        ShouldClearTPOnWeaponEquip(SlotMain, true, true, 41, 41, 42) || ShouldClearTPOnWeaponEquip(SlotHead, true, true, 1, 41, 42))
    {
        return false;
    }

    const auto noWeaponChange = equipitemfinalizehelpers::PlanFor({});
    if (noWeaponChange.clearTP || noWeaponChange.clearAftermath || noWeaponChange.checkUnarmedWeapon || noWeaponChange.buildWeaponSkills ||
        !noWeaponChange.buildSkills || !noWeaponChange.updateHealth || !noWeaponChange.markUpdateHP || !noWeaponChange.markUpdateLook)
    {
        return false;
    }

    const auto weaponChange = equipitemfinalizehelpers::PlanFor({ .clearTP = true, .mainNeedsUnarmed = true });
    if (!weaponChange.clearTP || !weaponChange.clearAftermath || !weaponChange.checkUnarmedWeapon || !weaponChange.buildWeaponSkills)
    {
        return false;
    }

    const auto instrument = equipitemfinalizehelpers::PlanFor({ .mainNeedsUnarmed = true });
    if (instrument.checkUnarmedWeapon || instrument.buildWeaponSkills)
    {
        return false;
    }

    const auto equipSuccess = equipitemsuccesshelpers::PlanFor({});
    if (equipSuccess.setScriptEquipFlag || equipSuccess.assignChargeTime || equipSuccess.addItemRecast || equipSuccess.pushItemAttr ||
        equipSuccess.checkUnarmedWeapon || !equipSuccess.lockItem || !equipSuccess.addEquipModifiers || !equipSuccess.addLatentEffects ||
        !equipSuccess.checkLatentsEquip || !equipSuccess.addPetModifiers || !equipSuccess.onItemEquip || !equipSuccess.queueEquipChange)
    {
        return false;
    }

    const auto conditionalEquipSuccess = equipitemsuccesshelpers::PlanFor({
        .hasEquipScript   = true,
        .hasUsableCharges = true,
        .isSubSlot        = true,
        .mainNeedsUnarmed = true,
    });
    if (!conditionalEquipSuccess.setScriptEquipFlag || !conditionalEquipSuccess.assignChargeTime || !conditionalEquipSuccess.addItemRecast ||
        !conditionalEquipSuccess.pushItemAttr || !conditionalEquipSuccess.checkUnarmedWeapon)
    {
        return false;
    }

    const auto noDirectRestrictions = equiparmordirecthelpers::PlanFor({
        .equipSlotID    = 5,
        .itemEquipSlots = static_cast<std::uint16_t>(1u << 4),
        .removeSlots    = static_cast<std::uint16_t>(1u << 4),
    });
    if (noDirectRestrictions.applies || noDirectRestrictions.actionCount != 0)
    {
        return false;
    }

    const auto directRestrictions = equiparmordirecthelpers::PlanFor({
        .equipSlotID    = 5,
        .itemEquipSlots = static_cast<std::uint16_t>(1u << 5),
        .removeSlots    = static_cast<std::uint16_t>((1u << 0) | (1u << 4) | (1u << 8) | (1u << 15)),
    });
    if (!directRestrictions.applies || directRestrictions.actionCount != 6 ||
        directRestrictions.actions[0] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 0 } ||
        directRestrictions.actions[1] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 4 } ||
        directRestrictions.actions[2] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::SetArmorLook, .slot = 4 } ||
        directRestrictions.actions[3] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 8 } ||
        directRestrictions.actions[4] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::SetArmorLook, .slot = 8 } ||
        directRestrictions.actions[5] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 15 })
    {
        return false;
    }

    if (equiparmorreversehelpers::ShouldUnequip({
            .isEquipment        = true,
            .removeSlots        = static_cast<std::uint16_t>(1u << 4),
            .incomingEquipSlots = static_cast<std::uint16_t>(1u << 5),
        }) ||
        equiparmorreversehelpers::ShouldUnequip({
            .isEquipment        = false,
            .removeSlots        = static_cast<std::uint16_t>(1u << 5),
            .incomingEquipSlots = static_cast<std::uint16_t>(1u << 5),
        }) ||
        !equiparmorreversehelpers::ShouldUnequip({
            .isEquipment        = true,
            .removeSlots        = static_cast<std::uint16_t>((1u << 1) | (1u << 5)),
            .incomingEquipSlots = static_cast<std::uint16_t>(1u << 5),
        }))
    {
        return false;
    }

    const auto gripPlan = equiparmormainsubhelpers::PlanFor({
        .hasSubEquipment = true,
        .subIsWeapon      = true,
        .subSkillNone     = true,
    });
    const auto h2hPlan = equiparmormainsubhelpers::PlanFor({
        .incomingIsH2H   = true,
        .hasSubEquipment = true,
        .subIsWeapon      = true,
        .subSkillNone     = true,
    });
    if (gripPlan.unequipSub || gripPlan.setH2HSubLook || !h2hPlan.unequipSub || !h2hPlan.setH2HSubLook)
    {
        return false;
    }

    const auto h2hShieldPlan = equiparmorsubhelpers::PlanFor({ .mainKind = equiparmorsubhelpers::MainKind::HandToHand });
    const auto oneHandedWeaponPlan = equiparmorsubhelpers::PlanFor({
        .mainKind         = equiparmorsubhelpers::MainKind::OneHanded,
        .incomingIsWeapon = true,
    });
    const auto otherGripPlan = equiparmorsubhelpers::PlanFor({
        .mainKind         = equiparmorsubhelpers::MainKind::Other,
        .incomingIsWeapon = true,
    });
    if (!h2hShieldPlan.unequipMain || !oneHandedWeaponPlan.setSubWeapon || !oneHandedWeaponPlan.setDualWield ||
        otherGripPlan.unequipMain || otherGripPlan.setSubWeapon || otherGripPlan.setDualWield)
    {
        return false;
    }

    if (equiparmorrangedhelpers::ShouldUnequipOther({
            .incomingIsWeapon = true,
            .otherIsWeapon    = true,
            .incomingSkill    = 25,
            .incomingSubSkill = 1,
            .otherSkill       = 25,
            .otherSubSkill    = 2,
        }) ||
        !equiparmorrangedhelpers::ShouldUnequipOther({
            .incomingIsWeapon = true,
            .otherIsWeapon    = true,
            .incomingSkill    = 26,
            .incomingSubSkill = 1,
            .otherSkill       = 26,
            .otherSubSkill    = 2,
        }) ||
        !equiparmorrangedhelpers::ShouldUnequipOther({
            .incomingIsWeapon = true,
            .otherIsWeapon    = true,
            .incomingSkill    = 25,
            .otherSkill       = 26,
        }))
    {
        return false;
    }

    if (!equiparmorammolookhelpers::ShouldSetRangedLook({ .incomingIsWeapon = true }) ||
        equiparmorammolookhelpers::ShouldSetRangedLook({ .incomingIsWeapon = true, .hasRangedAfterCompatibility = true }) ||
        equiparmorammolookhelpers::ShouldSetRangedLook({}))
    {
        return false;
    }

    if (!equiparmortargetlookhelpers::ShouldSetArmorLook(4) || !equiparmortargetlookhelpers::ShouldSetArmorLook(8) ||
        equiparmortargetlookhelpers::ShouldSetArmorLook(3) || equiparmortargetlookhelpers::ShouldSetArmorLook(9))
    {
        return false;
    }
    const auto armorTargetLook = equiparmortargetlookhelpers::PlanFor(4, 501);
    const auto nonArmorTargetLook = equiparmortargetlookhelpers::PlanFor(3, 502);
    if (!armorTargetLook.setArmorLook || armorTargetLook.slot != 4 || armorTargetLook.modelID != 501 ||
        nonArmorTargetLook.setArmorLook || nonArmorTargetLook.slot != 0 || nonArmorTargetLook.modelID != 0)
    {
        return false;
    }

    constexpr std::uint32_t removedLookMask = (1u << 4) | (1u << 6) | (1u << 8);
    if (!equiparmorremovedlookhelpers::IsSourceSlot(4) || !equiparmorremovedlookhelpers::IsSourceSlot(7) ||
        equiparmorremovedlookhelpers::IsSourceSlot(3) || equiparmorremovedlookhelpers::IsSourceSlot(8) ||
        !equiparmorremovedlookhelpers::ShouldSetTargetLook(removedLookMask, 4) ||
        !equiparmorremovedlookhelpers::ShouldSetTargetLook(removedLookMask, 6) ||
        !equiparmorremovedlookhelpers::ShouldSetTargetLook(removedLookMask, 8) ||
        equiparmorremovedlookhelpers::ShouldSetTargetLook(removedLookMask, 5) ||
        equiparmorremovedlookhelpers::ShouldSetTargetLook(removedLookMask, 9))
    {
        return false;
    }

    const auto removedLookPlan = equiparmorremovedlookhelpers::PlanFor({{
        { .itemPresent = true, .itemIsEquipment = true, .modelID = 501, .removeSlotLookID = (1u << 4) | (1u << 6) },
        { .itemPresent = true, .itemIsEquipment = false, .modelID = 502, .removeSlotLookID = 1u << 5 },
        { .itemPresent = true, .itemIsEquipment = true, .modelID = 503 },
        { .itemPresent = true, .itemIsEquipment = true, .modelID = 504, .removeSlotLookID = (1u << 5) | (1u << 8) },
    }});
    if (removedLookPlan.actionCount != 4 ||
        removedLookPlan.actions[0] != equiparmorremovedlookhelpers::Action{ .sourceSlot = 4, .targetSlot = 4, .modelID = 501 } ||
        removedLookPlan.actions[1] != equiparmorremovedlookhelpers::Action{ .sourceSlot = 4, .targetSlot = 6, .modelID = 501 } ||
        removedLookPlan.actions[2] != equiparmorremovedlookhelpers::Action{ .sourceSlot = 7, .targetSlot = 5, .modelID = 504 } ||
        removedLookPlan.actions[3] != equiparmorremovedlookhelpers::Action{ .sourceSlot = 7, .targetSlot = 8, .modelID = 504 })
    {
        return false;
    }

    const auto preferredLockstyleLook = lockstyleremovedlookhelpers::PlanFor({
        .styleItemID      = 100,
        .itemFound        = true,
        .modelID          = 501,
        .removeSlotLookID = 1u << 4,
        .removeSlotID     = 1u << 8,
    });
    const auto fallbackLockstyleLook = lockstyleremovedlookhelpers::PlanFor({
        .styleItemID  = 100,
        .itemFound    = true,
        .modelID      = 502,
        .removeSlotID = 1u << 8,
    });
    if (lockstyleremovedlookhelpers::PlanFor({}).applies ||
        lockstyleremovedlookhelpers::PlanFor({ .styleItemID = 100 }).applies ||
        lockstyleremovedlookhelpers::PlanFor({ .styleItemID = 100, .itemFound = true }).applies ||
        !preferredLockstyleLook.applies || preferredLockstyleLook.modelID != 501 || preferredLockstyleLook.effectiveRemoveID != (1u << 4) ||
        !fallbackLockstyleLook.applies || fallbackLockstyleLook.modelID != 502 || fallbackLockstyleLook.effectiveRemoveID != (1u << 8))
    {
        return false;
    }
    return true;
}
} // namespace

auto runEquipPolicy1516SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "equip policy 1516 self-test failed\n";
    }
    return ok;
}
