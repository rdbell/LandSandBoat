/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_job_points.h"

#include "map/job_points.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectU16(std::uint16_t actual, std::uint16_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "job points self-test failed: " << label << " got " << actual
                  << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectU8(std::uint8_t actual, std::uint8_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "job points self-test failed: " << label << " got "
                  << static_cast<unsigned>(actual) << " expected "
                  << static_cast<unsigned>(expected) << '\n';
        return false;
    }
    return true;
}

auto testJobPointCost() -> bool
{
    bool ok = true;
    ok      = expectU8(JobPointCost(0), 1, "cost(0)") && ok;
    ok      = expectU8(JobPointCost(1), 2, "cost(1)") && ok;
    ok      = expectU8(JobPointCost(19), 20, "cost(19)") && ok;
    ok      = expectU8(JobPointCost(20), 0, "cost(20) blocks raise") && ok;
    ok      = expectU8(JobPointCost(21), 1, "cost(21) wraps") && ok;
    return ok;
}

auto testCategoryAndTypeIndexMacros() -> bool
{
    bool ok = true;

    ok = expectU16(JobPointsCategoryByJobId(1), JPCATEGORY_WAR, "category WAR") && ok;
    ok = expectU16(JobPointsCategoryByJobId(2), JPCATEGORY_MNK, "category MNK") && ok;
    ok = expectU16(JobPointsCategoryByJobId(22), JPCATEGORY_RUN, "category RUN") && ok;

    ok = expectU16(JobPointsCategoryIndexByJpType(JP_MIGHTY_STRIKES_EFFECT), 1, "index WAR") && ok;
    ok = expectU16(JobPointsCategoryIndexByJpType(JP_ELEMENTAL_SFORZO_EFFECT), 22, "index RUN") && ok;

    ok = expectU8(JobPointTypeIndex(JP_MIGHTY_STRIKES_EFFECT), 0, "type index 0") && ok;
    ok = expectU8(JobPointTypeIndex(JP_BERSERK_EFFECT), 1, "type index 1") && ok;
    ok = expectU8(JobPointTypeIndex(JP_BRAZEN_RUSH_EFFECT), 2, "type index 2") && ok;
    ok = expectU8(JobPointTypeIndex(JP_DOUBLE_ATTACK_EFFECT), 9, "type index 9") && ok;

    return ok;
}

auto testCategoryConstants() -> bool
{
    bool ok = true;
    ok      = expectU16(JOBPOINTS_CATEGORY_COUNT, 22, "category count") && ok;
    ok      = expectU16(JOBPOINTS_CATEGORY_START, 0x020, "category start") && ok;
    ok      = expectU16(JOBPOINTS_JPTYPE_COUNT, 220, "type count") && ok;
    ok      = expectU16(JOBPOINTS_JPTYPE_PER_CATEGORY, 10, "types per category") && ok;
    ok      = expectU16(JOBPOINTS_MAX, 500, "max job points") && ok;
    ok      = expectU16(JOBPOINTS_CAPACITY_MAX, 30000, "capacity max") && ok;
    return ok;
}

