#include "test_equip_policy_1516.h"

#include "map/equip_item_finalize_capacity.h"
#include "map/equip_item_success_capacity.h"
#include "map/equip_armor_direct_restrictions_capacity.h"
#include "map/equip_armor_ammo_look_capacity.h"
#include "map/equip_armor_main_look_capacity.h"
#include "map/equip_armor_main_attack_timer_capacity.h"
#include "map/equip_armor_main_sub_capacity.h"
#include "map/equip_armor_weapon_slot_state_capacity.h"
#include "map/equip_armor_ranged_compatibility_capacity.h"
#include "map/equip_armor_ranged_look_capacity.h"
#include "map/equip_armor_removed_look_capacity.h"
#include "map/equip_armor_reverse_restrictions_capacity.h"
#include "map/equip_armor_sub_capacity.h"
#include "map/equip_armor_sub_look_capacity.h"
#include "map/equip_armor_target_look_capacity.h"
#include "map/unequip_armor_look_capacity.h"
#include "map/unequip_main_attack_timer_capacity.h"
#include "map/unequip_main_sub_look_capacity.h"
#include "map/unequip_item_recast_capacity.h"
#include "map/unequip_item_unlock_capacity.h"
#include "map/unequip_post_switch_effects_capacity.h"
#include "map/unequip_recalculate_capacity.h"
#include "map/unequip_removed_armor_look_capacity.h"
#include "map/unequip_script_flags_capacity.h"
#include "map/unequip_ranged_look_capacity.h"
#include "map/unequip_sub_look_capacity.h"
#include "map/unequip_sub_state_capacity.h"
#include "map/unequip_weapon_slot_state_capacity.h"
#include "map/unequip_weapon_finalize_capacity.h"
#include "map/equip_policy_capacity.h"
#include "map/lockstyle_removed_look_capacity.h"

