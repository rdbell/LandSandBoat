#include "test_automaton_frame_skill_1612.h"

#include "map/automaton_frame_skill_capacity.h"

#include <iostream>

namespace
{
using namespace automatonframeskillhelpers;

auto Check() -> bool
{
    if (EvasionSkillRank(FrameHarlequin) != 4 || EvasionSkillRank(0) != 4 || EvasionSkillRank(FrameValoredge) != 7 ||
        EvasionSkillRank(FrameSharpshot) != 2 || EvasionSkillRank(FrameStormwaker) != 10)
    {
        return false;
    }
    if (DefenseSkillRank(FrameHarlequin) != 11 || DefenseSkillRank(FrameValoredge) != 8 || DefenseSkillRank(FrameSharpshot) != 12 ||
        DefenseSkillRank(FrameStormwaker) != 12)
    {
        return false;
    }
    if (SpellInterrupt != 85)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAutomatonFrameSkill1612SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "automaton_frame_skill_1612 self-tests failed\n";
        return false;
    }
    return true;
}
