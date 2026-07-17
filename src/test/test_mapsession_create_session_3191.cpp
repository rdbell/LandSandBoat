#include "test_mapsession_create_session_3191.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldCreateSession 3191 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline createSession post-SELECT formula for dual-wire cross-check
// (dedicated slice 3191):
//   queryOK && hasAccountsSessionRow
auto inlineShouldCreateSession(const bool queryOK, const bool hasAccountsSessionRow) -> bool
{
    return queryOK && hasAccountsSessionRow;
}

// Compact dual-wire pin matching Go pinShouldCreateSession3191 /
// C++ capacity (positive if form — avoid De Morgan rewrites):
//   if (queryOK) { if (hasAccountsSessionRow) return true; } return false
auto pinShouldCreateSession(const bool queryOK, const bool hasAccountsSessionRow) -> bool
{
    if (queryOK)
    {
        if (hasAccountsSessionRow)
        {
            return true;
        }
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldCreateSession
// (queryOK && hasAccountsSessionRow; dedicated slice 3191 expand residual 2925 /
// pure 2783).
//
// Coverage:
//   - free == inline == pin == (queryOK && hasAccountsSessionRow)
//   - residual 2783 / 2925 pins still hold
//   - full 2² bool poles (queryOK × hasAccountsSessionRow)
//   - sibling residual independence (ShouldCreatePendingSession / 2936 not re-expanded)
auto runMapsessionCreateSession3191SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;

    bool ok = true;

    // Residual 2783 / 2925 pins still hold under dual-wire.
    ok = expect(ShouldCreateSession(true, true), "residual 2925/2783: ok with row creates") && ok;
    ok = expect(!ShouldCreateSession(true, false), "residual 2925/2783: ok empty rejects") && ok;
    ok = expect(!ShouldCreateSession(false, false), "residual 2925/2783: query fail rejects") && ok;
    ok = expect(!ShouldCreateSession(false, true), "residual 2925/2783: query fail ignores hasRow") && ok;

    const struct
    {
        bool        queryOK;
        bool        hasAccountsSessionRow;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, true, "pole query ok with row creates" },
        { true, false, false, "pole query ok empty rejects" },
        { false, false, false, "pole query fail rejects" },
        { false, true, false, "pole query fail ignores hasRow" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreateSession(c.queryOK, c.hasAccountsSessionRow);
        const bool inlineF = inlineShouldCreateSession(c.queryOK, c.hasAccountsSessionRow);
        const bool pinF    = pinShouldCreateSession(c.queryOK, c.hasAccountsSessionRow);
        const bool wantPin = c.queryOK && c.hasAccountsSessionRow;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreateSession dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldCreateSession dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldCreateSession == pin formula AND of two") && ok;
    }

    // Pin composition: both inputs required; hasRow alone never creates.
    ok = expect(!ShouldCreateSession(false, true), "!queryOK + hasRow must not create") && ok;
    ok = expect(ShouldCreateSession(true, true), "queryOK + hasRow must create") && ok;
    ok = expect(!ShouldCreateSession(true, false), "queryOK + !hasRow must not create") && ok;

    // Explicit polarity: create only when both injects are true.
    for (const bool queryOK : { false, true })
    {
        for (const bool hasRow : { false, true })
        {
            const bool got  = ShouldCreateSession(queryOK, hasRow);
            const bool want = queryOK && hasRow;
            ok              = expect(got == want, "polarity: free == queryOK && hasRow") && ok;
            ok              = expect(!(got && !(queryOK && hasRow)), "polarity: never create unless both true") &&
                 ok;
            ok = expect(!(!got && queryOK && hasRow), "polarity: always create when both true") && ok;
        }
    }

    // Host path: createSession post-SELECT inject before allocate.
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
        const bool pinF    = pinShouldCreateSession(c.queryOK, hasRow);

        ok = expect(got == c.wantCreate, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == (c.queryOK && hasRow), "host compose free == queryOK && hasRow") && ok;
    }

    // Dense compose: free == inline == pin formula for full 2×2 (2²).
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
            ok = expect(got == pinShouldCreateSession(queryOK, hasRow), "compose free == pin") && ok;
        }
    }

    // Residual independence (2783 / 2925 / 2936): confirmed row-count gate is
    // distinct from pending create (queryOK identity; no rowsCount).
    // Sibling residual ShouldCreatePendingSession not re-expanded under this suite.
    ok = expect(!ShouldCreateSession(true, false) && ShouldCreatePendingSession(true),
                "empty row only blocks confirmed createSession") &&
         ok;
    ok = expect(!ShouldCreateSession(false, false) && !ShouldCreatePendingSession(false),
                "query fail rejects confirmed and pending create") &&
         ok;
    ok = expect(ShouldCreateSession(true, true) && ShouldCreatePendingSession(true),
                "confirmed full-true / pending queryOK still admit under dual-wire") &&
         ok;

    return ok;
}
