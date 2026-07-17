#include "test_ranger_evaluate_camouflage_retain_3174.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldEvaluateCamouflageRetain 3174 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack RETAIN_CAMOUFLAGE > 0 gate for dual-wire
// cross-check (dedicated 3174):
//   retainCamouflageMod > 0
auto inlineShouldEvaluateCamouflageRetain3174(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod > 0;
}

// Compact dual-wire pin matching Go pinShouldEvaluateCamouflageRetain3174 /
// C++ capacity form (formula unchanged from 1391 / 3047):
//   retainCamouflageMod > 0
auto pinShouldEvaluateCamouflageRetain3174(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod > 0;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldEvaluateCamouflageRetain
// (RETAIN_CAMOUFLAGE mod gate before retain roll / strip logic;
// OmegaXI internal/ranger; dedicated slice 3174; residual expand 3047 / pure 1391).
//
// Coverage:
//   - free == inline == pin
//   - residual 3047 / 1391 pins still hold
//   - poles 0, 1, -1, 100
//   - host inject poles
//
// Siblings left alone: 3058, 3082, 3118 strip helpers.
auto runRangerEvaluateCamouflageRetain3174SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldEvaluateCamouflageRetain;
    using camouflageretainhelpers::ShouldStripAllDetectableWithoutRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dedicated dual-wire.
    ok = expect(ShouldEvaluateCamouflageRetain(1), "residual 1391: positive 1 permits") && ok;
    ok = expect(!ShouldEvaluateCamouflageRetain(0), "residual 1391: zero rejects") && ok;

    // Residual 3047 edges still hold under dedicated dual-wire.
    ok = expect(!ShouldEvaluateCamouflageRetain(0), "residual 3047: edge 0 rejects") && ok;
    ok = expect(ShouldEvaluateCamouflageRetain(1), "residual 3047: edge 1 permits") && ok;
    ok = expect(!ShouldEvaluateCamouflageRetain(-1), "residual 3047: edge -1 rejects") && ok;
    ok = expect(ShouldEvaluateCamouflageRetain(99), "residual 3047: edge 99 permits") && ok;

    // --- Core poles: free == inline == pin (required 0, 1, -1, 100 + stability) ---
    const struct
    {
        int16       retainCamouflageMod;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dedicated poles.
        { 0, false, "pole 0 rejects evaluate (no RETAIN_CAMOUFLAGE)" },
        { 1, true, "pole 1 permits evaluate" },
        { -1, false, "pole -1 rejects evaluate" },
        { 100, true, "pole 100 permits evaluate" },

        // Additional poles for dual-wire stability.
        { 2, true, "positive 2 permits" },
        { 40, true, "base retain chance value still gates evaluate" },
        { 99, true, "99 permits evaluate" },
        { -32768, false, "int16 min rejects" },
        { 32767, true, "int16 max permits" },

        // Residual 1391 / 3047 re-pins.
        { 1, true, "residual positive 1" },
        { 0, false, "residual zero" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEvaluateCamouflageRetain(c.retainCamouflageMod);
        const bool inlineF = inlineShouldEvaluateCamouflageRetain3174(c.retainCamouflageMod);
        const bool pin     = pinShouldEvaluateCamouflageRetain3174(c.retainCamouflageMod);
        // Positive form pin composition (explicit retainCamouflageMod > 0).
        const bool wantPin = c.retainCamouflageMod > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldEvaluateCamouflageRetain free == inline == pin") &&
             ok;
    }

    // Free == pin across required poles 0, 1, -1, 100.
    const int16 requiredPoles[] = { 0, 1, -1, 100 };
    for (const int16 mod : requiredPoles)
    {
        const bool got     = ShouldEvaluateCamouflageRetain(mod);
        const bool pin     = pinShouldEvaluateCamouflageRetain3174(mod);
        const bool inlineF = inlineShouldEvaluateCamouflageRetain3174(mod);
        const bool want    = mod > 0;
        ok                 = expect(got == want, "required pole free == pin formula") && ok;
        ok                 = expect(got == pin && got == inlineF, "required pole free == inline == pin") && ok;
    }

    // Dense compose over required + representative poles: free == inline == pin.
    const int16 poles[] = { -32768, -1, 0, 1, 2, 40, 99, 100, 32767 };
    for (const int16 mod : poles)
    {
        const bool got     = ShouldEvaluateCamouflageRetain(mod);
        const bool inlineF = inlineShouldEvaluateCamouflageRetain3174(mod);
        const bool pin     = pinShouldEvaluateCamouflageRetain3174(mod);
        const bool want    = mod > 0;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        // Evaluate and without-retain complementary for all poles.
        ok = expect(ShouldStripAllDetectableWithoutRetain(mod) == !got,
                    "compose without-retain complements evaluate") &&
             ok;
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
        { 100, true, "RETAIN_CAMOUFLAGE 100 → evaluate retain roll" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got     = ShouldEvaluateCamouflageRetain(h.retainCamouflageMod);
        const bool inlineF = inlineShouldEvaluateCamouflageRetain3174(h.retainCamouflageMod);
        const bool pin     = pinShouldEvaluateCamouflageRetain3174(h.retainCamouflageMod);
        ok                 = expect(got == h.wantEvaluate, h.label) && ok;
        ok                 = expect(got == (h.retainCamouflageMod > 0), "host inject free == pin") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        // Complementary residual without-retain gate (1391).
        ok = expect(ShouldStripAllDetectableWithoutRetain(h.retainCamouflageMod) == !got,
                    "without-retain must complement evaluate") &&
             ok;
    }

    // Production inject pin: OnRangedAttack (~3472) calls
    // ShouldEvaluateCamouflageRetain(getMod(Mod::RETAIN_CAMOUFLAGE)).
    ok = expect(ShouldEvaluateCamouflageRetain(1), "production inject positive must open retain roll path") && ok;
    ok = expect(!ShouldEvaluateCamouflageRetain(0), "production inject zero must skip retain roll path") && ok;
    ok = expect(ShouldEvaluateCamouflageRetain(100), "production inject 100 must open retain roll path") && ok;

    return ok;
}
