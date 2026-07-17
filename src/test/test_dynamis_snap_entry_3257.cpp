#include "test_dynamis_snap_entry_3257.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis ShouldSnapToEntryPos 3257 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua / capacity formula for dual-wire cross-check
// (slice 3257 dedicated expand residual 3197):
//   hasDynamisEffect && x==0 && y==0 && z==0
// Direct return form matching production free function / capacity.
auto inlineShouldSnapToEntryPos(const bool hasDynamis, const float x, const float y, const float z) -> bool
{
    return hasDynamis && x == 0.0f && y == 0.0f && z == 0.0f;
}

// Compact dual-wire pin matching free function / capacity body (slice 3257).
// Direct return only — same formula as production ShouldSnapToEntryPos:
//   hasDynamisEffect && AtOrigin(x, y, z)
auto pinShouldSnapToEntryPos3257(const bool hasDynamis, const float x, const float y, const float z) -> bool
{
    return hasDynamis && dynamishelpers::AtOrigin(x, y, z);
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::ShouldSnapToEntryPos
// (zoneOnZoneIn origin-snap gate; OmegaXI internal/dynamis/snap_entry.go;
// slice 3257 dedicated expand residual 3197). Formula unchanged.
//
// Coverage:
//   - free == inline == pin == hasDynamis && AtOrigin
//   - residual 3197 / 3078 / 2857 / 1119 pins still hold
//   - residual poles: effect on/off, origin vs non-origin
//   - dense hasDynamis × position poles
// Not registered in CMake/main.
auto runDynamisSnapEntry3257SelfTests() -> bool
{
    using dynamishelpers::AtOrigin;
    using dynamishelpers::ShouldSnapToEntryPos;

    bool ok = true;

    // Residual 3197 / 3078 / 2857 / 1119 pins still hold under dual-wire.
    ok = expect(!ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f), "residual: no effect at origin → false") && ok;
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f), "residual: effect + origin → true") && ok;
    ok = expect(!ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f), "residual: effect + non-origin → false") && ok;
    ok = expect(!ShouldSnapToEntryPos(false, 5.0f, 5.0f, 5.0f), "residual: no effect non-origin → false") && ok;

    // --- Residual poles: effect on/off × origin vs non-origin (+ axis / −0) ---
    const struct
    {
        bool        hasDynamis;
        float       x;
        float       y;
        float       z;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 3197 compose / 3078 / 2857 classic poles.
        { false, 0.0f, 0.0f, 0.0f, false, "residual no effect → false even at origin" },
        { true, 0.0f, 0.0f, 0.0f, true, "residual effect + origin → true" },
        { true, 1.0f, 0.0f, 0.0f, false, "residual effect + non-origin x → false" },
        { true, 0.0f, 1.0f, 0.0f, false, "residual effect + non-origin y → false" },
        { true, 0.0f, 0.0f, 1.0f, false, "residual effect + non-origin z → false" },
        { false, 5.0f, 5.0f, 5.0f, false, "residual no effect non-origin → false" },

        // Residual fractional / exact-equality poles.
        { true, 0.1f, 0.0f, 0.0f, false, "residual fractional x" },
        { true, 0.0f, -0.1f, 0.0f, false, "residual fractional y" },
        { true, 0.0f, 0.0f, 0.1f, false, "residual fractional z" },
        { true, -0.0f, 0.0f, 0.0f, true, "residual negative zero x (IEEE −0.0 == +0.0)" },
        { true, 0.0f, -0.0f, 0.0f, true, "residual negative zero y" },
        { true, 0.0f, 0.0f, -0.0f, true, "residual negative zero z" },
        { true, -0.0f, -0.0f, -0.0f, true, "residual all negative zeros" },

        // Dual-wire polarity repeats (effect on/off × origin).
        { false, 0.0f, 0.0f, 0.0f, false, "polarity no effect at origin" },
        { true, 0.0f, 0.0f, 0.0f, true, "polarity effect at origin" },
        { true, 10.0f, 0.0f, 0.0f, false, "polarity effect not origin" },
        { false, 10.0f, 0.0f, 0.0f, false, "polarity no effect not origin" },
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldSnapToEntryPos(p.hasDynamis, p.x, p.y, p.z);
        const bool inlineF = inlineShouldSnapToEntryPos(p.hasDynamis, p.x, p.y, p.z);
        const bool pin     = pinShouldSnapToEntryPos3257(p.hasDynamis, p.x, p.y, p.z);
        const bool wantPin = p.hasDynamis && AtOrigin(p.x, p.y, p.z);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline LSB formula") && ok;
        ok = expect(got == pin, "dual-wire free == pin") && ok;
        ok = expect(got == wantPin, "dual-wire free == hasDynamis && AtOrigin") && ok;
    }

    // Dense: free == inline == pin over hasDynamis × host position poles.
    const float positions[][3] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.1f, 0.0f, 0.0f },
        { 0.0f, 0.1f, 0.0f },
        { 0.0f, 0.0f, 0.1f },
        { 5.0f, 5.0f, 5.0f },
        { -0.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, 0.0f, -1.0f },
    };
    for (const bool hasDynamis : { false, true })
    {
        for (const auto& pos : positions)
        {
            const float x    = pos[0];
            const float y    = pos[1];
            const float z    = pos[2];
            const bool  got  = ShouldSnapToEntryPos(hasDynamis, x, y, z);
            const bool  want = hasDynamis && x == 0.0f && y == 0.0f && z == 0.0f;
            ok               = expect(got == want, "dense free == pin formula") && ok;
            ok               = expect(got == inlineShouldSnapToEntryPos(hasDynamis, x, y, z), "dense free == inline") && ok;
            ok               = expect(got == pinShouldSnapToEntryPos3257(hasDynamis, x, y, z), "dense free == pin") && ok;
        }
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(!ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f), "no effect must not snap even at origin") && ok;
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f), "effect at origin must snap") && ok;
    ok = expect(!ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f), "effect not at origin must not snap") && ok;
    ok = expect(!ShouldSnapToEntryPos(false, 1.0f, 0.0f, 0.0f), "no effect not origin must not snap") && ok;

    // Free == pin matching C++ across residual poles.
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) == pinShouldSnapToEntryPos3257(true, 0.0f, 0.0f, 0.0f),
                "free==pin effect+origin") &&
         ok;
    ok = expect(ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f) == pinShouldSnapToEntryPos3257(false, 0.0f, 0.0f, 0.0f),
                "free==pin no-effect+origin") &&
         ok;
    ok = expect(ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f) == pinShouldSnapToEntryPos3257(true, 1.0f, 0.0f, 0.0f),
                "free==pin effect+non-origin") &&
         ok;

    // Branches distinct: effect on/off at origin; origin vs non-origin with effect.
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) != ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f),
                "effect on/off branches distinct at origin") &&
         ok;
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) != ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f),
                "origin vs non-origin branches distinct with effect") &&
         ok;

    // Composition: free dual-wires hasDynamis && AtOrigin (position half 3197).
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) == AtOrigin(0.0f, 0.0f, 0.0f),
                "compose snap(true, origin) == AtOrigin") &&
         ok;
    ok = expect(!ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f), "compose snap(true, non-origin) false") && ok;
    ok = expect(!ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f), "compose snap(false, origin) false") && ok;

    return ok;
}
