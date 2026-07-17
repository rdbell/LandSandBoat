#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>

// Pure CAttackRound multi-hit / kick / daken / follow-up preference and clamp policy.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1374 / 1375: multi-hit + kick / daken / follow-up pure policy suite
//   - 2742: PlanInitialWeaponAttacks
//   - 2756: ResolveCreateAttacksPlan
//   - 2757: ResolveCreateKickAttacksPlan
//   - 2758: ResolveCreateDakenAttackPlan
//   - 2768: post-build plan helpers
//   - 3045: ShouldCreateDakenAttack residual dual-wire suite (isPC identity)
//   - 3184: ShouldCreateDakenAttack prior dedicated dual-wire
//           (create_daken_attack.go; expand residual 3045 / pure 1375)
//   - 3251: ShouldCreateDakenAttack dedicated dual-wire expand residual 3045
//           (prior dedicated 3184)
//
// Production host: CAttackRound::CreateDakenAttack (attackround.cpp ~541)
// injects (m_attacker->objtype == TYPE_PC) into ShouldCreateDakenAttack before
// shuriken ammo lookup and DAKEN roll.
// Go dual-wire: attackround.ShouldCreateDakenAttack
// (internal/attackround/create_daken_attack.go).

namespace attackroundhelpers
{

// InitialWeaponAttackPlan is the constructor's initial main/off-hand weapon
// creation policy. mainAttackCalls represents calls to CreateAttacks (not
// produced swings, which depend on the weapon and multi-hit hosts).
struct InitialWeaponAttackPlan
{
    uint8 mainAttackCalls{};
    bool  mainUsesLeftDirection{};
    bool  createSubAttack{};
};

// PlanInitialWeaponAttacks mirrors CAttackRound's initial weapon setup. H2H
// invokes CreateAttacks once or twice with LEFTATTACK; AddAttackSwing owns the
// second-swing right-hand flip. A sub attack is independent of a missing main
// weapon and requires dual wielding.
inline auto PlanInitialWeaponAttacks(
    const bool hasMainWeapon,
    const bool isH2H,
    const bool h2hSingleSwing,
    const bool hasSubWeapon,
    const bool isDualWielding) -> InitialWeaponAttackPlan
{
    InitialWeaponAttackPlan plan{};
    if (hasMainWeapon)
    {
        plan.mainUsesLeftDirection = isH2H;
        plan.mainAttackCalls       = isH2H ? static_cast<uint8>(h2hSingleSwing ? 1 : 2) : static_cast<uint8>(1);
    }
    plan.createSubAttack = hasSubWeapon && isDualWielding;
    return plan;
}

// MAX_SWINGS hard cap used with Occasionally Attacks X Times gear.
constexpr uint8 MaxSwingsHardCap = 8;

// ClampMaxSwings mirrors std::min(mod, 8).
inline auto ClampMaxSwings(const uint8 maxSwingsMod) -> uint8
{
    return std::min(maxSwingsMod, MaxSwingsHardCap);
}

// ShouldUseWeaponHitCount mirrors weapon req level <= attacker level.
inline auto ShouldUseWeaponHitCount(const uint8 weaponReqLevel, const uint8 attackerLevel) -> bool
{
    return weaponReqLevel <= attackerLevel;
}

// ShouldApplyMaxSwingsMod mirrors isPC && MAX_SWINGS mod nonzero.
inline auto ShouldApplyMaxSwingsMod(const bool isPC, const uint8 maxSwingsMod) -> bool
{
    return isPC && maxSwingsMod != 0;
}

// ShouldApplyMobMultiHit mirrors multiHitMax > 0 for mob entities.
inline auto ShouldApplyMobMultiHit(const uint8 multiHitMax) -> bool
{
    return multiHitMax > 0;
}

// MobMultiHitSwingCount mirrors 1 + getHitCount(multiHitMax) host result.
// Host injects the rolled hit count from battleutils::getHitCount.
inline auto MobMultiHitSwingCount(const uint8 rolledHitCount) -> uint8
{
    return static_cast<uint8>(1 + rolledHitCount);
}

// ClampAttackRate mirrors std::clamp(rate, 0, 100) for DA/TA/QA.
inline auto ClampAttackRate(const int16 rate) -> int16
{
    if (rate < 0)
    {
        return 0;
    }
    if (rate > 100)
    {
        return 100;
    }
    return rate;
}

// IsMainHandForMultiHit mirrors (IsH2H && !swingsEmpty) || direction == RIGHTATTACK.
// Host injects isH2H, swingsEmpty, isRightAttack.
inline auto IsMainHandForMultiHit(const bool isH2H, const bool swingsEmpty, const bool isRightAttack) -> bool
{
    return (isH2H && !swingsEmpty) || isRightAttack;
}

// ShouldAddMikageSwings mirrors Mikage status && weapon is main-hand weapon.
inline auto ShouldAddMikageSwings(const bool hasMikage, const bool weaponIsMainHandWeapon) -> bool
{
    return hasMikage && weaponIsMainHandWeapon;
}

// Multi-hit preference outcome for the exclusive DA/TA/QA/mythic path.
// Host rolls 0-99 for each chance check; preference is evaluated top-down.
enum class MultiHitPreference : uint8
{
    None            = 0,
    Quad            = 1,
    Triple          = 2,
    Double          = 3,
    MythicThrice    = 4, // +2 swings NORMAL
    MythicTwice     = 5, // +1 swing NORMAL
    OccasionalExtra = 6, // num-1 NORMAL
};

// ResolveExclusiveMultiHitPreference mirrors Mikage-excluded exclusive ladder:
// Quad > Triple > Double > Mythic thrice > Mythic twice > occasional num>1.
// rolled values are 0..99 style (true if rolled < rate).
inline auto ResolveExclusiveMultiHitPreference(
    const bool quadProcs,
    const bool tripleProcs,
    const bool doubleProcs,
    const bool isMainHand,
    const bool mythicThriceProcs,
    const bool mythicTwiceProcs,
    const uint8 weaponSwingCount) -> MultiHitPreference
{
    if (quadProcs)
    {
        return MultiHitPreference::Quad;
    }
    if (tripleProcs)
    {
        return MultiHitPreference::Triple;
    }
    if (doubleProcs)
    {
        return MultiHitPreference::Double;
    }
    if (isMainHand && mythicThriceProcs)
    {
        return MultiHitPreference::MythicThrice;
    }
    if (isMainHand && mythicTwiceProcs)
    {
        return MultiHitPreference::MythicTwice;
    }
    if (weaponSwingCount > 1)
    {
        return MultiHitPreference::OccasionalExtra;
    }
    return MultiHitPreference::None;
}

// ExclusiveMultiHitSwingCount returns swings to AddAttackSwing for preference
// (excluding Mikage which uses shadow count). OccasionalExtra is num-1.
inline auto ExclusiveMultiHitSwingCount(const MultiHitPreference pref, const uint8 weaponSwingCount) -> uint8
{
    switch (pref)
    {
        case MultiHitPreference::Quad:
            return 4;
        case MultiHitPreference::Triple:
            return 3;
        case MultiHitPreference::Double:
            return 2;
        case MultiHitPreference::MythicThrice:
            return 2;
        case MultiHitPreference::MythicTwice:
            return 1;
        case MultiHitPreference::OccasionalExtra:
            return static_cast<uint8>(weaponSwingCount - 1);
        default:
            return 0;
    }
}

// MultiHitOccurred mirrors multiHitOccurred for QA/TA/DA only (not mythic/occasional).
inline auto MultiHitOccurred(const MultiHitPreference pref) -> bool
{
    return pref == MultiHitPreference::Quad || pref == MultiHitPreference::Triple || pref == MultiHitPreference::Double;
}

// ShouldAddDefaultHit mirrors multiHitOccurred == false.
inline auto ShouldAddDefaultHit(const bool multiHitOccurred) -> bool
{
    return !multiHitOccurred;
}

// ShouldAddAdditionalSwing mirrors isPC && rolled < ADDITIONAL_SWING_CHANCE.
inline auto ShouldAddAdditionalSwing(const bool isPC, const bool additionalSwingProcs) -> bool
{
    return isPC && additionalSwingProcs;
}

// ShouldAddOffhandExtraDualWield mirrors !isMainHand for EXTRA_DUAL_WIELD_ATTACK.
inline auto ShouldAddOffhandExtraDualWield(const bool isMainHand) -> bool
{
    return !isMainHand;
}

// AmbushRotationWindow mirrors abs(rotation diff) < 23 for Ambush augment.
inline auto AmbushRotationInWindow(const int16 rotationDiffAbs) -> bool
{
    return rotationDiffAbs < 23;
}

// ShouldApplyAmbushTripleBonus mirrors hasTrait Ambush && AUGMENTS_AMBUSH > 0 && rotation window.
inline auto ShouldApplyAmbushTripleBonus(
    const bool hasAmbushTrait,
    const bool hasAugmentsAmbushMod,
    const bool rotationInWindow) -> bool
{
    return hasAmbushTrait && hasAugmentsAmbushMod && rotationInWindow;
}

// --- Slice 1375: kick / daken / follow-up pure policy ---

// PHYSICAL_ATTACK_TYPE numeric mirrors (attack.h).
constexpr uint8 AttackTypeNormal    = 0;
constexpr uint8 AttackTypeDouble    = 1;
constexpr uint8 AttackTypeTriple    = 2;
constexpr uint8 AttackTypeZanshin   = 3;
constexpr uint8 AttackTypeKick      = 4;
constexpr uint8 AttackTypeRanged    = 5;
constexpr uint8 AttackTypeRapidShot = 6;
constexpr uint8 AttackTypeSamba     = 7;
constexpr uint8 AttackTypeQuad      = 8;
constexpr uint8 AttackTypeDaken     = 9;
constexpr uint8 AttackTypeFollowUp  = 10;

// ExclusiveMultiHitAttackType returns PHYSICAL_ATTACK_TYPE for the exclusive ladder
// (QA/TA/DA use their type; mythic/occasional use NORMAL).
inline auto ExclusiveMultiHitAttackType(const MultiHitPreference pref) -> uint8
{
    switch (pref)
    {
        case MultiHitPreference::Quad:
            return AttackTypeQuad;
        case MultiHitPreference::Triple:
            return AttackTypeTriple;
        case MultiHitPreference::Double:
            return AttackTypeDouble;
        case MultiHitPreference::MythicThrice:
        case MultiHitPreference::MythicTwice:
        case MultiHitPreference::OccasionalExtra:
            return AttackTypeNormal;
        default:
            return AttackTypeNormal;
    }
}

// ShouldApplyExclusiveMultiHitSwings: any exclusive preference except None.
inline auto ShouldApplyExclusiveMultiHitSwings(const MultiHitPreference pref) -> bool
{
    return pref != MultiHitPreference::None;
}

// CreateAttacksPlan is the final pure assembly of CreateAttacks swing adds,
// after entity lookups and RNG have selected the preference inputs.
struct CreateAttacksPlan
{
    uint8 initialAttackType{ AttackTypeNormal };
    uint8 initialSwingCount{};
    bool  addAdditionalSwing{};
    bool  addDefaultHit{};
};

inline auto ResolveCreateAttacksPlan(
    const bool hasMikageSwings,
    const uint8 mikageShadows,
    const MultiHitPreference preference,
    const uint8 weaponSwingCount,
    const bool addAdditionalSwing) -> CreateAttacksPlan
{
    CreateAttacksPlan plan{};
    plan.addAdditionalSwing = addAdditionalSwing;

    if (hasMikageSwings)
    {
        plan.initialSwingCount = mikageShadows;
        plan.addDefaultHit     = true;
        return plan;
    }

    if (ShouldApplyExclusiveMultiHitSwings(preference))
    {
        plan.initialAttackType = ExclusiveMultiHitAttackType(preference);
        plan.initialSwingCount = ExclusiveMultiHitSwingCount(preference, weaponSwingCount);
    }
    plan.addDefaultHit = ShouldAddDefaultHit(MultiHitOccurred(preference));
    return plan;
}

// Virtue Stone item ID for ammo-swing follow-up.
constexpr uint16 VirtueStoneItemID = 18244;

// Max follow-up swings stored (one per hand).
constexpr std::size_t MaxFollowUpSwings = 2;

// ShouldCreateKickAttacks mirrors IsH2H() gate.
inline auto ShouldCreateKickAttacks(const bool isH2H) -> bool
{
    return isH2H;
}

// ShouldAddMNKKickMerit mirrors MNK main job && TYPE_PC.
inline auto ShouldAddMNKKickMerit(const bool isMNKMain, const bool isPC) -> bool
{
    return isMNKMain && isPC;
}

// ClampKickAttackRate mirrors std::clamp(rate, 0, 100).
inline auto ClampKickAttackRate(const uint16 rate) -> uint16
{
    return rate > 100 ? static_cast<uint16>(100) : rate;
}

// ShouldProcKickAttack mirrors rolled < kickAttack rate.
inline auto ShouldProcKickAttack(const bool rateProcs) -> bool
{
    return rateProcs;
}

// ShouldProcExtraKick mirrors kick occurred && rolled < EXTRA_KICK_ATTACK.
inline auto ShouldProcExtraKick(const bool kickOccurred, const bool extraKickProcs) -> bool
{
    return kickOccurred && extraKickProcs;
}

// CreateKickAttacksPlan is the final pure assembly of CreateKickAttacks swing adds
// after H2H gating and kick-rate rolls.
struct CreateKickAttacksPlan
{
    bool addRightKick{};
    bool addLeftKick{};
    bool markKickOccurred{};
};

inline auto ResolveCreateKickAttacksPlan(
    const bool isH2H,
    const bool kickRateProcs,
    const bool extraKickProcs) -> CreateKickAttacksPlan
{
    CreateKickAttacksPlan plan{};
    if (!ShouldCreateKickAttacks(isH2H))
    {
        return plan;
    }

    if (ShouldProcKickAttack(kickRateProcs))
    {
        plan.addRightKick      = true;
        plan.markKickOccurred = true;
    }
    if (ShouldProcExtraKick(plan.markKickOccurred, extraKickProcs))
    {
        plan.addLeftKick = true;
    }
    return plan;
}

// ---------------------------------------------------------------------------
// Slice 3251 — CreateDakenAttack TYPE_PC gate
// (dedicated expand residual 3045 / prior dedicated 3184 / pure 1375)
// ---------------------------------------------------------------------------

// ShouldCreateDakenAttack mirrors TYPE_PC gate before daken throw ammo/proc work.
//
// Formula (slice 3251 dedicated dual-wire; residual expand 3045 / prior
// dedicated 3184 / pure 1375 — formula unchanged):
//   isPC
//
// isPC — host-evaluated (m_attacker->objtype == TYPE_PC)
// true  → host may inspect shuriken ammo and roll DAKEN (then plan throw)
// false → skip ammo/proc RNG consumption for the TYPE_PC path
//
// Dual-wire of Go attackround.ShouldCreateDakenAttack
// (residual 1375 / residual dual-wire 3045 / prior dedicated 3184 /
// dedicated dual-wire 3251).
// Call site: CAttackRound::CreateDakenAttack before ammo / DAKEN roll:
//   if (ShouldCreateDakenAttack(m_attacker->objtype == TYPE_PC)) {
//       // ammo isShuriken + rolled < DAKEN
//   }
// Residual dual-wire suite: 3045 (test_attackround_create_daken_3045).
// Prior dedicated dual-wire suite: 3184 (test_attackround_create_daken_attack_3184).
// Dedicated dual-wire suite: 3251 (test_attackround_create_daken_3251).
// Sibling residual only (not re-expanded under 3251):
// ShouldProcDakenThrow (1375), ResolveCreateDakenAttackPlan (2758).
inline auto ShouldCreateDakenAttack(const bool isPC) -> bool
{
    return isPC;
}

// ShouldProcDakenThrow mirrors ammo is shuriken && rolled < DAKEN.
inline auto ShouldProcDakenThrow(const bool ammoIsShuriken, const bool dakenProcs) -> bool
{
    return ammoIsShuriken && dakenProcs;
}

// CreateDakenAttackPlan is the final pure assembly of CreateDakenAttack swing
// adds after PC gating and shuriken/daken rolls.
struct CreateDakenAttackPlan
{
    bool addDakenThrow{};
};

inline auto ResolveCreateDakenAttackPlan(
    const bool isPC,
    const bool ammoIsShuriken,
    const bool dakenProcs) -> CreateDakenAttackPlan
{
    CreateDakenAttackPlan plan{};
    if (!ShouldCreateDakenAttack(isPC))
    {
        return plan;
    }
    plan.addDakenThrow = ShouldProcDakenThrow(ammoIsShuriken, dakenProcs);
    return plan;
}

// IsAmmoSwingEligibleAttackType mirrors NORMAL/DOUBLE/TRIPLE/SAMBA/QUAD.
inline auto IsAmmoSwingEligibleAttackType(const uint8 attackType) -> bool
{
    switch (attackType)
    {
        case AttackTypeNormal:
        case AttackTypeDouble:
        case AttackTypeTriple:
        case AttackTypeSamba:
        case AttackTypeQuad:
            return true;
        default:
            return false;
    }
}

// IsFollowUpModSupported currently only AMMO_SWING is implemented.
inline auto IsFollowUpModSupported(const bool isAmmoSwingMod) -> bool
{
    return isAmmoSwingMod;
}

// IsAttackTypeEligibleForFollowUp combines mod support + attack type.
inline auto IsAttackTypeEligibleForFollowUp(const bool isAmmoSwingMod, const uint8 attackType) -> bool
{
    return IsFollowUpModSupported(isAmmoSwingMod) && IsAmmoSwingEligibleAttackType(attackType);
}

// ShouldUseMainWeaponForFollowUp mirrors IsH2H || RIGHTATTACK.
inline auto ShouldUseMainWeaponForFollowUp(const bool isH2H, const bool isRightAttack) -> bool
{
    return isH2H || isRightAttack;
}

// ShouldUseSubWeaponForFollowUp mirrors LEFTATTACK.
inline auto ShouldUseSubWeaponForFollowUp(const bool isLeftAttack) -> bool
{
    return isLeftAttack;
}

// ShouldProcAmmoSwing mirrors weapon present && rolled < scaled AMMO_SWING.
inline auto ShouldProcAmmoSwing(const bool weaponPresent, const bool ammoSwingProcs) -> bool
{
    return weaponPresent && ammoSwingProcs;
}

// IsVirtueStoneAmmo mirrors ammo ID == 18244 && quantity > 0.
inline auto IsVirtueStoneAmmo(const uint16 ammoID, const uint32 quantity) -> bool
{
    return ammoID == VirtueStoneItemID && quantity > 0;
}

// ShouldUnequipAmmoAfterConsume mirrors quantity == 1 before consume.
inline auto ShouldUnequipAmmoAfterConsume(const uint32 quantityBeforeConsume) -> bool
{
    return quantityBeforeConsume == 1;
}

// CanStoreFollowUpSwing mirrors size < 2 && (empty || back != direction).
inline auto CanStoreFollowUpSwing(
    const std::size_t currentCount,
    const bool empty,
    const bool lastDirectionDiffers) -> bool
{
    if (currentCount >= MaxFollowUpSwings)
    {
        return false;
    }
    return empty || lastDirectionDiffers;
}

// ShouldAppendStoredFollowUps mirrors !m_followUpSwings.empty().
inline auto ShouldAppendStoredFollowUps(const bool hasStoredFollowUps) -> bool
{
    return hasStoredFollowUps;
}

// ShouldProcFollowUpForChar mirrors attacker is PC && has AMMO_SWING mod.
inline auto ShouldProcFollowUpForChar(const bool isPC, const bool hasAmmoSwingMod) -> bool
{
    return isPC && hasAmmoSwingMod;
}

// AttackRoundPostBuildPlan is the pure post-construction tail of CAttackRound
// after CreateKickAttacks / CreateDakenAttack / ProcFollowUpAttacks: mark the
// first swing and always request HasteSambaHaste deletion.
// setFirstSwing is gated on swingCount > 0 so empty lists never index [0].
// deleteHasteSamba is always true (production DelStatusEffect is unconditional;
// absent effects are a no-op).
struct AttackRoundPostBuildPlan
{
    bool setFirstSwing{};
    bool deleteHasteSamba{};
};

// PlanAttackRoundPostBuild mirrors the constructor post-build steps.
inline auto PlanAttackRoundPostBuild(const std::size_t swingCount) -> AttackRoundPostBuildPlan
{
    AttackRoundPostBuildPlan plan{};
    plan.setFirstSwing    = swingCount > 0;
    plan.deleteHasteSamba = true;
    return plan;
}
} // namespace attackroundhelpers
