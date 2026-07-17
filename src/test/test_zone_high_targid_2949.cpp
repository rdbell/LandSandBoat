#include "test_zone_high_targid_2949.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldRejectHighCharTargid 2949 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline IncreaseZoneCounter high-char-targid formula for dual-wire cross-check (slice 2949):
//   targid >= CharTargidHighThreshold  // pin 0x700
auto inlineShouldRejectHighCharTargid(const uint16 targid) -> bool
{
    return targid >= zonehelpers::CharTargidHighThreshold;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldRejectHighCharTargid
// (GetNewCharTargID after IncreaseZoneCounter; slice 2949).
auto runZoneHighTargid2949SelfTests() -> bool
{
    using zonehelpers::CharTargidHighThreshold;
    using zonehelpers::ShouldRejectHighCharTargid;

    bool ok = true;

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

        // Residual 1363 pins.
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
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectHighCharTargid(c.targid);
        const bool inlineF = inlineShouldRejectHighCharTargid(c.targid);
        const bool wantPin = c.targid >= CharTargidHighThreshold;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectHighCharTargid dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectHighCharTargid == pin formula targid>=0x700") && ok;
    }

    // Pin threshold constant.
    ok = expect(CharTargidHighThreshold == static_cast<uint16>(0x700), "CharTargidHighThreshold == 0x700") && ok;

    // Pin composition: reject iff >= threshold.
    ok = expect(!ShouldRejectHighCharTargid(0x6FF), "0x6FF must accept") && ok;
    ok = expect(ShouldRejectHighCharTargid(0x700), "0x700 must reject") && ok;
    ok = expect(ShouldRejectHighCharTargid(0x701), "0x701 must reject") && ok;

    // Host-style inject poles: GetNewCharTargID() as uint16 after assign.
    const uint16 samples[] = { 0, 0x3FF, 0x400, 0x6FE, 0x6FF, 0x700, 0x701, 0x800, 0xFFFF };
    for (const uint16 targid : samples)
    {
        const bool reject = ShouldRejectHighCharTargid(targid);
        ok                = expect(reject == (targid >= CharTargidHighThreshold), "host inject dual-wire identity") && ok;
        ok                = expect(reject == inlineShouldRejectHighCharTargid(targid), "host inject free == inline") && ok;
        // Accept path is exclusive of reject path.
        const bool acceptInsert = !reject;
        ok                      = expect(acceptInsert != reject, "accept and reject exclusive") && ok;
    }

    // Dense dual-wire sweep around the threshold boundary.
    for (uint16 targid = 0x6F0; targid <= 0x710; ++targid)
    {
        const bool got  = ShouldRejectHighCharTargid(targid);
        const bool want = targid >= 0x700;
        ok              = expect(got == want, "boundary free == pin") && ok;
        ok              = expect(got == inlineShouldRejectHighCharTargid(targid), "boundary free == inline") && ok;
    }

    // Production IncreaseZoneCounter path semantics:
    // high → ShowError + return; under → InsertPC path.
    ok = expect(ShouldRejectHighCharTargid(0x700), "IncreaseZoneCounter high → reject path") && ok;
    ok = expect(!ShouldRejectHighCharTargid(0x6FF), "IncreaseZoneCounter under → insert path") && ok;

    return ok;
}
