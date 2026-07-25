#include "test_aid_block_6854.h"

#include "map/aid_block_capacity.h"

#include <iostream>

auto runAidBlock6854SelfTests() -> bool
{
    const bool ok = !aidblockhelpers::IsAidBlocked(false, false) &&
                    !aidblockhelpers::IsAidBlocked(false, true) &&
                    !aidblockhelpers::IsAidBlocked(true, true) &&
                    aidblockhelpers::IsAidBlocked(true, false);
    if (!ok)
    {
        std::cerr << "aid block 6854 self-test failed\\n";
    }
    return ok;
}
