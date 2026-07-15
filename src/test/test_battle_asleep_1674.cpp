#include "test_battle_asleep_1674.h"

#include "map/battle_asleep_capacity.h"

#include <iostream>

namespace
{
using namespace battleasleephelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle asleep 1674 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleAsleep1674SelfTests() -> bool
{
    bool ok = true;

    // --- pure pass-through of CInactiveState inject ---
    ok = expect(IsAsleep(true), "inactive true") && ok;
    ok = expect(!IsAsleep(false), "inactive false") && ok;

    // --- identity: result equals inject ---
    ok = expect(IsAsleep(true) == true, "true identity") && ok;
    ok = expect(IsAsleep(false) == false, "false identity") && ok;

    return ok;
}
