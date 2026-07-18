#include "test_targetfind_allegiance_3921.h"
#include "map/ai/helpers/targetfind_allegiance_capacity.h"
#include <iostream>
auto runTargetfindAllegiance3921SelfTests() -> bool
{
    using targetfindallegiancehelpers::ShouldRejectInitialTargetAllegiance;
    const bool ok = !ShouldRejectInitialTargetAllegiance(false, false) &&
                    !ShouldRejectInitialTargetAllegiance(false, true) &&
                    !ShouldRejectInitialTargetAllegiance(true, false) &&
                    ShouldRejectInitialTargetAllegiance(true, true);
    if (!ok) std::cerr << "targetfind allegiance 3921 self-test failed\n";
    return ok;
}
