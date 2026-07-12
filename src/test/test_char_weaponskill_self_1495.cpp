#include "test_char_weaponskill_self_1495.h"

#include "map/char_weaponskill_self_capacity.h"

#include <iostream>

namespace
{
using charweaponskillselfhelpers::BuildResult;

auto Check() -> bool
{
    {
        const auto r = BuildResult(true, 50);
        if (r.messageID != charweaponskillselfhelpers::MsgUsesSkillRecoversMP || r.healAmount != 50)
        {
            return false;
        }
    }
    {
        const auto r = BuildResult(false, 25);
        if (r.messageID != charweaponskillselfhelpers::MsgTargetRecoversMP || r.healAmount != 25)
        {
            return false;
        }
    }
    {
        const auto r = BuildResult(true, -10);
        if (r.healAmount != 0)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runCharWeaponSkillSelf1495SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char weaponskill self 1495 self-test failed\n";
    }
    return ok;
}
