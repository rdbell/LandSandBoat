#include "test_mapsession_pending_session_2936.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldCreatePendingSession 2936 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline createPendingSession post-SELECT formula for dual-wire cross-check
// (slice 2936):
//   queryOK
auto inlineShouldCreatePendingSession(const bool queryOK) -> bool
{
    return queryOK;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldCreatePendingSession
// (queryOK identity; slice 2936).
auto runMapsessionPendingSession2936SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;

    bool ok = true;

    // Residual 2783 truth-table pins.
    ok = expect(ShouldCreatePendingSession(true), "residual: query ok creates") && ok;
    ok = expect(!ShouldCreatePendingSession(false), "residual: query fail rejects") && ok;

    const struct
    {
        bool        queryOK;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "query ok creates" },
        { false, false, "query fail rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreatePendingSession(c.queryOK);
        const bool inlineF = inlineShouldCreatePendingSession(c.queryOK);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreatePendingSession dual-wire == inline LSB formula") && ok;
    }

    // Pin composition: identity of queryOK only.
    ok = expect(ShouldCreatePendingSession(true), "queryOK must create") && ok;
    ok = expect(!ShouldCreatePendingSession(false), "!queryOK must not create") && ok;

    // Host path: after accounts_sessions SELECT by charid.
    // Production does not consult rowsCount for pending create.
    const struct
    {
        bool        queryOK;
        int         rowsCount;
        bool        wantCreate;
        const char* label;
    } hostCases[] = {
        { true, 1, true, "SELECT ok rows=1 → create" },
        { true, 2, true, "SELECT ok rows>1 → create" },
        { true, 0, true, "SELECT ok rows=0 → create (no row gate)" },
        { false, 0, false, "SELECT fail → reject" },
        { false, 1, false, "query fail with rowsCount must still reject" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldCreatePendingSession(c.queryOK);
        const bool inlineF = inlineShouldCreatePendingSession(c.queryOK);

        ok = expect(got == c.wantCreate, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        // rowsCount must never flip the pending gate (LSB asymmetry).
        ok = expect(got == c.queryOK, "host compose free == queryOK (rowsCount ignored)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool queryOK : { false, true })
    {
        const bool got  = ShouldCreatePendingSession(queryOK);
        const bool want = queryOK;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldCreatePendingSession(queryOK),
                    "compose free == inline") &&
             ok;
    }

    // Asymmetry residual (2783 / 2925): empty row blocks confirmed only.
    ok = expect(!ShouldCreateSession(true, false) && ShouldCreatePendingSession(true),
                "empty row only blocks confirmed createSession") &&
         ok;
    // Failed query rejects both paths.
    ok = expect(!ShouldCreateSession(false, false) && !ShouldCreatePendingSession(false),
                "query fail rejects confirmed and pending create") &&
         ok;
    // Confirmed requires hasRow; pending does not.
    ok = expect(ShouldCreateSession(true, true) && ShouldCreatePendingSession(true),
                "queryOK+row creates confirmed; queryOK alone creates pending") &&
         ok;

    return ok;
}
