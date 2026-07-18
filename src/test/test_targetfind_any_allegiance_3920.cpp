#include "test_targetfind_any_allegiance_3920.h"
#include "map/ai/helpers/targetfind_any_allegiance_capacity.h"
#include <iostream>
auto runTargetfindAnyAllegiance3920SelfTests() -> bool
{
    using targetfindanyallegiancehelpers::ShouldRejectAnyAllegianceSelf;
    const bool ok = ShouldRejectAnyAllegianceSelf(true, true) &&
                    !ShouldRejectAnyAllegianceSelf(true, false) &&
                    !ShouldRejectAnyAllegianceSelf(false, true) &&
                    !ShouldRejectAnyAllegianceSelf(false, false);
    if (!ok) std::cerr << "targetfind any allegiance 3920 self-test failed\n";
    return ok;
}
