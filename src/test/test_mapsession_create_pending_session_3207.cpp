#include "test_mapsession_create_pending_session_3207.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldCreatePendingSession 3207 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline createPendingSession post-SELECT formula for dual-wire cross-check
// (dedicated slice 3207):
//   queryOK
auto inlineShouldCreatePendingSession(const bool queryOK) -> bool
{
    return queryOK;
}

// Compact dual-wire pin matching Go pinShouldCreatePendingSession3207 /
// C++ capacity (positive if form — avoid De Morgan rewrites):
//   if (queryOK) return true; return false
auto pinShouldCreatePendingSession(const bool queryOK) -> bool
{
    if (queryOK)
    {
        return true;
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldCreatePendingSession
// (queryOK identity; dedicated slice 3207 expand residual 2936 / pure 2783).
//
// Coverage:
//   - free == inline == pin == queryOK
//   - residual 2783 / 2936 pins still hold
//   - dense bool poles (queryOK only)
//   - sibling residual independence (ShouldCreateSession / 3191 not re-expanded;
//     contrast: confirmed has rowsCount gate; pending has none)
auto runMapsessionCreatePendingSession3207SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;

    bool ok = true;

    // Residual 2783 / 2936 pins still hold under dual-wire.
    ok = expect(ShouldCreatePendingSession(true), "residual 2936/2783: query ok creates") && ok;
    ok = expect(!ShouldCreatePendingSession(false), "residual 2936/2783: query fail rejects") && ok;

    const struct
    {
        bool        queryOK;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "pole query ok creates" },
        { false, false, "pole query fail rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreatePendingSession(c.queryOK);
        const bool inlineF = inlineShouldCreatePendingSession(c.queryOK);
        const bool pinF    = pinShouldCreatePendingSession(c.queryOK);
        const bool wantPin = c.queryOK;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreatePendingSession dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldCreatePendingSession dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldCreatePendingSession == queryOK identity") && ok;
    }

    // Pin composition: identity of queryOK only (no row-count input).
    ok = expect(ShouldCreatePendingSession(true), "queryOK must create") && ok;
    ok = expect(!ShouldCreatePendingSession(false), "!queryOK must not create") && ok;

    // Explicit polarity: create only when queryOK is true.
    for (const bool queryOK : { false, true })
    {
        const bool got  = ShouldCreatePendingSession(queryOK);
        const bool want = queryOK;
        ok              = expect(got == want, "polarity: free == queryOK") && ok;
        ok              = expect(!(got && !queryOK), "polarity: never create when !queryOK") && ok;
        ok              = expect(!(!got && queryOK), "polarity: always create when queryOK") && ok;
    }

    // Host path: createPendingSession post-SELECT inject before allocate.
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
        const bool pinF    = pinShouldCreatePendingSession(c.queryOK);

        ok = expect(got == c.wantCreate, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        // rowsCount must never flip the pending gate (LSB asymmetry vs
        // ShouldCreateSession which gates on hasAccountsSessionRow).
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
        ok = expect(got == pinShouldCreatePendingSession(queryOK), "compose free == pin") && ok;
    }

    // Residual independence (2783 / 2936 / 3191): pending has no rowsCount gate
    // (queryOK identity). Confirmed createSession (3191) still requires hasRow.
    // Sibling residual ShouldCreateSession not re-expanded under this suite.
    ok = expect(!ShouldCreateSession(true, false) && ShouldCreatePendingSession(true),
                "empty row only blocks confirmed createSession") &&
         ok;
    ok = expect(!ShouldCreateSession(false, false) && !ShouldCreatePendingSession(false),
                "query fail rejects confirmed and pending create") &&
         ok;
    ok = expect(ShouldCreateSession(true, true) && ShouldCreatePendingSession(true),
                "confirmed full-true / pending queryOK still admit under dual-wire") &&
         ok;
    // Contrast: confirmed rejects empty row; pending admits on queryOK alone.
    ok = expect(ShouldCreatePendingSession(true),
                "pending: no rowsCount gate — queryOK alone creates") &&
         ok;
    ok = expect(!ShouldCreateSession(true, false),
                "confirmed: rowsCount gate — empty row rejects") &&
         ok;

    return ok;
}
