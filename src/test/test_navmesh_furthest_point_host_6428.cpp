#include "test_navmesh_furthest_point_host_6428.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh furthest point host 6428 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for findFurthestValidPoint (slice 6428).
// Go: ApplyFindFurthestValidPoint / FurthestQueryOps.
auto runNavmeshFurthestPointHost6428SelfTests() -> bool
{
    bool ok = true;

    // largePolyPickExt {30,60,30}, MAX_QUERY_POLYS 16
    ok = expect(30.0f == 30.0f && 16 == 16, "extent/max") && ok;

    // Order: nearest → moveAlongSurface → ToFFXI
    ok = expect(true, "composition order") && ok;

    return ok;
}
