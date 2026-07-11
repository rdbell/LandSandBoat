#include "test_login_data_a1_1323.h"

#include "common/cbasetypes.h"
#include "login/data_a1.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login data A1 1323 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginDataA11323SelfTests() -> bool
{
    using match_gate = loginHelpers::data_a1_account_match_gate;
    using row_gate   = loginHelpers::data_a1_account_row_gate;
    using maint_gate = loginHelpers::data_a1_maint_empty_gate;
    bool ok          = true;

    ok = expect(loginHelpers::DataA1AccountIDOffset == 1, "account id offset") && ok;
    ok = expect(loginHelpers::DataA1ServerIPOffset == 5, "server ip offset") && ok;

    ok = expect(loginHelpers::ClassifyDataA1AccountMatch(100, 100) == match_gate::MATCH, "match") && ok;
    ok = expect(loginHelpers::ClassifyDataA1AccountMatch(100, 101) == match_gate::MISMATCH, "mismatch") && ok;
    ok = expect(loginHelpers::ClassifyDataA1AccountMatch(0, 0) == match_gate::MATCH, "zero match") && ok;

    ok = expect(loginHelpers::ClassifyDataA1AccountRow(true, true) == row_gate::FOUND, "row found") && ok;
    ok = expect(loginHelpers::ClassifyDataA1AccountRow(true, false) == row_gate::MISSING, "no row") && ok;
    ok = expect(loginHelpers::ClassifyDataA1AccountRow(false, true) == row_gate::MISSING, "query fail") && ok;
    ok = expect(loginHelpers::ClassifyDataA1AccountRow(false, false) == row_gate::MISSING, "both fail") && ok;

    ok = expect(loginHelpers::FormatClaimedAccountMissingWarning(42) ==
                    "Claimed accountID 42 somehow doesn't have an account and should not have gotten this far.",
                "missing account warning") &&
         ok;
    ok = expect(loginHelpers::FormatClaimedAccountMissingWarning(std::numeric_limits<uint32>::max()) ==
                    "Claimed accountID 4294967295 somehow doesn't have an account and should not have gotten this far.",
                "missing account max id") &&
         ok;

    ok = expect(loginHelpers::ClassifyDataA1MaintEmptyList(0, 0) == maint_gate::ALLOW, "no maint empty allow") && ok;
    ok = expect(loginHelpers::ClassifyDataA1MaintEmptyList(0, 5) == maint_gate::ALLOW, "no maint with chars") && ok;
    ok = expect(loginHelpers::ClassifyDataA1MaintEmptyList(1, 1) == maint_gate::ALLOW, "maint with gm char") && ok;
    ok = expect(loginHelpers::ClassifyDataA1MaintEmptyList(1, 0) == maint_gate::REJECT, "maint empty reject") && ok;
    ok = expect(loginHelpers::ClassifyDataA1MaintEmptyList(2, 0) == maint_gate::REJECT, "maint 2 empty") && ok;

    // Preserve LSB misspelling and (0xA2) label on the 0xA1 path.
    ok = expect(loginHelpers::FormatMaintModeLoginAttemptWarning(7) ==
                    "char:(7) attmpted login during maintenance mode (0xA2). Sending error to client.",
                "maint warning typo parity") &&
         ok;

    ok = expect(loginHelpers::MainJobLevelColumnIndex(1) == 14, "war column") && ok;
    ok = expect(loginHelpers::MainJobLevelColumnIndex(6) == 19, "thf column") && ok;
    ok = expect(loginHelpers::MainJobLevelColumnIndex(0) == 13, "job 0") && ok;

    ok = expect(loginHelpers::ShouldIncludeCharacterInMaintList(0, 0), "normal list non-gm") && ok;
    ok = expect(loginHelpers::ShouldIncludeCharacterInMaintList(0, 1), "normal list gm") && ok;
    ok = expect(!loginHelpers::ShouldIncludeCharacterInMaintList(1, 0), "maint hide non-gm") && ok;
    ok = expect(loginHelpers::ShouldIncludeCharacterInMaintList(1, 1), "maint show gm") && ok;

    return ok;
}
