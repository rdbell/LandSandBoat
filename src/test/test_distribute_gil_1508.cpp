#include "test_distribute_gil_1508.h"

#include "map/distribute_gil_capacity.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

namespace
{
using distributegilhelpers::AllMobsGilBonusAmount;
using distributegilhelpers::ApplyAllMobsGilBonus;
using distributegilhelpers::ApplyGilfinder;
using distributegilhelpers::ApplyKillshotGil;
using distributegilhelpers::ApplyMobGilMultiplier;
using distributegilhelpers::ClampAllMobsGilBonus;
using distributegilhelpers::GilfinderMultiplier;
using distributegilhelpers::GilfinderRollMax;
using distributegilhelpers::GilPerPerson;
using distributegilhelpers::IsGilShareMemberEligible;
using distributegilhelpers::KillshotGilMultiplier;
using distributegilhelpers::ShouldApplyAllMobsGilBonus;
using distributegilhelpers::ShouldApplyGilfinder;
using distributegilhelpers::ShouldApplyKillshotGilBonus;
using distributegilhelpers::ShouldApplyMobGilMultiplier;
using distributegilhelpers::ShouldAwardSoloGil;

auto nearly(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-9;
}

auto Check() -> bool
{
    if (!ShouldApplyMobGilMultiplier(10, 1.0f) || ShouldApplyMobGilMultiplier(0, 1.0f) || ShouldApplyMobGilMultiplier(10, -0.1f))
    {
        return false;
    }
    if (ApplyMobGilMultiplier(100, 1.5f) != 150)
    {
        return false;
    }
    if (!ShouldApplyAllMobsGilBonus(5) || ShouldApplyAllMobsGilBonus(0))
    {
        return false;
    }
    if (AllMobsGilBonusAmount(2, 50) != 100 || ClampAllMobsGilBonus(100, 50) != 50 || ClampAllMobsGilBonus(0, 50) != 1)
    {
        return false;
    }
    if (ApplyAllMobsGilBonus(200, 50) != 250)
    {
        return false;
    }
    if (!ShouldApplyGilfinder(3) || ShouldApplyGilfinder(0) || GilfinderRollMax(3) != 48)
    {
        return false;
    }
    if (!nearly(GilfinderMultiplier(0), 1.0 + 128.0 / 256.0) || !nearly(GilfinderMultiplier(128), 1.0 + 256.0 / 256.0))
    {
        return false;
    }
    if (ApplyGilfinder(100, 1.5) != 150)
    {
        return false;
    }
    if (!ShouldApplyKillshotGilBonus(25) || ShouldApplyKillshotGilBonus(0))
    {
        return false;
    }
    if (!nearly(KillshotGilMultiplier(25), 1.25) || ApplyKillshotGil(100, 1.25) != 125)
    {
        return false;
    }
    if (!IsGilShareMemberEligible(true, true) || IsGilShareMemberEligible(true, false) || IsGilShareMemberEligible(false, true))
    {
        return false;
    }
    if (GilPerPerson(100, 4) != 25 || GilPerPerson(100, 0) != 0)
    {
        return false;
    }
    const auto maxGil = std::numeric_limits<std::uint32_t>::max();
    if (GilPerPerson(maxGil, -1) != static_cast<std::uint32_t>(maxGil / static_cast<std::size_t>(-1)))
    {
        return false;
    }
    if (!ShouldAwardSoloGil(false, true) || ShouldAwardSoloGil(true, true) || ShouldAwardSoloGil(false, false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runDistributeGil1508SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "distribute gil 1508 self-test failed\n";
    }
    return ok;
}
