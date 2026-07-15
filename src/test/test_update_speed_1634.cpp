#include "test_update_speed_1634.h"

#include "map/update_speed_capacity.h"

#include <iostream>

auto runUpdateSpeed1634SelfTests() -> bool
{
    using namespace updatespeedhelpers;

    bool ok = true;

    // Mounted: (50/2) * 1.0 = 25
    {
        UpdateSpeedParams p{};
        p.isMounted          = true;
        p.mountSpeedSetting  = 50;
        ok                   = ok && ResolveUpdateSpeed(p) == 25;
        p.mountMoveMod       = 100; // +100% → 25 * 2 = 50
        ok                   = ok && ResolveUpdateSpeed(p) == 50;
    }

    // baseSpeed 0 → 0
    {
        UpdateSpeedParams p{};
        p.baseSpeed = 0;
        ok          = ok && ResolveUpdateSpeed(p) == 0;
    }

    // Override negative → 0
    {
        UpdateSpeedParams p{};
        p.baseSpeed         = 40;
        p.moveSpeedOverride = -1;
        ok                  = ok && ResolveUpdateSpeed(p) == 0;
    }

    // Override positive (GM)
    {
        UpdateSpeedParams p{};
        p.baseSpeed         = 40;
        p.moveSpeedOverride = 200;
        ok                  = ok && ResolveUpdateSpeed(p) == 200;
    }

    // Simple walk PC base 40, no mods, speed limit 80
    {
        UpdateSpeedParams p{};
        p.baseSpeed  = 40;
        p.isPC       = true;
        p.speedLimit = 80;
        ok           = ok && ResolveUpdateSpeed(p) == 40;
    }

    // Flee +100% (10000/10000) → *2
    {
        UpdateSpeedParams p{};
        p.baseSpeed = 40;
        p.fleeMod   = 10000;
        ok          = ok && ResolveUpdateSpeed(p) == 80;
    }

    // PC speed limit clamp
    {
        UpdateSpeedParams p{};
        p.baseSpeed  = 40;
        p.fleeMod    = 10000;
        p.isPC       = true;
        p.speedLimit = 50;
        ok           = ok && ResolveUpdateSpeed(p) == 50;
    }

    // Mob run multiplier 2.5
    {
        UpdateSpeedParams p{};
        p.baseSpeed        = 40;
        p.run              = true;
        p.isMob            = true;
        p.mobRunMultiplier = 2.5f;
        ok                 = ok && ResolveUpdateSpeed(p) == 100; // 40 * 2.5
    }

    // Mob run with weight penalty: 40 * (2.5 * 0.48) float→int16 truncates to 47
    {
        UpdateSpeedParams p{};
        p.baseSpeed             = 40;
        p.run                   = true;
        p.isMob                 = true;
        p.mobRunMultiplier      = 2.5f;
        p.mobWeightPenaltyMod   = 1;
        ok                      = ok && ResolveUpdateSpeed(p) == 47;
    }

    // Mount path ignores base walk formula
    {
        UpdateSpeedParams p{};
        p.isMounted         = true;
        p.mountSpeedSetting = 80;
        p.baseSpeed         = 40;
        p.fleeMod           = 10000;
        ok                  = ok && ResolveUpdateSpeed(p) == 40; // 80/2
    }

    // Run multiplier max with custom mob mod 300 → *3
    {
        UpdateSpeedParams p{};
        p.baseSpeed          = 40;
        p.run                = true;
        p.isMob              = true;
        p.mobRunMultiplier   = 2.5f;
        p.mobRunSpeedMultMod = 300;
        ok                   = ok && ResolveUpdateSpeed(p) == 120;
    }

    if (!ok)
    {
        std::cerr << "update speed 1634 self-test failed\n";
    }
    return ok;
}
