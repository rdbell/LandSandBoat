#include "test_mapsession_destroy_pending_ptr_3056.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldDestroyPendingByPointer 3056 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::destroyPendingSession(MapSession*) gate for
// dual-wire cross-check (slice 3056):
//   found && pointerMatches
auto inlineShouldDestroyPendingByPointer(const bool found, const bool pointerMatches) -> bool
{
    return found && pointerMatches;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldDestroyPendingByPointer
// (destroyPendingSession(MapSession*) gate found && pointerMatches; slice 3056).
auto runMapsessionDestroyPendingPtr3056SelfTests() -> bool
{
    using mapsessionhelpers::ShouldDestroyPendingByCharID;
    using mapsessionhelpers::ShouldDestroyPendingByPointer;

    bool ok = true;

    // Residual 2787 pins still hold under dual-wire.
    ok = expect(ShouldDestroyPendingByPointer(true, true), "residual: found match destroys") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(true, false), "residual: found mismatch rejects") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false), "residual: missing rejects") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, true), "residual: missing ignores match flag") && ok;

    const struct
    {
        bool        found;
        bool        pointerMatches;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — found and pointer identity.
        { true, true, true, "found match destroys" },

        // Residual 2787 reject poles.
        { true, false, false, "found mismatch rejects" },
        { false, false, false, "missing rejects" },
        { false, true, false, "missing ignores match flag" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool inlineF = inlineShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool wantPin = c.found && c.pointerMatches;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDestroyPendingByPointer dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDestroyPendingByPointer == pin formula found && pointerMatches") && ok;
    }

    // Pin composition: only found && pointerMatches admits.
    ok = expect(ShouldDestroyPendingByPointer(true, true), "found true, pointerMatches true must admit") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(true, false), "pointerMatches false must reject") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false), "found false must reject") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, true), "both false/true poles must reject") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
    for (const bool found : { false, true })
    {
        for (const bool pointerMatches : { false, true })
        {
            const bool got  = ShouldDestroyPendingByPointer(found, pointerMatches);
            const bool want = found && pointerMatches;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldDestroyPendingByPointer(found, pointerMatches),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production MapSessionContainer::destroyPendingSession(MapSession*) ---
    // Host injects (after null session guard; guard outside pure):
    //   found           = pending[session->charID] != nullptr
    //   pointerMatches  = pending[session->charID] == session
    // when true  → erase pending index + delete MapSession
    // when false → skip erase/delete
    ok = expect(ShouldDestroyPendingByPointer(true, true), "destroy found match → erase path") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(true, false), "destroy found mismatch → skip") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false), "destroy missing → skip") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, true), "destroy missing ignore-match → skip") && ok;

    // Explicit dual-wire: free function is found && pointerMatches of injects.
    for (const bool found : { false, true })
    {
        for (const bool pointerMatches : { false, true })
        {
            ok = expect(ShouldDestroyPendingByPointer(found, pointerMatches) ==
                            (found && pointerMatches),
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldDestroyPendingByPointer(found, pointerMatches) ==
                            inlineShouldDestroyPendingByPointer(found, pointerMatches),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Sibling residual independence: ShouldDestroyPendingByCharID only needs
    // presence (2787 residual; not dual-wired in this slice).
    ok = expect(!ShouldDestroyPendingByPointer(true, false) && ShouldDestroyPendingByCharID(true),
                "mismatch must only block pointer destroyPendingSession") &&
         ok;
    ok = expect(ShouldDestroyPendingByPointer(true, true) && ShouldDestroyPendingByCharID(true),
                "found match admits both pointer and charID residual paths") &&
         ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false) && !ShouldDestroyPendingByCharID(false),
                "missing must reject both pointer and charID residual paths") &&
         ok;

    return ok;
}
