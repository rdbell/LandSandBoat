#include "test_ranger_evaluate_camouflage_retain_3047.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldEvaluateCamouflageRetain 3047 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack RETAIN_CAMOUFLAGE > 0 gate for dual-wire
// cross-check (slice 3047):
//   retainCamouflageMod > 0
auto inlineShouldEvaluateCamouflageRetain(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod > 0;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldEvaluateCamouflageRetain
// (RETAIN_CAMOUFLAGE mod gate before retain roll / strip logic; slice 3047).
// Edges: 0, 1, -1, 99.
auto runRangerEvaluateCamouflageRetain3047SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldEvaluateCamouflageRetain;
    using camouflageretainhelpers::ShouldStripAllDetectableWithoutRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dual-wire.
    ok = expect(ShouldEvaluateCamouflageRetain(1), "residual positive 1 permits") && ok;
    ok = expect(!ShouldEvaluateCamouflageRetain(0), "residual zero rejects") && ok;

    const struct
    {
        int16       retainCamouflageMod;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire edges.
        { 0, false, "edge 0 rejects evaluate (no RETAIN_CAMOUFLAGE)" },
        { 1, true, "edge 1 permits evaluate" },
        { -1, false, "edge -1 rejects evaluate" },
        { 99, true, "edge 99 permits evaluate" },

        // Additional poles for dual-wire stability.
        { 2, true, "positive 2 permits" },
        { 40, true, "base retain chance value still gates evaluate" },
        { 100, true, "100 permits evaluate" },
        { -32768, false, "int16 min rejects" },
        { 32767, true, "int16 max permits" },

        // Residual 1391 re-pins.
        { 1, true, "residual positive 1" },
        { 0, false, "residual zero" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEvaluateCamouflageRetain(c.retainCamouflageMod);
        const bool inlineF = inlineShouldEvaluateCamouflageRetain(c.retainCamouflageMod);
        const bool wantPin = c.retainCamouflageMod > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldEvaluateCamouflageRetain dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldEvaluateCamouflageRetain == pin formula mod > 0") && ok;
    }

    // Pin composition: free function is the retainCamouflageMod > 0 gate only.
    ok = expect(!ShouldEvaluateCamouflageRetain(0), "0 must reject evaluate") && ok;
    ok = expect(ShouldEvaluateCamouflageRetain(1), "1 must permit evaluate") && ok;
    ok = expect(!ShouldEvaluateCamouflageRetain(-1), "negative must reject evaluate") && ok;
    ok = expect(ShouldEvaluateCamouflageRetain(99), "99 must permit evaluate") && ok;

    // Dense compose over required + representative poles: free == pin == inline.
    const int16 poles[] = { -32768, -1, 0, 1, 2, 40, 99, 100, 32767 };
    for (const int16 mod : poles)
    {
        const bool got  = ShouldEvaluateCamouflageRetain(mod);
        const bool want = mod > 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldEvaluateCamouflageRetain(mod), "compose free == inline") && ok;
    }

    // Host-style inject poles: getMod(Mod::RETAIN_CAMOUFLAGE) as int16.
    // Production OnRangedAttack (~3472):
    //   if ShouldEvaluateCamouflageRetain(retainMod) → retain roll / strip ladder
    //   else if ShouldStripAllDetectableWithoutRetain(retainMod) → strip all
    const struct
    {
        int16       retainCamouflageMod;
        bool        wantEvaluate;
        const char* label;
    } hostPoles[] = {
        { 0, false, "no RETAIN_CAMOUFLAGE → without-retain strip path" },
        { 1, true, "RETAIN_CAMOUFLAGE 1 → evaluate retain roll" },
        { -1, false, "negative mod → without-retain strip path" },
        { 99, true, "RETAIN_CAMOUFLAGE 99 → evaluate retain roll" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got = ShouldEvaluateCamouflageRetain(h.retainCamouflageMod);
        ok             = expect(got == h.wantEvaluate, h.label) && ok;
        ok             = expect(got == (h.retainCamouflageMod > 0), "host inject free == pin") && ok;
        ok             = expect(got == inlineShouldEvaluateCamouflageRetain(h.retainCamouflageMod),
                    "host inject free == inline") &&
             ok;
        // Complementary residual without-retain gate (1391).
        ok = expect(ShouldStripAllDetectableWithoutRetain(h.retainCamouflageMod) == !got,
                    "without-retain must complement evaluate") &&
             ok;
    }

    // Production inject pin: OnRangedAttack (~3472) calls
    // ShouldEvaluateCamouflageRetain(getMod(Mod::RETAIN_CAMOUFLAGE)).
    ok = expect(ShouldEvaluateCamouflageRetain(1), "production inject positive must open retain roll path") && ok;
    ok = expect(!ShouldEvaluateCamouflageRetain(0), "production inject zero must skip retain roll path") && ok;

    // Dense compose: evaluate and without-retain complementary for all poles.
    for (const int16 mod : poles)
    {
        const bool eval = ShouldEvaluateCamouflageRetain(mod);
        ok              = expect(eval == (mod > 0), "dense free == pin") && ok;
        ok              = expect(eval == inlineShouldEvaluateCamouflageRetain(mod), "dense free == inline") && ok;
        ok              = expect(ShouldStripAllDetectableWithoutRetain(mod) == !eval,
                    "dense without-retain complements evaluate") &&
             ok;
    }

    return ok;
}
