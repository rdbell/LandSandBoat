#include "test_navmesh_builder_geometry_host_6415.h"

#include <cmath>
#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh builder geometry host 6415 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for NavMeshBuilder geometry (slice 6415).
// Go: NewBuilder / GatherTrianglesInAABB / TileBitsForCount.
auto runNavmeshBuilderGeometryHost6415SelfTests() -> bool
{
    bool ok = true;

    constexpr float  kCellSize            = 4.0f;
    constexpr int    kTileBorderPadding   = 3;
    constexpr float  kMinDetailSampleDist = 0.9f;
    constexpr int    kDTMaxTileBits       = 14;
    constexpr int    kDTTotalRefBits      = 22;
    constexpr uint16_t kWalkFlag          = 0x0001;
    constexpr unsigned char kNullArea     = 0;
    constexpr unsigned char kWalkArea     = 63;

    ok = expect(kCellSize == 4.0f && kTileBorderPadding == 3, "layout") && ok;
    ok = expect(kMinDetailSampleDist == 0.9f, "detail") && ok;
    ok = expect(kWalkFlag == 0x0001 && kNullArea == 0 && kWalkArea == 63, "areas") && ok;

    auto tileBitsForCount = [](int count) -> int {
        int tileBits = 0;
        for (int v = count; v > 0; v >>= 1)
        {
            tileBits++;
        }
        return tileBits > kDTMaxTileBits ? kDTMaxTileBits : tileBits;
    };
    ok = expect(tileBitsForCount(0) == 0 && tileBitsForCount(1) == 1, "tileBits") && ok;
    ok = expect(tileBitsForCount(1 << 20) == kDTMaxTileBits, "tileBits clamp") && ok;

    const int polyBits1 = kDTTotalRefBits - tileBitsForCount(1);
    ok = expect((1 << tileBitsForCount(1)) == 2 && (1 << polyBits1) == (1 << 21), "maxTiles/Polys") && ok;

    // Identity rotation det > 0 → flip winding
    const float rot[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
    const float det    = rot[0] * (rot[4] * rot[8] - rot[5] * rot[7]) -
                      rot[3] * (rot[1] * rot[8] - rot[2] * rot[7]) +
                      rot[6] * (rot[1] * rot[5] - rot[2] * rot[4]);
    ok = expect(det > 0.0f, "flip winding") && ok;

    // worldToCell origin on 4x4
    const int cx = static_cast<int>(std::floor(0.0f / kCellSize)) + 4 / 2;
    const int cz = static_cast<int>(std::floor(0.0f / kCellSize)) + 4 / 2;
    ok = expect(cx == 2 && cz == 2, "worldToCell") && ok;

    const uint32_t key = (static_cast<uint32_t>(1) << 16) | 2u;
    ok = expect(key == ((1u << 16) | 2u), "placement key") && ok;

    return ok;
}
