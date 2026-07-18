#include "test_targetfind_identity_3915.h"
#include "map/ai/helpers/targetfind_identity_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind identity 3915 failed: " << l << '\n'; return v; } }
auto runTargetfindIdentity3915SelfTests() -> bool
{
    using targetfindidentityhelpers::ShouldRejectIdentityOrVisibility;
    bool ok = true;
    ok = expect(!ShouldRejectIdentityOrVisibility(false, false, false, false), "ordinary target allowed") && ok;
    ok = expect(ShouldRejectIdentityOrVisibility(true, false, false, false), "self target") && ok;
    ok = expect(ShouldRejectIdentityOrVisibility(false, true, false, false), "wrong zone") && ok;
    ok = expect(ShouldRejectIdentityOrVisibility(false, false, true, false), "untargetable") && ok;
    ok = expect(ShouldRejectIdentityOrVisibility(false, false, false, true), "invisible") && ok;
    return ok;
}
