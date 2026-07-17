#include "test_dynamis_snap_entry_2857.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis snap entry 2857 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua formula for dual-wire cross-check:
// hasDynamis && x==0 && y==0 && z==0
auto inlineShouldSnap(const bool hasDynamis, const float x, const float y, const float z) -> bool
{
    return hasDynamis && x == 0.0f && y == 0.0f && z == 0.0f;
}

} // namespace

auto runDynamisSnapEntry2857SelfTests() -> bool
{
    using dynamishelpers::AtOrigin;
    using dynamishelpers::ShouldSnapToEntryPos;

    bool ok = true;

    // --- AtOrigin exact equality ---
    ok = expect(AtOrigin(0.0f, 0.0f, 0.0f), "origin") && ok;
    ok = expect(!AtOrigin(0.1f, 0.0f, 0.0f), "x non-zero") && ok;
    ok = expect(!AtOrigin(0.0f, 1.0f, 0.0f), "y non-zero") && ok;
    ok = expect(!AtOrigin(0.0f, 0.0f, -0.1f), "z non-zero") && ok;
    ok = expect(!AtOrigin(1.0f, 1.0f, 1.0f), "all non-zero") && ok;

    // --- ShouldSnapToEntryPos table ---
    const struct
    {
        bool        hasDynamis;
        float       x;
        float       y;
        float       z;
        bool        want;
        const char* label;
    } cases[] = {
        { true, 0.0f, 0.0f, 0.0f, true, "has DYNAMIS at origin" },
        { false, 0.0f, 0.0f, 0.0f, false, "no DYNAMIS at origin" },
        { true, 1.0f, 0.0f, 0.0f, false, "has DYNAMIS x non-zero" },
        { true, 0.0f, 1.0f, 0.0f, false, "has DYNAMIS y non-zero" },
        { true, 0.0f, 0.0f, 1.0f, false, "has DYNAMIS z non-zero" },
        { true, 0.1f, 0.0f, 0.0f, false, "has DYNAMIS fractional x" },
        { false, 5.0f, 5.0f, 5.0f, false, "no DYNAMIS not origin" },
        { true, -0.0f, 0.0f, 0.0f, true, "has DYNAMIS negative zero x" },
    };

    for (const auto& c : cases)
    {
        const bool got    = ShouldSnapToEntryPos(c.hasDynamis, c.x, c.y, c.z);
        const bool pure   = AtOrigin(c.x, c.y, c.z) && c.hasDynamis;
        const bool inlineF = inlineShouldSnap(c.hasDynamis, c.x, c.y, c.z);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire ShouldSnap == hasDynamis && AtOrigin") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Composition: free function dual-wires AtOrigin (single source of truth).
    ok = expect(ShouldSnapToEntryPos(true, 0.0f, 0.0f, 0.0f) == AtOrigin(0.0f, 0.0f, 0.0f), "compose true origin") && ok;
    ok = expect(ShouldSnapToEntryPos(true, 1.0f, 0.0f, 0.0f) == false, "compose true not origin") && ok;
    ok = expect(ShouldSnapToEntryPos(false, 0.0f, 0.0f, 0.0f) == false, "compose false origin") && ok;

    return ok;
}
