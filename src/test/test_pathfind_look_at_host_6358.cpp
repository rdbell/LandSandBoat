#include "test_pathfind_look_at_host_6358.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind LookAt host 6358 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for LookAtWithinDistance + residual admission (slice 6358).
auto runPathfindLookAtHost6358SelfTests() -> bool
{
    using pathfindstatushelpers::HasNextRoamTurn;
    using pathfindstatushelpers::LookAtWithinDistance;
    using pathfindstatushelpers::ShouldUpdateLookAt;

    bool ok = true;

    ok = expect(LookAtWithinDistance() == 0.1f, "within 0.1") && ok;
    ok = expect(ShouldUpdateLookAt(false), "not within → update") && ok;
    ok = expect(!ShouldUpdateLookAt(true), "within → skip") && ok;
    ok = expect(HasNextRoamTurn(0, 1), "6357 residual") && ok;

    return ok;
}
