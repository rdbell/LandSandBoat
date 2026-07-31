#include "test_combat_status_mitigation_1564.h"

#include "map/combat_status_mitigation_capacity.h"

#include <iostream>

namespace
{
using namespace combatstatusmitigationhelpers;

auto Check() -> bool
{
    // Shared int32 clamp used by damage-cap tails.
    if (ClampI32(-120, -100, 50) != -100 || ClampI32(-100, -100, 50) != -100 ||
        ClampI32(25, -100, 50) != 25 || ClampI32(50, -100, 50) != 50 || ClampI32(75, -100, 50) != 50)
    {
        return false;
    }

    // One For All
    if (HandleOneForAll(100, 30, true) != 70 || HandleOneForAll(20, 30, true) != 0 ||
        HandleOneForAll(100, 30, false) != 100 || HandleOneForAll(0, 30, true) != 0)
    {
        return false;
    }

    // Stoneskin
    {
        const auto r = HandleStoneskin(50, 80);
        if (r.remainingDamage != 0 || r.skinLeft != 30 || r.removeEffect)
        {
            return false;
        }
    }
    {
        const auto r = HandleStoneskin(80, 50);
        if (r.remainingDamage != 30 || r.skinLeft != 0 || !r.removeEffect)
        {
            return false;
        }
    }
    {
        const auto r = HandleStoneskin(50, 50);
        if (r.remainingDamage != 0 || r.skinLeft != 0 || !r.removeEffect)
        {
            return false;
        }
    }

    // Damage cap
    if (CheckAndApplyDamageCap(50, 100, 10, 0) != 50)
    {
        return false;
    }
    if (CheckAndApplyDamageCap(200, 100, 10, 5) != 95)
    {
        return false;
    }
    // variant > cap fail-safe → variant 0, subtract ignored in clamp range
    if (CheckAndApplyDamageCap(200, 100, 150, 0) != 100)
    {
        return false;
    }

    // Store TP merit
    if (StoreTPBonusFromMerit(true, true, 15) != 15 || StoreTPBonusFromMerit(true, false, 15) != 0 ||
        StoreTPBonusFromMerit(false, true, 15) != 0)
    {
        return false;
    }

    // Fan Dance: power 3000 → resist 0.7, new power 2000
    {
        const auto r = HandleFanDance(1000, 3000, true);
        if (r.newDamage != 700 || r.newPower != 2000)
        {
            return false;
        }
    }
    {
        const auto r = HandleFanDance(1000, 1500, true);
        if (r.newDamage != 850 || r.newPower != 1500)
        {
            return false;
        }
    }

    // Scarlet Delirium power
    if (ScarletDeliriumPower(1000, 1000) != 500 || ScarletDeliriumPower(100, 1000) != 50)
    {
        return false;
    }
    if (ScarletDeliriumDurationSec(10) != 100)
    {
        return false;
    }

    // Severe damage: Migawari-like 100% reduce
    {
        const auto r = HandleSevereDamageEffect(500, 1000, 20, 100, true);
        // threshold 200; 500 > 200; factor 0 → 0
        if (r.newDamage != 0 || !r.triggered)
        {
            return false;
        }
    }
    {
        const auto r = HandleSevereDamageEffect(100, 1000, 20, 100, true);
        if (r.newDamage != 100 || r.triggered)
        {
            return false;
        }
    }

    // Schurzen
    if (ApplySchurzenCap(100, 100, true, true, true) != 99 ||
        ApplySchurzenCap(500, 100, true, true, true) != 99 ||
        ApplySchurzenCap(99, 100, true, true, true) != 99)
    {
        return false;
    }
    if (ApplySchurzenCap(500, 100, false, true, true) != 500 ||
        ApplySchurzenCap(500, 100, true, false, true) != 500 ||
        ApplySchurzenCap(500, 100, true, true, false) != 500)
    {
        return false;
    }

    return true;
}
} // namespace

auto runCombatStatusMitigation1564SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "combat_status_mitigation_1564 self-tests failed\n";
        return false;
    }
    return true;
}
