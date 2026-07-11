#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CAbilityState policy halves (CanUseAbility gates, PetSkillDistanceCheck
// distance bands, ApplyEnmity hostile/ally selection). Entity lookups, recast
// container, Lua OnAbilityCheck, and packets remain host-injected.

namespace abilitystatehelpers
{

// Ability ID band pins for pet-ability ownership and luopan skip.
constexpr uint16 AbilityHealingRuby     = 512; // ABILITY_HEALING_RUBY
constexpr uint16 AbilityConcentricPulse = 353; // ABILITY_CONCENTRIC_PULSE
constexpr uint16 AbilityRadialArcana    = 355; // ABILITY_RADIAL_ARCANA
constexpr uint16 AbilityCharm           = 52;  // ABILITY_CHARM

// Distance bands for Blood Pact / Ready intermediary checks.
constexpr float BloodPactPCRange  = 20.0f;
constexpr float ReadyPCToPetRange = 4.0f;

// Impairment power bits that block abilities (0x01 or 0x03).
constexpr uint16 ImpairmentBlockPower1 = 0x01;
constexpr uint16 ImpairmentBlockPower3 = 0x03;

// MsgBasic pins used by PC CanUseAbility (for dual-home tests).
enum class AbilityUseFail : uint16
{
    None                   = 0,
    TargetOutOfRange       = 4,  // MsgBasic::TargetOutOfRange
    UnableToSeeTarget      = 5,  // MsgBasic::UnableToSeeTarget
    TooFarAway             = 78, // MsgBasic::TooFarAway
    UnableToUseJobAbility2 = 88, // MsgBasic::UnableToUseJobAbility2
    WaitLonger             = 94, // MsgBasic::WaitLonger
};

// HasRecastBusy is host-injected; pure pass-through for completeness.
inline auto HasAbilityRecast(const bool hasRecast) -> bool
{
    return hasRecast;
}

// ImpairmentBlocksAbility mirrors power == 0x01 || power == 0x03.
inline auto ImpairmentBlocksAbility(const bool hasImpairment, const uint16 impairmentPower) -> bool
{
    if (!hasImpairment)
    {
        return false;
    }
    return impairmentPower == ImpairmentBlockPower1 || impairmentPower == ImpairmentBlockPower3;
}

// StatusBlocksAbility mirrors Amnesia || Impairment block.
inline auto StatusBlocksAbility(const bool hasAmnesia, const bool hasImpairment, const uint16 impairmentPower) -> bool
{
    return hasAmnesia || ImpairmentBlocksAbility(hasImpairment, impairmentPower);
}

// IsLuopanAbility mirrors ID in [CONCENTRIC_PULSE, RADIAL_ARCANA].
inline auto IsLuopanAbility(const uint16 abilityID) -> bool
{
    return abilityID >= AbilityConcentricPulse && abilityID <= AbilityRadialArcana;
}

// NeedsPetSkillDistanceCheck mirrors pet ability with BP/Ready skill and not luopan.
inline auto NeedsPetSkillDistanceCheck(
    const bool isPetAbility,
    const bool hasPetSkill,
    const bool isBloodPactRage,
    const bool isBloodPactWard,
    const bool hasMobSkillID,
    const uint16 abilityID) -> bool
{
    if (!isPetAbility || !hasPetSkill || IsLuopanAbility(abilityID))
    {
        return false;
    }
    return isBloodPactRage || isBloodPactWard || hasMobSkillID;
}

// OutOfAbilityRange mirrors distance > range + casterHitbox + targetHitbox.
inline auto OutOfAbilityRange(
    const float distance,
    const float abilityRange,
    const float casterHitbox,
    const float targetHitbox) -> bool
{
    return distance > abilityRange + casterHitbox + targetHitbox;
}

// BloodPactPCOutOfRange mirrors PC != target && dist > 20 + hitboxes.
inline auto BloodPactPCOutOfRange(
    const bool isSelfTarget,
    const float pcToTargetDist,
    const float pcHitbox,
    const float targetHitbox) -> bool
{
    if (isSelfTarget)
    {
        return false;
    }
    return pcToTargetDist > BloodPactPCRange + pcHitbox + targetHitbox;
}

// BloodPactPetOutOfRange mirrors pet-to-target > skillDist + hitboxes.
inline auto BloodPactPetOutOfRange(
    const float petToTargetDist,
    const float skillDistance,
    const float petHitbox,
    const float targetHitbox) -> bool
{
    return petToTargetDist > skillDistance + petHitbox + targetHitbox;
}

// ReadyPCToPetOutOfRange mirrors PC-to-pet > 4 + hitboxes.
inline auto ReadyPCToPetOutOfRange(
    const float pcToPetDist,
    const float pcHitbox,
    const float petHitbox) -> bool
{
    return pcToPetDist > ReadyPCToPetRange + pcHitbox + petHitbox;
}

// ReadyPetToEnemyOutOfRange mirrors pet-to-enemy > skillDist + hitboxes when enemy target.
inline auto ReadyPetToEnemyOutOfRange(
    const bool targetsEnemy,
    const bool hasPetTarget,
    const float petToEnemyDist,
    const float skillDistance,
    const float petHitbox,
    const float enemyHitbox) -> bool
{
    if (!targetsEnemy || !hasPetTarget)
    {
        return false;
    }
    return petToEnemyDist > skillDistance + petHitbox + enemyHitbox;
}

// EvaluatePetSkillDistance is the pure PetSkillDistanceCheck decision.
// bloodPact path when isBloodPact; else jug/ready path when hasMobSkillID.
// Returns true when distance checks pass.
inline auto EvaluatePetSkillDistance(
    const bool hasPetAndSkill,
    const bool isBloodPact,
    const bool isSelfTarget,
    const float pcToTargetDist,
    const float pcHitbox,
    const float targetHitbox,
    const float petToTargetDist,
    const float skillDistance,
    const float petHitbox,
    const bool isJugReady,
    const float pcToPetDist,
    const bool targetsEnemy,
    const bool hasPetTarget,
    const float petToEnemyDist,
    const float enemyHitbox) -> bool
{
    if (!hasPetAndSkill)
    {
        return false;
    }
    if (isBloodPact)
    {
        if (BloodPactPCOutOfRange(isSelfTarget, pcToTargetDist, pcHitbox, targetHitbox))
        {
            return false;
        }
        if (BloodPactPetOutOfRange(petToTargetDist, skillDistance, petHitbox, targetHitbox))
        {
            return false;
        }
        return true;
    }
    if (isJugReady)
    {
        if (ReadyPCToPetOutOfRange(pcToPetDist, pcHitbox, petHitbox))
        {
            return false;
        }
        if (ReadyPetToEnemyOutOfRange(targetsEnemy, hasPetTarget, petToEnemyDist, skillDistance, petHitbox, enemyHitbox))
        {
            return false;
        }
        return true;
    }
    return true;
}

// PCLacksAbilityAccess mirrors the job-ability access half of the PC block:
//   (!isPetAbility && !hasAbility) ||
//   (isPetAbility && id >= HEALING_RUBY && !hasPetAbility)
// Pet abilities below HEALING_RUBY are not gated here (LSB quirk).
// Host injects hasAbility / hasPetAbility for the resolved IDs.
inline auto PCLacksAbilityAccess(
    const bool isPetAbility,
    const uint16 abilityID,
    const bool hasAbility,
    const bool hasPetAbility) -> bool
{
    if (!isPetAbility)
    {
        return !hasAbility;
    }
    if (abilityID >= AbilityHealingRuby && !hasPetAbility)
    {
        return true;
    }
    return false;
}

// ShouldCheckPlayerAbilityLOS mirrors zone MISC_LOS_PLAYER_BLOCK (host injects).
inline auto ShouldCheckPlayerAbilityLOS(const bool losPlayerBlock) -> bool
{
    return losPlayerBlock;
}

// NonPCCancelAbility mirrors the mob/pet cancelAbility ladder pure half.
inline auto NonPCCancelAbility(
    const bool hasTarget,
    const bool hasAmnesia,
    const bool hasImpairment,
    const uint16 impairmentPower,
    const bool targetValid,
    const bool isSelf,
    const float distance,
    const float abilityRange,
    const float casterHitbox,
    const float targetHitbox) -> bool
{
    if (!hasTarget)
    {
        return true;
    }
    if (StatusBlocksAbility(hasAmnesia, hasImpairment, impairmentPower))
    {
        return true;
    }
    if (targetValid && !isSelf &&
        OutOfAbilityRange(distance, abilityRange, casterHitbox, targetHitbox))
    {
        return true;
    }
    return false;
}

// ShouldUpdateHostileEnmity mirrors TARGET_ENEMY && allegiance != && TYPE_MOB && !(ce==0 && ve==0).
inline auto ShouldUpdateHostileEnmity(
    const bool validTargetEnemy,
    const bool differentAllegiance,
    const bool targetIsMob,
    const int16 ce,
    const int16 ve) -> bool
{
    return validTargetEnemy && differentAllegiance && targetIsMob && (ce != 0 || ve != 0);
}

// ShouldGenerateAllyEnmity mirrors target allegiance == caster.
inline auto ShouldGenerateAllyEnmity(const bool sameAllegiance) -> bool
{
    return sameAllegiance;
}

// IsCharmAbility mirrors ability ID == ABILITY_CHARM (passed to UpdateEnmity).
inline auto IsCharmAbility(const uint16 abilityID) -> bool
{
    return abilityID == AbilityCharm;
}

} // namespace abilitystatehelpers
