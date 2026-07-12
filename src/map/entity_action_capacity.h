#pragma once

#include <cstdint>

// Pure MakeEntityStandUp / assist / turn / ranged delay-bonus / cover helpers.

namespace entityactionhelpers
{

constexpr std::uint8_t AnimationNone       = 0;
constexpr std::uint8_t AnimationHealing    = 33;
constexpr std::uint8_t AnimationSit        = 47;
constexpr std::uint8_t AnimationSitChair0  = 63;
constexpr std::uint8_t AnimationSitChair10 = 73;

constexpr std::uint8_t PetTypeWyvern    = 1;
constexpr std::uint8_t PetTypeAutomaton = 4;

constexpr std::int16_t SnapshotCap             = 70;
constexpr int          VelocityShotBasePercent = 15;
constexpr std::uint16_t BehaviorNoTurn         = 0x400;

enum class StandUpAction : std::uint8_t
{
    None = 0,
    CancelHealing,
    LeaveSit,
};

constexpr auto ClassifyStandUp(const bool isPC, const std::uint8_t animation) -> StandUpAction
{
    if (!isPC)
    {
        return StandUpAction::None;
    }
    if (animation == AnimationHealing)
    {
        return StandUpAction::CancelHealing;
    }
    if (animation == AnimationSit || (animation >= AnimationSitChair0 && animation <= AnimationSitChair10))
    {
        return StandUpAction::LeaveSit;
    }
    return StandUpAction::None;
}

constexpr auto ShouldStandUpPet(const bool hasPet, const std::uint8_t petType) -> bool
{
    if (!hasPet)
    {
        return false;
    }
    return petType == PetTypeWyvern || petType == PetTypeAutomaton;
}

enum class AssistLockOn : std::uint8_t
{
    None = 0,
    LockOnResolved,
};

constexpr auto ClassifyAssistTarget(const bool hasEntity,
                                    const bool targetIsPC,
                                    const bool hasBattleTargetID,
                                    const bool hasResolvedBattleTarget) -> AssistLockOn
{
    if (!hasEntity)
    {
        return AssistLockOn::None;
    }
    if (targetIsPC)
    {
        if (hasBattleTargetID && hasResolvedBattleTarget)
        {
            return AssistLockOn::LockOnResolved;
        }
        return AssistLockOn::None;
    }
    if (hasBattleTargetID)
    {
        return AssistLockOn::LockOnResolved;
    }
    return AssistLockOn::None;
}

constexpr auto ShouldTurnTowardsTarget(const bool          hasEntity,
                                       const bool          hasTarget,
                                       const bool          isMob,
                                       const std::uint16_t behavior,
                                       const bool          force) -> bool
{
    if (!hasEntity || !hasTarget)
    {
        return false;
    }
    if (isMob && (behavior & BehaviorNoTurn) != 0 && !force)
    {
        return false;
    }
    return true;
}

inline auto RangedDelayReduction(const std::int16_t delay,
                                 const std::int16_t snapshotMod,
                                 const bool         hasVelocityShot,
                                 const std::int16_t velocitySnapshotBonus) -> std::int16_t
{
    std::int16_t snapshot = snapshotMod;
    if (snapshot > SnapshotCap)
    {
        snapshot = SnapshotCap;
    }
    int velocity = 0;
    if (hasVelocityShot)
    {
        velocity = VelocityShotBasePercent + static_cast<int>(velocitySnapshotBonus);
    }
    const float result = static_cast<float>(delay) * ((100.0f - static_cast<float>(snapshot)) / 100.0f) *
                         ((100.0f - static_cast<float>(velocity)) / 100.0f);
    return static_cast<std::int16_t>(result);
}

constexpr auto RangedAttackBonuses(const bool isPC, const bool hasVelocityShot, const std::int32_t velocityRattBonus) -> std::int32_t
{
    if (!isPC)
    {
        return 0;
    }
    if (hasVelocityShot)
    {
        return velocityRattBonus;
    }
    return 0;
}

constexpr auto RangedAccuracyBonuses(const bool isPC, const bool hasBarrage, const std::int32_t barrageAcc) -> std::int32_t
{
    if (!isPC)
    {
        return 0;
    }
    if (hasBarrage)
    {
        return barrageAcc;
    }
    return 0;
}

constexpr auto IsMagicCovered(const bool hasUser, const std::int16_t coverMagicAndRanged) -> bool
{
    return hasUser && coverMagicAndRanged == 1;
}

constexpr auto CoverMemberEligible(const bool localVarMatches, const bool hasCover, const bool memberAlive) -> bool
{
    return localVarMatches && hasCover && memberAlive;
}

constexpr auto CoverGeometryOK(const float distUserMob,
                               const float meleeRange,
                               const float worldAngleMinDistance,
                               const float distTargetMob,
                               const bool  areInLine) -> bool
{
    return distUserMob <= meleeRange && distUserMob >= worldAngleMinDistance && distUserMob < distTargetMob && areInLine;
}

constexpr auto ShouldSearchCoverParty(const bool hasParty) -> bool
{
    return hasParty;
}

constexpr auto ShouldApplyAbsorbedMP(const std::int16_t absorbedMP) -> bool
{
    return absorbedMP > 0;
}

// ConvertDmgToMP amount (matches dmgtaken.AbsorbedPhysDmgToMP).
inline auto AbsorbedPhysDmgToMP(const std::int32_t damage,
                                const bool         isCovered,
                                const std::int16_t coverToMP,
                                const std::int16_t absorbPhysDmgToMP) -> std::int16_t
{
    double dmgToMPMods = 0.0;
    if (isCovered)
    {
        dmgToMPMods += coverToMP;
    }
    dmgToMPMods += absorbPhysDmgToMP;
    return static_cast<std::int16_t>(damage * (dmgToMPMods / 100.0));
}

} // namespace entityactionhelpers
