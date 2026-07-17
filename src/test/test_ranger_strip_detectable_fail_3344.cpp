#include "test_ranger_strip_detectable_fail_3344.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldStripAllDetectableOnFail 3344 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack roll > retainChance gate for dual-wire
// cross-check (dedicated 3344):
//   roll0to99 > retainChance
auto inlineShouldStripAllDetectableOnFail3344(const int16 retainChance, const int roll0to99) -> bool
{
    return roll0to99 > retainChance;
}

// Compact dual-wire pin matching Go pinShouldStripAllDetectableOnFail3344 /
// C++ capacity form (formula unchanged from 1391 / 3058):
//   roll0to99 > retainChance
auto pinShouldStripAllDetectableOnFail3344(const int16 retainChance, const int roll0to99) -> bool
{
    return roll0to99 > retainChance;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldStripAllDetectableOnFail
// (retain-fail strip-all Detectable gate; OmegaXI internal/ranger;
// dedicated slice 3344; residual expand 3058 / pure 1391).
//
// Coverage:
//   - free == inline == pin
//   - residual 3058 / 1391 pins still hold
//   - poles: equality / +1 / roll 0/99 / retain 0/40/100
//   - dense pole compose
//   - host inject poles + partial-stealth complement
//
// Siblings left alone: evaluate_camouflage_retain / strip_partial /
// strip_without_retain (3174 / 3082 / 3118).
auto runRangerStripDetectableFail3344SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldStripAllDetectableOnFail;
    using camouflageretainhelpers::ShouldStripPartialStealthOnRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dedicated dual-wire.
    ok = expect(ShouldStripAllDetectableOnFail(40, 41), "residual 1391: (40, 41) strips") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(40, 40), "residual 1391: (40, 40) retains") && ok;

    // Residual 3058 edges still hold under dedicated dual-wire.
    ok = expect(!ShouldStripAllDetectableOnFail(40, 40), "residual 3058: equality must not strip") && ok;
    ok = expect(ShouldStripAllDetectableOnFail(40, 41), "residual 3058: retainChance+1 must strip") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(0, 0), "residual 3058: roll 0 == retain 0 must not strip") && ok;
    ok = expect(ShouldStripAllDetectableOnFail(0, 99), "residual 3058: roll 99 > retain 0 must strip") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(100, 99), "residual 3058: full chance roll in [0,100) retains") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(99, 99), "residual 3058: roll 99 == retain 99 retains") && ok;

    // --- Core poles: free == inline == pin (required edges + stability) ---
    const struct
    {
        int16       retainChance;
        int         roll0to99;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dedicated poles.
        { 40, 40, false, "pole equality retains (not greater)" },
        { 40, 41, true, "pole retainChance+1 strips" },
        { 0, 0, false, "pole roll 0 == retain 0 retains" },
        { 0, 99, true, "pole roll 99 / retain 0 strips" },
        { 100, 99, false, "pole full chance roll 99 retains" },
        { 99, 99, false, "pole roll 99 == retain 99 retains" },

        // Additional poles for dual-wire stability.
        { 0, 1, true, "roll 1 > retain 0 strips" },
        { 40, 0, false, "roll 0 / retain 40 always retains" },
        { 40, 99, true, "roll 99 / retain 40 strips" },
        { 100, 0, false, "full chance roll 0 retains" },
        { 100, 100, false, "roll 100 == retain 100 retains" },
        { 100, 101, true, "roll 101 > retain 100 strips" },
        { 1, 0, false, "roll below retainChance" },
        { 1, 1, false, "roll equal retainChance 1" },
        { 1, 2, true, "roll just above retainChance 1" },
        { 50, 50, false, "mid equality" },
        { 50, 51, true, "mid greater" },
        { -1, 0, true, "negative retainChance: roll 0 strips" },
        { -1, -1, false, "roll == negative retainChance" },
        { -1, -2, false, "roll below negative retainChance" },

        // Residual 1391 / 3058 re-pins.
        { 40, 41, true, "residual strip fail" },
        { 40, 40, false, "residual retain hold" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStripAllDetectableOnFail(c.retainChance, c.roll0to99);
        const bool inlineF = inlineShouldStripAllDetectableOnFail3344(c.retainChance, c.roll0to99);
        const bool pin     = pinShouldStripAllDetectableOnFail3344(c.retainChance, c.roll0to99);
        // Positive form pin composition (explicit roll0to99 > retainChance).
        const bool wantPin = c.roll0to99 > c.retainChance;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldStripAllDetectableOnFail free == inline == pin") &&
             ok;
    }

    // Free == pin across required poles.
    const struct
    {
        int16 retainChance;
        int   roll0to99;
    } requiredPoles[] = {
        { 40, 40 }, { 40, 41 },
        { 0, 0 }, { 0, 99 },
        { 100, 99 }, { 99, 99 },
    };
    for (const auto& p : requiredPoles)
    {
        const bool got     = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        const bool pin     = pinShouldStripAllDetectableOnFail3344(p.retainChance, p.roll0to99);
        const bool inlineF = inlineShouldStripAllDetectableOnFail3344(p.retainChance, p.roll0to99);
        const bool want    = p.roll0to99 > p.retainChance;
        ok                 = expect(got == want, "required pole free == pin formula") && ok;
        ok                 = expect(got == pin && got == inlineF, "required pole free == inline == pin") && ok;
    }

    // Dense compose over required + representative poles: free == inline == pin.
    const struct
    {
        int16 retainChance;
        int   roll0to99;
    } poles[] = {
        { 0, 0 }, { 0, 1 }, { 0, 99 },
        { 40, 0 }, { 40, 39 }, { 40, 40 }, { 40, 41 }, { 40, 99 },
        { 99, 0 }, { 99, 98 }, { 99, 99 }, { 99, 100 },
        { 100, 99 }, { 100, 100 }, { 100, 101 },
        { 1, 0 }, { 1, 1 }, { 1, 2 },
        { -1, -1 }, { -1, 0 },
        { 32767, 32766 }, { 32767, 32767 }, { 32767, 32768 },
    };
    for (const auto& p : poles)
    {
        const bool got     = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        const bool inlineF = inlineShouldStripAllDetectableOnFail3344(p.retainChance, p.roll0to99);
        const bool pin     = pinShouldStripAllDetectableOnFail3344(p.retainChance, p.roll0to99);
        const bool want    = p.roll0to99 > p.retainChance;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        // Strip-all and partial-stealth complementary for all poles.
        ok = expect(ShouldStripPartialStealthOnRetain(got) == !got,
                    "compose partial-stealth complements strip-all") &&
             ok;
    }

    // Host-style inject poles: retainChance + roll after evaluate gate opens.
    // Production OnRangedAttack retain path:
    //   if ShouldStripAllDetectableOnFail(retainChance, roll) → strip all Detectable
    //   else → ShouldStripPartialStealthOnRetain → Sneak/Deodorize/Illusion only
    const struct
    {
        int16       retainChance;
        int         roll0to99;
        bool        wantStrip;
        const char* label;
    } hostPoles[] = {
        { 40, 40, false, "base chance equality → retain (partial stealth)" },
        { 40, 41, true, "base chance fail → strip all Detectable" },
        { 0, 0, false, "zero chance equality → retain" },
        { 0, 1, true, "zero chance any positive roll → strip" },
        { 100, 99, false, "full chance roll in [0,100) always retains" },
        { 100, 0, false, "full chance roll 0 retains" },
        { 48, 48, false, "partial-band chance equality retains" },
        { 48, 49, true, "partial-band chance fail strips" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got     = ShouldStripAllDetectableOnFail(h.retainChance, h.roll0to99);
        const bool inlineF = inlineShouldStripAllDetectableOnFail3344(h.retainChance, h.roll0to99);
        const bool pin     = pinShouldStripAllDetectableOnFail3344(h.retainChance, h.roll0to99);
        ok                 = expect(got == h.wantStrip, h.label) && ok;
        ok                 = expect(got == (h.roll0to99 > h.retainChance), "host inject free == pin") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        // Complementary residual partial-stealth gate (1391).
        ok = expect(ShouldStripPartialStealthOnRetain(got) == !got,
                    "partial-stealth must complement strip-all") &&
             ok;
    }

    // Production inject pin: OnRangedAttack retain path uses free function.
    ok = expect(!ShouldStripAllDetectableOnFail(40, 40), "production inject equality must not strip all") && ok;
    ok = expect(ShouldStripAllDetectableOnFail(40, 41), "production inject retainChance+1 must strip all") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(100, 99), "production inject full chance roll 99 must retain") && ok;

    return ok;
}
