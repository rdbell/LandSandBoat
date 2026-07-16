#include "test_mob_behavior_policy_2655.h"

#include "map/entities/mob_behavior_policy.h"

#include <iostream>

namespace
{
auto expect(const bool actual, const bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mob behavior policy 2655 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}
} // namespace

auto runMobBehaviorPolicy2655SelfTests() -> bool
{
    bool ok = true;
    ok      = expect(mobbehaviorhelpers::CanDeaggro(false, false), true, "ordinary mob deaggroes") && ok;
    ok      = expect(mobbehaviorhelpers::CanDeaggro(true, false), false, "notorious mob does not deaggro") && ok;
    ok      = expect(mobbehaviorhelpers::CanDeaggro(false, true), false, "battlefield mob does not deaggro") && ok;
    ok      = expect(mobbehaviorhelpers::CanDeaggro(true, true), false, "combined flags do not deaggro") && ok;
    ok      = expect(mobbehaviorhelpers::CanBeNeutral(false), true, "ordinary mob can be neutral") && ok;
    ok      = expect(mobbehaviorhelpers::CanBeNeutral(true), false, "notorious mob cannot be neutral") && ok;
    return ok;
}