#include <array>
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
        .modelID        = 0x1234,
    });
    if (!directRestrictions.applies || directRestrictions.actionCount != 6 ||
        directRestrictions.actions[0] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 0 } ||
        directRestrictions.actions[1] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 4 } ||
        directRestrictions.actions[2] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::SetArmorLook, .slot = 4, .modelID = 0x1234 } ||
        directRestrictions.actions[3] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::Unequip, .slot = 8 } ||
        directRestrictions.actions[4] != equiparmordirecthelpers::Action{ .kind = equiparmordirecthelpers::ActionKind::SetArmorLook, .slot = 8, .modelID = 0x1234 } ||
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
        .modelID          = 0x1234,
    });
    const auto h2hWrapPlan = equiparmormainsubhelpers::PlanFor({ .incomingIsH2H = true, .modelID = 0xf123 });
    if (gripPlan.unequipSub || gripPlan.setH2HSubLook || gripPlan.h2hSubModel != 0 ||
        !h2hPlan.unequipSub || !h2hPlan.setH2HSubLook || h2hPlan.h2hSubModel != 0x2234 ||
        !h2hWrapPlan.setH2HSubLook || h2hWrapPlan.h2hSubModel != 0x0123)
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
    const auto ammoLookPlan = equiparmorammolookhelpers::PlanFor({ .incomingIsWeapon = true, .modelID = 501 });
    const auto noAmmoLookPlan = equiparmorammolookhelpers::PlanFor({ .incomingIsWeapon = true, .hasRangedAfterCompatibility = true, .modelID = 502 });
    const auto nonWeaponAmmoLookPlan = equiparmorammolookhelpers::PlanFor({ .modelID = 503 });
    if (!ammoLookPlan.setRangedLook || ammoLookPlan.modelID != 501 ||
        noAmmoLookPlan.setRangedLook || noAmmoLookPlan.modelID != 0 ||
        nonWeaponAmmoLookPlan.setRangedLook || nonWeaponAmmoLookPlan.modelID != 0)
    {
        return false;
    }
    const auto rangedLookPlan = equiparmorrangedlookhelpers::PlanFor(504);
    if (!rangedLookPlan.setRangedLook || rangedLookPlan.modelID != 504)
    {
        return false;
    }
    const auto subLookPlan = equiparmorsublookhelpers::PlanFor(505);
    if (!subLookPlan.setSubLook || subLookPlan.modelID != 505)
    {
        return false;
    }
    const auto mainLookPlan = equiparmormainlookhelpers::PlanFor(506);
    if (!mainLookPlan.setMainLook || mainLookPlan.modelID != 506)
    {
        return false;
    }
    const auto unequipArmorLook = unequiparmorlookhelpers::PlanFor(4);
    const auto noUnequipArmorLook = unequiparmorlookhelpers::PlanFor(3);
    if (!unequipArmorLook.setArmorLook || unequipArmorLook.slot != 4 || unequipArmorLook.modelID != 0 ||
        noUnequipArmorLook.setArmorLook || noUnequipArmorLook.slot != 0 || noUnequipArmorLook.modelID != 0)
    {
        return false;
    }

    const auto ammoRangedClear = unequiprangedlookhelpers::PlanFor({ .equipSlotID = 3 });
    const auto ammoRangedKeep  = unequiprangedlookhelpers::PlanFor({
        .equipSlotID          = 3,
        .hasRangedAfterClear = true,
    });
    const auto rangedClear   = unequiprangedlookhelpers::PlanFor({ .equipSlotID = 2 });
    const auto unrelatedSlot = unequiprangedlookhelpers::PlanFor({ .equipSlotID = 1 });
    if (!ammoRangedClear.setRangedLook || ammoRangedClear.modelID != 0 ||
        ammoRangedKeep.setRangedLook || ammoRangedKeep.modelID != 0 ||
        !rangedClear.setRangedLook || rangedClear.modelID != 0 ||
        unrelatedSlot.setRangedLook || unrelatedSlot.modelID != 0)
    {
        return false;
    }

    const auto subLookClear = unequipsublookhelpers::PlanFor(1);
    const auto noSubLook    = unequipsublookhelpers::PlanFor(2);
    if (!subLookClear.setSubLook || subLookClear.modelID != 0 ||
        noSubLook.setSubLook || noSubLook.modelID != 0)
    {
        return false;
    }

    const auto h2hMainSubClear = unequipmainsublookhelpers::PlanFor({
        .removedMainIsWeapon     = true,
        .removedMainIsHandToHand = true,
        .hasSubAfterClear        = true,
    });
    const auto emptySubMainClear = unequipmainsublookhelpers::PlanFor({ .removedMainIsWeapon = true });
    const auto retainedSub       = unequipmainsublookhelpers::PlanFor({
        .removedMainIsWeapon = true,
        .hasSubAfterClear    = true,
    });
    const auto nonWeaponMain = unequipmainsublookhelpers::PlanFor({});
    if (!h2hMainSubClear.setSubLook || h2hMainSubClear.modelID != 0 ||
        !emptySubMainClear.setSubLook || emptySubMainClear.modelID != 0 ||
        retainedSub.setSubLook || retainedSub.modelID != 0 ||
        nonWeaponMain.setSubLook || nonWeaponMain.modelID != 0)
    {
        return false;
    }

    const auto subWeaponFinalize = unequipweaponfinalizehelpers::PlanFor({ .equipSlotID = 1 });
    const auto mainWeaponFinalize = unequipweaponfinalizehelpers::PlanFor({ .equipSlotID = 0 });
    const auto rangedWeaponFinalize = unequipweaponfinalizehelpers::PlanFor({ .equipSlotID = 2 });
    const auto stringInstrumentFinalize = unequipweaponfinalizehelpers::PlanFor({
        .equipSlotID                        = 2,
        .removedRangedIsStringInstrument = true,
    });
    const auto windInstrumentFinalize = unequipweaponfinalizehelpers::PlanFor({
        .equipSlotID                      = 2,
        .removedRangedIsWindInstrument = true,
    });
    const auto ammoFinalize = unequipweaponfinalizehelpers::PlanFor({ .equipSlotID = 3 });
    if (!subWeaponFinalize.clearTP || !subWeaponFinalize.clearAftermath || !subWeaponFinalize.buildWeaponSkills ||
        !mainWeaponFinalize.clearTP || !mainWeaponFinalize.clearAftermath || !mainWeaponFinalize.buildWeaponSkills ||
        !rangedWeaponFinalize.clearTP || !rangedWeaponFinalize.clearAftermath || !rangedWeaponFinalize.buildWeaponSkills ||
        stringInstrumentFinalize.clearTP || stringInstrumentFinalize.clearAftermath || !stringInstrumentFinalize.buildWeaponSkills ||
        windInstrumentFinalize.clearTP || windInstrumentFinalize.clearAftermath || !windInstrumentFinalize.buildWeaponSkills ||
        ammoFinalize.clearTP || ammoFinalize.clearAftermath || ammoFinalize.buildWeaponSkills)
    {
        return false;
    }

    const auto emptyMainSubState = unequipsubstatehelpers::PlanFor({ .equipSlotID = 1 });
    const auto nonEquipmentMainSubState = unequipsubstatehelpers::PlanFor({
        .equipSlotID       = 1,
        .hasMainAfterClear = true,
    });
    const auto equipmentMainSubState = unequipsubstatehelpers::PlanFor({
        .equipSlotID                = 1,
        .hasMainAfterClear          = true,
        .mainAfterClearIsEquipment = true,
    });
    const auto otherSlotSubState = unequipsubstatehelpers::PlanFor({ .equipSlotID = 0 });
    if (!emptyMainSubState.checkUnarmedWeapon || !emptyMainSubState.clearDualWield ||
        !nonEquipmentMainSubState.checkUnarmedWeapon || !nonEquipmentMainSubState.clearDualWield ||
        equipmentMainSubState.checkUnarmedWeapon || !equipmentMainSubState.clearDualWield ||
        otherSlotSubState.checkUnarmedWeapon || otherSlotSubState.clearDualWield)
    {
        return false;
    }

    const auto resetMainAttackTimer = unequipmainattacktimerhelpers::PlanFor({
        .equipSlotID         = 0,
        .isEngaged           = true,
        .currentStateIsAttack = true,
    });
    const auto disengagedMainAttackTimer = unequipmainattacktimerhelpers::PlanFor({
        .equipSlotID         = 0,
        .currentStateIsAttack = true,
    });
    const auto nonAttackMainTimer = unequipmainattacktimerhelpers::PlanFor({
        .equipSlotID = 0,
        .isEngaged   = true,
    });
    const auto subAttackTimer = unequipmainattacktimerhelpers::PlanFor({
        .equipSlotID         = 1,
        .isEngaged           = true,
        .currentStateIsAttack = true,
    });
    if (!resetMainAttackTimer.resetAttackTimer || disengagedMainAttackTimer.resetAttackTimer ||
        nonAttackMainTimer.resetAttackTimer || subAttackTimer.resetAttackTimer)
    {
        return false;
    }

    const auto resetEquipMainAttackTimer = equiparmormainattacktimerhelpers::PlanFor({
        .isEngaged           = true,
        .currentStateIsAttack = true,
    });
    const auto disengagedEquipMainAttackTimer = equiparmormainattacktimerhelpers::PlanFor({
        .currentStateIsAttack = true,
    });
    const auto nonAttackEquipMainTimer = equiparmormainattacktimerhelpers::PlanFor({
        .isEngaged = true,
    });
    if (!resetEquipMainAttackTimer.resetAttackTimer || disengagedEquipMainAttackTimer.resetAttackTimer ||
        nonAttackEquipMainTimer.resetAttackTimer)
    {
        return false;
    }

    const auto mainWeaponState = equiparmorweaponslotstatehelpers::PlanFor(0, true);
    const auto rangedWeaponState = equiparmorweaponslotstatehelpers::PlanFor(2, true);
    const auto ammoWeaponState = equiparmorweaponslotstatehelpers::PlanFor(3, true);
    const auto noSubWeaponState = equiparmorweaponslotstatehelpers::PlanFor(1, true);
    const auto noWeaponState = equiparmorweaponslotstatehelpers::PlanFor(2, false);
    if (!mainWeaponState.setWeapon || mainWeaponState.slot != 0 ||
        !rangedWeaponState.setWeapon || rangedWeaponState.slot != 2 ||
        !ammoWeaponState.setWeapon || ammoWeaponState.slot != 3 ||
        noSubWeaponState.setWeapon || noWeaponState.setWeapon)
    {
        return false;
    }

    const auto unarmedSubWeapon = unequipweaponslotstatehelpers::PlanFor(1);
    const auto clearAmmoWeapon = unequipweaponslotstatehelpers::PlanFor(3);
    const auto clearRangedWeapon = unequipweaponslotstatehelpers::PlanFor(2);
    const auto unchangedMainWeapon = unequipweaponslotstatehelpers::PlanFor(0);
    if (unarmedSubWeapon.slot != 1 || unarmedSubWeapon.action != unequipweaponslotstatehelpers::Action::SetUnarmed ||
        clearAmmoWeapon.slot != 3 || clearAmmoWeapon.action != unequipweaponslotstatehelpers::Action::Clear ||
        clearRangedWeapon.slot != 2 || clearRangedWeapon.action != unequipweaponslotstatehelpers::Action::Clear ||
        unchangedMainWeapon.action != unequipweaponslotstatehelpers::Action::None)
    {
        return false;
    }

    const auto chargedItemRecast = unequipitemrecasthelpers::PlanFor({
        .itemIsCharged  = true,
        .itemSlotID     = 0x34,
        .itemLocationID = 0x12,
    });
    const auto normalItemRecast = unequipitemrecasthelpers::PlanFor({
        .itemSlotID     = 0x34,
        .itemLocationID = 0x12,
    });
    if (!chargedItemRecast.removeItemRecast || chargedItemRecast.recastKey != 0x3412 ||
        normalItemRecast.removeItemRecast || normalItemRecast.recastKey != 0)
    {
        return false;
    }

    const auto itemUnlock = unequipitemunlockhelpers::PlanFor();
    if (!itemUnlock.setItemSubtype || itemUnlock.subtype != 0xFE)
    {
        return false;
    }

    const auto noRecalculate = unequiprecalculatehelpers::PlanFor(false);
    const auto recalculate = unequiprecalculatehelpers::PlanFor(true);
    if (noRecalculate.buildSkills || noRecalculate.updateHealth || noRecalculate.markUpdateHP || noRecalculate.markUpdateLook ||
        !recalculate.buildSkills || !recalculate.updateHealth || !recalculate.markUpdateHP || !recalculate.markUpdateLook)
    {
        return false;
    }

    const auto postSwitchEffects = unequippostswitcheffectshelpers::PlanFor(2);
    if (!postSwitchEffects.callItemUnequipScript || !postSwitchEffects.queueEquipChange ||
        postSwitchEffects.locationID != 0 || postSwitchEffects.itemSlotID != 0 || postSwitchEffects.equipSlotID != 2)
    {
        return false;
    }

    constexpr std::array<uint16_t, 3> remainingScriptTypes{ 0x0002, 0x0004, 0x0010 };
    const auto recomputeScriptFlags = unequipscriptflagshelpers::PlanFor(0x0001, remainingScriptTypes);
    const auto noRecomputeScriptFlags = unequipscriptflagshelpers::PlanFor(0x0002, remainingScriptTypes);
    if (!recomputeScriptFlags.recomputeEquipFlag || recomputeScriptFlags.equipFlag != 0x0016 ||
        noRecomputeScriptFlags.recomputeEquipFlag || noRecomputeScriptFlags.equipFlag != 0)
    {
        return false;
    }

    std::array<unequipremovedarmorlookhelpers::EquippedModel, 16> equippedModels{};
    equippedModels[4] = { .present = true, .modelID = 0x1001 };
    equippedModels[6] = { .present = true, .modelID = 0x1003 };
    const auto removedArmorLook = unequipremovedarmorlookhelpers::PlansFor((1u << 4) | (1u << 6) | (1u << 9), 1u << 5, equippedModels);
    const auto fallbackArmorLook = unequipremovedarmorlookhelpers::PlansFor(0, 1u << 5, equippedModels);
    if (removedArmorLook.size() != 2 || removedArmorLook[0].slot != 4 || removedArmorLook[0].modelID != 0x1001 ||
        removedArmorLook[1].slot != 6 || removedArmorLook[1].modelID != 0x1003 ||
        fallbackArmorLook.size() != 1 || fallbackArmorLook[0].slot != 5 || fallbackArmorLook[0].modelID != 0)
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
