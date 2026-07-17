#include "test_zone_stamp_empty_2939.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldStampZoneEmptyTime 2939 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DecreaseZoneCounter empty-stamp formula for dual-wire cross-check (slice 2939):
//   charListEmpty
auto inlineShouldStampZoneEmptyTime(const bool charListEmpty) -> bool
{
    return charListEmpty;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldStampZoneEmptyTime
// (CharListEmpty() after decrease; slice 2939).
auto runZoneStampEmpty2939SelfTests() -> bool
{
    using zonehelpers::ShouldDespawnPCOnLeave;
    using zonehelpers::ShouldStampZoneEmptyTime;

    bool ok = true;

    const struct
    {
        bool        charListEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "empty list stamps empty time" },
        { false, false, "non-empty list does not stamp" },

        // Residual 1363 pins.
        { true, true, "residual empty stamp" },
        { false, false, "residual non-empty no stamp" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity true" },
        { false, false, "identity false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStampZoneEmptyTime(c.charListEmpty);
        const bool inlineF = inlineShouldStampZoneEmptyTime(c.charListEmpty);
        const bool wantPin = c.charListEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStampZoneEmptyTime dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStampZoneEmptyTime == pin formula charListEmpty") && ok;
    }

    // Pin composition: identity with charListEmpty only.
    ok = expect(ShouldStampZoneEmptyTime(true), "charListEmpty true must stamp") && ok;
    ok = expect(!ShouldStampZoneEmptyTime(false), "charListEmpty false must not stamp") && ok;

    // Host-style inject poles: CharListEmpty() as bool after DecreaseZoneCounter.
    for (const bool empty : { true, false })
    {
        const bool stamp   = ShouldStampZoneEmptyTime(empty);
        const bool despawn = ShouldDespawnPCOnLeave(empty);
        ok                 = expect(stamp == empty, "host inject dual-wire identity") && ok;
        ok                 = expect(stamp == inlineShouldStampZoneEmptyTime(empty),
                    "host inject free == inline") &&
             ok;
        // Stamp and despawn are exclusive mirror branches of the same flag.
        ok = expect(stamp != despawn, "stamp and despawn exclusive") && ok;
    }

    // Production DecreaseZoneCounter path semantics:
    // empty → stamp m_timeZoneEmpty; non-empty → DespawnPC (else branch).
    ok = expect(ShouldStampZoneEmptyTime(true), "DecreaseZoneCounter empty → stamp path") && ok;
    ok = expect(!ShouldStampZoneEmptyTime(false), "DecreaseZoneCounter non-empty → no stamp") && ok;
    ok = expect(ShouldDespawnPCOnLeave(false), "DecreaseZoneCounter non-empty → despawn path") && ok;
    ok = expect(!ShouldDespawnPCOnLeave(true), "DecreaseZoneCounter empty → no despawn") && ok;

    return ok;
}
