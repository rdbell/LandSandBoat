#include "test_login_character_delete_1319.h"

#include "common/cbasetypes.h"
#include "login/character_delete.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login character delete 1319 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginCharacterDelete1319SelfTests() -> bool
{
    using gate = loginHelpers::character_delete_ownership_gate;
    bool ok    = true;

    ok = expect(loginHelpers::LookedUpAccountIDFromDeleteQuery(false, false, 99) == 0, "query fail → 0") && ok;
    ok = expect(loginHelpers::LookedUpAccountIDFromDeleteQuery(true, false, 99) == 0, "no row → 0") && ok;
    ok = expect(loginHelpers::LookedUpAccountIDFromDeleteQuery(false, true, 99) == 0, "fail ignores row") && ok;
    ok = expect(loginHelpers::LookedUpAccountIDFromDeleteQuery(true, true, 42) == 42, "found returns accid") && ok;
    ok = expect(loginHelpers::LookedUpAccountIDFromDeleteQuery(true, true, 0) == 0, "found zero accid") && ok;

    ok = expect(loginHelpers::ClassifyCharacterDeleteOwnership(100, 100) == gate::PROCEED, "match proceeds") && ok;
    ok = expect(loginHelpers::ClassifyCharacterDeleteOwnership(0, 0) == gate::PROCEED, "zero match proceeds") && ok;
    ok = expect(loginHelpers::ClassifyCharacterDeleteOwnership(0, 100) == gate::DENIED, "missing row denied") && ok;
    ok = expect(loginHelpers::ClassifyCharacterDeleteOwnership(200, 100) == gate::DENIED, "foreign denied") && ok;
    ok = expect(loginHelpers::ClassifyCharacterDeleteOwnership(100, 0) == gate::DENIED, "session zero foreign denied") && ok;

    ok = expect(loginHelpers::FormatCharacterDeleteWrongAccount(42) ==
                    "Account ID 42 tried to delete character not in their account.",
                "wrong-account diagnostic") &&
         ok;
    ok = expect(loginHelpers::FormatCharacterDeleteWrongAccount(std::numeric_limits<uint32>::max()) ==
                    "Account ID 4294967295 tried to delete character not in their account.",
                "wrong-account max id") &&
         ok;

    ok = expect(loginHelpers::FormatCharacterDeleteAttemptInfo(7, "10.0.0.1") ==
                    "attempt to delete char:<7> from ip:<10.0.0.1>",
                "attempt info") &&
         ok;
    ok = expect(loginHelpers::FormatCharacterDeleteAttemptInfo(0, "127.0.0.1") ==
                    "attempt to delete char:<0> from ip:<127.0.0.1>",
                "attempt info zero char") &&
         ok;

    return ok;
}
