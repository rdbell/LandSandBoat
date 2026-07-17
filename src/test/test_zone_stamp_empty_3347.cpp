#include "test_zone_stamp_empty_3347.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldStampZoneEmptyTime 3347 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DecreaseZoneCounter empty-stamp formula for dual-wire cross-check
// (dedicated slice 3347 expand residual 2939):
//   charListEmpty
auto inlineShouldStampZoneEmptyTime(const bool charListEmpty) -> bool
{
    return charListEmpty;
}

// Compact dual-wire pin matching Go pinShouldStampZoneEmptyTime3347 /
// C++ capacity zonehelpers::ShouldStampZoneEmptyTime:
//   charListEmpty
auto pinShouldStampZoneEmptyTime(const bool charListEmpty) -> bool
{
    return charListEmpty;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldStampZoneEmptyTime
// (CharListEmpty() after decrease; OmegaXI internal/zone;
// dedicated slice 3347 expand residual 2939 / pure 1363).
//
// Coverage:
//   - free == inline == pin == charListEmpty
//   - residual 1363 / 2939 poles still hold
//   - dense 2¹ truth table for both bool poles
// Formula unchanged — not registered in CMake/main.
auto runZoneStampEmpty3347SelfTests() -> bool
{
    using zonehelpers::ShouldDespawnPCOnLeave;
    using zonehelpers::ShouldStampZoneEmptyTime;

    bool ok = true;

    // Residual 1363 / 2939 pins still hold under dual-wire.
    ok = expect(ShouldStampZoneEmptyTime(true), "residual empty list → stamp") && ok;
    ok = expect(!ShouldStampZoneEmptyTime(false), "residual non-empty → no stamp") && ok;
    ok = expect(ShouldDespawnPCOnLeave(false), "residual non-empty → despawn leave pin") && ok;
    ok = expect(!ShouldDespawnPCOnLeave(true), "residual empty → no despawn leave pin") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        charListEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2¹ boolean space (both poles).
        { true, true, "empty list stamps empty time" },
        { false, false, "non-empty list does not stamp" },

        // Residual 1363 / 2939 pins.
        { true, true, "residual empty stamp" },
        { false, false, "residual non-empty no stamp" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity true" },
        { false, false, "identity false" },

        // Host-style inject poles (CharListEmpty after DecreaseZoneCounter).
        { true, true, "host inject empty → stamp" },
        { false, false, "host inject non-empty → no stamp" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStampZoneEmptyTime(c.charListEmpty);
        const bool inlineF = inlineShouldStampZoneEmptyTime(c.charListEmpty);
        const bool pin     = pinShouldStampZoneEmptyTime(c.charListEmpty);
        const bool wantPin = c.charListEmpty;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula charListEmpty") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        bool        charListEmpty;
        bool        want;
        const char* label;
    } poles[] = {
        { true, true, "pole empty stamp" },
        { false, false, "pole non-empty no stamp" },
    };
    for (const auto& p : poles)
    {
        const bool got     = ShouldStampZoneEmptyTime(p.charListEmpty);
        const bool inlineF = inlineShouldStampZoneEmptyTime(p.charListEmpty);
        const bool pin     = pinShouldStampZoneEmptyTime(p.charListEmpty);
        ok                 = expect(got == p.want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense 2¹ truth table: free == inline == pin for both bool poles.
    for (const bool empty : { false, true })
    {
        const bool got     = ShouldStampZoneEmptyTime(empty);
        const bool inlineF = inlineShouldStampZoneEmptyTime(empty);
        const bool pin     = pinShouldStampZoneEmptyTime(empty);
        const bool want    = empty;
        ok                 = expect(got == want, "dense free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
    }

    // Host-style inject poles: free == inline == pin for DecreaseZoneCounter,
    // plus exclusivity vs ShouldDespawnPCOnLeave.
    for (const bool empty : { false, true })
    {
        const bool got     = ShouldStampZoneEmptyTime(empty);
        const bool inlineF = inlineShouldStampZoneEmptyTime(empty);
        const bool pin     = pinShouldStampZoneEmptyTime(empty);
        const bool despawn = ShouldDespawnPCOnLeave(empty);
        const bool want    = empty;
        ok                 = expect(got == want, "inject free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "inject free == inline == pin") && ok;
        // Stamp and despawn are exclusive mirror branches of the same flag.
        ok = expect(got != despawn, "stamp and despawn exclusive") && ok;
    }

    // Production DecreaseZoneCounter path semantics still hold.
    ok = expect(ShouldStampZoneEmptyTime(true), "DecreaseZoneCounter empty → stamp path") && ok;
    ok = expect(!ShouldStampZoneEmptyTime(false), "DecreaseZoneCounter non-empty → no stamp") && ok;
    ok = expect(ShouldDespawnPCOnLeave(false), "DecreaseZoneCounter non-empty → despawn path") && ok;
    ok = expect(!ShouldDespawnPCOnLeave(true), "DecreaseZoneCounter empty → no despawn") && ok;

    return ok;
}
