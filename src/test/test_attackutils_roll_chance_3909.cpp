#include "test_attackutils_roll_chance_3909.h"
#include "map/utils/attackutils_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* label) -> bool { if (!v) std::cerr << "attackutils RollChancePercent 3909 failed: " << label << '\n'; return v; } }
auto runAttackutilsRollChance3909SelfTests() -> bool
{
    using attackutilshelpers::RollChancePercent;
    bool ok = true;
    ok = expect(!RollChancePercent(-1, 0), "negative chance") && ok;
    ok = expect(!RollChancePercent(0, 0), "zero chance") && ok;
    ok = expect(RollChancePercent(1, 0), "one accepts first roll") && ok;
    ok = expect(!RollChancePercent(1, 1), "one rejects second roll") && ok;
    ok = expect(RollChancePercent(100, 99), "100 accepts last valid roll") && ok;
    ok = expect(!RollChancePercent(99, 99), "99 rejects last valid roll") && ok;
    return ok;
}
