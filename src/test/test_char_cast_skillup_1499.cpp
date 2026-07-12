#include "test_char_cast_skillup_1499.h"

#include "map/char_cast_skillup_capacity.h"

#include <iostream>

namespace
{
using charcastskilluphelpers::ShouldSkillUpRanged;

auto Check() -> bool
{
    if (!ShouldSkillUpRanged(charcastskilluphelpers::SkillGeomancy, charcastskilluphelpers::SkillHandbell) ||
        ShouldSkillUpRanged(charcastskilluphelpers::SkillGeomancy, charcastskilluphelpers::SkillSinging))
    {
        return false;
    }
    if (!ShouldSkillUpRanged(charcastskilluphelpers::SkillSinging, charcastskilluphelpers::SkillStringInstrument) ||
        !ShouldSkillUpRanged(charcastskilluphelpers::SkillSinging, charcastskilluphelpers::SkillWindInstrument) ||
        !ShouldSkillUpRanged(charcastskilluphelpers::SkillSinging, charcastskilluphelpers::SkillSinging) ||
        ShouldSkillUpRanged(charcastskilluphelpers::SkillSinging, charcastskilluphelpers::SkillHandbell))
    {
        return false;
    }
    if (ShouldSkillUpRanged(0, charcastskilluphelpers::SkillHandbell))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharCastSkillup1499SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char cast skillup 1499 self-test failed\n";
    }
    return ok;
}
