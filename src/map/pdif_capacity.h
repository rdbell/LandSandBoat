#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure calculateMeleePDIF / calculateRangedPDIF product helpers.
// Parity: internal/pdif MeleePDIF / RangedPDIF (+ curve helpers).

namespace pdifhelpers
{

constexpr double MeleeLevelCorrectionPerLevel  = 3.0 / 64.0;
constexpr double RangedLevelCorrectionPerLevel = 3.0 / 128.0;
constexpr double RangedCRatioMin               = 0.0;
constexpr double RangedCRatioMax               = 10.0;
constexpr double RangedCritMult                = 1.25;
constexpr double DefaultWeaponCap              = 3.0;

inline auto ClampFloat(const double v, const double lo, const double hi) -> double
{
    return std::max(lo, std::min(hi, v));
}

inline auto ClampInt(const int v, const int lo, const int hi) -> int
{
    return std::max(lo, std::min(hi, v));
}

// Sparse weapon caps matching pDifWeaponCapTable / internal/pdif.
inline auto WeaponCap(const std::uint8_t skillType) -> double
{
    switch (skillType)
    {
        case 0:
            return 3.0;
        case 1:
            return 3.5; // H2H
        case 2:
        case 3:
        case 5:
        case 9:
        case 11:
            return 3.25; // Dagger/Sword/Axe/Katana/Club
        case 4:
        case 6:
        case 8:
        case 12:
            return 3.75; // GS/GA/Polearm/Staff
        case 7:
            return 4.0; // Scythe
        case 10:
            return 3.5; // GKatana
        case 22:
        case 23:
        case 24:
            return 3.0; // Automaton
        case 25:
        case 27:
            return 3.25; // Archery/Throwing
        case 26:
            return 3.5; // Marksmanship
        default:
            return DefaultWeaponCap;
    }
}

inline auto LevelDifFactor(const int actorLevel, const int targetLevel, const bool apply, const bool actorIsPC, const bool ranged) -> double
{
    if (!apply)
    {
        return 0.0;
    }
    const double slope  = ranged ? RangedLevelCorrectionPerLevel : MeleeLevelCorrectionPerLevel;
    double       factor = static_cast<double>(actorLevel - targetLevel) * slope;
    if (!actorIsPC && factor < 0.0)
    {
        return 0.0;
    }
    if (actorIsPC && factor > 0.0)
    {
        return 0.0;
    }
    return factor;
}

inline auto SpikeRatio(const bool isPC, const double wRatio) -> double
{
    if (isPC)
    {
        if (wRatio > 0.5 && wRatio < 1.5)
        {
            const double sRatio = (0.5 - std::fabs(wRatio - 1.0)) * 1.2;
            return ClampFloat(sRatio, 0.0, 1.0 / 3.0);
        }
        return 0.0;
    }
    double sRatio = 0.0;
    if (wRatio > 0.0 && wRatio < 0.75)
    {
        sRatio = -5.0 / 9.0 + (10.0 / 9.0) * wRatio;
    }
    else if (wRatio <= 1.3)
    {
        sRatio = 0.3;
    }
    else
    {
        sRatio = 5.0 / 3.0 - (270.0 / 256.0) * wRatio;
    }
    return ClampFloat(sRatio, 0.0, 0.3);
}

inline auto WRatioCapPC(const double wRatio, const double pDifFinalCap, double& lower, double& upper) -> void
{
    if (wRatio < 0.5)
    {
        upper = wRatio + 0.5;
    }
    else if (wRatio < 0.7)
    {
        upper = 1.0;
    }
    else if (wRatio < 1.2)
    {
        upper = wRatio + 0.3;
    }
    else if (wRatio < 1.5)
    {
        upper = wRatio + wRatio * 0.25;
    }
    else
    {
        upper = std::min(wRatio + 0.375, pDifFinalCap);
    }

    if (wRatio < 0.38)
    {
        lower = 0.0;
    }
    else if (wRatio < 1.25)
    {
        lower = wRatio * (1176.0 / 1024.0) - (448.0 / 1024.0);
    }
    else if (wRatio < 1.51)
    {
        lower = 1.0;
    }
    else if (wRatio < 2.44)
    {
        lower = wRatio * (1176.0 / 1024.0) - (775.0 / 1024.0);
    }
    else
    {
        lower = std::min(wRatio - 0.375, pDifFinalCap);
    }
}

inline auto WRatioCapOthers(const double wRatio, const double pDifFinalCap, double& lower, double& upper) -> void
{
    if (wRatio < 0.55)
    {
        upper = 0.6 + (760.0 / 1024.0) * wRatio;
    }
    else if (wRatio <= 0.8)
    {
        upper = 1.0;
    }
    else if (wRatio < 1.2)
    {
        upper = 1.0 + (1127.0 / 1024.0) * (wRatio - 0.8);
    }
    else if (wRatio < 1.5)
    {
        upper = (1474.0 / 1024.0) + (1105.0 / 1024.0) * (wRatio - (1235.0 / 1024.0));
    }
    else
    {
        upper = std::min((1803.0 / 1024.0) + (1070.0 / 1024.0) * (wRatio - 1.5), pDifFinalCap);
    }

    if (wRatio <= 0.4)
    {
        lower = 0.25;
    }
    else if (wRatio < 1.35)
    {
        lower = 0.25 + (827.0 / 1024.0) * (wRatio - 0.4);
    }
    else if (wRatio <= 1.60)
    {
        lower = 1.0;
    }
    else
    {
        lower = std::min(1.0 + (1120.0 / 1024.0) * (wRatio - 1.59), pDifFinalCap);
    }
}

inline auto FinalCapPC(const double weaponCap, const int damageLimit, const int damageLimitP, const bool meleeCritBonus) -> double
{
    double cap = (weaponCap + static_cast<double>(damageLimit) / 100.0) * (1.0 + static_cast<double>(damageLimitP) / 100.0);
    if (meleeCritBonus)
    {
        cap += 1.0;
    }
    return cap;
}

inline auto FinalCapMeleeOthers(const bool applyLevelCorrection, const bool isCritical, const int damageLimit, const int damageLimitP) -> double
{
    const double base      = applyLevelCorrection ? 2.0 : 4.0;
    const double critBonus = (applyLevelCorrection && isCritical) ? 1.0 : 0.0;
    return (base + static_cast<double>(damageLimit) / 100.0) * (1.0 + static_cast<double>(damageLimitP) / 100.0) + critBonus;
}

inline auto FinalCapRangedOthers(const bool applyLevelCorrection, const int damageLimit, const int damageLimitP) -> double
{
    const double base = applyLevelCorrection ? 3.0 : 4.0;
    return (base + static_cast<double>(damageLimit) / 100.0) * (1.0 + static_cast<double>(damageLimitP) / 100.0);
}

inline auto CritDamageMult(const int critDmgIncrease, const int critDefBonus) -> double
{
    const int bonus = ClampInt(critDmgIncrease - critDefBonus, 0, 100);
    return (100.0 + static_cast<double>(bonus)) / 100.0;
}

// Effective defense with optional ignore fraction (tpIgnoresDefense).
inline auto EffectiveDefense(double def, const bool ignores, const double ignoreFraction) -> double
{
    if (def < 1.0)
    {
        def = 1.0;
    }
    if (!ignores)
    {
        return def;
    }
    return std::max(1.0, std::floor(def * (1.0 - ignoreFraction)));
}

// Melee pDIF after host supplies RNG: spikeRoll 1..10000, upperMaxCoin 0|1,
// ratioRoll integer (lower*1000..upper*1000), meleeRandStep 0..5.
// Returns true when spike short-circuit fired (out = 1.0).
inline auto MeleePDIF(const double actorAttack, const double targetDefense, const bool isCritical, const bool applyLevelCorrection, const bool actorIsPC, const int actorLevel, const int targetLevel, const double weaponCap, const int damageLimit, const int damageLimitP, const int critDmgIncrease, const int critDefBonus, const int spikeRoll, const int upperMaxCoin, const int ratioRoll, const int meleeRandStep, double& out) -> bool
{
    const double def       = targetDefense < 1.0 ? 1.0 : targetDefense;
    const double baseRatio = def != 0.0 ? actorAttack / def : 0.0;
    const double levelDif  = LevelDifFactor(actorLevel, targetLevel, applyLevelCorrection, actorIsPC, false);
    double       wRatio    = baseRatio + (isCritical ? 1.0 : 0.0);

    const double pDifFinalCap = actorIsPC ? FinalCapPC(weaponCap, damageLimit, damageLimitP, isCritical)
                                          : FinalCapMeleeOthers(applyLevelCorrection, isCritical, damageLimit, damageLimitP);

    const double sRatio = SpikeRatio(actorIsPC, wRatio);
    if (static_cast<double>(spikeRoll) / 10000.0 <= sRatio)
    {
        out = 1.0;
        return true;
    }

    double lower = 0.0;
    double upper = 0.0;
    if (actorIsPC)
    {
        WRatioCapPC(wRatio, pDifFinalCap, lower, upper);
    }
    else
    {
        WRatioCapOthers(wRatio, pDifFinalCap, lower, upper);
    }

    const double upperMax  = (upperMaxCoin == 0) ? 0.5 : 0.0;
    const double upperBound = std::max(upper + levelDif, upperMax);
    const double lowerBound = std::max(lower + levelDif, 0.0);
    if (upperBound == 0.0)
    {
        out = 0.0;
        return false;
    }

    double pDif = static_cast<double>(ratioRoll) / 1000.0;
    pDif *= 1.0 + static_cast<double>(ClampInt(meleeRandStep, 0, 5)) * 0.01;
    if (isCritical)
    {
        pDif *= CritDamageMult(critDmgIncrease, critDefBonus);
    }
    out = pDif;
    return false;
}

// Compute melee bounds for host RNG sampling (after spike check).
// spikeOut set when spike fires; otherwise lower/upper filled.
inline auto MeleeBounds(const double actorAttack, const double targetDefense, const bool isCritical, const bool applyLevelCorrection, const bool actorIsPC, const int actorLevel, const int targetLevel, const double weaponCap, const int damageLimit, const int damageLimitP, const int spikeRoll, const int upperMaxCoin, double& lowerOut, double& upperOut, bool& spiked) -> void
{
    const double def       = targetDefense < 1.0 ? 1.0 : targetDefense;
    const double baseRatio = def != 0.0 ? actorAttack / def : 0.0;
    const double levelDif  = LevelDifFactor(actorLevel, targetLevel, applyLevelCorrection, actorIsPC, false);
    const double wRatio    = baseRatio + (isCritical ? 1.0 : 0.0);
    const double pDifFinalCap = actorIsPC ? FinalCapPC(weaponCap, damageLimit, damageLimitP, isCritical)
                                          : FinalCapMeleeOthers(applyLevelCorrection, isCritical, damageLimit, damageLimitP);

    if (static_cast<double>(spikeRoll) / 10000.0 <= SpikeRatio(actorIsPC, wRatio))
    {
        spiked = true;
        return;
    }
    spiked = false;
    double lower = 0.0;
    double upper = 0.0;
    if (actorIsPC)
    {
        WRatioCapPC(wRatio, pDifFinalCap, lower, upper);
    }
    else
    {
        WRatioCapOthers(wRatio, pDifFinalCap, lower, upper);
    }
    const double upperMax = (upperMaxCoin == 0) ? 0.5 : 0.0;
    upperOut              = std::max(upper + levelDif, upperMax);
    lowerOut              = std::max(lower + levelDif, 0.0);
}

inline auto RangedBounds(const double actorAttack, const double targetDefense, const bool applyLevelCorrection, const bool actorIsPC, const int actorLevel, const int targetLevel, const double weaponCap, const int damageLimit, const int damageLimitP, double& lowerOut, double& upperOut) -> void
{
    const double def       = targetDefense < 1.0 ? 1.0 : targetDefense;
    const double baseRatio = def != 0.0 ? actorAttack / def : 0.0;
    const double levelDif  = LevelDifFactor(actorLevel, targetLevel, applyLevelCorrection, actorIsPC, true);
    double       cRatio    = ClampFloat(baseRatio, RangedCRatioMin, RangedCRatioMax);
    const double pDifFinalCap = actorIsPC ? FinalCapPC(weaponCap, damageLimit, damageLimitP, false)
                                          : FinalCapRangedOthers(applyLevelCorrection, damageLimit, damageLimitP);

    double lower = 0.0;
    double upper = 0.0;
    if (cRatio < 0.9)
    {
        upper = cRatio * 10.0 / 9.0;
        lower = cRatio;
    }
    else if (cRatio < 1.1)
    {
        upper = 1.0;
        lower = 1.0;
    }
    else
    {
        upper = std::min(cRatio, pDifFinalCap);
        lower = std::min(cRatio * 20.0 / 19.0 - 3.0 / 19.0, pDifFinalCap);
    }
    lowerOut = lower + levelDif;
    upperOut = upper + levelDif;
}

inline auto FinishRangedPDIF(const double ratioRollDiv1000, const bool isCritical, const int critDmgIncrease, const int critDefBonus) -> double
{
    double pDif = std::max(ratioRollDiv1000, 0.0);
    if (isCritical)
    {
        pDif *= RangedCritMult;
        pDif *= CritDamageMult(critDmgIncrease, critDefBonus);
    }
    return pDif;
}

inline auto FinishMeleePDIF(const double ratioRollDiv1000, const int meleeRandStep, const bool isCritical, const int critDmgIncrease, const int critDefBonus) -> double
{
    double pDif = ratioRollDiv1000 * (1.0 + static_cast<double>(ClampInt(meleeRandStep, 0, 5)) * 0.01);
    if (isCritical)
    {
        pDif *= CritDamageMult(critDmgIncrease, critDefBonus);
    }
    return pDif;
}

// Inclusive integer range for math.random(lo*1000, hi*1000). If inverted, swap.
inline auto RatioRollRange(const double lower, const double upper, int& lo, int& hi) -> bool
{
    if (upper <= 0.0 && lower <= 0.0)
    {
        return false;
    }
    lo = static_cast<int>(std::floor(lower * 1000.0));
    hi = static_cast<int>(std::floor(upper * 1000.0));
    if (lo > hi)
    {
        std::swap(lo, hi);
    }
    return true;
}

} // namespace pdifhelpers
