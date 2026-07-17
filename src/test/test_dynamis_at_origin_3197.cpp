#include "test_dynamis_at_origin_3197.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis AtOrigin 3197 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua / capacity formula for dual-wire cross-check (slice 3197):
//   x == 0.0f && y == 0.0f && z == 0.0f
auto inlineAtOrigin(const float x, const float y, const float z) -> bool
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

// Compact dual-wire pin matching Go pinAtOrigin3197:
//   x == 0.0f && y == 0.0f && z == 0.0f
auto pinAtOrigin(const float x, const float y, const float z) -> bool
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::AtOrigin
// (zoneOnZoneIn origin equality half; OmegaXI internal/dynamis/at_origin.go;
// slice 3197).
//
// Coverage:
//   - free == inline == pin == x == 0 && y == 0 && z == 0
//   - residual 2857 / 1119 pins still hold
//   - poles: zeros, nonzero axes, −0.0f
//   - ShouldSnapToEntryPos composes hasDynamis && AtOrigin (sibling 3078)
// Not registered in CMake/main.
auto runDynamisAtOrigin3197SelfTests() -> bool
{
    using dynamishelpers::AtOrigin;
    using dynamishelpers::ShouldSnapToEntryPos;

    bool ok = true;

    // Residual 2857 / 1119 pins still hold under dual-wire.
    ok = expect(AtOrigin(0.0f, 0.0f, 0.0f), "residual: origin → true") && ok;
    ok = expect(!AtOrigin(0.1f, 0.0f, 0.0f), "residual: fractional x") && ok;
    ok = expect(!AtOrigin(0.0f, 1.0f, 0.0f), "residual: nonzero y") && ok;
    ok = expect(!AtOrigin(0.0f, 0.0f, -0.1f), "residual: fractional z") && ok;

    // --- Core poles: free == inline == pin == formula ---
    const struct
    {
        float       x;
        float       y;
        float       z;
        bool        want;
        const char* label;
    } cases[] = {
        // residual 2857 / 1119 poles
        { 0.0f, 0.0f, 0.0f, true, "residual origin zeros" },
        { 0.1f, 0.0f, 0.0f, false, "residual fractional x" },
        { 0.0f, 1.0f, 0.0f, false, "residual nonzero y" },
        { 0.0f, 0.0f, -0.1f, false, "residual fractional z" },
        { 1.0f, 1.0f, 1.0f, false, "residual all nonzero" },

        // dedicated dual-wire poles: zeros / nonzero axes / −0
        { 0.0f, 0.0f, 0.0f, true, "zeros → true" },
        { 1.0f, 0.0f, 0.0f, false, "nonzero x → false" },
        { 0.0f, 1.0f, 0.0f, false, "nonzero y → false" },
        { 0.0f, 0.0f, 1.0f, false, "nonzero z → false" },
        { -1.0f, 0.0f, 0.0f, false, "negative x → false" },
        { 0.0f, -1.0f, 0.0f, false, "negative y → false" },
        { 0.0f, 0.0f, -1.0f, false, "negative z → false" },
        { -0.0f, 0.0f, 0.0f, true, "negative zero x (IEEE −0.0 == +0.0)" },
        { 0.0f, -0.0f, 0.0f, true, "negative zero y" },
        { 0.0f, 0.0f, -0.0f, true, "negative zero z" },
        { -0.0f, -0.0f, -0.0f, true, "all negative zeros" },
        { 0.1f, 0.0f, 0.0f, false, "fractional x exact equality only" },
        { 0.0f, 0.1f, 0.0f, false, "fractional y" },
        { 0.0f, 0.0f, 0.1f, false, "fractional z" },
        { 5.0f, 5.0f, 5.0f, false, "far from origin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = AtOrigin(c.x, c.y, c.z);
        const bool inlineF = inlineAtOrigin(c.x, c.y, c.z);
        const bool pinGot  = pinAtOrigin(c.x, c.y, c.z);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(AtOrigin(0.0f, 0.0f, 0.0f) == pinAtOrigin(0.0f, 0.0f, 0.0f), "free==pin origin") && ok;
    ok = expect(AtOrigin(1.0f, 0.0f, 0.0f) == pinAtOrigin(1.0f, 0.0f, 0.0f), "free==pin nonzero x") && ok;

    // Branches distinct: origin vs non-origin.
    ok = expect(AtOrigin(0.0f, 0.0f, 0.0f) != AtOrigin(1.0f, 0.0f, 0.0f), "branches distinct") && ok;

    // Dense compose over host poles: free == inline == pin == formula.
    const float positions[][3] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.1f, 0.0f, 0.0f },
        { 5.0f, 5.0f, 5.0f },
        { -0.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
    };
    for (const auto& pos : positions)
    {
        const float x    = pos[0];
        const float y    = pos[1];
        const float z    = pos[2];
        const bool  got  = AtOrigin(x, y, z);
        const bool  want = x == 0.0f && y == 0.0f && z == 0.0f;
        ok               = expect(got == want, "compose free == pin formula") && ok;
        ok               = expect(got == inlineAtOrigin(x, y, z), "compose free==inline") && ok;
        ok               = expect(got == pinAtOrigin(x, y, z), "compose free==pin") && ok;
    }

    // Composition: ShouldSnapToEntryPos dual-wires hasDynamis && AtOrigin
    // (sibling 3078 left alone; pin residual compose path only).
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) == AtOrigin(0.0f, 0.0f, 0.0f),
                "compose snap(true, origin) == AtOrigin") &&
         ok;
    ok = expect(!ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f), "compose snap(true, non-origin) false") && ok;
    ok = expect(!ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f), "compose snap(false, origin) false") && ok;

    return ok;
}
