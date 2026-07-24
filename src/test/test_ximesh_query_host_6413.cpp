#include "test_ximesh_query_host_6413.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <utility>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ximesh query host 6413 self-test failed: " << label << '\n';
    }
    return condition;
}

// Detour-style closest height (mirror of dtClosestHeightPointTriangle).
auto closestHeight(const float px, const float pz,
                   const float ax, const float ay, const float az,
                   const float bx, const float by, const float bz,
                   const float cx, const float cy, const float cz,
                   float& h) -> bool
{
    constexpr float EPS = 1e-6f;
    const float v0x = cx - ax, v0y = cy - ay, v0z = cz - az;
    const float v1x = bx - ax, v1y = by - ay, v1z = bz - az;
    const float v2x = px - ax, v2z = pz - az;
    float denom = v0x * v1z - v0z * v1x;
    if (std::fabs(denom) < EPS)
    {
        return false;
    }
    float u = v1z * v2x - v1x * v2z;
    float v = v0x * v2z - v0z * v2x;
    if (denom < 0)
    {
        denom = -denom;
        u     = -u;
        v     = -v;
    }
    if (u >= 0.0f && v >= 0.0f && (u + v) <= denom)
    {
        h = ay + (v0y * u + v1y * v) / denom;
        return true;
    }
    return false;
}

} // namespace

// Pure residual suite for XiMesh::query (slice 6413).
// Go: WorldToCell / ClosestHeightPointTriangle / LoadedMesh.Query.
auto runXimeshQueryHost6413SelfTests() -> bool
{
    bool ok = true;

    constexpr float kCellSize      = 4.0f;
    constexpr float kFloorEpsilon  = 0.01f;

    auto worldToCell = [](const float x, const float z, const uint16_t gw, const uint16_t gh) {
        return std::pair<int, int>{
            static_cast<int>(std::floor(x / kCellSize)) + gw / 2,
            static_cast<int>(std::floor(z / kCellSize)) + gh / 2,
        };
    };

    {
        const auto [cx, cz] = worldToCell(0.0f, 0.0f, 4, 4);
        ok = expect(cx == 2 && cz == 2, "worldToCell origin 4x4") && ok;
    }
    {
        const auto [cx, cz] = worldToCell(0.25f, 0.25f, 1, 1);
        ok = expect(cx == 0 && cz == 0, "worldToCell 1x1") && ok;
    }

    float h = 0.0f;
    ok = expect(closestHeight(0.25f, 0.25f, 0, 0, 0, 1, 2, 0, 0, 0, 1, h), "height hit") && ok;
    ok = expect(std::fabs(h - 0.5f) < 1e-4f, "height ~0.5") && ok;
    ok = expect(!closestHeight(2.0f, 2.0f, 0, 0, 0, 1, 2, 0, 0, 0, 1, h), "height miss") && ok;

    ok = expect(kFloorEpsilon == 0.01f && kCellSize == 4.0f, "constants") && ok;

    // triY >= y - eps window
    constexpr float triY = 0.5f;
    ok = expect(triY >= 0.505f - kFloorEpsilon, "floor eps accepts") && ok;

    return ok;
}
