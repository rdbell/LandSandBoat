#include "test_zone_create_timers_2992.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldCreateZoneTimers 2992 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline IncreaseZoneCounter create-timers formula for dual-wire
// cross-check (slice 2992):
//   !hasZoneTimerToken && !charListEmpty
auto inlineShouldCreateZoneTimers(const bool hasZoneTimerToken, const bool charListEmpty) -> bool
{
    return !hasZoneTimerToken && !charListEmpty;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldCreateZoneTimers
// (!hasZoneTimerToken && !charListEmpty after InsertPC; slice 2992).
// Dense 2² boolean space.
auto runZoneCreateTimers2992SelfTests() -> bool
{
    using zonehelpers::ShouldCreateZoneTimers;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldCreateZoneTimers(false, false), "residual no token + non-empty → create") && ok;
    ok = expect(!ShouldCreateZoneTimers(true, false), "residual has token → no create") && ok;

    const struct
    {
        bool        hasZoneTimerToken;
        bool        charListEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2² boolean space.
        { false, false, true, "no token + non-empty → create timers" },
        { false, true, false, "no token + empty → no create" },
        { true, false, false, "has token + non-empty → no create" },
        { true, true, false, "has token + empty → no create" },

        // Residual 1363 pins.
        { false, false, true, "residual create timers" },
        { true, false, false, "residual has token" },

        // Identity / polarity repeats for dual-wire stability.
        { false, false, true, "identity create pole" },
        { false, true, false, "identity empty pole" },
        { true, false, false, "identity token pole" },
        { true, true, false, "identity both true pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreateZoneTimers(c.hasZoneTimerToken, c.charListEmpty);
        const bool inlineF = inlineShouldCreateZoneTimers(c.hasZoneTimerToken, c.charListEmpty);
        const bool wantPin = !c.hasZoneTimerToken && !c.charListEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreateZoneTimers dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldCreateZoneTimers == pin formula !token && !empty") && ok;
    }

    // Pin composition: only (false, false) creates.
    ok = expect(ShouldCreateZoneTimers(false, false), "no token + non-empty must create") && ok;
    ok = expect(!ShouldCreateZoneTimers(false, true), "no token + empty must not create") && ok;
    ok = expect(!ShouldCreateZoneTimers(true, false), "has token + non-empty must not create") && ok;
    ok = expect(!ShouldCreateZoneTimers(true, true), "has token + empty must not create") && ok;

    // Explicit polarity: create requires both negations true.
    for (const bool token : { false, true })
    {
        for (const bool empty : { false, true })
        {
            const bool got  = ShouldCreateZoneTimers(token, empty);
            const bool want = !token && !empty;
            ok              = expect(got == want, "polarity: create == !token && !empty") && ok;
            ok              = expect(!(got && (token || empty)),
                        "polarity: create only when !token && !empty") &&
                 ok;
        }
    }

    // Host-style inject poles: zoneTimerToken_.has_value() + CharListEmpty()
    // after InsertPC in IncreaseZoneCounter.
    for (const bool token : { false, true })
    {
        for (const bool empty : { false, true })
        {
            const bool create = ShouldCreateZoneTimers(token, empty);
            const bool want   = !token && !empty;
            ok                = expect(create == want, "host inject dual-wire identity") && ok;
            ok                = expect(create == inlineShouldCreateZoneTimers(token, empty),
                        "host inject free == inline") &&
                 ok;
            // create only when first PC present and no timer token.
            ok = expect(!(create && token), "never create when token present") && ok;
            ok = expect(!(create && empty), "never create when char list empty") && ok;
            ok = expect(!(!create && !token && !empty), "no token + non-empty must create") && ok;
        }
    }

    // Production IncreaseZoneCounter path semantics:
    // no token + non-empty (after InsertPC) → createZoneTimers().
    ok = expect(ShouldCreateZoneTimers(false, false), "IncreaseZoneCounter no token + non-empty → create") && ok;
    ok = expect(!ShouldCreateZoneTimers(true, false), "IncreaseZoneCounter has token → no create") && ok;
    ok = expect(!ShouldCreateZoneTimers(false, true), "IncreaseZoneCounter empty list → no create") && ok;
    ok = expect(!ShouldCreateZoneTimers(true, true), "IncreaseZoneCounter has token + empty → no create") && ok;

    // Dense compose: full 2² free == inline == pin.
    for (const bool token : { false, true })
    {
        for (const bool empty : { false, true })
        {
            const bool got  = ShouldCreateZoneTimers(token, empty);
            const bool want = !token && !empty;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldCreateZoneTimers(token, empty),
                        "compose free == inline") &&
                 ok;
        }
    }

    return ok;
}
