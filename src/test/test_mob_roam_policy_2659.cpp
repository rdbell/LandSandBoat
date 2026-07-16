#include "test_mob_roam_policy_2659.h"
#include "map/entities/mob_roam_policy.h"
#include <iostream>
namespace
{
auto expect(bool actual, bool expected, const char* label) -> bool { if (actual != expected) { std::cerr << "mob roam policy 2658 failed: " << label << '\n'; return false; } return true; }
}
auto runMobRoamPolicy2659SelfTests() -> bool
{
    bool ok = true;
    ok = expect(mobroamhelpers::CanRoam(false, false, true, false, false), true, "ordinary roaming") && ok;
    ok = expect(mobroamhelpers::CanRoam(true, false, true, false, false), false, "scripted") && ok;
    ok = expect(mobroamhelpers::CanRoam(false, true, true, false, false), false, "mastered") && ok;
    ok = expect(mobroamhelpers::CanRoam(false, false, false, false, false), false, "stationary") && ok;
    ok = expect(mobroamhelpers::CanRoam(false, false, false, true, false), true, "worm roams without speed") && ok;
    ok = expect(mobroamhelpers::CanRoam(false, false, true, true, true), false, "no-move overrides") && ok;
    return ok;
}
