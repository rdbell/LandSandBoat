#include "test_targetfind_context_3913.h"
#include "map/ai/helpers/targetfind_context_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind context 3913 failed: " << l << '\n'; return v; } }
auto runTargetfindContext3913SelfTests() -> bool
{
    using targetfindcontexthelpers::ShouldRejectContext;
    bool ok = true;
    ok = expect(!ShouldRejectContext(false, false, false, true, false), "matching context accepted") && ok;
    ok = expect(ShouldRejectContext(true, false, false, false, false), "confrontation mismatch") && ok;
    ok = expect(ShouldRejectContext(false, true, false, false, false), "battlefield mismatch") && ok;
    ok = expect(ShouldRejectContext(false, false, true, false, false), "instance mismatch") && ok;
    ok = expect(ShouldRejectContext(false, false, false, true, true), "checked battle ID mismatch") && ok;
    ok = expect(!ShouldRejectContext(false, false, false, false, true), "ignored battle ID mismatch") && ok;
    ok = expect(ShouldRejectContext(true, false, false, false, true), "ignore battle ID does not ignore confrontation") && ok;
    return ok;
}
