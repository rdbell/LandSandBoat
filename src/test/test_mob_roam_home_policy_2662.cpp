#include "test_mob_roam_home_policy_2662.h"
#include "map/entities/mob_roam_home_policy.h"
#include <iostream>
namespace { auto expect(bool a, bool w, const char* l) -> bool { if (a != w) { std::cerr << "mob roam-home 2662 failed: " << l << '\n'; return false; } return true; } }
auto runMobRoamHomePolicy2662SelfTests() -> bool
{
    bool ok = true;
    ok = expect(mobroamhomehelpers::CanRoamHome(false, false, false, false, false, true), false, "stationary") && ok;
    ok = expect(mobroamhomehelpers::CanRoamHome(true, false, true, true, true, true), false, "no move overrides") && ok;
    ok = expect(mobroamhomehelpers::CanRoamHome(false, true, false, false, false, false), false, "worm needs distance") && ok;
    ok = expect(mobroamhomehelpers::CanRoamHome(true, false, false, true, false, false), true, "mob no despawn") && ok;
    ok = expect(mobroamhomehelpers::CanRoamHome(true, false, false, false, true, false), true, "global no despawn") && ok;
    ok = expect(mobroamhomehelpers::CanRoamHome(true, false, false, false, false, true), true, "within distance") && ok;
    return ok;
}
