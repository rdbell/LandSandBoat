#include "test_targetfind_first_target_3919.h"
#include "map/ai/helpers/targetfind_first_target_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind first target 3919 failed: " << l << '\n'; return v; } }
auto runTargetfindFirstTarget3919SelfTests() -> bool
{
    using targetfindfirsttargethelpers::ShouldAcceptFirstTarget;
    bool ok = true;
    ok = expect(ShouldAcceptFirstTarget(false, false, false), "ordinary first target") && ok;
    ok = expect(!ShouldAcceptFirstTarget(true, false, false), "existing initial target") && ok;
    ok = expect(!ShouldAcceptFirstTarget(false, true, false), "self centered requires range validation") && ok;
    ok = expect(ShouldAcceptFirstTarget(false, true, true), "conal self centered admits main target") && ok;
    return ok;
}
