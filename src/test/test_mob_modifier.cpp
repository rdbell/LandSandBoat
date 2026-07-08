/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_mob_modifier.h"

#include "map/mob_modifier.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{

struct EnumCase
{
    std::int32_t actual;
    std::int32_t expected;
    std::string  label;
};

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        if (test.actual != test.expected)
        {
            std::cerr << "mob modifier self-test failed: " << test.label << " got "
                      << test.actual << " expected " << test.expected << '\n';
            ok = false;
        }
    }
    return ok;
}

auto enumValue(MOBMODIFIER value) -> std::int32_t
{
    return static_cast<std::int32_t>(value);
}

} // namespace

auto runMobModifierSelfTests() -> bool
{
    return expectAll({
        { enumValue(MOBMOD_NONE), 0, "MOBMOD_NONE" },
        { enumValue(MOBMOD_GIL_MIN), 1, "MOBMOD_GIL_MIN" },
        { enumValue(MOBMOD_GIL_MAX), 2, "MOBMOD_GIL_MAX" },
        { enumValue(MOBMOD_MP_BASE), 3, "MOBMOD_MP_BASE" },
        { enumValue(MOBMOD_SIGHT_RANGE), 4, "MOBMOD_SIGHT_RANGE" },
        { enumValue(MOBMOD_SOUND_RANGE), 5, "MOBMOD_SOUND_RANGE" },
        { enumValue(MOBMOD_BUFF_CHANCE), 6, "MOBMOD_BUFF_CHANCE" },
        { enumValue(MOBMOD_GA_CHANCE), 7, "MOBMOD_GA_CHANCE" },
        { enumValue(MOBMOD_HEAL_CHANCE), 8, "MOBMOD_HEAL_CHANCE" },
        { enumValue(MOBMOD_HP_HEAL_CHANCE), 9, "MOBMOD_HP_HEAL_CHANCE" },
        { enumValue(MOBMOD_SUBLINK), 10, "MOBMOD_SUBLINK" },
        { enumValue(MOBMOD_LINK_RADIUS), 11, "MOBMOD_LINK_RADIUS" },
        { enumValue(MOBMOD_SEES_THROUGH_ILLUSION), 12, "MOBMOD_SEES_THROUGH_ILLUSION" },
        { enumValue(MOBMOD_SEVERE_SPELL_CHANCE), 13, "MOBMOD_SEVERE_SPELL_CHANCE" },
        { enumValue(MOBMOD_SKILL_LIST), 14, "MOBMOD_SKILL_LIST" },
        { enumValue(MOBMOD_MUG_GIL), 15, "MOBMOD_MUG_GIL" },
        { enumValue(MOBMOD_DETECTION), 16, "MOBMOD_DETECTION" },
        { enumValue(MOBMOD_NO_DESPAWN), 17, "MOBMOD_NO_DESPAWN" },
        { enumValue(MOBMOD_VAR), 18, "MOBMOD_VAR" },
        { enumValue(MOBMOD_CAN_SHIELD_BLOCK), 19, "MOBMOD_CAN_SHIELD_BLOCK" },
        { enumValue(MOBMOD_NO_H2H_PENALTY), 20, "MOBMOD_NO_H2H_PENALTY" },
        { enumValue(MOBMOD_PET_SPELL_LIST), 21, "MOBMOD_PET_SPELL_LIST" },
        { enumValue(MOBMOD_NA_CHANCE), 22, "MOBMOD_NA_CHANCE" },
        { enumValue(MOBMOD_IMMUNITY), 23, "MOBMOD_IMMUNITY" },
        { enumValue(MOBMOD_GRADUAL_RAGE), 24, "MOBMOD_GRADUAL_RAGE" },
        { enumValue(MOBMOD_BUILD_RESIST), 25, "MOBMOD_BUILD_RESIST" },
        { enumValue(MOBMOD_SUPERLINK), 26, "MOBMOD_SUPERLINK" },
        { enumValue(MOBMOD_SPELL_LIST), 27, "MOBMOD_SPELL_LIST" },
        { enumValue(MOBMOD_EXP_BONUS), 28, "MOBMOD_EXP_BONUS" },
        { enumValue(MOBMOD_ASSIST), 29, "MOBMOD_ASSIST" },
        { enumValue(MOBMOD_SPECIAL_SKILL), 30, "MOBMOD_SPECIAL_SKILL" },
        { enumValue(MOBMOD_ROAM_DISTANCE), 31, "MOBMOD_ROAM_DISTANCE" },
        { enumValue(MOBMOD_DONT_ROAM_HOME), 32, "MOBMOD_DONT_ROAM_HOME" },
        { enumValue(MOBMOD_SPECIAL_COOL), 33, "MOBMOD_SPECIAL_COOL" },
        { enumValue(MOBMOD_MAGIC_COOL), 34, "MOBMOD_MAGIC_COOL" },
        { enumValue(MOBMOD_STANDBACK_COOL), 35, "MOBMOD_STANDBACK_COOL" },
        { enumValue(MOBMOD_ROAM_COOL), 36, "MOBMOD_ROAM_COOL" },
        { enumValue(MOBMOD_ALWAYS_AGGRO), 37, "MOBMOD_ALWAYS_AGGRO" },
        { enumValue(MOBMOD_NO_DROPS), 38, "MOBMOD_NO_DROPS" },
        { enumValue(MOBMOD_SHARE_POS), 39, "MOBMOD_SHARE_POS" },
        { enumValue(MOBMOD_TELEPORT_CD), 40, "MOBMOD_TELEPORT_CD" },
        { enumValue(MOBMOD_TELEPORT_START), 41, "MOBMOD_TELEPORT_START" },
        { enumValue(MOBMOD_TELEPORT_END), 42, "MOBMOD_TELEPORT_END" },
        { enumValue(MOBMOD_TELEPORT_TYPE), 43, "MOBMOD_TELEPORT_TYPE" },
        { enumValue(MOBMOD_DUAL_WIELD), 44, "MOBMOD_DUAL_WIELD" },
        { enumValue(MOBMOD_ADD_EFFECT), 45, "MOBMOD_ADD_EFFECT" },
        { enumValue(MOBMOD_AUTO_SPIKES), 46, "MOBMOD_AUTO_SPIKES" },
        { enumValue(MOBMOD_SPAWN_LEASH), 47, "MOBMOD_SPAWN_LEASH" },
        { enumValue(MOBMOD_SHARE_TARGET), 48, "MOBMOD_SHARE_TARGET" },
        { enumValue(MOBMOD_CHECK_AS_NM), 49, "MOBMOD_CHECK_AS_NM" },
        { enumValue(MOBMOD_ROAM_RESET_FACING), 50, "MOBMOD_ROAM_RESET_FACING" },
        { enumValue(MOBMOD_ROAM_TURNS), 51, "MOBMOD_ROAM_TURNS" },
        { enumValue(MOBMOD_ROAM_RATE), 52, "MOBMOD_ROAM_RATE" },
        { enumValue(MOBMOD_BEHAVIOR), 53, "MOBMOD_BEHAVIOR" },
        { enumValue(MOBMOD_GIL_BONUS), 54, "MOBMOD_GIL_BONUS" },
        { enumValue(MOBMOD_IDLE_DESPAWN), 55, "MOBMOD_IDLE_DESPAWN" },
        { enumValue(MOBMOD_HP_STANDBACK), 56, "MOBMOD_HP_STANDBACK" },
        { enumValue(MOBMOD_MAGIC_DELAY), 57, "MOBMOD_MAGIC_DELAY" },
        { enumValue(MOBMOD_SPECIAL_DELAY), 58, "MOBMOD_SPECIAL_DELAY" },
        { enumValue(MOBMOD_BASE_DAMAGE_MODIFIER), 59, "MOBMOD_BASE_DAMAGE_MODIFIER" },
        { enumValue(MOBMOD_SPAWN_ANIMATIONSUB), 60, "MOBMOD_SPAWN_ANIMATIONSUB" },
        { enumValue(MOBMOD_HP_SCALE), 61, "MOBMOD_HP_SCALE" },
        { enumValue(MOBMOD_NO_STANDBACK), 62, "MOBMOD_NO_STANDBACK" },
        { enumValue(MOBMOD_ATTACK_SKILL_LIST), 63, "MOBMOD_ATTACK_SKILL_LIST" },
        { enumValue(MOBMOD_CHARMABLE), 64, "MOBMOD_CHARMABLE" },
        { enumValue(MOBMOD_NO_MOVE), 65, "MOBMOD_NO_MOVE" },
        { enumValue(MOBMOD_MULTI_HIT), 66, "MOBMOD_MULTI_HIT" },
        { enumValue(MOBMOD_NO_AGGRO), 67, "MOBMOD_NO_AGGRO" },
        { enumValue(MOBMOD_ALLI_HATE), 68, "MOBMOD_ALLI_HATE" },
        { enumValue(MOBMOD_NO_LINK), 69, "MOBMOD_NO_LINK" },
        { enumValue(MOBMOD_NO_REST), 70, "MOBMOD_NO_REST" },
        { enumValue(MOBMOD_LEADER), 71, "MOBMOD_LEADER" },
        { enumValue(MOBMOD_MAGIC_RANGE), 72, "MOBMOD_MAGIC_RANGE" },
        { enumValue(MOBMOD_TARGET_DISTANCE_OFFSET), 73, "MOBMOD_TARGET_DISTANCE_OFFSET" },
        { enumValue(MOBMOD_ONE_WAY_LINKING), 74, "MOBMOD_ONE_WAY_LINKING" },
        { enumValue(MOBMOD_CAN_PARRY), 75, "MOBMOD_CAN_PARRY" },
        { enumValue(MOBMOD_NO_WIDESCAN), 76, "MOBMOD_NO_WIDESCAN" },
        { enumValue(MOBMOD_TRUST_DISTANCE), 77, "MOBMOD_TRUST_DISTANCE" },
        { enumValue(MOBMOD_STANDBACK_RANGE), 78, "MOBMOD_STANDBACK_RANGE" },
        { enumValue(MOBMOD_CANNOT_GUARD), 79, "MOBMOD_CANNOT_GUARD" },
        { enumValue(MOBMOD_SKIP_ALLEGIANCE_CHECK), 80, "MOBMOD_SKIP_ALLEGIANCE_CHECK" },
        { enumValue(MOBMOD_ABILITY_RESPONSE), 81, "MOBMOD_ABILITY_RESPONSE" },
        { enumValue(MOBMOD_RUN_SPEED_MULT), 82, "MOBMOD_RUN_SPEED_MULT" },
        { enumValue(MOBMOD_CLAIM_TYPE), 83, "MOBMOD_CLAIM_TYPE" },
        { enumValue(MOBMOD_NO_SPELL_COST), 84, "MOBMOD_NO_SPELL_COST" },
        { enumValue(MOBMOD_ASTRAL_PET_OFFSET), 85, "MOBMOD_ASTRAL_PET_OFFSET" },
        { enumValue(MOBMOD_BASE_DAMAGE_MULTIPLIER), 86, "MOBMOD_BASE_DAMAGE_MULTIPLIER" },
        { enumValue(MOBMOD_DAMAGE_OFFSET), 87, "MOBMOD_DAMAGE_OFFSET" },
        { enumValue(MOBMOD_RANGED_DAMAGE_OFFSET), 88, "MOBMOD_RANGED_DAMAGE_OFFSET" },
        { enumValue(MOBMOD_AVATAR_PETID), 89, "MOBMOD_AVATAR_PETID" },
        { enumValue(MOBMOD_AVATAR_ASTRAL_DELAY), 90, "MOBMOD_AVATAR_ASTRAL_DELAY" },
        { enumValue(MOBMOD_H2H_SINGLE_SWING), 91, "MOBMOD_H2H_SINGLE_SWING" },
        { enumValue(MOBMOD_AOE_HIT_ALL), 92, "MOBMOD_AOE_HIT_ALL" },
        { enumValue(MOBMOD_RANGED_ATTACK_RANGE), 93, "MOBMOD_RANGED_ATTACK_RANGE" },
        { enumValue(MOBMOD_FOLLOW_LEASH_RANGE), 94, "MOBMOD_FOLLOW_LEASH_RANGE" },
        { enumValue(MOBMOD_FOLLOW_STOP_RANGE), 95, "MOBMOD_FOLLOW_STOP_RANGE" },
        { enumValue(MOBMOD_TRUST_SHIELD_SIZE), 96, "MOBMOD_TRUST_SHIELD_SIZE" },
    });
}
