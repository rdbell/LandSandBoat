#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CRangeState policy halves (ctor Rapid Shot / mob delays, CanUseRangedAttack
// gates, HasMoved threshold). Entity lookups, equip objects, facing/LOS geometry,
// packets, and RNG draws remain host-injected.

namespace rangestatehelpers
{

// SKILL_* pins used by PC ranged equip validation.
constexpr uint8 SkillThrowing     = 27;
constexpr uint8 SkillArchery      = 25;
constexpr uint8 SkillMarksmanship = 26;

// ANIMATION_* pins for the free-to-range check.
constexpr uint8 AnimationNone   = 0;
constexpr uint8 AnimationAttack = 1;

// MsgBasic-style reason codes for pure CanUseRangedAttack outcomes.
// Values match enums/msg_basic.h so hosts can build packets without remapping.
enum class RangedUseFail : uint16
{
    None                = 0,
    CannotPerformAction = 71,  // MsgBasic::CannotPerformAction
    WaitLonger          = 94,  // MsgBasic::WaitLonger
    NoRangedWeapon      = 216, // MsgBasic::NoRangedWeapon
    CannotSee           = 217, // MsgBasic::CannotSee
    CannotAttackTarget  = 446, // MsgBasic::CannotAttackTarget
};

// Rapid Shot reduction band: GetRandomNumber<uint16>(2, 50) → [2, 50).
constexpr uint16 RapidShotReductionMin = 2;
constexpr uint16 RapidShotReductionMaxExclusive = 50;

// HasMoved horizontal distance threshold (ignoreVertical distance).
constexpr float HasMovedDistanceThreshold = 0.3f;

// Mob free-phase base + exclusive upper for jitter (GetRandomNumber(0, 1500)).
constexpr int32 MobFreePhaseBaseMs     = 6500;
constexpr int32 MobFreePhaseJitterMax  = 1500; // exclusive
constexpr int32 MobReturnWeaponDelayMs = 2850;

// PC default free-phase / return-weapon delays (milliseconds).
constexpr int32 PCReturnWeaponDelayMs = 1000;
constexpr int32 PCFreePhaseTimeMs     = 1100;

// ShouldTryRapidShot mirrors PC|TRUST && !throwing gate before chance/RNG.
inline auto ShouldTryRapidShot(const bool isPCOrTrust, const bool isThrowing) -> bool
{
    return isPCOrTrust && !isThrowing;
}

// RapidShotChanceEligible mirrors chance > 0 before rolling.
inline auto RapidShotChanceEligible(const int16 chance) -> bool
{
    return chance > 0;
}

// RapidShotProcs mirrors xirand::GetRandomNumber(100) < chance.
// Host injects roll in [0, 100).
inline auto RapidShotProcs(const int16 chance, const int roll0to99) -> bool
{
    if (!RapidShotChanceEligible(chance))
    {
        return false;
    }
    return roll0to99 < chance;
}

// ClampRapidShotReductionPercent pins reduction to the live band [2, 49].
// Host supplies the value from GetRandomNumber(2, 50); invalid injects are clamped.
inline auto ClampRapidShotReductionPercent(const uint16 reductionPercent) -> uint16
{
    if (reductionPercent < RapidShotReductionMin)
    {
        return RapidShotReductionMin;
    }
    if (reductionPercent >= RapidShotReductionMaxExclusive)
    {
        return static_cast<uint16>(RapidShotReductionMaxExclusive - 1);
    }
    return reductionPercent;
}

// ApplyRapidShotDelayReduction mirrors:
//   delay = (int16)(delay * (1.0f - reductionPercent / 100.0f))
// reductionPercent is the raw draw (clamped to [2, 49]).
inline auto ApplyRapidShotDelayReduction(const int16 delay, const uint16 reductionPercent) -> int16
{
    const uint16 pct = ClampRapidShotReductionPercent(reductionPercent);
    return static_cast<int16>(delay * (1.0f - pct / 100.0f));
}

// MobFreePhaseTimeMs mirrors 6500ms + GetRandomNumber(0, 1500).
// Host injects jitter in [0, 1500); values are clamped into that half-open range.
inline auto MobFreePhaseTimeMs(const int32 jitter0to1499) -> int32
{
    int32 j = jitter0to1499;
    if (j < 0)
    {
        j = 0;
    }
    if (j >= MobFreePhaseJitterMax)
    {
        j = MobFreePhaseJitterMax - 1;
    }
    return MobFreePhaseBaseMs + j;
}

// ShouldApplyMobMeleeFreePhase mirrors distance <= GetMeleeRange for TYPE_MOB.
inline auto ShouldApplyMobMeleeFreePhase(const bool isMob, const bool inMeleeRange) -> bool
{
    return isMob && inMeleeRange;
}

// ShouldApplyMobReturnWeaponDelay mirrors objtype == TYPE_MOB.
inline auto ShouldApplyMobReturnWeaponDelay(const bool isMob) -> bool
{
    return isMob;
}

// HasInitialRangedEquip mirrors:
//   (PRanged && isType(ITEM_WEAPON)) || (PAmmo && isThrowing())
inline auto HasInitialRangedEquip(const bool hasRangedWeapon, const bool hasThrowingAmmo) -> bool
{
    return hasRangedWeapon || hasThrowingAmmo;
}

// ResolveRangedSkillType mirrors PRanged ? rangedSkill : ammoSkill.
inline auto ResolveRangedSkillType(const bool hasRanged, const uint8 rangedSkill, const uint8 ammoSkill) -> uint8
{
    return hasRanged ? rangedSkill : ammoSkill;
}

// PCRangedSkillEquipOK mirrors the skill-type switch after initial equip check:
// THROWING → ok; ARCHERY/MARKSMANSHIP → need ammo weapon; else fail.
inline auto PCRangedSkillEquipOK(const uint8 skillType, const bool hasAmmoWeapon) -> bool
{
    switch (skillType)
    {
        case SkillThrowing:
            return true;
        case SkillArchery:
        case SkillMarksmanship:
            return hasAmmoWeapon;
        default:
            return false;
    }
}

// IsAllowedRangedAnimation mirrors animation == NONE || ATTACK.
inline auto IsAllowedRangedAnimation(const uint8 animation) -> bool
{
    return animation == AnimationNone || animation == AnimationAttack;
}

// ShouldCheckLineOfSight mirrors !isEndOfAttack before CanSeeTarget.
inline auto ShouldCheckLineOfSight(const bool isEndOfAttack) -> bool
{
    return !isEndOfAttack;
}

// FreePhaseBusy mirrors tick - last < freePhaseTime (strict <).
// Host injects durations in the same unit (e.g. milliseconds).
inline auto FreePhaseBusy(const int64 tickMinusLast, const int64 freePhaseTime) -> bool
{
    return tickMinusLast < freePhaseTime;
}

// IsOutOfRangedAttackRange mirrors distance > GetRangedAttackRange().
inline auto IsOutOfRangedAttackRange(const float distance, const float rangedAttackRange) -> bool
{
    return distance > rangedAttackRange;
}

// RangeHasMoved mirrors TYPE_PC && distance(start, cur, ignoreVertical) > 0.3.
// Host injects isPC and the precomputed horizontal distance.
inline auto RangeHasMoved(const bool isPC, const float horizontalDistance) -> bool
{
    if (!isPC)
    {
        return false;
    }
    return horizontalDistance > HasMovedDistanceThreshold;
}

// ShouldInterruptOnError mirrors Update's interrupt path:
//   m_errorMsg && (!cast_errorMsg || messageId != CannotSee)
// isBattleMessage false means a non-battle error packet (always interrupt).
// messageId is only meaningful when isBattleMessage is true.
inline auto ShouldInterruptOnError(const bool hasErrorMsg, const bool isBattleMessage, const uint16 messageId) -> bool
{
    if (!hasErrorMsg)
    {
        return false;
    }
    if (!isBattleMessage)
    {
        return true;
    }
    return messageId != static_cast<uint16>(RangedUseFail::CannotSee);
}

// EvaluateCanUseRangedAttack is the pure decision ladder of CanUseRangedAttack
// with host-injected equip/facing/LOS/free-phase/animation flags.
// Barrage deletion on THROWING stays host-side (side effect).
// Returns RangedUseFail::None on success.
inline auto EvaluateCanUseRangedAttack(
    const bool hasTarget,
    const bool isPC,
    const bool hasRangedWeapon,
    const bool hasThrowingAmmo,
    const uint8 skillType,
    const bool hasAmmoWeapon,
    const bool isFacing,
    const bool isEndOfAttack,
    const bool canSeeTarget,
    const bool freePhaseBusy,
    const uint8 animation) -> RangedUseFail
{
    if (!hasTarget)
    {
        return RangedUseFail::CannotAttackTarget;
    }

    if (isPC)
    {
        if (!HasInitialRangedEquip(hasRangedWeapon, hasThrowingAmmo))
        {
            return RangedUseFail::NoRangedWeapon;
        }
        if (!PCRangedSkillEquipOK(skillType, hasAmmoWeapon))
        {
            return RangedUseFail::NoRangedWeapon;
        }
    }

    if (!isFacing)
    {
        return RangedUseFail::CannotSee;
    }

    if (ShouldCheckLineOfSight(isEndOfAttack) && !canSeeTarget)
    {
        return RangedUseFail::CannotPerformAction;
    }

    if (isPC && freePhaseBusy)
    {
        return RangedUseFail::WaitLonger;
    }

    if (!IsAllowedRangedAnimation(animation))
    {
        return RangedUseFail::CannotPerformAction;
    }

    return RangedUseFail::None;
}

} // namespace rangestatehelpers
