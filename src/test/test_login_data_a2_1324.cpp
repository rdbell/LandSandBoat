#include "test_login_data_a2_1324.h"

#include "common/cbasetypes.h"
#include "login/data_a2.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login data A2 1324 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginDataA21324SelfTests() -> bool
{
    using account_gate  = loginHelpers::data_a2_session_account_gate;
    using row_gate      = loginHelpers::data_a2_character_row_gate;
    using existing_gate = loginHelpers::data_a2_existing_session_gate;
    using admission     = loginHelpers::data_a2_admission_decision;
    bool ok             = true;

    ok = expect(loginHelpers::ClassifyDataA2SessionAccount(0) == account_gate::CORRUPT, "account 0 corrupt") && ok;
    ok = expect(loginHelpers::ClassifyDataA2SessionAccount(1) == account_gate::OK, "account ok") && ok;
    ok = expect(loginHelpers::ClassifyDataA2SessionAccount(0xFFFFFFFF) == account_gate::OK, "account max ok") && ok;

    ok = expect(loginHelpers::FormatDataA2CorruptSessionWarning("127.0.0.1") ==
                    "data_session: login data corrupt (0xA2). Disconnecting client 127.0.0.1",
                "corrupt warning") &&
         ok;

    ok = expect(loginHelpers::ClassifyDataA2CharacterRow(true, true) == row_gate::FOUND, "row found") && ok;
    ok = expect(loginHelpers::ClassifyDataA2CharacterRow(true, false) == row_gate::MISSING, "no row") && ok;
    ok = expect(loginHelpers::ClassifyDataA2CharacterRow(false, true) == row_gate::MISSING, "query fail") && ok;
    ok = expect(loginHelpers::ClassifyDataA2CharacterRow(false, false) == row_gate::MISSING, "both fail") && ok;

    ok = expect(loginHelpers::ClassifyDataA2ExistingSession(false, false, 0, 42) == existing_gate::NONE, "no query") && ok;
    ok = expect(loginHelpers::ClassifyDataA2ExistingSession(true, false, 0, 42) == existing_gate::NONE, "no row") && ok;
    ok = expect(loginHelpers::ClassifyDataA2ExistingSession(true, true, 99, 42) == existing_gate::OTHER_CHARACTER, "other char") && ok;
    ok = expect(loginHelpers::ClassifyDataA2ExistingSession(true, true, 42, 42) == existing_gate::ALREADY_LOGGED_IN, "same char") && ok;
    ok = expect(loginHelpers::DataA2AlreadyLoggedInKeyIncrement == 1, "key increment") && ok;

    ok = expect(loginHelpers::ShouldUpdatePrevZone(0), "prev 0 update") && ok;
    ok = expect(!loginHelpers::ShouldUpdatePrevZone(1), "prev non-zero") && ok;
    ok = expect(!loginHelpers::ShouldUpdatePrevZone(0xFFFF), "prev max") && ok;

    ok = expect(!loginHelpers::IsGMLevel(0), "gm 0") && ok;
    ok = expect(loginHelpers::IsGMLevel(1), "gm 1") && ok;
    ok = expect(loginHelpers::IsGMLevel(5), "gm 5") && ok;

    ok = expect(loginHelpers::LoginLimitOK(0, 999, false), "limit disabled") && ok;
    ok = expect(loginHelpers::LoginLimitOK(2, 1, false), "below limit") && ok;
    ok = expect(!loginHelpers::LoginLimitOK(2, 2, false), "at limit") && ok;
    ok = expect(loginHelpers::LoginLimitOK(2, 2, true), "excepted") && ok;

    ok = expect(loginHelpers::DecideDataA2Admission(false, 0, 0, false, false, false) == admission::ALLOWED, "ordinary") && ok;
    ok = expect(loginHelpers::DecideDataA2Admission(false, 2, 2, false, false, false) == admission::LOBBY_DENIED, "at limit deny") && ok;
    ok = expect(loginHelpers::DecideDataA2Admission(false, 2, 2, true, false, false) == admission::ALLOWED, "exception allow") && ok;
    ok = expect(loginHelpers::DecideDataA2Admission(true, 0, 0, false, false, false) == admission::LOBBY_DENIED, "maint deny") && ok;
    ok = expect(loginHelpers::DecideDataA2Admission(true, 0, 0, false, true, false) == admission::ALLOWED, "gm overrides maint") && ok;
    ok = expect(loginHelpers::DecideDataA2Admission(false, 0, 0, false, false, true) == admission::ZONE_AT_CAP, "zone cap") && ok;
    ok = expect(loginHelpers::DecideDataA2Admission(true, 1, 1, false, true, true) == admission::ZONE_AT_CAP, "zone precedes gm") && ok;

    ok = expect(loginHelpers::FormatDataA2LoginLimitWarning(9, "10.0.0.1", 3, 2) ==
                    "data_session: account 9 attempting to login when 10.0.0.1 already has 3 active session(s), limit is 2",
                "login limit warning") &&
         ok;
    ok = expect(loginHelpers::FormatDataA2ZoneCapWarning(230, 1001, false) ==
                    "data_session: zone 230 at player cap, denying charid 1001 (gm=0)",
                "zone cap non-gm") &&
         ok;
    ok = expect(loginHelpers::FormatDataA2ZoneCapWarning(230, 1001, true) ==
                    "data_session: zone 230 at player cap, denying charid 1001 (gm=1)",
                "zone cap gm") &&
         ok;

    return ok;
}
