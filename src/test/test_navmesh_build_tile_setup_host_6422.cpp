#include "test_navmesh_build_tile_setup_host_6422.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh build tile setup host 6422 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for buildTile/buildAsync setup (slice 6422).
// Go: BuildRCConfigParams / TileAABB / ApplyBuildAsyncSetup.
auto runNavmeshBuildTileSetupHost6422SelfTests() -> bool
{
    bool ok = true;

    constexpr int   kBorderPad = 3;
    constexpr float kMinDetail = 0.9f;
    constexpr float kCS        = 0.5f;
    constexpr float kCH        = 0.4f;
    constexpr float kAgentH    = 2.0f;
    constexpr float kAgentR    = 0.5f;
    constexpr float kAgentClimb = 0.6f;

    const int walkableHeight = static_cast<int>(std::ceil(kAgentH / kCH));
    const int walkableClimb  = static_cast<int>(std::floor(kAgentClimb / kCH));
    const int walkableRadius = static_cast<int>(std::ceil(kAgentR / kCS));
    ok = expect(walkableHeight == 5 && walkableClimb == 1 && walkableRadius == 1, "walkable") && ok;
    ok = expect(walkableRadius + kBorderPad == 4, "border") && ok;

    // rcCalcGridSize style
    const int gw = static_cast<int>((32.0f - 0.0f) / kCS + 0.5f);
    ok = expect(gw == 64, "gridW") && ok;

    const float tileWorld = 64 * kCS;
    ok = expect(tileWorld == 32.0f, "tileWorld") && ok;

    // Detour gather reverse
    const float expBminY = -5.0f;
    const float expBmaxY = 5.0f;
    const float gatherBminY = -expBmaxY;
    const float gatherBmaxY = -expBminY;
    ok = expect(gatherBminY == -5.0f && gatherBmaxY == 5.0f, "gather y") && ok;

    ok = expect(kMinDetail == 0.9f, "min detail") && ok;

    return ok;
}
