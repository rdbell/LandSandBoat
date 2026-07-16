#include "test_mob_link_policy_2692.h"

#include "map/entities/mob_link_policy.h"

#include <iostream>

auto runMobLinkPolicy2692SelfTests() -> bool
{
    bool ok = true;
    auto test = [&](const char* name, const bool expected, const bool superLink, const bool neutral, const bool hiddenWorm, const bool hiddenAmbush,
                    const bool sightOnly, const bool facing, const bool inRange, const bool noLink, const bool visible, const int expectedFacingCalls,
                    const int expectedRangeCalls, const int expectedVisibilityCalls) {
        int facingCalls     = 0;
        int rangeCalls      = 0;
        int visibilityCalls = 0;
        const auto actual = moblinkhelpers::CanLink(superLink, neutral, hiddenWorm, hiddenAmbush, sightOnly,
            [&] { ++facingCalls; return facing; }, [&] { ++rangeCalls; return inRange; }, noLink, [&] { ++visibilityCalls; return visible; });
        if (actual != expected || facingCalls != expectedFacingCalls || rangeCalls != expectedRangeCalls || visibilityCalls != expectedVisibilityCalls)
        {
            std::cerr << "mob link 2692 failed: " << name << '\n';
            ok = false;
        }
    };

    test("super link overrides all", true, true, true, true, true, true, false, false, true, false, 0, 0, 0);
    test("neutral", false, false, true, false, false, false, true, true, false, true, 0, 0, 0);
    test("hidden worm", false, false, false, true, false, false, true, true, false, true, 0, 0, 0);
    test("hidden ambush", false, false, false, false, true, false, true, true, false, true, 0, 0, 0);
    test("sight only faces away", false, false, false, false, false, true, false, true, false, true, 1, 0, 0);
    test("sight and hearing skip facing", true, false, false, false, false, false, false, true, false, true, 0, 1, 1);
    test("outside radius", false, false, false, false, false, false, true, false, false, true, 0, 1, 0);
    test("no link", false, false, false, false, false, false, true, true, true, true, 0, 1, 0);
    test("cannot see", false, false, false, false, false, false, true, true, false, false, 0, 1, 1);
    test("eligible", true, false, false, false, false, true, true, true, false, true, 1, 1, 1);
    return ok;
}
