#include "test_mapsession_destroy_pending_char_3066.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldDestroyPendingByCharID 3066 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::destroyPendingSession(uint32) gate for
// dual-wire cross-check (slice 3066):
//   found
auto inlineShouldDestroyPendingByCharID(const bool found) -> bool
{
    return found;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldDestroyPendingByCharID
// (destroyPendingSession(uint32) gate found identity; slice 3066).
auto runMapsessionDestroyPendingChar3066SelfTests() -> bool
{
    using mapsessionhelpers::ShouldDestroyPendingByCharID;
    using mapsessionhelpers::ShouldDestroyPendingByPointer;

    bool ok = true;

    // Residual 2787 pins still hold under dual-wire.
    ok = expect(ShouldDestroyPendingByCharID(true), "residual: found destroys") && ok;
    ok = expect(!ShouldDestroyPendingByCharID(false), "residual: missing rejects") && ok;

    const struct
    {
        bool        found;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — pending owner present for charId.
        { true, true, "found destroys" },

        // Residual 2787 reject pole.
        { false, false, "missing rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDestroyPendingByCharID(c.found);
        const bool inlineF = inlineShouldDestroyPendingByCharID(c.found);
        const bool wantPin = c.found;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDestroyPendingByCharID dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDestroyPendingByCharID == pin formula found") && ok;
    }

    // Pin composition: identity of found only.
    ok = expect(ShouldDestroyPendingByCharID(true), "found true must admit") && ok;
    ok = expect(!ShouldDestroyPendingByCharID(false), "found false must reject") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldDestroyPendingByCharID(found);
        const bool want = found;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDestroyPendingByCharID(found),
                    "compose free == inline") &&
             ok;
    }

    // --- Production MapSessionContainer::destroyPendingSession(uint32) ---
    // Host injects:
    //   found = pending[charId] != nullptr
    // when true  → erase pending index + delete MapSession
    // when false → skip erase/delete
    ok = expect(ShouldDestroyPendingByCharID(true), "destroy found → erase path") && ok;
    ok = expect(!ShouldDestroyPendingByCharID(false), "destroy missing → skip") && ok;

    // Explicit dual-wire: free function is found of injects.
    for (const bool found : { false, true })
    {
        ok = expect(ShouldDestroyPendingByCharID(found) == found, "host inject identity") && ok;
        ok = expect(ShouldDestroyPendingByCharID(found) == inlineShouldDestroyPendingByCharID(found),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Sibling dual-wire independence: ShouldDestroyPendingByPointer (3056)
    // requires identity; charID path only needs presence.
    ok = expect(!ShouldDestroyPendingByPointer(true, false) && ShouldDestroyPendingByCharID(true),
                "mismatch must only block pointer destroyPendingSession") &&
         ok;
    ok = expect(ShouldDestroyPendingByPointer(true, true) && ShouldDestroyPendingByCharID(true),
                "found match admits both pointer and charID paths") &&
         ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false) && !ShouldDestroyPendingByCharID(false),
                "missing must reject both pointer and charID paths") &&
         ok;

    return ok;
}
