#include "test_map_session_create_policy_2783.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session create policy 2783 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapSessionCreatePolicy2783SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;

    bool ok = true;

    // createSession: requires successful query AND at least one accounts_sessions row.
    ok = expect(ShouldCreateSession(true, true), "create: query ok with row") && ok;
    ok = expect(!ShouldCreateSession(true, false), "create: query ok empty rejects") && ok;
    ok = expect(!ShouldCreateSession(false, false), "create: query fail rejects") && ok;
    ok = expect(!ShouldCreateSession(false, true), "create: query fail ignores hasRow") && ok;

    // createPendingSession: only a failed query rejects; empty result still creates.
    ok = expect(ShouldCreatePendingSession(true), "pending: query ok creates") && ok;
    ok = expect(!ShouldCreatePendingSession(false), "pending: query fail rejects") && ok;

    // Asymmetry: confirmed empty-row is reject; pending has no row gate.
    ok = expect(!ShouldCreateSession(true, false) && ShouldCreatePendingSession(true),
                "asymmetry: empty row only blocks confirmed") &&
        ok;

    return ok;
}
