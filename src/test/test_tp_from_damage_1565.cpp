#include "test_tp_from_damage_1565.h"

#include "map/tp_from_damage_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace tpfromdamagehelpers;

auto NearlyEqual(const double a, const double b, const double eps = 1e-9) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    // Modified delay: single swing canZanshin, DELAYP 0
    {
        ModifiedDelayParams p{};
        p.delay = 480;
        const auto r = GetModifiedDelayAndCanZanshin(p);
        if (r.delay != 480 || !r.canZanshin)
        {
            return false;
        }
    }
    // Dual wield 0% DW: half delay, no zanshin
    {
        ModifiedDelayParams p{};
        p.delay     = 480;
        p.dualWield = true;
        p.dualWieldMod = 0;
        const auto r = GetModifiedDelayAndCanZanshin(p);
        if (r.delay != 240 || r.canZanshin)
        {
            return false;
        }
    }

    // ApplyStoreTP
    if (ApplyStoreTP(100, 0) != 100 || ApplyStoreTP(100, 50) != 150)
    {
        return false;
    }

    // Single melee: PC 480 → 134; zanshin+30
    {
        const auto base = static_cast<std::int32_t>(tpreturnhelpers::CalculateTPReturn(true, 480));
        if (base != 134)
        {
            return false;
        }
        if (SingleMeleeHitTPReturn(true, true, true, 480, true, 30, 0) != 0)
        {
            return false;
        }
        if (SingleMeleeHitTPReturn(false, true, true, 480, true, 30, 0) != base + 30)
        {
            return false;
        }
        if (SingleMeleeHitTPReturn(false, true, true, 480, false, 30, 0) != base)
        {
            return false;
        }
    }

    // Single ranged
    if (SingleRangedHitTPReturn(false, true, 0, 0) != 0 ||
        SingleRangedHitTPReturn(true, true, 360, 0) != 0)
    {
        return false;
    }
    {
        const auto base = static_cast<std::int32_t>(tpreturnhelpers::CalculateTPReturn(true, 360));
        if (SingleRangedHitTPReturn(false, true, 360, 0) != base)
        {
            return false;
        }
    }

    // dAGI Lua precedence ~1
    if (!NearlyEqual(DAGIModifierLua(0), 1.0) || !NearlyEqual(DAGIModifierLua(70), 1.0))
    {
        return false;
    }

    // Physical TP gain player→mob
    {
        PhysicalTPGainParams p{};
        p.baseTPGain  = 100;
        p.targetIsMob = true;
        p.actorIsMob  = false;
        if (PhysicalTPGain(p) != 130)
        {
            return false;
        }
    }
    {
        PhysicalTPGainParams p{};
        p.baseTPGain  = 100;
        p.targetIsMob = true;
        p.actorIsMob  = true;
        if (PhysicalTPGain(p) != 33)
        {
            return false;
        }
    }

    if (!ShouldZeroPhysicalTPGain(true, 10, false) || !ShouldZeroPhysicalTPGain(false, 0, false) ||
        !ShouldZeroPhysicalTPGain(false, 10, true) ||
        ShouldZeroPhysicalTPGain(false, 10, false))
    {
        return false;
    }

    return true;
}
} // namespace

auto runTPFromDamage1565SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "tp_from_damage_1565 self-tests failed\n";
        return false;
    }
    return true;
}
