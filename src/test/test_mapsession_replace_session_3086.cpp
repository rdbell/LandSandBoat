#include "test_mapsession_replace_session_3086.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldReplaceExistingSession 3086 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer createSession / createPendingSession same-key
// replace gate for dual-wire cross-check (slice 3086):
//   previousPresent
auto inlineShouldReplaceExistingSession(const bool previousPresent) -> bool
{
    return previousPresent;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldReplaceExistingSession
// (createSession / createPendingSession same-key replace gate previousPresent
// identity; slice 3086).
auto runMapsessionReplaceSession3086SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;
    using mapsessionhelpers::ShouldDestroyPendingByCharID;
    using mapsessionhelpers::ShouldDestroyPendingByPointer;
    using mapsessionhelpers::ShouldReplaceExistingSession;

    bool ok = true;

    // Residual 2795 pins still hold under dual-wire.
    ok = expect(ShouldReplaceExistingSession(true), "residual: previous present replaces") && ok;
    ok = expect(!ShouldReplaceExistingSession(false), "residual: previous absent skips remove") && ok;

    const struct
    {
        bool        previousPresent;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — same-key owner present.
        { true, true, "previous present replaces" },

        // Residual 2795 reject pole.
        { false, false, "previous absent skips remove" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReplaceExistingSession(c.previousPresent);
        const bool inlineF = inlineShouldReplaceExistingSession(c.previousPresent);
        const bool wantPin = c.previousPresent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReplaceExistingSession dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldReplaceExistingSession == pin formula previousPresent") && ok;
    }

    // Pin composition: identity of previousPresent only.
    ok = expect(ShouldReplaceExistingSession(true), "previousPresent true must admit") && ok;
    ok = expect(!ShouldReplaceExistingSession(false), "previousPresent false must reject") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool previousPresent : { false, true })
    {
        const bool got  = ShouldReplaceExistingSession(previousPresent);
        const bool want = previousPresent;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldReplaceExistingSession(previousPresent),
                    "compose free == inline") &&
             ok;
    }

    // Gate is pure identity of previousPresent; both create paths share it.
    ok = expect(ShouldReplaceExistingSession(true) && !ShouldReplaceExistingSession(false),
                "confirmed and pending create share replace gate") &&
         ok;

    // --- Production MapSessionContainer::createSession / createPendingSession ---
    // Host injects:
    //   previousPresent = getSessionByIPP(ipp) != nullptr
    //                   | getPendingSessionByCharId(charId) != nullptr
    // when true  → index_.removeSession / removePendingSession(previous)
    // when false → skip index remove
    ok = expect(ShouldReplaceExistingSession(true), "replace previous present → remove path") && ok;
    ok = expect(!ShouldReplaceExistingSession(false), "replace previous absent → skip") && ok;

    // Explicit dual-wire: free function is previousPresent of injects.
    for (const bool previousPresent : { false, true })
    {
        ok = expect(ShouldReplaceExistingSession(previousPresent) == previousPresent, "host inject identity") && ok;
        ok = expect(ShouldReplaceExistingSession(previousPresent) ==
                        inlineShouldReplaceExistingSession(previousPresent),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Residual create-policy independence: SQL admission is separate from
    // same-key replace.
    ok = expect(!ShouldCreateSession(true, false) && ShouldReplaceExistingSession(true),
                "empty row must only block confirmed createSession, not replace") &&
         ok;
    ok = expect(ShouldCreatePendingSession(true) && !ShouldReplaceExistingSession(false),
                "pending create admits on queryOK alone; absent previous still skips remove") &&
         ok;

    // Sibling dual-wires 3056/3066 (destroy-pending) left alone — residual
    // coexistence only.
    ok = expect(ShouldReplaceExistingSession(true) && ShouldDestroyPendingByCharID(true),
                "previous present / found admit independent replace and charID destroy paths") &&
         ok;
    ok = expect(!ShouldReplaceExistingSession(false) && !ShouldDestroyPendingByPointer(true, false),
                "absent previous skips replace; pointer mismatch only blocks pointer destroy") &&
         ok;

    return ok;
}
