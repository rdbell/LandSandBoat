#include "test_avatar_stats_1603.h"

#include "map/avatar_stats_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace avatarstatshelpers;

auto nearly(const float a, const float b) -> bool
{
    return std::fabs(a - b) < 1e-4f;
}

auto Check() -> bool
{
    if (RaceGradeGroup != 3)
    {
        return false;
    }
    if (!IsAstralPet(PetIDOdin) || !IsAstralPet(PetIDAlexander) || IsAstralPet(8))
    {
        return false;
    }
    if (ApplyAstralHPBonus(30, PetIDOdin) != 30 + AstralHPBonus || ApplyAstralHPBonus(30, 8) != 30)
    {
        return false;
    }
    if (!nearly(ScaleJobStat(10.0f), 15.0f) || !nearly(ScaleJobStat(7.5f), 11.25f))
    {
        return false;
    }
    if (EvasionFromSkill(100) != 100 || EvasionFromSkill(200) != 200)
    {
        return false;
    }
    if (EvasionFromSkill(250) != 245 || EvasionFromSkill(201) != 200 || EvasionFromSkill(210) != 209)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAvatarStats1603SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "avatar_stats_1603 self-tests failed\n";
        return false;
    }
    return true;
}
