#include "test_login_character_select_1318.h"

#include "common/cbasetypes.h"
#include "login/character_select.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login character select 1318 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginCharacterSelect1318SelfTests() -> bool
{
    using gate = loginHelpers::character_select_gate;
    bool ok    = true;

    ok = expect(loginHelpers::CharacterSelectIDOffset == 28, "id offset") && ok;
    ok = expect(loginHelpers::CharacterSelectNameOffset == 36, "name offset") && ok;
    ok = expect(loginHelpers::DataSelectNotifyPacketSize == 0x05, "notify size") && ok;
    ok = expect(loginHelpers::DataSelectNotifyCommand == 0x02, "notify command") && ok;

    ok = expect(loginHelpers::ClassifyCharacterSelect(false, false, 0, 100) == gate::MISMATCHED_NAME,
                "query fail is mismatched name") &&
         ok;
    ok = expect(loginHelpers::ClassifyCharacterSelect(true, false, 0, 100) == gate::MISMATCHED_NAME,
                "no row is mismatched name") &&
         ok;
    ok = expect(loginHelpers::ClassifyCharacterSelect(true, true, 200, 100) == gate::WRONG_ACCOUNT,
                "other account is wrong account") &&
         ok;
    ok = expect(loginHelpers::ClassifyCharacterSelect(true, true, 100, 100) == gate::PROCEED,
                "matching account proceeds") &&
         ok;
    ok = expect(loginHelpers::ClassifyCharacterSelect(true, true, 0, 0) == gate::PROCEED,
                "zero account match proceeds") &&
         ok;

    // queryOk false ignores rowFound/accountID (still mismatched name).
    ok = expect(loginHelpers::ClassifyCharacterSelect(false, true, 100, 100) == gate::MISMATCHED_NAME,
                "query fail ignores row") &&
         ok;

    ok = expect(loginHelpers::FormatCharacterSelectMismatchedName(42) ==
                    "Account ID 42 tried to select a character id with a mismatched character name.",
                "mismatch diagnostic") &&
         ok;
    ok = expect(loginHelpers::FormatCharacterSelectWrongAccount(std::numeric_limits<uint32>::max()) ==
                    "Account ID 4294967295 tried to login as character not in their account.",
                "wrong-account diagnostic") &&
         ok;

    ok = expect(loginHelpers::FormatMissingSessionHashWarning("1.2.3.4") ==
                    "Session requested without valid sessionHash from 1.2.3.4",
                "missing hash warning") &&
         ok;

    std::array<uint8, loginHelpers::DataSelectNotifyPacketSize> notify{};
    notify.fill(0xFF);
    loginHelpers::GenerateDataSelectNotifyPacket(notify.data());
    ok = expect(notify[0] == 0x02, "notify command byte") && ok;
    ok = expect(notify[1] == 0 && notify[2] == 0 && notify[3] == 0 && notify[4] == 0, "notify rest zero") && ok;

    return ok;
}
