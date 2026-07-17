#include "test_zone_despawn_pc_leave_2975.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldDespawnPCOnLeave 2975 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DecreaseZoneCounter else-branch despawn formula for dual-wire
// cross-check (slice 2975):
//   !charListEmpty
auto inlineShouldDespawnPCOnLeave(const bool charListEmpty) -> bool
{
    return !charListEmpty;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldDespawnPCOnLeave
// (!CharListEmpty after decrease; slice 2975).
auto runZoneDespawnPCLeave2975SelfTests() -> bool
{
    using zonehelpers::ShouldDespawnPCOnLeave;
    using zonehelpers::ShouldStampZoneEmptyTime;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldDespawnPCOnLeave(false), "residual non-empty → despawn") && ok;
    ok = expect(!ShouldDespawnPCOnLeave(true), "residual empty → no despawn") && ok;

    const struct
    {
        bool        charListEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles (polarity opposite of ShouldStampZoneEmptyTime).
        { false, true, "non-empty list despawns leaving PC" },
        { true, false, "empty list does not despawn" },

        // Residual 1363 pins.
        { false, true, "residual non-empty despawn" },
        { true, false, "residual empty no despawn" },

        // Identity / polarity repeats for dual-wire stability.
        { false, true, "identity false → true" },
        { true, false, "identity true → false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnPCOnLeave(c.charListEmpty);
        const bool inlineF = inlineShouldDespawnPCOnLeave(c.charListEmpty);
        const bool wantPin = !c.charListEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDespawnPCOnLeave dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDespawnPCOnLeave == pin formula !charListEmpty") && ok;
    }

    // Pin composition: negation of charListEmpty only.
    ok = expect(ShouldDespawnPCOnLeave(false), "charListEmpty false must despawn") && ok;
    ok = expect(!ShouldDespawnPCOnLeave(true), "charListEmpty true must not despawn") && ok;

    // Explicit polarity: opposite of ShouldStampZoneEmptyTime.
    for (const bool empty : { true, false })
    {
        ok = expect(ShouldDespawnPCOnLeave(empty) != ShouldStampZoneEmptyTime(empty),
                    "polarity: despawn and stamp exclusive") &&
             ok;
        ok = expect(ShouldDespawnPCOnLeave(empty) == !empty, "polarity: despawn == !empty") && ok;
    }

    // Host-style inject poles: CharListEmpty() as bool after DecreaseZoneCounter.
    for (const bool empty : { true, false })
    {
        const bool stamp   = ShouldStampZoneEmptyTime(empty);
        const bool despawn = ShouldDespawnPCOnLeave(empty);
        ok                 = expect(despawn == !empty, "host inject dual-wire identity") && ok;
        ok                 = expect(despawn == inlineShouldDespawnPCOnLeave(empty),
                    "host inject free == inline") &&
             ok;
        // Stamp and despawn are exclusive mirror branches of the same flag.
        ok = expect(stamp != despawn, "stamp and despawn exclusive") && ok;
    }

    // Production DecreaseZoneCounter path semantics:
    // empty → stamp m_timeZoneEmpty; non-empty → DespawnPC (else branch).
    ok = expect(ShouldDespawnPCOnLeave(false), "DecreaseZoneCounter non-empty → despawn path") && ok;
    ok = expect(!ShouldDespawnPCOnLeave(true), "DecreaseZoneCounter empty → no despawn") && ok;
    ok = expect(ShouldStampZoneEmptyTime(true), "DecreaseZoneCounter empty → stamp path") && ok;
    ok = expect(!ShouldStampZoneEmptyTime(false), "DecreaseZoneCounter non-empty → no stamp") && ok;

    // Dense compose: full bool space free == inline == pin.
    for (const bool empty : { false, true })
    {
        const bool got  = ShouldDespawnPCOnLeave(empty);
        const bool want = !empty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDespawnPCOnLeave(empty), "compose free == inline") && ok;
    }

    return ok;
}
