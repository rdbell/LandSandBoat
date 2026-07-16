#include "test_mob_valid_target_policy_2695.h"

#include "map/entities/mob_valid_target_policy.h"

#include <iostream>

auto runMobValidTargetPolicy2695SelfTests() -> bool
{
    using mobvalidtargethelpers::ValidTarget;
    int baseCalls = 0;
    const auto base = [&](const bool value) { return [&, value] { ++baseCalls; return value; }; };
    const bool ok =
        !ValidTarget(false, base(true), true, true, true, true, true, true, false, false) && // confrontation precedes every acceptance
        baseCalls == 0 &&
        ValidTarget(true, base(true), false, false, false, false, false, false, true, true) && // base target precedes mob-specific gates
        baseCalls == 1 &&
        ValidTarget(true, base(false), true, true, true, false, false, false, true, true) && // raisable dead mob
        !ValidTarget(true, base(false), true, false, true, false, false, true, false, false) && // not raisable
        !ValidTarget(true, base(false), true, true, false, false, false, true, false, false) && // not dead
        ValidTarget(true, base(false), false, false, false, true, false, true, false, false) && // assisting player
        ValidTarget(true, base(false), false, false, false, false, true, true, false, false) && // assisting NPC
        !ValidTarget(true, base(false), false, false, false, true, true, false, false, false) && // allegiance mismatch
        !ValidTarget(true, base(false), false, false, false, true, true, true, true, false) && // no assist
        !ValidTarget(true, base(false), false, false, false, true, true, true, false, true) && // charmed
        !ValidTarget(true, base(false), false, false, false, false, false, true, false, false);
    if (!ok)
    {
        std::cerr << "mob valid target policy 2695 failed\n";
    }
    return ok;
}
