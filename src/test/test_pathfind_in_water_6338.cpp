#include "test_pathfind_in_water_6338.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind InWater 6338 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CPathFind::InWater terrain membership (slice 6338).
auto runPathfindInWater6338SelfTests() -> bool
{
    using pathfindstatushelpers::AtPointThreshold;
    using pathfindstatushelpers::IsWaterTerrain;

    bool ok = true;

    ok = expect(!IsWaterTerrain(false, false), "neither") && ok;
    ok = expect(IsWaterTerrain(true, false), "shallow") && ok;
    ok = expect(IsWaterTerrain(false, true), "deep") && ok;
    ok = expect(IsWaterTerrain(true, true), "both") && ok;
    ok = expect(AtPointThreshold(0.f) == 0.1f, "6337 residual") && ok;

    return ok;
}
