#include "test_mapsession_create_session_2925.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldCreateSession 2925 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline createSession post-SELECT formula for dual-wire cross-check (slice 2925):
//   queryOK && hasAccountsSessionRow
auto inlineShouldCreateSession(const bool queryOK, const bool hasAccountsSessionRow) -> bool
{
    return queryOK && hasAccountsSessionRow;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldCreateSession
// (queryOK && hasAccountsSessionRow; slice 2925).
auto runMapsessionCreateSession2925SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;

    bool ok = true;

    // Residual 2783 truth-table pins.
    ok = expect(ShouldCreateSession(true, true), "residual: ok with row creates") && ok;
    ok = expect(!ShouldCreateSession(true, false), "residual: ok empty rejects") && ok;
    ok = expect(!ShouldCreateSession(false, false), "residual: query fail rejects") && ok;
    ok = expect(!ShouldCreateSession(false, true), "residual: query fail ignores hasRow") && ok;

    const struct
    {
        bool        queryOK;
        bool        hasAccountsSessionRow;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, true, "query ok with row creates" },
        { true, false, false, "query ok empty rejects" },
        { false, false, false, "query fail rejects" },
        { false, true, false, "query fail ignores hasRow" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreateSession(c.queryOK, c.hasAccountsSessionRow);
        const bool inlineF = inlineShouldCreateSession(c.queryOK, c.hasAccountsSessionRow);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreateSession dual-wire == inline LSB formula") && ok;
    }

    // Pin composition: both inputs required; hasRow alone never creates.
    ok = expect(!ShouldCreateSession(false, true), "!queryOK + hasRow must not create") && ok;
    ok = expect(ShouldCreateSession(true, true), "queryOK + hasRow must create") && ok;
    ok = expect(!ShouldCreateSession(true, false), "queryOK + !hasRow must not create") && ok;

    // Host-derived hasRow compose: production uses queryOK && rowsCount != 0.
    const struct
    {
        bool        queryOK;
        int         rowsCount;
        bool        wantCreate;
        const char* label;
    } hostCases[] = {
        { true, 1, true, "SELECT ok rows=1 → create" },
        { true, 2, true, "SELECT ok rows>1 → create" },
        { true, 0, false, "SELECT ok rows=0 → reject" },
        { false, 0, false, "SELECT fail → reject" },
        { false, 1, false, "query fail with rowsCount must still reject" },
    };

    for (const auto& c : hostCases)
    {
        const bool hasRow  = c.queryOK && c.rowsCount != 0;
        const bool got     = ShouldCreateSession(c.queryOK, hasRow);
        const bool inlineF = inlineShouldCreateSession(c.queryOK, hasRow);

        ok = expect(got == c.wantCreate, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == (c.queryOK && hasRow), "host compose free == queryOK && hasRow") && ok;
    }

    // Dense compose: full 2x2 truth table free == inline == pin formula.
    for (const bool queryOK : { false, true })
    {
        for (const bool hasRow : { false, true })
        {
            const bool got  = ShouldCreateSession(queryOK, hasRow);
            const bool want = queryOK && hasRow;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldCreateSession(queryOK, hasRow),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Asymmetry residual (2783): empty row blocks confirmed only.
    ok = expect(!ShouldCreateSession(true, false) && ShouldCreatePendingSession(true),
                "empty row only blocks confirmed createSession") &&
         ok;
    // Failed query rejects both paths.
    ok = expect(!ShouldCreateSession(false, false) && !ShouldCreatePendingSession(false),
                "query fail rejects confirmed and pending create") &&
         ok;

    return ok;
}
