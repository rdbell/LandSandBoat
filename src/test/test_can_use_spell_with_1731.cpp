#include "test_can_use_spell_with_1731.h"

#include "map/can_use_spell_with_capacity.h"

#include <iostream>

namespace
{
using namespace canusespellwithhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "can_use_spell_with 1731 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runCanUseSpellWith1731SelfTests() -> bool
{
    bool ok = true;

    // Missing spell (GetSpell == nullptr) → false for any levels.
    ok = expect(!CanUseSpellWith(false, 1, 99), "missing spell high level") && ok;
    ok = expect(!CanUseSpellWith(false, 0, 0), "missing spell zero levels") && ok;
    ok = expect(!CanUseSpellWith(false, 255, 255), "missing spell 255") && ok;

    // Strict greater-than (level > jobLevel), not >=.
    ok = expect(CanUseSpellWith(true, 10, 11), "level 11 > job 10") && ok;
    ok = expect(CanUseSpellWith(true, 1, 99), "level 99 > job 1") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 10), "level == jobLevel false") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 9), "level < jobLevel false") && ok;
    ok = expect(!CanUseSpellWith(true, 0, 0), "both zero false") && ok;
    ok = expect(CanUseSpellWith(true, 0, 1), "job 0 level 1 true") && ok;
    ok = expect(CanUseSpellWith(true, 254, 255), "254/255 true") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 255), "255/255 false") && ok;

    // getJob unusable mapping: stored 0 → 255; level > 255 impossible.
    ok = expect(!CanUseSpellWith(true, 255, 0), "unusable jobLevel 255 / 0") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 99), "unusable jobLevel 255 / 99") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 255), "unusable jobLevel 255 / 255") && ok;

    return ok;
}
