#include "test_ranger_strip_partial_retain_3082.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldStripPartialStealthOnRetain 3082 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack retain-success partial-stealth gate for dual-wire
// cross-check (slice 3082):
//   !stripAllDetectable
auto inlineShouldStripPartialStealthOnRetain(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldStripPartialStealthOnRetain
// (retain-success partial-stealth gate; slice 3082).
// Edges: stripAllDetectable false → true; true → false.
auto runRangerStripPartialRetain3082SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldStripAllDetectableOnFail;
    using camouflageretainhelpers::ShouldStripPartialStealthOnRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dual-wire.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "residual false → partial") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "residual true → no partial") && ok;

    const struct
    {
        bool        stripAllDetectable;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire edges: complement of strip-all Detectable gate.
        { false, true, "stripAllDetectable false → true (partial stealth)" },
        { true, false, "stripAllDetectable true → false (no partial; strip-all path)" },

        // Residual 1391 re-pins.
        { false, true, "residual retain success partial" },
        { true, false, "residual retain fail no partial" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStripPartialStealthOnRetain(c.stripAllDetectable);
        const bool inlineF = inlineShouldStripPartialStealthOnRetain(c.stripAllDetectable);
        const bool wantPin = !c.stripAllDetectable;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStripPartialStealthOnRetain dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStripPartialStealthOnRetain == pin formula !stripAllDetectable") && ok;
    }

    // Pin composition: free function is the !stripAllDetectable gate only.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "false strip-all must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "true strip-all must not partial-strip") && ok;

    // Dense compose over required poles: free == pin == inline.
    for (const bool stripAll : { false, true })
    {
        const bool got  = ShouldStripPartialStealthOnRetain(stripAll);
        const bool want = !stripAll;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldStripPartialStealthOnRetain(stripAll),
                    "compose free == inline") &&
             ok;
    }

    // Host-style inject poles: OnRangedAttack after retain path opens:
    //   if ShouldStripAllDetectableOnFail(retainChance, roll) → strip all Detectable
    //   else if ShouldStripPartialStealthOnRetain(false) → Sneak/Deodorize/Illusion only
    const struct
    {
        bool        stripAllDetectable;
        bool        wantPartial;
        const char* label;
    } hostPoles[] = {
        { false, true, "retain success → partial stealth (Sneak/Deodorize/Illusion)" },
        { true, false, "retain fail → strip-all already; no partial" },
        { false, true, "production inject false after strip-all false" },
        { true, false, "production inject true when strip-all true (dead branch)" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got = ShouldStripPartialStealthOnRetain(h.stripAllDetectable);
        ok             = expect(got == h.wantPartial, h.label) && ok;
        ok             = expect(got == !h.stripAllDetectable, "host inject free == pin") && ok;
        ok             = expect(got == inlineShouldStripPartialStealthOnRetain(h.stripAllDetectable),
                    "host inject free == inline") &&
             ok;
        // Complementary sibling strip-all gate (3058): opposite of partial.
        ok = expect(got != h.stripAllDetectable,
                    "partial-stealth must complement strip-all") &&
             ok;
    }

    // Production inject pin: OnRangedAttack else-if uses free function with false.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "production inject false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "production inject true must not partial-strip") && ok;

    // Compose with sibling 3058 free function: partial complements strip-all.
    const struct
    {
        int16 retainChance;
        int   roll0to99;
    } composePoles[] = {
        { 40, 40 }, { 40, 41 },
        { 0, 0 }, { 0, 1 }, { 0, 99 },
        { 100, 99 }, { 48, 48 }, { 48, 49 },
    };
    for (const auto& p : composePoles)
    {
        const bool stripAll = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        const bool partial  = ShouldStripPartialStealthOnRetain(stripAll);
        ok                  = expect(partial == !stripAll, "sibling compose partial complements strip-all") && ok;
        ok                  = expect(partial == inlineShouldStripPartialStealthOnRetain(stripAll),
                    "sibling compose free == inline") &&
             ok;
    }

    // Dense compose: free == pin == inline over strip-all poles from retainChance/roll.
    const struct
    {
        int16 retainChance;
        int   roll0to99;
    } poles[] = {
        { 0, 0 }, { 0, 1 }, { 0, 99 },
        { 40, 0 }, { 40, 39 }, { 40, 40 }, { 40, 41 }, { 40, 99 },
        { 99, 0 }, { 99, 98 }, { 99, 99 }, { 99, 100 },
        { 100, 99 }, { 100, 100 },
        { 1, 0 }, { 1, 1 }, { 1, 2 },
        { -1, -1 }, { -1, 0 },
    };
    for (const auto& p : poles)
    {
        const bool stripAll = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        const bool got      = ShouldStripPartialStealthOnRetain(stripAll);
        ok                  = expect(got == !stripAll, "dense free == pin") && ok;
        ok                  = expect(got == inlineShouldStripPartialStealthOnRetain(stripAll),
                    "dense free == inline") &&
             ok;
    }

    // Explicit production inject poles matching OnRangedAttack else-if(false).
    ok = expect(ShouldStripPartialStealthOnRetain(false), "production inject equality path must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(ShouldStripAllDetectableOnFail(40, 41)),
                "production inject fail path must not partial-strip") &&
         ok;
    ok = expect(ShouldStripPartialStealthOnRetain(ShouldStripAllDetectableOnFail(40, 40)),
                "production inject retain path must partial-strip") &&
         ok;

    return ok;
}
