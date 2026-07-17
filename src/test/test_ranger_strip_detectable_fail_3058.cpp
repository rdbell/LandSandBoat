#include "test_ranger_strip_detectable_fail_3058.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldStripAllDetectableOnFail 3058 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack roll > retainChance gate for dual-wire
// cross-check (slice 3058):
//   roll0to99 > retainChance
auto inlineShouldStripAllDetectableOnFail(const int16 retainChance, const int roll0to99) -> bool
{
    return roll0to99 > retainChance;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldStripAllDetectableOnFail
// (retain-fail strip-all Detectable gate; slice 3058).
// Edges: roll == retainChance → false; roll = retainChance+1 → true; roll 0/99.
auto runRangerStripDetectableFail3058SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldStripAllDetectableOnFail;
    using camouflageretainhelpers::ShouldStripPartialStealthOnRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dual-wire.
    ok = expect(ShouldStripAllDetectableOnFail(40, 41), "residual (40, 41) strips") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(40, 40), "residual (40, 40) retains") && ok;

    const struct
    {
        int16       retainChance;
        int         roll0to99;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire edges: equality is not greater → retain holds.
        { 40, 40, false, "roll == retainChance → false (not greater)" },
        { 40, 41, true, "roll = retainChance+1 → true" },
        { 0, 0, false, "roll 0 == retainChance 0 → false" },
        { 0, 1, true, "roll 1 > retainChance 0 → true" },
        { 99, 99, false, "roll 99 == retainChance 99 → false" },
        { 99, 100, true, "roll 100 > retainChance 99 → true" },

        // roll 0 / 99 edges with various retainChance.
        { 0, 0, false, "roll 0 / retainChance 0" },
        { 0, 99, true, "roll 99 / retainChance 0" },
        { 40, 0, false, "roll 0 / retainChance 40 (always retain)" },
        { 40, 99, true, "roll 99 / retainChance 40" },
        { 100, 0, false, "roll 0 / retainChance 100" },
        { 100, 99, false, "roll 99 / retainChance 100 (always retain)" },
        { 100, 100, false, "roll 100 == retainChance 100 → false" },
        { 100, 101, true, "roll 101 > retainChance 100 → true" },

        // Additional poles for dual-wire stability.
        { 1, 0, false, "roll below retainChance" },
        { 1, 1, false, "roll equal retainChance 1" },
        { 1, 2, true, "roll just above retainChance 1" },
        { 50, 50, false, "mid equality" },
        { 50, 51, true, "mid greater" },
        { -1, 0, true, "negative retainChance: roll 0 strips" },
        { -1, -1, false, "roll == negative retainChance" },
        { -1, -2, false, "roll below negative retainChance" },

        // Residual 1391 re-pins.
        { 40, 41, true, "residual strip fail" },
        { 40, 40, false, "residual retain hold" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStripAllDetectableOnFail(c.retainChance, c.roll0to99);
        const bool inlineF = inlineShouldStripAllDetectableOnFail(c.retainChance, c.roll0to99);
        const bool wantPin = c.roll0to99 > c.retainChance;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStripAllDetectableOnFail dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStripAllDetectableOnFail == pin formula roll > retainChance") && ok;
    }

    // Pin composition: free function is the roll > retainChance gate only.
    ok = expect(!ShouldStripAllDetectableOnFail(40, 40), "equality must not strip") && ok;
    ok = expect(ShouldStripAllDetectableOnFail(40, 41), "retainChance+1 must strip") && ok;
    ok = expect(!ShouldStripAllDetectableOnFail(0, 0), "roll 0 == retain 0 must not strip") && ok;
    ok = expect(ShouldStripAllDetectableOnFail(0, 99), "roll 99 > retain 0 must strip") && ok;

    // Dense compose over required + representative poles: free == pin == inline.
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
    };
    for (const auto& p : poles)
    {
        const bool got  = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        const bool want = p.roll0to99 > p.retainChance;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99),
                    "compose free == inline") &&
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
        const bool got = ShouldStripAllDetectableOnFail(h.retainChance, h.roll0to99);
        ok             = expect(got == h.wantStrip, h.label) && ok;
        ok             = expect(got == (h.roll0to99 > h.retainChance), "host inject free == pin") && ok;
        ok             = expect(got == inlineShouldStripAllDetectableOnFail(h.retainChance, h.roll0to99),
                    "host inject free == inline") &&
             ok;
        // Complementary residual partial-stealth gate (1391).
        ok = expect(ShouldStripPartialStealthOnRetain(got) == !got,
                    "partial-stealth must complement strip-all") &&
             ok;
    }

    // Production inject pin: OnRangedAttack retain path uses free function.
    ok = expect(!ShouldStripAllDetectableOnFail(40, 40), "production inject equality must not strip all") && ok;
    ok = expect(ShouldStripAllDetectableOnFail(40, 41), "production inject retainChance+1 must strip all") && ok;

    // Dense compose: strip-all and partial-stealth complementary for all poles.
    for (const auto& p : poles)
    {
        const bool strip = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        ok               = expect(strip == (p.roll0to99 > p.retainChance), "dense free == pin") && ok;
        ok               = expect(strip == inlineShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99),
                    "dense free == inline") &&
             ok;
        ok = expect(ShouldStripPartialStealthOnRetain(strip) == !strip,
                    "dense partial-stealth complements strip-all") &&
             ok;
    }

    return ok;
}
