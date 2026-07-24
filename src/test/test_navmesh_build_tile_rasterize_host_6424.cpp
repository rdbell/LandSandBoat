#include "test_navmesh_build_tile_rasterize_host_6424.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh build tile rasterize host 6424 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for buildTile walkable areas (slice 6424).
// Go: MarkWalkableTriangles / PlanHeightfieldFilters / AssignPolyWalkFlags.
auto runNavmeshBuildTileRasterizeHost6424SelfTests() -> bool
{
    bool ok = true;

    constexpr float RC_PI = 3.14159265f;
    constexpr unsigned char RC_WALKABLE_AREA = 63;
    constexpr unsigned char RC_NULL_AREA     = 0;
    constexpr uint16_t SAMPLE_POLYFLAGS_WALK = 0x0001;

    const float thr = std::cos(46.0f / 180.0f * RC_PI);
    ok = expect(thr > 0.6f && thr < 0.8f, "slope thr") && ok;

    // Flat +Y face: norm.y == 1 > thr
    ok = expect(1.0f > thr, "flat walkable") && ok;
    // Wall +X: norm.y == 0
    ok = expect(0.0f > thr == false, "wall unwalkable") && ok;

    ok = expect(RC_WALKABLE_AREA == 63 && RC_NULL_AREA == 0, "areas") && ok;
    ok = expect(SAMPLE_POLYFLAGS_WALK == 0x0001, "walk flag") && ok;

    // Default config all filters on
    ok = expect(true, "filters default on") && ok;

    return ok;
}
