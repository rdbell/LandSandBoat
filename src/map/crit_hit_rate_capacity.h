#pragma once

#include <algorithm>
#include <cstdint>

// Pure GetCritHitRate / GetRangedCritHitRate / GetDexCritBonus / GetAGICritBonus.
// Parity: internal/attackutils melee_crit_hit_rate, ranged_crit_hit_rate,
// dex_crit_bonus, agi_crit_bonus.

namespace crithitratehelpers
{

constexpr auto ClampCritRate(std::int32_t rate) -> std::uint8_t
{
    if (rate < 0)
    {
        rate = 0;
    }
    else if (rate > 100)
    {
        rate = 100;
    }
    return static_cast<std::uint8_t>(rate);
}

constexpr auto DexCritBonus(const std::int32_t attackerDex, const std::int32_t defenderAgi) -> std::int8_t
{
    auto dDex = attackerDex - defenderAgi;
    if (dDex < 0)
    {
        dDex = 0;
    }
    else if (dDex > 50)
    {
        dDex = 50;
    }

    std::int32_t critRate = 0;
    if (dDex > 39)
    {
        critRate = dDex - 35;
    }
    else if (dDex > 29)
    {
        critRate = 4;
    }
    else if (dDex > 19)
    {
        critRate = 3;
    }
    else if (dDex > 13)
    {
        critRate = 2;
    }
    else if (dDex > 6)
    {
        critRate = 1;
    }

    if (critRate > 15)
    {
        critRate = 15;
    }
    return static_cast<std::int8_t>(critRate);
}

constexpr auto AgiCritBonus(const std::int32_t attackerAgi, const std::int32_t defenderAgi) -> std::int8_t
{
    auto dAgi = attackerAgi - defenderAgi;
    if (dAgi < 0)
    {
        dAgi = 0;
    }
    return static_cast<std::int8_t>(dAgi / 10);
}

// Melee normal-path assembly after short-circuit gates have not forced 100.
constexpr auto MeleeCritHitRate(const std::int32_t attackerCritHitRateMerit,
                                const std::int32_t fencerCritHitRate,
                                const std::int32_t defenderEnemyCritRateMerit,
                                const std::int32_t inninPower,
                                const std::int32_t yoninPower,
                                const std::int32_t dexCritBonus,
                                const std::int32_t critHitRateMod,
                                const std::int32_t defenderCriticalHitEvasion,
                                const std::int32_t weaponOnlyCritHitRate) -> std::uint8_t
{
    auto critHitRate = static_cast<std::int32_t>(5);
    critHitRate += attackerCritHitRateMerit;
    critHitRate += fencerCritHitRate;
    critHitRate -= defenderEnemyCritRateMerit;
    critHitRate += inninPower;
    critHitRate -= yoninPower;
    critHitRate += dexCritBonus;
    critHitRate += critHitRateMod;
    critHitRate -= defenderCriticalHitEvasion;
    critHitRate += weaponOnlyCritHitRate;
    return ClampCritRate(critHitRate);
}

// Ranged assembly; Mighty Strikes CRITHITRATE is subtracted (ranged ignores MS).
constexpr auto RangedCritHitRate(const std::int32_t attackerCritHitRateMerit,
                                 const std::int32_t defenderEnemyCritRateMerit,
                                 const std::int32_t inninPower,
                                 const std::int32_t yoninPower,
                                 const std::int32_t critHitRateMod,
                                 const bool         hasMightyStrikes,
                                 const std::int32_t agiCritBonus,
                                 const std::int32_t defenderCriticalHitEvasion) -> std::uint8_t
{
    auto critHitRate = static_cast<std::int32_t>(5);
    critHitRate += attackerCritHitRateMerit;
    critHitRate -= defenderEnemyCritRateMerit;
    critHitRate += inninPower;
    critHitRate -= yoninPower;
    critHitRate += critHitRateMod - (hasMightyStrikes ? 100 : 0);
    critHitRate += agiCritBonus;
    critHitRate -= defenderCriticalHitEvasion;
    return ClampCritRate(critHitRate);
}

// Classify GetCritHitRate control flow after Mighty Strikes short-circuit.
enum class MeleeCritPath : std::uint8_t
{
    // Normal assembly path (else branch).
    Assembly = 0,
    // SA/TA arm matched entity conditions but position/TA-char failed → bare 5.
    BareFive,
    // Forced 100 from SA behind/Hide or TA with valid char.
    Forced100,
};

// hasMightyStrikes already handled by host (return 100).
// For SA: isPC && !ignoreSTA && hasSneakAttack → if behindOrHide then Forced100 else BareFive.
// For TA: isPC && THF && assassin && !ignoreSTA && hasTrickAttack → if hasTAChar Forced100 else BareFive.
// Else Assembly.
constexpr auto ClassifyMeleeCritPath(const bool isPC,
                                     const bool ignoreSneakTrickAttack,
                                     const bool hasSneakAttack,
                                     const bool behindOrHide,
                                     const bool isTHFMain,
                                     const bool hasAssassin,
                                     const bool hasTrickAttack,
                                     const bool hasTAChar) -> MeleeCritPath
{
    if (isPC && !ignoreSneakTrickAttack && hasSneakAttack)
    {
        return behindOrHide ? MeleeCritPath::Forced100 : MeleeCritPath::BareFive;
    }
    if (isPC && isTHFMain && hasAssassin && !ignoreSneakTrickAttack && hasTrickAttack)
    {
        return hasTAChar ? MeleeCritPath::Forced100 : MeleeCritPath::BareFive;
    }
    return MeleeCritPath::Assembly;
}

// Fencer gear gate for crit rate (same shape as WS TP FencerEligible).
constexpr auto FencerCritEligible(const bool mainPresent,
                                  const bool mainTwoHanded,
                                  const bool mainHandToHand,
                                  const bool subPresent,
                                  const bool subIsWeapon,
                                  const std::uint8_t subSkillType,
                                  const bool subIsShield) -> bool
{
    if (!mainPresent || mainTwoHanded || mainHandToHand)
    {
        return false;
    }
    if (!subPresent)
    {
        return true;
    }
    if (subIsShield)
    {
        return true;
    }
    if (subIsWeapon && subSkillType == 0)
    {
        return true;
    }
    return false;
}

} // namespace crithitratehelpers
