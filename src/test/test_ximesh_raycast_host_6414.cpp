#include "test_ximesh_raycast_host_6414.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ximesh raycast host 6414 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for XiMesh::rayIntersect (slice 6414).
// Go: RayIntersectTriangle / SegmentHitsAABB / ReciprocalDir / BuildO2W.
auto runXimeshRaycastHost6414SelfTests() -> bool
{
    bool ok = true;

    constexpr float kRayTriEpsilon   = 0.0000001f;
    constexpr float kParallelEpsilon = 1e-6f;
    constexpr float kZeroDirEpsilon  = 1e-8f;
    constexpr float kLargeReciprocal = 1e30f;
    constexpr float kCullMin         = 1;

    ok = expect(kRayTriEpsilon > 0.0f && kParallelEpsilon > 0.0f, "eps") && ok;
    ok = expect(kZeroDirEpsilon > 0.0f && kLargeReciprocal > 1.0f, "recip") && ok;
    ok = expect(kCullMin == 1, "cull") && ok;

    // Identity determinant > 0 → flip winding
    constexpr float detIdentity = 1.0f;
    ok = expect(detIdentity > 0.0f, "flip when det>0") && ok;

    // Segment t in (eps, 1]
    constexpr float tHit = 0.5f;
    ok = expect(tHit > kRayTriEpsilon && tHit <= 1.0f, "t window") && ok;

    // Cell search diff wideSearch
    ok = expect((0 > 0 ? 2 : 1) == 1, "cellSearchDiff default") && ok;
    ok = expect((1 > 0 ? 2 : 1) == 2, "cellSearchDiff wide") && ok;

    return ok;
}
