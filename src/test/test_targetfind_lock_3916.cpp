#include "test_targetfind_lock_3916.h"
#include "map/ai/helpers/targetfind_lock_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind lock 3916 failed: " << l << '\n'; return v; } }
auto runTargetfindLock3916SelfTests() -> bool
{
    using targetfindlockhelpers::ShouldRejectAIOrLocked;
    bool ok = true;
    ok = expect(!ShouldRejectAIOrLocked(false, false, false), "ordinary non-character allowed") && ok;
    ok = expect(ShouldRejectAIOrLocked(true, false, false), "AI untargetable") && ok;
    ok = expect(ShouldRejectAIOrLocked(false, true, true), "locked character") && ok;
    ok = expect(!ShouldRejectAIOrLocked(false, true, false), "unlocked character allowed") && ok;
    ok = expect(!ShouldRejectAIOrLocked(false, false, true), "non-character lock ignored") && ok;
    return ok;
}
