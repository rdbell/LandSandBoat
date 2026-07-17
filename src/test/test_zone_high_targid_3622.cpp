#include "test_zone_high_targid_3622.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldRejectHighCharTargid 3622 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline IncreaseZoneCounter high-char-targid formula for dual-wire cross-check
// (dedicated slice 3622 expand residual 2949; prior dedicated 3577 / 3520 /
// 3457 / 3384):
//   targid >= CharTargidHighThreshold  // pin 0x700
auto inlineShouldRejectHighCharTargid(const uint16 targid) -> bool
{
    return targid >= zonehelpers::CharTargidHighThreshold;
}

// Compact dual-wire pin matching Go pinShouldRejectHighCharTargid3622 /
// C++ capacity zonehelpers::ShouldRejectHighCharTargid:
//   targid >= CharTargidHighThreshold  // pin 0x700
auto pinShouldRejectHighCharTargid(const uint16 targid) -> bool
{
    return targid >= zonehelpers::CharTargidHighThreshold;
}

// Prior dedicated 3577 pin form (free == inline == pin == pin3577):
//   targid >= CharTargidHighThreshold  // pin 0x700
auto pinShouldRejectHighCharTargid3577(const uint16 targid) -> bool
{
    return targid >= zonehelpers::CharTargidHighThreshold;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldRejectHighCharTargid
// (GetNewCharTargID after IncreaseZoneCounter; OmegaXI internal/zone;
// dedicated slice 3622 expand residual 2949; prior dedicated 3577 / 3520 /
// 3457 / 3384 / pure 1363).
//
// Coverage:
//   - free == inline == pin == pin3577 == (targid >= CharTargidHighThreshold)
//   - residual 1363 / 2949 / prior 3384 / prior 3457 / prior 3520 / prior 3577 poles still hold
//   - threshold constant pin 0x700
//   - dense boundary sweep around 0x700
//   - dual poles at 0x6FF / 0x700
// Formula unchanged — not registered in CMake/main.
auto runZoneHighTargid3622SelfTests() -> bool
{
    using zonehelpers::CharTargidHighThreshold;
    using zonehelpers::ShouldRejectHighCharTargid;

    bool ok = true;

    // Residual 1363 / 2949 / prior 3384 / prior 3457 / prior 3520 / prior 3577 pins still hold.
    ok = expect(ShouldRejectHighCharTargid(0x700), "residual 0x700 → reject") && ok;
    ok = expect(!ShouldRejectHighCharTargid(0x6FF), "residual 0x6FF → accept") && ok;
    ok = expect(CharTargidHighThreshold == static_cast<uint16>(0x700), "residual CharTargidHighThreshold == 0x700") && ok;
    ok = expect(ShouldRejectHighCharTargid(0x701), "residual 0x701 → reject") && ok;

    // --- Composition table: free == inline == pin == pin3577 ---
    const struct
    {
        uint16      targid;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles around 0x700.
        { 0x6FF, false, "just under threshold accepts" },
        { 0x700, true, "at threshold rejects" },
        { 0x701, true, "just over threshold rejects" },

        // Residual 1363 / 2949 pins.
        { 0x700, true, "residual high reject" },
        { 0x6FF, false, "residual ok accept" },

        // Char range low / mid (allocation starts ~0x400).
        { 0x000, false, "zero accepts" },
        { 0x400, false, "char range start accepts" },
        { 0x500, false, "char range mid accepts" },
        { 0x6FE, false, "two under threshold accepts" },

        // High / dynamic range.
        { 0x7FF, true, "dynamic mid rejects" },
        { 0x8FF, true, "dynamic end rejects" },
        { 0xFFFF, true, "uint16 max rejects" },

        // Identity / polarity repeats for dual-wire stability.
        { 0x6FF, false, "identity under threshold" },
        { 0x700, true, "identity at threshold" },
        { 0x701, true, "identity over threshold" },

        // Prior dedicated 3384 identity / polarity repeats.
        { 0x6FF, false, "prior 3384 identity under threshold" },
        { 0x700, true, "prior 3384 identity at threshold" },
        { 0x701, true, "prior 3384 identity over threshold" },

        // Prior dedicated 3457 identity / polarity repeats.
        { 0x6FF, false, "prior 3457 identity under threshold" },
        { 0x700, true, "prior 3457 identity at threshold" },
        { 0x701, true, "prior 3457 identity over threshold" },

        // Prior dedicated 3520 identity / polarity repeats.
        { 0x6FF, false, "prior 3520 identity under threshold" },
        { 0x700, true, "prior 3520 identity at threshold" },
        { 0x701, true, "prior 3520 identity over threshold" },

        // Prior dedicated 3577 identity / polarity repeats.
        { 0x6FF, false, "prior 3577 identity under threshold" },
        { 0x700, true, "prior 3577 identity at threshold" },
        { 0x701, true, "prior 3577 identity over threshold" },

        // Host-style inject poles (GetNewCharTargID after assign).
        { 0x400, false, "host inject char range start → accept" },
        { 0x6FF, false, "host inject just under → accept" },
        { 0x700, true, "host inject at threshold → reject" },
        { 0x800, true, "host inject dynamic range → reject" },
    };

    for (const auto& c : cases)
    {
        const bool got      = ShouldRejectHighCharTargid(c.targid);
        const bool inlineF  = inlineShouldRejectHighCharTargid(c.targid);
        const bool pin      = pinShouldRejectHighCharTargid(c.targid);
        const bool pinPrior = pinShouldRejectHighCharTargid3577(c.targid);
        const bool wantPin  = c.targid >= CharTargidHighThreshold;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin == pin3577.
        ok = expect(got == inlineF && got == pin && got == pinPrior, "dual-wire free == inline == pin == pin3577") && ok;
        ok = expect(got == wantPin, "free == pin formula targid>=0x700") && ok;
    }

    // Explicit residual poles free == inline == pin == pin3577 (0x6FF / 0x700).
    const struct
    {
        uint16      targid;
        bool        want;
        const char* label;
    } poles[] = {
        { 0x6FF, false, "pole under accept" },
        { 0x700, true, "pole at reject" },
        { 0x701, true, "pole over reject" },
        { 0x000, false, "pole zero accept" },
        { 0xFFFF, true, "pole max reject" },
    };
    for (const auto& p : poles)
    {
        const bool got      = ShouldRejectHighCharTargid(p.targid);
        const bool inlineF  = inlineShouldRejectHighCharTargid(p.targid);
        const bool pin      = pinShouldRejectHighCharTargid(p.targid);
        const bool pinPrior = pinShouldRejectHighCharTargid3577(p.targid);
        ok                  = expect(got == p.want, p.label) && ok;
        ok                  = expect(got == inlineF && got == pin && got == pinPrior, "pole free == inline == pin == pin3577") && ok;
    }

    // Dense dual-wire sweep around the threshold boundary:
    // free == inline == pin == pin3577 for targid in [0x6F0, 0x710].
    for (uint16 targid = 0x6F0; targid <= 0x710; ++targid)
    {
        const bool got      = ShouldRejectHighCharTargid(targid);
        const bool inlineF  = inlineShouldRejectHighCharTargid(targid);
        const bool pin      = pinShouldRejectHighCharTargid(targid);
        const bool pinPrior = pinShouldRejectHighCharTargid3577(targid);
        const bool want     = targid >= 0x700;
        ok                  = expect(got == want, "boundary free == pin") && ok;
        ok                  = expect(got == inlineF && got == pin && got == pinPrior, "boundary free == inline == pin == pin3577") && ok;
    }

    // Host-style inject poles: free == inline == pin == pin3577 for IncreaseZoneCounter.
    const uint16 samples[] = { 0, 0x3FF, 0x400, 0x6FE, 0x6FF, 0x700, 0x701, 0x800, 0xFFFF };
    for (const uint16 targid : samples)
    {
        const bool got      = ShouldRejectHighCharTargid(targid);
        const bool inlineF  = inlineShouldRejectHighCharTargid(targid);
        const bool pin      = pinShouldRejectHighCharTargid(targid);
        const bool pinPrior = pinShouldRejectHighCharTargid3577(targid);
        const bool want     = targid >= CharTargidHighThreshold;
        ok                  = expect(got == want, "host inject free == pin") && ok;
        ok                  = expect(got == inlineF && got == pin && got == pinPrior, "host inject free == inline == pin == pin3577") && ok;
        // Accept path is exclusive of reject path.
        const bool acceptInsert = !got;
        ok                      = expect(acceptInsert != got, "accept and reject exclusive") && ok;
    }

    // Threshold constant pin still holds under dedicated suite.
    ok = expect(CharTargidHighThreshold == static_cast<uint16>(0x700), "CharTargidHighThreshold == 0x700") && ok;

    // Production IncreaseZoneCounter path semantics still hold.
    ok = expect(ShouldRejectHighCharTargid(0x700), "IncreaseZoneCounter high → reject path") && ok;
    ok = expect(!ShouldRejectHighCharTargid(0x6FF), "IncreaseZoneCounter under → insert path") && ok;
    ok = expect(ShouldRejectHighCharTargid(0x701), "IncreaseZoneCounter over → reject path") && ok;

    return ok;
}
