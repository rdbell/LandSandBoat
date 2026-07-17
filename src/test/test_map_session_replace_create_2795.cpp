#include "test_map_session_replace_create_2795.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session replace create 2795 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapSessionReplaceCreate2795SelfTests() -> bool
{
    using mapsessionhelpers::ShouldReplaceExistingSession;

    bool ok = true;

    // createSession / createPendingSession: remove previous index owner only
    // when the same-key lookup returns a non-null session.
    ok = expect(ShouldReplaceExistingSession(true), "previous present replaces") && ok;
    ok = expect(!ShouldReplaceExistingSession(false), "previous absent skips remove") && ok;

    // Gate is pure identity of previousPresent; both create paths share it.
    ok = expect(ShouldReplaceExistingSession(true) && !ShouldReplaceExistingSession(false),
                "confirmed and pending create share replace gate") &&
        ok;

    return ok;
}
