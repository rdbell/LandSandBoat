#include "test_dynamis_snap_entry_3078.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis ShouldSnapToEntryPos 3078 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua / capacity formula for dual-wire cross-check (slice 3078):
//   hasDynamisEffect && x==0 && y==0 && z==0
auto inlineShouldSnapToEntryPos(const bool hasDynamis, const float x, const float y, const float z) -> bool
{
    return hasDynamis && x == 0.0f && y == 0.0f && z == 0.0f;
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::ShouldSnapToEntryPos
// (zoneOnZoneIn origin-snap gate; OmegaXI internal/dynamis/snap_entry.go).
// Not registered in CMake/main.
auto runDynamisSnapEntry3078SelfTests() -> bool
{
    using dynamishelpers::AtOrigin;
    using dynamishelpers::ShouldSnapToEntryPos;

    bool ok = true;

    // --- Residual 2857 / 1119 pins still hold under dual-wire ---
    ok = expect(!ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f), "no effect at origin → false") && ok;
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f), "effect + origin → true") && ok;
    ok = expect(!ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f), "effect + non-origin → false") && ok;

    // --- ShouldSnapToEntryPos table (3078 poles + residual) ---
    const struct
    {
        bool        hasDynamis;
        float       x;
        float       y;
        float       z;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles.
        { false, 0.0f, 0.0f, 0.0f, false, "no effect → false even at origin" },
        { true, 0.0f, 0.0f, 0.0f, true, "effect + origin → true" },
        { true, 1.0f, 0.0f, 0.0f, false, "effect + non-origin x → false" },
        { true, 0.0f, 1.0f, 0.0f, false, "effect + non-origin y → false" },
        { true, 0.0f, 0.0f, 1.0f, false, "effect + non-origin z → false" },

        // Residual 2857 pins.
        { true, 0.1f, 0.0f, 0.0f, false, "residual fractional x" },
        { true, 0.0f, -0.1f, 0.0f, false, "residual fractional y" },
        { false, 5.0f, 5.0f, 5.0f, false, "residual no DYNAMIS not origin" },
        { true, -0.0f, 0.0f, 0.0f, true, "residual negative zero x" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSnapToEntryPos(c.hasDynamis, c.x, c.y, c.z);
        const bool viaOrig = c.hasDynamis && AtOrigin(c.x, c.y, c.z);
        const bool inlineF = inlineShouldSnapToEntryPos(c.hasDynamis, c.x, c.y, c.z);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == viaOrig, "dual-wire free == hasDynamis && AtOrigin") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline LSB formula") && ok;
    }

    // Pin composition: free == identity of inject formula across host poles.
    for (const bool hasDynamis : { false, true })
    {
        const float positions[][3] = {
            { 0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.1f, 0.0f, 0.0f },
            { 5.0f, 5.0f, 5.0f },
        };
        for (const auto& pos : positions)
        {
            const float x    = pos[0];
            const float y    = pos[1];
            const float z    = pos[2];
            const bool  got  = ShouldSnapToEntryPos(hasDynamis, x, y, z);
            const bool  want = hasDynamis && x == 0.0f && y == 0.0f && z == 0.0f;
            ok               = expect(got == want, "compose free == pin formula") && ok;
            ok               = expect(got == inlineShouldSnapToEntryPos(hasDynamis, x, y, z), "compose free == inline") && ok;
            ok               = expect(got == (hasDynamis && AtOrigin(x, y, z)), "compose free == hasDynamis && AtOrigin") && ok;
        }
    }

    // Branches distinct at origin.
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) != ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f),
                "branches distinct at origin") &&
         ok;

    return ok;
}
