#include "test_map_session_destroy_pending_2787.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session destroy pending 2787 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapSessionDestroyPending2787SelfTests() -> bool
{
    using mapsessionhelpers::ShouldDestroyPendingByCharID;
    using mapsessionhelpers::ShouldDestroyPendingByPointer;

    bool ok = true;

    // destroyPendingSession(MapSession*): require found AND pointer identity.
    ok = expect(ShouldDestroyPendingByPointer(true, true), "pointer: found match destroys") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(true, false), "pointer: found mismatch rejects") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false), "pointer: missing rejects") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, true), "pointer: missing ignores match flag") && ok;

    // destroyPendingSession(uint32): found alone is sufficient.
    ok = expect(ShouldDestroyPendingByCharID(true), "charid: found destroys") && ok;
    ok = expect(!ShouldDestroyPendingByCharID(false), "charid: missing rejects") && ok;

    // Asymmetry: pointer path needs identity; charid path only needs presence.
    ok = expect(!ShouldDestroyPendingByPointer(true, false) && ShouldDestroyPendingByCharID(true),
                "asymmetry: mismatch only blocks pointer overload") &&
        ok;

    return ok;
}
