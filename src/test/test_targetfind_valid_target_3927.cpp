#include "test_targetfind_valid_target_3927.h"
#include "map/ai/helpers/targetfind_valid_target_capacity.h"
#include <iostream>
auto runTargetfindValidTarget3927SelfTests() -> bool
{
    using targetfindvalidtargethelpers::Resolve;
    const bool ok = Resolve(false, true, true, true, true, true) == targetfindvalidtargethelpers::Result::None &&
                    Resolve(true, true, true, false, false, false) == targetfindvalidtargethelpers::Result::Pet &&
                    Resolve(true, false, false, false, true, true) == targetfindvalidtargethelpers::Result::Candidate &&
                    Resolve(true, false, false, true, false, true) == targetfindvalidtargethelpers::Result::Candidate &&
                    Resolve(true, false, false, false, false, true) == targetfindvalidtargethelpers::Result::None;
    if (!ok) std::cerr << "targetfind valid target 3927 self-test failed\n";
    return ok;
}
