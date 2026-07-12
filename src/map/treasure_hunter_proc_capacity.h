#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure Treasure Hunter proc path from HandleEnspell.

namespace treasurehunterprochelpers
{

constexpr std::uint8_t JobTHF            = 6;
constexpr std::int16_t THAutoUpgradeCap  = 8;
constexpr float        THProcBaseRate    = 0.04f;

constexpr auto ShouldEvaluateTreasureHunterProc(const bool        isPC,
                                                const bool        disableTHProcs,
                                                const std::int32_t finalDamage,
                                                const bool        isFirstSwing,
                                                const bool        defenderIsMob,
                                                const bool        isTHFMain,
                                                const bool        hasTHTrait) -> bool
{
    if (!isPC || disableTHProcs)
    {
        return false;
    }
    return finalDamage > 0 && isFirstSwing && defenderIsMob && isTHFMain && hasTHTrait;
}

constexpr auto CanUpgradeTreasureHunter(const std::int16_t mobTHLvl, const std::int16_t treasureHunterCapMod) -> bool
{
    return mobTHLvl < static_cast<std::int16_t>(12 + treasureHunterCapMod);
}

inline auto AutoUpgradeMobTH(const std::int16_t mobTHLvl, const std::int16_t playerTH, std::int16_t& outNewTH) -> bool
{
    const auto diff = static_cast<std::int16_t>(mobTHLvl - playerTH);
    if (diff < 0 && mobTHLvl < THAutoUpgradeCap)
    {
        outNewTH = std::min<std::int16_t>(THAutoUpgradeCap, playerTH);
        return true;
    }
    outNewTH = mobTHLvl;
    return false;
}

inline auto TreasureHunterProcRate(const std::int16_t mobTHLvl,
                                   const std::int16_t playerTH,
                                   const std::int16_t playerTHProc,
                                   const std::int16_t mobTHProc,
                                   const bool         isSneakAttack,
                                   const bool         isTrickAttack) -> float
{
    auto diff = static_cast<std::int16_t>(mobTHLvl - playerTH);
    if (diff < 0)
    {
        diff = 0;
    }
    float procRate      = THProcBaseRate / std::pow(2.0f, static_cast<float>(std::max<std::int16_t>(0, diff)));
    float procRateBonus = 1.0f + (playerTHProc + mobTHProc) / 100.0f;
    float sneakTrick    = 0.0f;
    if (isSneakAttack)
    {
        sneakTrick += 10.0f;
    }
    if (isTrickAttack)
    {
        sneakTrick += 10.0f;
    }
    if (sneakTrick > 1.0f)
    {
        procRateBonus *= sneakTrick;
    }
    return procRate * procRateBonus;
}

constexpr auto TreasureHunterProcs(const float procRate, const float roll) -> bool
{
    return roll <= procRate;
}

struct TreasureHunterResult
{
    bool         eligible{ false };
    std::int16_t newMobTH{ 0 };
    bool         autoUpgraded{ false };
    float        procRate{ 0.0f };
    bool         procced{ false };
};

inline auto ResolveTreasureHunterProc(const std::int16_t mobTHLvl,
                                      const std::int16_t playerTH,
                                      const std::int16_t treasureHunterCapMod,
                                      const std::int16_t playerTHProc,
                                      const std::int16_t mobTHProc,
                                      const bool         isSneakAttack,
                                      const bool         isTrickAttack,
                                      const float        roll) -> TreasureHunterResult
{
    if (!CanUpgradeTreasureHunter(mobTHLvl, treasureHunterCapMod))
    {
        return {};
    }
    std::int16_t newTH = mobTHLvl;
    const bool   up    = AutoUpgradeMobTH(mobTHLvl, playerTH, newTH);
    const float  rate  = TreasureHunterProcRate(newTH, playerTH, playerTHProc, mobTHProc, isSneakAttack, isTrickAttack);
    return TreasureHunterResult{
        true,
        newTH,
        up,
        rate,
        TreasureHunterProcs(rate, roll),
    };
}

} // namespace treasurehunterprochelpers
