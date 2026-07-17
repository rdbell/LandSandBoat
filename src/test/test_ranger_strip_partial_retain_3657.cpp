#include "test_ranger_strip_partial_retain_3657.h"

#include "map/camouflage_retain_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldStripPartialStealthOnRetain 3657 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack retain-success partial-stealth gate for dual-wire
// cross-check (dedicated 3657 expand residual 3082; prior dedicated 3612 / 3567 / 3510 / 3445 / 3394):
//   !stripAllDetectable
auto inlineShouldStripPartialStealthOnRetain3657(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// Compact dual-wire pin matching Go pinShouldStripPartialStealthOnRetain3657 /
// C++ capacity form (formula unchanged from 1391 / 3082 / 3394 / 3445 / 3510 / 3567 / 3612):
//   !stripAllDetectable
auto pinShouldStripPartialStealthOnRetain3657(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// Residual prior dedicated pin (slice 3612) — formula identical.
auto pinShouldStripPartialStealthOnRetain3612(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// Residual prior dedicated pin (slice 3567) — formula identical.
auto pinShouldStripPartialStealthOnRetain3567(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// Residual prior dedicated pin (slice 3510) — formula identical.
auto pinShouldStripPartialStealthOnRetain3510(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// Residual prior dedicated pin (slice 3445) — formula identical.
auto pinShouldStripPartialStealthOnRetain3445(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// Residual prior dedicated pin (slice 3394) — formula identical.
auto pinShouldStripPartialStealthOnRetain3394(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

} // namespace

// Pure dual-wire expansion for camouflageretainhelpers::ShouldStripPartialStealthOnRetain
// (retain-success partial-stealth gate; OmegaXI internal/ranger;
// dedicated slice 3657 expand residual 3082; prior dedicated 3612 / 3567 / 3510 / 3445 / 3394 / pure 1391 —
// formula unchanged).
//
// Coverage:
//   - free == inline == pin == pin3612
//   - residual 3612 / 3567 / 3510 / 3445 / 3394 / 3082 / 1391 pins still hold
//   - poles: stripAllDetectable false → true; true → false
//   - dense pole compose
//   - host inject poles + strip-all sibling complement
//
// Sibling suites RETAINED: test_ranger_strip_partial_retain_3082,
// test_ranger_strip_partial_retain_3394, test_ranger_strip_partial_retain_3445,
// test_ranger_strip_partial_retain_3510, test_ranger_strip_partial_retain_3567,
// test_ranger_strip_partial_retain_3612.
// Siblings left alone: evaluate_camouflage_retain / strip_detectable_fail /
// strip_without_retain (3174 / 3344·3058 / 3118).
auto runRangerStripPartialRetain3657SelfTests() -> bool
{
    using camouflageretainhelpers::ShouldStripAllDetectableOnFail;
    using camouflageretainhelpers::ShouldStripPartialStealthOnRetain;

    bool ok = true;

    // Residual 1391 pins still hold under dedicated dual-wire.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "residual 1391: false → partial") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "residual 1391: true → no partial") && ok;

    // Residual 3082 edges still hold under dedicated dual-wire.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "residual 3082: false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "residual 3082: true must not partial-strip") && ok;

    // Prior dedicated 3394 edges still hold under expand residual 3657.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "prior 3394: false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "prior 3394: true must not partial-strip") && ok;

    // Prior dedicated 3445 edges still hold under expand residual 3657.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "prior 3445: false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "prior 3445: true must not partial-strip") && ok;

    // Prior dedicated 3510 edges still hold under expand residual 3657.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "prior 3510: false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "prior 3510: true must not partial-strip") && ok;

    // Prior dedicated 3567 edges still hold under expand residual 3657.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "prior 3567: false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "prior 3567: true must not partial-strip") && ok;

    // Prior dedicated 3612 edges still hold under expand residual 3657.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "prior 3612: false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "prior 3612: true must not partial-strip") && ok;

    // --- Core poles: free == inline == pin == pin3612 (required edges + stability) ---
    const struct
    {
        bool        stripAllDetectable;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dedicated poles: complement of strip-all Detectable gate.
        { false, true, "pole stripAllDetectable false → true (partial stealth)" },
        { true, false, "pole stripAllDetectable true → false (no partial; strip-all path)" },

        // Residual 1391 / 3082 / prior 3394 / prior 3445 / prior 3510 / prior 3567 / prior 3612 re-pins.
        { false, true, "residual retain success partial" },
        { true, false, "residual retain fail no partial" },
        { false, true, "prior 3394 retain success partial" },
        { true, false, "prior 3394 retain fail no partial" },
        { false, true, "prior 3445 retain success partial" },
        { true, false, "prior 3445 retain fail no partial" },
        { false, true, "prior 3510 retain success partial" },
        { true, false, "prior 3510 retain fail no partial" },
        { false, true, "prior 3567 retain success partial" },
        { true, false, "prior 3567 retain fail no partial" },
        { false, true, "prior 3612 retain success partial" },
        { true, false, "prior 3612 retain fail no partial" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStripPartialStealthOnRetain(c.stripAllDetectable);
        const bool inlineF = inlineShouldStripPartialStealthOnRetain3657(c.stripAllDetectable);
        const bool pin     = pinShouldStripPartialStealthOnRetain3657(c.stripAllDetectable);
        const bool pin3612 = pinShouldStripPartialStealthOnRetain3612(c.stripAllDetectable);
        const bool pin3567 = pinShouldStripPartialStealthOnRetain3567(c.stripAllDetectable);
        const bool pin3510 = pinShouldStripPartialStealthOnRetain3510(c.stripAllDetectable);
        const bool pin3445 = pinShouldStripPartialStealthOnRetain3445(c.stripAllDetectable);
        const bool pin3394 = pinShouldStripPartialStealthOnRetain3394(c.stripAllDetectable);
        // Positive form pin composition (explicit !stripAllDetectable).
        const bool wantPin = !c.stripAllDetectable;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldStripPartialStealthOnRetain free == inline == pin") &&
             ok;
        ok = expect(got == pin3612, "ShouldStripPartialStealthOnRetain free == pin3612") && ok;
        ok = expect(got == pin3567, "ShouldStripPartialStealthOnRetain free == pin3567") && ok;
        ok = expect(got == pin3510, "ShouldStripPartialStealthOnRetain free == pin3510") && ok;
        ok = expect(got == pin3445, "ShouldStripPartialStealthOnRetain free == pin3445") && ok;
        ok = expect(got == pin3394, "ShouldStripPartialStealthOnRetain free == pin3394") && ok;
    }

    // Free == pin across required poles.
    for (const bool stripAll : { false, true })
    {
        const bool got     = ShouldStripPartialStealthOnRetain(stripAll);
        const bool pin     = pinShouldStripPartialStealthOnRetain3657(stripAll);
        const bool inlineF = inlineShouldStripPartialStealthOnRetain3657(stripAll);
        const bool pin3612 = pinShouldStripPartialStealthOnRetain3612(stripAll);
        const bool pin3567 = pinShouldStripPartialStealthOnRetain3567(stripAll);
        const bool pin3510 = pinShouldStripPartialStealthOnRetain3510(stripAll);
        const bool pin3445 = pinShouldStripPartialStealthOnRetain3445(stripAll);
        const bool pin3394 = pinShouldStripPartialStealthOnRetain3394(stripAll);
        const bool want    = !stripAll;
        ok                 = expect(got == want, "required pole free == pin formula") && ok;
        ok                 = expect(got == pin && got == inlineF, "required pole free == inline == pin") && ok;
        ok                 = expect(got == pin3612, "required pole free == pin3612") && ok;
        ok                 = expect(got == pin3567, "required pole free == pin3567") && ok;
        ok                 = expect(got == pin3510, "required pole free == pin3510") && ok;
        ok                 = expect(got == pin3445, "required pole free == pin3445") && ok;
        ok                 = expect(got == pin3394, "required pole free == pin3394") && ok;
    }

    // Dense compose over strip-all poles from retainChance/roll: free == inline == pin == pin3612.
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
        const bool stripAll = ShouldStripAllDetectableOnFail(p.retainChance, p.roll0to99);
        const bool got      = ShouldStripPartialStealthOnRetain(stripAll);
        const bool inlineF  = inlineShouldStripPartialStealthOnRetain3657(stripAll);
        const bool pin      = pinShouldStripPartialStealthOnRetain3657(stripAll);
        const bool pin3612  = pinShouldStripPartialStealthOnRetain3612(stripAll);
        const bool pin3567  = pinShouldStripPartialStealthOnRetain3567(stripAll);
        const bool pin3510  = pinShouldStripPartialStealthOnRetain3510(stripAll);
        const bool pin3445  = pinShouldStripPartialStealthOnRetain3445(stripAll);
        const bool pin3394  = pinShouldStripPartialStealthOnRetain3394(stripAll);
        const bool want     = !stripAll;
        ok                  = expect(got == want, "compose free == pin formula") && ok;
        ok                  = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        ok                  = expect(got == pin3612, "compose free == pin3612") && ok;
        ok                  = expect(got == pin3567, "compose free == pin3567") && ok;
        ok                  = expect(got == pin3510, "compose free == pin3510") && ok;
        ok                  = expect(got == pin3445, "compose free == pin3445") && ok;
        ok                  = expect(got == pin3394, "compose free == pin3394") && ok;
        // Partial-stealth and strip-all complementary for all poles.
        ok = expect(got != stripAll, "compose partial-stealth complements strip-all") && ok;
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
        const bool got     = ShouldStripPartialStealthOnRetain(h.stripAllDetectable);
        const bool inlineF = inlineShouldStripPartialStealthOnRetain3657(h.stripAllDetectable);
        const bool pin     = pinShouldStripPartialStealthOnRetain3657(h.stripAllDetectable);
        const bool pin3612 = pinShouldStripPartialStealthOnRetain3612(h.stripAllDetectable);
        const bool pin3567 = pinShouldStripPartialStealthOnRetain3567(h.stripAllDetectable);
        const bool pin3510 = pinShouldStripPartialStealthOnRetain3510(h.stripAllDetectable);
        const bool pin3445 = pinShouldStripPartialStealthOnRetain3445(h.stripAllDetectable);
        const bool pin3394 = pinShouldStripPartialStealthOnRetain3394(h.stripAllDetectable);
        ok                 = expect(got == h.wantPartial, h.label) && ok;
        ok                 = expect(got == !h.stripAllDetectable, "host inject free == pin") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        ok                 = expect(got == pin3612, "host inject free == pin3612") && ok;
        ok                 = expect(got == pin3567, "host inject free == pin3567") && ok;
        ok                 = expect(got == pin3510, "host inject free == pin3510") && ok;
        ok                 = expect(got == pin3445, "host inject free == pin3445") && ok;
        ok                 = expect(got == pin3394, "host inject free == pin3394") && ok;
        // Complementary sibling strip-all gate: opposite of partial.
        ok = expect(got != h.stripAllDetectable,
                    "partial-stealth must complement strip-all") &&
             ok;
    }

    // Production inject pin: OnRangedAttack else-if uses free function with false.
    ok = expect(ShouldStripPartialStealthOnRetain(false), "production inject false must partial-strip") && ok;
    ok = expect(!ShouldStripPartialStealthOnRetain(true), "production inject true must not partial-strip") && ok;

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
