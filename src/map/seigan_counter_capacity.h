#pragma once

#include <cstdint>

// Pure xi.combat.counter.checkSeiganCounter.
// Parity: internal/seigancounter

namespace seigancounterhelpers
{

constexpr int BaseCounterRate = 25;
constexpr std::uint8_t FacingCone = 64;

// CheckSeiganCounter inject form after status/facing/weapon/hit-rate/roll known.
// roll1to100 is math.random(1,100); compare uses float base*hitRateFactor.
inline auto CheckSeiganCounter(const bool hasThirdEye, const bool hasSeigan, const bool isFacing64, const bool isEngaged, const bool isPC, const bool isWeaponTwoHanded, const int thirdEyeCounterRateMod, const double hitRateFactor, const int roll1to100) -> bool
{
    if (!hasThirdEye)
    {
        return false;
    }
    if (!hasSeigan)
    {
        return false;
    }
    if (!isFacing64)
    {
        return false;
    }
    if (!isEngaged)
    {
        return false;
    }
    if (isPC && !isWeaponTwoHanded)
    {
        return false;
    }

    const int    baseCounterRate = BaseCounterRate + thirdEyeCounterRateMod;
    const double threshold       = static_cast<double>(baseCounterRate) * hitRateFactor;
    return static_cast<double>(roll1to100) <= threshold;
}

} // namespace seigancounterhelpers
