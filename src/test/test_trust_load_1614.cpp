#include "test_trust_load_1614.h"

#include "map/trust_load_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace trustloadhelpers;

auto nearly(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-6;
}

auto Check() -> bool
{
    if (MainLevel(75) != 75 || SubLevel(75) != 37 || SubLevel(1) != 0)
    {
        return false;
    }
    if (!nearly(SpawnRadius(0), 3.0) || !nearly(SpawnRadius(2), 9.0) || !nearly(SpawnRadius(-1), 3.0))
    {
        return false;
    }
    // max(100 * 0.5 * 100/100, 1) = 50
    if (WeaponDamage(100, 100) != 50)
    {
        return false;
    }
    // max(10 * 0.5 * 50/100, 1) = max(2.5, 1) = 2
    if (WeaponDamage(10, 50) != 2)
    {
        return false;
    }
    // max(1 * 0.5 * 10/100, 1) = max(0.05, 1) = 1
    if (WeaponDamage(1, 10) != 1)
    {
        return false;
    }
    if (!nearly(WeaponDPS(50, 1000), 50.0) || WeaponDPS(50, 0) != 0.0)
    {
        return false;
    }
    if (!DualWieldFromCmbSkill(SkillHandToHand) || DualWieldFromCmbSkill(2))
    {
        return false;
    }
    // Lv3 TP skill gate (slice 1615, same capacity).
    // Gravitation = 9; non-Lv3 always allowed.
    if (!CanUseTPSkill(1, 1, 0, 0, 0))
    {
        return false;
    }
    // Lv3 at low level with empty list: special case onlyHasLv3
    if (!CanUseTPSkill(30, SkillchainGravitation, 0, 0, 0))
    {
        return false;
    }
    // Lv3 at low level with existing skills: blocked
    if (CanUseTPSkill(30, SkillchainGravitation, 0, 0, 1))
    {
        return false;
    }
    // Lv3 at 60+ always allowed
    if (!CanUseTPSkill(60, SkillchainGravitation, 0, 0, 5))
    {
        return false;
    }
    return true;
}
} // namespace

auto runTrustLoad1614SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "trust_load_1614 self-tests failed\n";
        return false;
    }
    return true;
}
