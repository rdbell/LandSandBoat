#include "test_char_weaponskill_range_1494.h"

#include "map/char_weaponskill_range_capacity.h"

#include <iostream>

namespace
{
using charweaponskillrangehelpers::DamageSlot;
using charweaponskillrangehelpers::InRange;

auto Check() -> bool
{
    // 10 <= 5+2+3 = 10 boundary inclusive.
    if (!InRange(10.0f, 5.0f, 2.0f, 3.0f) || InRange(10.01f, 5.0f, 2.0f, 3.0f))
    {
        return false;
    }
    if (DamageSlot(191) != charweaponskillrangehelpers::SlotMain ||
        DamageSlot(192) != charweaponskillrangehelpers::SlotRanged ||
        DamageSlot(221) != charweaponskillrangehelpers::SlotRanged ||
        DamageSlot(222) != charweaponskillrangehelpers::SlotMain)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharWeaponSkillRange1494SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char weaponskill range 1494 self-test failed\n";
    }
    return ok;
}
