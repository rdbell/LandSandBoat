#include "test_ranger_strip_without_retain_3118.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldStripAllDetectableWithoutRetain 3118 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack RETAIN_CAMOUFLAGE <= 0 without-retain strip gate for
// dual-wire cross-check (slice 3118):
//   retainCamouflageMod <= 0
auto inlineShouldStripAllDetectableWithoutRetain(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod <= 0;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldStripAllDetectableWithoutRetain
// (without-retain strip-all Detectable gate; slice 3118).
// Edges: 0, 1, -1, 99.
auto runRangerStripWithoutRetain3118SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldEvaluateCamouflageRetain;
    using camouflageretainhelpers::ShouldStripAllDetectableWithoutRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dual-wire.
    ok = expect(ShouldStripAllDetectableWithoutRetain(0), "residual zero strips") && ok;
    ok = expect(!ShouldStripAllDetectableWithoutRetain(1), "residual positive 1 does not strip") && ok;

    const struct
    {
        int16       retainCamouflageMod;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire edges.
        { 0, true, "edge 0 strips without retain (no RETAIN_CAMOUFLAGE)" },
        { 1, false, "edge 1 does not without-retain strip (evaluate path)" },
        { -1, true, "edge -1 strips without retain" },
        { 99, false, "edge 99 does not without-retain strip" },

        // Additional poles for dual-wire stability.
        { 2, false, "positive 2 evaluate path" },
        { 40, false, "base retain chance value still evaluate path" },
        { 100, false, "100 evaluate path" },
        { -32768, true, "int16 min strips without retain" },
        { 32767, false, "int16 max evaluate path" },

        // Residual 1391 re-pins.
        { 0, true, "residual zero" },
        { 1, false, "residual positive 1" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStripAllDetectableWithoutRetain(c.retainCamouflageMod);
        const bool inlineF = inlineShouldStripAllDetectableWithoutRetain(c.retainCamouflageMod);
        const bool wantPin = c.retainCamouflageMod <= 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStripAllDetectableWithoutRetain dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStripAllDetectableWithoutRetain == pin formula mod <= 0") && ok;
    }

    // Pin composition: free function is the retainCamouflageMod <= 0 gate only.
    ok = expect(ShouldStripAllDetectableWithoutRetain(0), "0 must strip without retain") && ok;
    ok = expect(!ShouldStripAllDetectableWithoutRetain(1), "1 must not strip without retain") && ok;
    ok = expect(ShouldStripAllDetectableWithoutRetain(-1), "negative must strip without retain") && ok;
    ok = expect(!ShouldStripAllDetectableWithoutRetain(99), "99 must not strip without retain") && ok;

    // Dense compose over required + representative poles: free == pin == inline.
    const int16 poles[] = { -32768, -1, 0, 1, 2, 40, 99, 100, 32767 };
    for (const int16 mod : poles)
    {
        const bool got  = ShouldStripAllDetectableWithoutRetain(mod);
        const bool want = mod <= 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldStripAllDetectableWithoutRetain(mod),
                    "compose free == inline") &&
             ok;
    }

    // Host-style inject poles: getMod(Mod::RETAIN_CAMOUFLAGE) as int16.
    // Production OnRangedAttack (~3472):
    //   if ShouldEvaluateCamouflageRetain(retainMod) → retain roll / strip ladder
    //   else if ShouldStripAllDetectableWithoutRetain(retainMod) → strip all
    const struct
    {
        int16       retainCamouflageMod;
        bool        wantWithout;
        const char* label;
    } hostPoles[] = {
        { 0, true, "no RETAIN_CAMOUFLAGE → without-retain strip all" },
        { 1, false, "RETAIN_CAMOUFLAGE 1 → evaluate path; not without-retain" },
        { -1, true, "negative mod → without-retain strip all" },
        { 99, false, "RETAIN_CAMOUFLAGE 99 → evaluate path; not without-retain" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got = ShouldStripAllDetectableWithoutRetain(h.retainCamouflageMod);
        ok             = expect(got == h.wantWithout, h.label) && ok;
        ok             = expect(got == (h.retainCamouflageMod <= 0), "host inject free == pin") && ok;
        ok             = expect(got == inlineShouldStripAllDetectableWithoutRetain(h.retainCamouflageMod),
                    "host inject free == inline") &&
             ok;
        // Complementary sibling evaluate gate (3047): opposite of without-retain.
        const bool eval = ShouldEvaluateCamouflageRetain(h.retainCamouflageMod);
        ok              = expect(eval == !got, "evaluate must complement without-retain") && ok;
    }

    // Production inject pin: OnRangedAttack else-if uses free function with mod.
    ok = expect(ShouldStripAllDetectableWithoutRetain(0), "production inject zero must strip without retain") && ok;
    ok = expect(!ShouldStripAllDetectableWithoutRetain(1), "production inject positive must not strip without retain") && ok;

    // Dense compose: free == pin == inline; complements sibling evaluate (3047).
    for (const int16 mod : poles)
    {
        const bool got  = ShouldStripAllDetectableWithoutRetain(mod);
        const bool want = mod <= 0;
        ok              = expect(got == want, "dense free == pin") && ok;
        ok              = expect(got == inlineShouldStripAllDetectableWithoutRetain(mod), "dense free == inline") && ok;
        ok              = expect(ShouldEvaluateCamouflageRetain(mod) == !got,
                    "dense evaluate must complement without-retain") &&
             ok;
    }

    // Explicit production inject poles matching OnRangedAttack else-if.
    ok = expect(ShouldStripAllDetectableWithoutRetain(0), "production inject zero path must strip") && ok;
    ok = expect(!ShouldStripAllDetectableWithoutRetain(1), "production inject positive path must not strip") && ok;
    ok = expect(ShouldStripAllDetectableWithoutRetain(0) == !ShouldEvaluateCamouflageRetain(0),
                "zero: without-retain must complement evaluate") &&
         ok;
    ok = expect(ShouldStripAllDetectableWithoutRetain(1) == !ShouldEvaluateCamouflageRetain(1),
                "one: without-retain must complement evaluate") &&
         ok;

    return ok;
}
