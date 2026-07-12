#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure can/rate/is gates for parry/guard/block production.
// Parity: internal/attackutils defense_gates + internal/parryrate +
// internal/guardrate + internal/blockrate (slices 1155 / 0836 / 0844 / 0839).
// Host wires attackutils::IsParried / IsGuarded / IsBlocked (slice 1580).

namespace defensegateshelpers
{

constexpr double RateMin = 5.0;
constexpr double RateMax = 25.0;
constexpr double BlockRateMin = 5.0;
constexpr double BlockRateMax = 100.0;
constexpr int    DefenseRollMax = 10000;

// Skill rank A+ for non-PC skill cap lookups (xi.skillRank.A_PLUS = 1).
constexpr std::uint8_t SkillRankAPlus = 1;

// Facing cone for isFacing (default 64).
constexpr std::uint8_t FacingCone = 64;

// Block skill-delta coefficients.
constexpr double BlockSkillDeltaCoeff          = 0.2325;
constexpr double AutomatonBlockSkillDeltaCoeff = 0.215;
constexpr double ReprisalMultDefault           = 1.5;
constexpr double ReprisalMultBonus             = 3.0;
constexpr double ReprisalSkillScale            = 1.15;

// shieldSizeToBlockRateTable[1..6]
inline auto BlockRateFromShieldSize(const int shieldSize) -> double
{
    switch (shieldSize)
    {
        case 1:
            return 55.0;
        case 2:
            return 40.0;
        case 3:
            return 45.0;
        case 4:
            return 30.0;
        case 5:
            return 50.0;
        case 6:
            return 100.0;
        default:
            return 0.0;
    }
}

inline auto ClampRate(const double v) -> double
{
    return std::max(RateMin, std::min(RateMax, v));
}

inline auto ClampBlockRate(const double v) -> double
{
    return std::max(BlockRateMin, std::min(BlockRateMax, v));
}

// canParry inject form.
constexpr auto CanParry(const bool isFacing, const bool isEngaged, const bool hasPreventAction, const bool abilityEligible) -> bool
{
    return isFacing && isEngaged && !hasPreventAction && abilityEligible;
}

// canGuard inject form.
constexpr auto CanGuard(const bool isFacing, const bool isEngaged, const bool hasPreventAction, const bool abilityEligible) -> bool
{
    return isFacing && isEngaged && !hasPreventAction && abilityEligible;
}

// canBlock inject form (no engaged).
constexpr auto CanBlock(const bool isFacing, const bool hasPreventAction, const bool abilityEligible) -> bool
{
    return isFacing && !hasPreventAction && abilityEligible;
}

// PC parry ability: skill rank != 0 and main weapon present with skill type != H2H.
constexpr auto PCParryAbilityEligible(const bool skillRankNonZero, const bool hasMainWeapon, const bool mainIsH2H) -> bool
{
    return skillRankNonZero && hasMainWeapon && !mainIsH2H;
}

// PC guard ability: skill rank > 0 and (no main or H2H).
constexpr auto PCGuardAbilityEligible(const bool skillRankPositive, const bool hasMainWeapon, const bool mainIsH2H) -> bool
{
    return skillRankPositive && (!hasMainWeapon || mainIsH2H);
}

// Non-PC parry: CAN_PARRY > 0.
constexpr auto NonPCParryAbilityEligible(const bool canParryMobMod) -> bool
{
    return canParryMobMod;
}

// Non-PC guard: (MNK or PUP) and CANNOT_GUARD == 0; only for mob/pet/trust.
constexpr auto NonPCGuardAbilityEligible(const bool isMobPetOrTrust, const bool isMNKOrPUP, const bool cannotGuard) -> bool
{
    return isMobPetOrTrust && isMNKOrPUP && !cannotGuard;
}

// PC block: skill rank > 0 and sub is shield.
constexpr auto PCBlockAbilityEligible(const bool skillRankPositive, const bool hasShield) -> bool
{
    return skillRankPositive && hasShield;
}

// Non-PC block: CAN_SHIELD_BLOCK > 0 for mob/pet/trust.
constexpr auto NonPCBlockAbilityEligible(const bool isMobPetOrTrust, const bool canShieldBlock) -> bool
{
    return isMobPetOrTrust && canShieldBlock;
}

// calculateParryRate pure (parryrate.Rate).
inline auto ParryRate(const double defenderSkill, const double attackerSkill, const double issekiganPower, const double inquartataMod) -> double
{
    const double skillDelta = defenderSkill - attackerSkill;
    double       raw        = 0.0;
    if (skillDelta <= 5.0)
    {
        raw = 10.0 + (skillDelta - 6.0) / (36.0 / 9.0);
    }
    else
    {
        raw = 10.0 + (skillDelta - 6.0) / (60.0 / 9.0);
    }
    double rate = std::floor(raw);
    rate        = ClampRate(rate);
    rate += issekiganPower;
    rate += inquartataMod;
    return rate;
}

// calculateGuardRate pure (guardrate.Rate).
inline auto GuardRate(const double defenderSkill, const double attackerSkill, const double additiveGuard) -> double
{
    const double skillDelta = defenderSkill - attackerSkill;
    double       raw        = 0.0;
    if (skillDelta <= 6.0)
    {
        raw = 10.0 + skillDelta / (36.0 / 9.0);
    }
    else
    {
        raw = 10.0 + skillDelta / (60.0 / 9.0);
    }
    double rate = std::floor(raw);
    rate        = ClampRate(rate);
    rate += additiveGuard;
    return rate;
}

// Shared assemble for PC / non-automaton mob block rates.
inline auto AssembleBlockRate(const double baseBlockRate, double blockSkill, const double attackSkill, const double palisade, const bool hasReprisal, const bool reprisalBonus) -> double
{
    double reprisalMult = 1.0;
    if (hasReprisal)
    {
        blockSkill *= ReprisalSkillScale;
        reprisalMult = reprisalBonus ? ReprisalMultBonus : ReprisalMultDefault;
    }
    const double skillMod = (blockSkill - attackSkill) * BlockSkillDeltaCoeff;
    return ClampBlockRate((baseBlockRate + skillMod + palisade) * reprisalMult);
}

// calculateBlockRate pure (blockrate.Rate) for PC with shield.
inline auto BlockRatePC(const bool hasShield, const int shieldSize, const double blockSkill, const double attackSkill, const double palisade, const bool hasReprisal, const bool reprisalBonus) -> double
{
    if (!hasShield)
    {
        return 0.0;
    }
    return AssembleBlockRate(BlockRateFromShieldSize(shieldSize), blockSkill, attackSkill, palisade, hasReprisal, reprisalBonus);
}

// calculateBlockRate pure for non-automaton mob/pet/trust.
inline auto BlockRateMob(const bool canShieldBlock, const double baseBlockRate, const double blockSkill, const double attackSkill, const double palisade, const bool hasReprisal, const bool reprisalBonus) -> double
{
    if (!canShieldBlock)
    {
        return 0.0;
    }
    return AssembleBlockRate(baseBlockRate, blockSkill, attackSkill, palisade, hasReprisal, reprisalBonus);
}

// calculateBlockRate pure for automaton (early return, no 5–100 clamp / Reprisal / Palisade).
inline auto BlockRateAutomaton(const bool canShieldBlock, const double baseBlockRate, const double automatonMeleeSkill, const double attackSkill) -> double
{
    if (!canShieldBlock)
    {
        return 0.0;
    }
    const double skillMod = (automatonMeleeSkill - attackSkill) * AutomatonBlockSkillDeltaCoeff;
    return std::max(0.0, baseBlockRate + skillMod);
}

// Defense roll: rate * 100 >= roll (1..10000).
constexpr auto DefenseRollSucceeds(const double rate, const int roll) -> bool
{
    return rate * 100.0 >= static_cast<double>(roll);
}

constexpr auto IsParried(const bool can, const double rate, const int roll) -> bool
{
    return can && DefenseRollSucceeds(rate, roll);
}

constexpr auto IsGuarded(const bool can, const double rate, const int roll) -> bool
{
    return can && DefenseRollSucceeds(rate, roll);
}

constexpr auto IsBlocked(const bool can, const double rate, const int roll) -> bool
{
    return can && DefenseRollSucceeds(rate, roll);
}

// Skill-up gate: isPC && (succeeded || !oldStyle).
constexpr auto ShouldTryDefensiveSkillUp(const bool isPC, const bool succeeded, const bool oldStyleSkillUp) -> bool
{
    return isPC && (succeeded || !oldStyleSkillUp);
}

// Parry HP recovery: recovery mod > 0 and not Curse II.
constexpr auto ShouldApplyParryHPRecovery(const bool parried, const std::int16_t recoveryMod, const bool hasCurseII) -> bool
{
    return parried && recoveryMod > 0 && !hasCurseII;
}

// Tactical parry/guard TP: PC && trait && succeeded.
// Prefer battleutils::HandleTacticalParry/Guard at the host (trait/mod assembly).
constexpr auto ShouldApplyTacticalTP(const bool isPC, const bool hasTrait, const bool succeeded) -> bool
{
    return isPC && hasTrait && succeeded;
}

} // namespace defensegateshelpers
