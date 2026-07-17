#include "test_mapsession_reject_null_char_3108.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldRejectNullCharLookup 3108 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline getSessionByChar null-PChar gate for dual-wire cross-check
// (slice 3108):
//   charNull
auto inlineShouldRejectNullCharLookup(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldRejectNullCharLookup
// (getSessionByChar null-PChar early reject gate charNull identity; slice 3108).
auto runMapsessionRejectNullChar3108SelfTests() -> bool
{
    using mapsessionhelpers::SessionMatchesCharID;
    using mapsessionhelpers::ShouldDestroyPendingByCharID;
    using mapsessionhelpers::ShouldDestroyPendingByPointer;
    using mapsessionhelpers::ShouldRejectNullCharLookup;
    using mapsessionhelpers::ShouldReplaceExistingSession;

    bool ok = true;

    // Residual 2790 / prior dual-wire 2954 pins still hold under dual-wire.
    ok = expect(ShouldRejectNullCharLookup(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullCharLookup(false), "residual: non-null char proceeds") && ok;

    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic reject pole — PChar == nullptr.
        { true, true, "null char rejects" },

        // Residual 2790 proceed pole.
        { false, false, "non-null char proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullCharLookup(c.charNull);
        const bool inlineF = inlineShouldRejectNullCharLookup(c.charNull);
        const bool wantPin = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullCharLookup dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectNullCharLookup == pin formula charNull") && ok;
    }

    // Pin composition: identity of charNull only.
    ok = expect(ShouldRejectNullCharLookup(true), "charNull true must reject") && ok;
    ok = expect(!ShouldRejectNullCharLookup(false), "charNull false must proceed") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullCharLookup(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullCharLookup(charNull),
                    "compose free == inline") &&
             ok;
    }

    // Gate is pure identity of charNull.
    ok = expect(ShouldRejectNullCharLookup(true) && !ShouldRejectNullCharLookup(false),
                "null-char lookup gate must be identity of charNull") &&
         ok;

    // --- Production MapSessionContainer::getSessionByChar ---
    // Host injects:
    //   charNull = (PChar == nullptr)
    // when true  → return nullptr before confirmed-map scan
    // when false → proceed to SessionMatchesCharID loop match
    ok = expect(ShouldRejectNullCharLookup(true), "null PChar → reject before scan") && ok;
    ok = expect(!ShouldRejectNullCharLookup(false), "non-null PChar → proceed to scan") && ok;

    // Explicit dual-wire: free function is charNull of injects.
    for (const bool charNull : { false, true })
    {
        ok = expect(ShouldRejectNullCharLookup(charNull) == charNull, "host inject identity") && ok;
        ok = expect(ShouldRejectNullCharLookup(charNull) ==
                        inlineShouldRejectNullCharLookup(charNull),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Residual independence (2790): null reject and no-char match both block
    // via different pure helpers.
    ok = expect(ShouldRejectNullCharLookup(true) && !SessionMatchesCharID(false, 1, 1),
                "null reject and no-char mismatch both block") &&
         ok;
    ok = expect(!ShouldRejectNullCharLookup(false),
                "non-null must proceed even if later match fails") &&
         ok;
    ok = expect(SessionMatchesCharID(true, 42, 42),
                "has-char id match still holds under dual-wire residual") &&
         ok;

    // Sibling dual-wires 3056/3066/3086 left alone — residual coexistence only.
    ok = expect(ShouldRejectNullCharLookup(true) && ShouldDestroyPendingByCharID(true),
                "null reject / found admit independent lookup and charID destroy paths") &&
         ok;
    ok = expect(!ShouldRejectNullCharLookup(false) && !ShouldDestroyPendingByPointer(true, false),
                "non-null proceeds; pointer mismatch only blocks pointer destroy") &&
         ok;
    ok = expect(ShouldRejectNullCharLookup(true) && ShouldReplaceExistingSession(true),
                "null reject / previous present admit independent lookup and replace paths") &&
         ok;
    ok = expect(!ShouldRejectNullCharLookup(false) && !ShouldReplaceExistingSession(false),
                "non-null proceeds; absent previous skips replace") &&
         ok;

    return ok;
}
