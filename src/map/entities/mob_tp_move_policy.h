#pragma once
#include "common/cbasetypes.h"
namespace mobtpmovehelpers
{
inline auto ShouldUseTPMove(uint16 tp, bool hasSkills, bool weaponSkillEnabled, uint8 hpp, bool meikyo, uint32 meikyoCount, uint16 threshold) -> bool
{
    if (tp < 1000 || !hasSkills || !weaponSkillEnabled) return false;
    return tp == 3000 || hpp < 25 || (meikyo && meikyoCount > 0) || tp >= threshold;
}
}
