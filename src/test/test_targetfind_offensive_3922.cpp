#include "test_targetfind_offensive_3922.h"
#include "map/ai/helpers/targetfind_offensive_capacity.h"
#include <iostream>
auto runTargetfindOffensive3922SelfTests() -> bool
{
    using targetfindoffensivehelpers::ShouldRejectOffensiveAlly;
    const bool ok = !ShouldRejectOffensiveAlly(false, false, true) &&
                    !ShouldRejectOffensiveAlly(true, false, false) &&
                    ShouldRejectOffensiveAlly(true, false, true) &&
                    !ShouldRejectOffensiveAlly(true, true, true);
    if (!ok) std::cerr << "targetfind offensive 3922 self-test failed\n";
    return ok;
}