auto testMenuCatalogAnchors() -> bool
{
    struct CatalogRow
    {
        std::uint16_t   category;
        JOBPOINT_TYPE    first;
        JOBPOINT_TYPE    second;
        JOBPOINT_TYPE    third;
        JOBPOINT_TYPE    last;
    };

    constexpr CatalogRow rows[] = {
        { JPCATEGORY_WAR, JP_MIGHTY_STRIKES_EFFECT, JP_BRAZEN_RUSH_EFFECT, JP_BERSERK_EFFECT, JP_DOUBLE_ATTACK_EFFECT },
        { JPCATEGORY_MNK, JP_HUNDRED_FISTS_EFFECT, JP_INNER_STRENGTH_EFFECT, JP_DODGE_EFFECT, JP_KICK_ATTACKS_EFFECT },
        { JPCATEGORY_WHM, JP_BENEDICTION_EFFECT, JP_ASYLUM_EFFECT, JP_DIVINE_SEAL_EFFECT, JP_BAR_SPELL_EFFECT },
        { JPCATEGORY_BLM, JP_MANAFONT_EFFECT, JP_SUBTLE_SORCERY_EFFECT, JP_ELEMENTAL_SEAL_EFFECT, JP_MAGIC_DMG_BONUS },
        { JPCATEGORY_RDM, JP_CHAINSPELL_EFFECT, JP_STYMIE_EFFECT, JP_CONVERT_EFFECT, JP_ENHANCING_DURATION },
        { JPCATEGORY_THF, JP_PERFECT_DODGE_EFFECT, JP_LARCENY_EFFECT, JP_SNEAK_ATTACK_EFFECT, JP_TRIPLE_ATTACK_EFFECT },
        { JPCATEGORY_PLD, JP_INVINCIBLE_EFFECT, JP_INTERVENE_EFFECT, JP_HOLY_CIRCLE_EFFECT, JP_ENLIGHT_EFFECT },
        { JPCATEGORY_DRK, JP_BLOOD_WEAPON_EFFECT, JP_SOUL_ENSLAVEMENT_EFFECT, JP_ARCANE_CIRCLE_EFFECT, JP_ENDARK_EFFECT },
        { JPCATEGORY_BST, JP_FAMILIAR_EFFECT, JP_UNLEASH_EFFECT, JP_PET_ACC_BONUS, JP_PET_EMNITY_BONUS },
        { JPCATEGORY_BRD, JP_SOUL_VOICE_EFFECT, JP_CLARION_CALL_EFFECT, JP_MINNE_EFFECT, JP_REQUIEM_EFFECT },
        { JPCATEGORY_RNG, JP_EAGLE_EYE_SHOT_EFFECT, JP_OVERKILL_EFFECT, JP_SHARPSHOT_EFFECT, JP_UNLIMITED_SHOT_EFFECT },
        { JPCATEGORY_SAM, JP_MEIKYO_SHISUI_EFFECT, JP_YAEGASUMI_EFFECT, JP_WARDING_CIRCLE_EFFECT, JP_ZANSHIN_EFFECT },
        { JPCATEGORY_NIN, JP_MIJIN_GAUKURE_EFFECT, JP_MIKAGE_EFFECT, JP_YONIN_EFFECT, JP_TACTICAL_PARRY_EFFECT },
        { JPCATEGORY_DRG, JP_SPIRIT_SURGE_EFFECT, JP_FLY_HIGH_EFFECT, JP_ANCIENT_CIRCLE_EFFECT, JP_WYVERN_ATTR_BONUS },
        { JPCATEGORY_SMN, JP_ASTRAL_FLOW_EFFECT, JP_ASTRAL_CONDUIT_EFFECT, JP_SUMMON_ACC_BONUS, JP_BLOOD_PACT_DMG_BONUS },
        { JPCATEGORY_BLU, JP_AZURE_LORE_EFFECT, JP_UNBRIDLED_WISDOM_EFFECT, JP_BLUE_MAGIC_POINT_BONUS, JP_BLU_MAGIC_ACC_BONUS },
        { JPCATEGORY_COR, JP_WILD_CARD_EFFECT, JP_CUTTING_CARDS_EFFECT, JP_PHANTOM_ROLL_DURATION, JP_OPTIMAL_RANGE_BONUS },
        { JPCATEGORY_PUP, JP_OVERDRIVE_EFFECT, JP_HEADY_ARTIFICE_EFFECT, JP_AUTOMATON_HP_MP_BONUS, JP_PUP_MARTIAL_ARTS_EFFECT },
        { JPCATEGORY_DNC, JP_TRANCE_EFFECT, JP_GRAND_PAS_EFFECT, JP_STEP_DURATION, JP_CONTRADANCE_EFFECT },
        { JPCATEGORY_SCH, JP_TABULA_RASA_EFFECT, JP_CAPER_EMMISSARIUS_EFFECT, JP_LIGHT_ARTS_EFFECT, JP_SUBLIMATION_EFFECT },
        { JPCATEGORY_GEO, JP_BOLSTER_EFFECT, JP_WIDENED_COMPASS_EFFECT, JP_LIFE_CYCLE_EFFECT, JP_INDI_SPELL_DURATION },
        { JPCATEGORY_RUN, JP_ELEMENTAL_SFORZO_EFFECT, JP_ODYLLIC_SUBTER_EFFECT, JP_RUNE_ENCHANTMENT_EFFECT, JP_GAMBIT_DURATION },
    };

    bool ok = true;
    for (const auto& row : rows)
    {
        ok = expectU16(row.first, row.category, "catalog first") && ok;
        ok = expectU16(row.second, row.category + 2, "catalog second") && ok;
        ok = expectU16(row.third, row.category + 1, "catalog third") && ok;
        ok = expectU16(row.last, row.category + 9, "catalog last") && ok;
    }
    return ok;
}

} // namespace

auto runJobPointsSelfTests() -> bool
{
    bool ok = true;
    ok      = testJobPointCost() && ok;
    ok      = testCategoryAndTypeIndexMacros() && ok;
    ok      = testCategoryConstants() && ok;
    ok      = testMenuCatalogAnchors() && ok;
    return ok;
}
