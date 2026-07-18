#include "test_targetfind_candidate_3914.h"
#include "map/ai/helpers/targetfind_candidate_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind candidate 3914 failed: " << l << '\n'; return v; } }
auto runTargetfindCandidate3914SelfTests() -> bool
{
    using targetfindcandidatehelpers::ShouldRejectDuplicateOrDead;
    bool ok = true;
    int checks = 0;
    auto dead = [&checks]() { ++checks; return true; };
    ok = expect(ShouldRejectDuplicateOrDead(true, false, dead), "duplicate rejected") && ok;
    ok = expect(checks == 0, "duplicate skips dead query") && ok;
    ok = expect(ShouldRejectDuplicateOrDead(false, false, dead), "dead rejected when not included") && ok;
    ok = expect(checks == 1, "dead queried once") && ok;
    ok = expect(!ShouldRejectDuplicateOrDead(false, true, dead), "dead allowed by flag") && ok;
    ok = expect(checks == 1, "include-dead skips dead query") && ok;
    return ok;
}
