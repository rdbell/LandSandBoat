#include "test_targetfind_ally_3918.h"
#include "map/ai/helpers/targetfind_ally_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind ally 3918 failed: " << l << '\n'; return v; } }
auto runTargetfindAlly3918SelfTests() -> bool
{
    using targetfindallyhelpers::ShouldRejectSelfCenteredAllyOnly;
    bool ok = true;
    int checks = 0;
    auto mismatch = [&checks]() { ++checks; return true; };
    ok = expect(!ShouldRejectSelfCenteredAllyOnly(false, false, false, mismatch), "non-self skips") && ok;
    ok = expect(!ShouldRejectSelfCenteredAllyOnly(true, true, false, mismatch), "any allegiance skips") && ok;
    ok = expect(!ShouldRejectSelfCenteredAllyOnly(true, false, true, mismatch), "enemy flag skips") && ok;
    ok = expect(checks == 0, "skipped paths do not resolve master") && ok;
    ok = expect(ShouldRejectSelfCenteredAllyOnly(true, false, false, mismatch), "ally-only mismatch rejects") && ok;
    ok = expect(checks == 1, "ally-only resolves master once") && ok;
    ok = expect(!ShouldRejectSelfCenteredAllyOnly(true, false, false, []() { return false; }), "matching allegiance allows") && ok;
    return ok;
}
