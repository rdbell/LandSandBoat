#include "test_targetfind_mob_owner_3928.h"
#include "map/ai/helpers/targetfind_mob_owner_capacity.h"
#include <iostream>
auto runTargetfindMobOwner3928SelfTests() -> bool
{
    using targetfindmobownerhelpers::IsMobOwner;
    const bool ok = IsMobOwner(false, false, false, false, false, false, false) &&
                    IsMobOwner(true, true, false, false, false, false, false) &&
                    IsMobOwner(true, false, true, false, false, false, false) &&
                    IsMobOwner(true, false, false, true, false, false, false) &&
                    IsMobOwner(true, false, false, false, true, true, false) &&
                    IsMobOwner(true, false, false, false, false, false, true) &&
                    !IsMobOwner(true, false, false, false, false, false, false);
    if (!ok) std::cerr << "targetfind mob owner 3928 self-test failed\n";
    return ok;
}
