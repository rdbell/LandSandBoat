#include "test_pathfind_resume_patrol_6341.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind ResumePatrol 6341 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CPathFind::ResumePatrol admission (slice 6341).
auto runPathfindResumePatrol6341SelfTests() -> bool
{
    using pathfindstatushelpers::ShouldPrunePath;
    using pathfindstatushelpers::ShouldResumePatrol;
    using pathfindstatushelpers::ValidPosition;

    bool ok = true;

    ok = expect(ShouldResumePatrol(true) && !ShouldResumePatrol(false), "identity") && ok;
    ok = expect(ShouldPrunePath(true) && ValidPosition(true), "6340 residual") && ok;

    return ok;
}
