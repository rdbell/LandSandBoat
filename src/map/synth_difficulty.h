#pragma once

#include "modifier.h"

namespace synthdifficultyhelpers
{

constexpr auto ModForSkill(const uint8 skillID) -> Mod
{
    switch (skillID)
    {
        case 49: // SKILL_WOODWORKING
            return Mod::WOOD;
        case 50: // SKILL_SMITHING
            return Mod::SMITH;
        case 51: // SKILL_GOLDSMITHING
            return Mod::GOLDSMITH;
        case 52: // SKILL_CLOTHCRAFT
            return Mod::CLOTH;
        case 53: // SKILL_LEATHERCRAFT
            return Mod::LEATHER;
        case 54: // SKILL_BONECRAFT
            return Mod::BONE;
        case 55: // SKILL_ALCHEMY
            return Mod::ALCHEMY;
        case 56: // SKILL_COOKING
            return Mod::COOK;
        default:
            return Mod::NONE;
    }
}

constexpr auto Calculate(const uint8 skillRequired, const uint16 realSkillTenths, const int16 skillMod) -> int16
{
    const uint8 characterSkill = realSkillTenths / 10;
    return skillRequired - characterSkill - skillMod;
}

} // namespace synthdifficultyhelpers
