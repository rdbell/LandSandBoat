#include "test_char_cast_finish_1497.h"

#include "map/char_cast_finish_capacity.h"

#include <iostream>

namespace
{
using charcastfinishhelpers::RemainingTPAfterBlueSkillchain;
using charcastfinishhelpers::ShouldApplyBlueSkillchain;
using charcastfinishhelpers::ShouldApplyImmanenceSkillchain;
using charcastfinishhelpers::ShouldResolveTrickAttack;

auto Check() -> bool
{
    if (!ShouldResolveTrickAttack(true, true, true) ||
        ShouldResolveTrickAttack(false, true, true) ||
        ShouldResolveTrickAttack(true, false, true) ||
        ShouldResolveTrickAttack(true, true, false))
    {
        return false;
    }

    if (!ShouldApplyBlueSkillchain(1, true, true, true, true) ||
        ShouldApplyBlueSkillchain(0, true, true, true, true) ||
        ShouldApplyBlueSkillchain(1, false, true, true, true) ||
        ShouldApplyBlueSkillchain(1, true, false, true, true) ||
        ShouldApplyBlueSkillchain(1, true, true, false, true) ||
        ShouldApplyBlueSkillchain(1, true, true, true, false))
    {
        return false;
    }

    if (RemainingTPAfterBlueSkillchain(1500, true) != 500 ||
        RemainingTPAfterBlueSkillchain(500, true) != 0 ||
        RemainingTPAfterBlueSkillchain(1500, false) != 0)
    {
        return false;
    }

    if (!ShouldApplyImmanenceSkillchain(true, 0, true, true, true) ||
        ShouldApplyImmanenceSkillchain(false, 0, true, true, true) ||
        ShouldApplyImmanenceSkillchain(true, -1, true, true, true) ||
        ShouldApplyImmanenceSkillchain(true, 0, false, true, true) ||
        ShouldApplyImmanenceSkillchain(true, 0, true, false, true) ||
        ShouldApplyImmanenceSkillchain(true, 0, true, true, false))
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharCastFinish1497SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char cast finish 1497 self-test failed\n";
    }
    return ok;
}
