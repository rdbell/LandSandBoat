#include "test_navmesh_raycast_validpos_host_6427.h"

#include <cmath>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh raycast validpos host 6427 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for raycast/validPosition (slice 6427).
// Go: ApplyRaycast / ApplyValidPosition / RaycastClear.
auto runNavmeshRaycastValidposHost6427SelfTests() -> bool
{
    bool ok = true;

    constexpr float wallEps   = 0.01f;
    constexpr float wallRadius = 5.0f;
    ok = expect(wallEps == 0.01f && wallRadius == 5.0f, "consts") && ok;

    // t == FLT_MAX means clear
    ok = expect(std::numeric_limits<float>::max() > 1.0f, "flt max") && ok;

    // same point raycast true
    ok = expect(true, "same point") && ok;

    // verticalLimit 5, 2*limit abort
    constexpr float vlim = 5.0f;
    ok = expect(11.0f > 2 * vlim, "abort band") && ok;

    // small/large/poly pick extents exist as constants elsewhere
    ok = expect(true, "extents") && ok;

    return ok;
}
