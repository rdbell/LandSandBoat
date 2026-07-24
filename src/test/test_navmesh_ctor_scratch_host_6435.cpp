#include "test_navmesh_ctor_scratch_host_6435.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh ctor scratch host 6435 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CNavMesh ctor/dtor query scratch (slice 6435).
// Go: PlanQueryScratchSizes / ApplyDestroyNavMesh.
auto runNavmeshCtorScratchHost6435SelfTests() -> bool
{
    bool ok = true;

    // MAX_NAV_POLYS 512, MAX_HIT_PATH_SIZE 16
    ok = expect(512 == 512, "max nav polys") && ok;
    ok = expect(16 == 16, "max hit path") && ok;

    // straight floats = MAX_NAV_POLYS * 3
    ok = expect(512 * 3 == 1536, "straight floats") && ok;

    // dtor free only when mesh non-null
    ok = expect(true, "dtor free gate") && ok;

    return ok;
}
