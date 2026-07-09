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

#include "test_modifier.h"

#include "map/modifier.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectInt(std::int64_t actual, std::int64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "modifier self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testPetModTypeValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(PetModType::All), 0, "PetModType::All") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Avatar), 1, "PetModType::Avatar") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Wyvern), 2, "PetModType::Wyvern") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Automaton), 3, "PetModType::Automaton") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Harlequin), 4, "PetModType::Harlequin") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Valoredge), 5, "PetModType::Valoredge") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Sharpshot), 6, "PetModType::Sharpshot") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Stormwaker), 7, "PetModType::Stormwaker") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Luopan), 8, "PetModType::Luopan") && ok;
    return ok;
}

auto testModCoreValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::NONE), 0, "Mod::NONE") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEF), 1, "Mod::DEF") && ok;
    ok      = expectInt(static_cast<int>(Mod::HP), 2, "Mod::HP") && ok;
    ok      = expectInt(static_cast<int>(Mod::HPP), 3, "Mod::HPP") && ok;
    ok      = expectInt(static_cast<int>(Mod::MP), 5, "Mod::MP") && ok;
    ok      = expectInt(static_cast<int>(Mod::MPP), 6, "Mod::MPP") && ok;
    ok      = expectInt(static_cast<int>(Mod::STR), 8, "Mod::STR") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEX), 9, "Mod::DEX") && ok;
    ok      = expectInt(static_cast<int>(Mod::VIT), 10, "Mod::VIT") && ok;
    ok      = expectInt(static_cast<int>(Mod::AGI), 11, "Mod::AGI") && ok;
    ok      = expectInt(static_cast<int>(Mod::INT), 12, "Mod::INT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MND), 13, "Mod::MND") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHR), 14, "Mod::CHR") && ok;
    return ok;
}

auto testModElementalMagicEvasionValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FIRE_MEVA), 15, "Mod::FIRE_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_MEVA), 16, "Mod::ICE_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_MEVA), 17, "Mod::WIND_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_MEVA), 18, "Mod::EARTH_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_MEVA), 19, "Mod::THUNDER_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_MEVA), 20, "Mod::WATER_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_MEVA), 21, "Mod::LIGHT_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_MEVA), 22, "Mod::DARK_MEVA") && ok;
    return ok;
}

auto testModCombatRatingValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::ATT), 23, "Mod::ATT") && ok;
    ok      = expectInt(static_cast<int>(Mod::RATT), 24, "Mod::RATT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ACC), 25, "Mod::ACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::RACC), 26, "Mod::RACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENMITY), 27, "Mod::ENMITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::MATT), 28, "Mod::MATT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MDEF), 29, "Mod::MDEF") && ok;
    ok      = expectInt(static_cast<int>(Mod::MACC), 30, "Mod::MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::MEVA), 31, "Mod::MEVA") && ok;
    return ok;
}

auto testModElementalMagicAttackAndAccuracyValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FIRE_MAB), 32, "Mod::FIRE_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_MAB), 33, "Mod::ICE_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_MAB), 34, "Mod::WIND_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_MAB), 35, "Mod::EARTH_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_MAB), 36, "Mod::THUNDER_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_MAB), 37, "Mod::WATER_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_MAB), 38, "Mod::LIGHT_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_MAB), 39, "Mod::DARK_MAB") && ok;
    ok      = expectInt(static_cast<int>(Mod::FIRE_MACC), 40, "Mod::FIRE_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_MACC), 41, "Mod::ICE_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_MACC), 42, "Mod::WIND_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_MACC), 43, "Mod::EARTH_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_MACC), 44, "Mod::THUNDER_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_MACC), 45, "Mod::WATER_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_MACC), 46, "Mod::LIGHT_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_MACC), 47, "Mod::DARK_MACC") && ok;
    return ok;
}

auto testModWeaponSkillAttackPercentageAndSkillupValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::WSACC), 48, "Mod::WSACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::ATTP), 62, "Mod::ATTP") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEFP), 63, "Mod::DEFP") && ok;
    ok      = expectInt(static_cast<int>(Mod::COMBAT_SKILLUP_RATE), 64, "Mod::COMBAT_SKILLUP_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAGIC_SKILLUP_RATE), 65, "Mod::MAGIC_SKILLUP_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::RATTP), 66, "Mod::RATTP") && ok;
    return ok;
}

auto testModEvasionAndRecoveryValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::EVA), 68, "Mod::EVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::RDEF), 69, "Mod::RDEF") && ok;
    ok      = expectInt(static_cast<int>(Mod::REVA), 70, "Mod::REVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::MPHEAL), 71, "Mod::MPHEAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::HPHEAL), 72, "Mod::HPHEAL") && ok;
    return ok;
}

auto testModWeaponCombatSkillValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::HTH), 80, "Mod::HTH") && ok;
    ok      = expectInt(static_cast<int>(Mod::DAGGER), 81, "Mod::DAGGER") && ok;
    ok      = expectInt(static_cast<int>(Mod::SWORD), 82, "Mod::SWORD") && ok;
    ok      = expectInt(static_cast<int>(Mod::GSWORD), 83, "Mod::GSWORD") && ok;
    ok      = expectInt(static_cast<int>(Mod::AXE), 84, "Mod::AXE") && ok;
    ok      = expectInt(static_cast<int>(Mod::GAXE), 85, "Mod::GAXE") && ok;
    ok      = expectInt(static_cast<int>(Mod::SCYTHE), 86, "Mod::SCYTHE") && ok;
    ok      = expectInt(static_cast<int>(Mod::POLEARM), 87, "Mod::POLEARM") && ok;
    ok      = expectInt(static_cast<int>(Mod::KATANA), 88, "Mod::KATANA") && ok;
    ok      = expectInt(static_cast<int>(Mod::GKATANA), 89, "Mod::GKATANA") && ok;
    ok      = expectInt(static_cast<int>(Mod::CLUB), 90, "Mod::CLUB") && ok;
    ok      = expectInt(static_cast<int>(Mod::STAFF), 91, "Mod::STAFF") && ok;
    return ok;
}

auto testModAutomatonRangedAndDefensiveSkillValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::AUTO_MELEE_SKILL), 101, "Mod::AUTO_MELEE_SKILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_RANGED_SKILL), 102, "Mod::AUTO_RANGED_SKILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_MAGIC_SKILL), 103, "Mod::AUTO_MAGIC_SKILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ARCHERY), 104, "Mod::ARCHERY") && ok;
    ok      = expectInt(static_cast<int>(Mod::MARKSMAN), 105, "Mod::MARKSMAN") && ok;
    ok      = expectInt(static_cast<int>(Mod::THROW), 106, "Mod::THROW") && ok;
    ok      = expectInt(static_cast<int>(Mod::GUARD), 107, "Mod::GUARD") && ok;
    ok      = expectInt(static_cast<int>(Mod::EVASION), 108, "Mod::EVASION") && ok;
    ok      = expectInt(static_cast<int>(Mod::SHIELD), 109, "Mod::SHIELD") && ok;
    ok      = expectInt(static_cast<int>(Mod::PARRY), 110, "Mod::PARRY") && ok;
    return ok;
}

auto testModMagicSkillValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::DIVINE), 111, "Mod::DIVINE") && ok;
    ok      = expectInt(static_cast<int>(Mod::HEALING), 112, "Mod::HEALING") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCE), 113, "Mod::ENHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENFEEBLE), 114, "Mod::ENFEEBLE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ELEM), 115, "Mod::ELEM") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK), 116, "Mod::DARK") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUMMONING), 117, "Mod::SUMMONING") && ok;
    ok      = expectInt(static_cast<int>(Mod::NINJUTSU), 118, "Mod::NINJUTSU") && ok;
    ok      = expectInt(static_cast<int>(Mod::SINGING), 119, "Mod::SINGING") && ok;
    ok      = expectInt(static_cast<int>(Mod::STRING), 120, "Mod::STRING") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND), 121, "Mod::WIND") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLUE), 122, "Mod::BLUE") && ok;
    ok      = expectInt(static_cast<int>(Mod::GEOMANCY), 123, "Mod::GEOMANCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::HANDBELL), 124, "Mod::HANDBELL") && ok;
    return ok;
}

auto testModSynthesisAndRidingSkillValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FISH), 127, "Mod::FISH") && ok;
    ok      = expectInt(static_cast<int>(Mod::WOOD), 128, "Mod::WOOD") && ok;
    ok      = expectInt(static_cast<int>(Mod::SMITH), 129, "Mod::SMITH") && ok;
    ok      = expectInt(static_cast<int>(Mod::GOLDSMITH), 130, "Mod::GOLDSMITH") && ok;
    ok      = expectInt(static_cast<int>(Mod::CLOTH), 131, "Mod::CLOTH") && ok;
    ok      = expectInt(static_cast<int>(Mod::LEATHER), 132, "Mod::LEATHER") && ok;
    ok      = expectInt(static_cast<int>(Mod::BONE), 133, "Mod::BONE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ALCHEMY), 134, "Mod::ALCHEMY") && ok;
    ok      = expectInt(static_cast<int>(Mod::COOK), 135, "Mod::COOK") && ok;
    ok      = expectInt(static_cast<int>(Mod::SYNERGY), 136, "Mod::SYNERGY") && ok;
    ok      = expectInt(static_cast<int>(Mod::RIDING), 137, "Mod::RIDING") && ok;
    return ok;
}

auto testModFishingGearValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::PENGUIN_RING_EFFECT), 152, "Mod::PENGUIN_RING_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ALBATROSS_RING_EFFECT), 153, "Mod::ALBATROSS_RING_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::PELICAN_RING_EFFECT), 154, "Mod::PELICAN_RING_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::FISHING_SKILL_GAIN), 155, "Mod::FISHING_SKILL_GAIN") && ok;
    return ok;
}

auto testModBaseDamageTakenValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::DMG_AOE), 158, "Mod::DMG_AOE") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMG), 160, "Mod::DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMGPHYS), 161, "Mod::DMGPHYS") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMGBREATH), 162, "Mod::DMGBREATH") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMGMAGIC), 163, "Mod::DMGMAGIC") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMGRANGE), 164, "Mod::DMGRANGE") && ok;
    return ok;
}

auto testModExtendedDamageTakenValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::DMGPHYS_II), 190, "Mod::DMGPHYS_II") && ok;
    ok      = expectInt(static_cast<int>(Mod::UDMGPHYS), 387, "Mod::UDMGPHYS") && ok;
    ok      = expectInt(static_cast<int>(Mod::UDMGBREATH), 388, "Mod::UDMGBREATH") && ok;
    ok      = expectInt(static_cast<int>(Mod::UDMGMAGIC), 389, "Mod::UDMGMAGIC") && ok;
    ok      = expectInt(static_cast<int>(Mod::UDMGRANGE), 390, "Mod::UDMGRANGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMGMAGIC_II), 831, "Mod::DMGMAGIC_II") && ok;
    return ok;
}

auto testModReceivedDamageCapValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::RECEIVED_DAMAGE_CAP), 221, "Mod::RECEIVED_DAMAGE_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::RECEIVED_DAMAGE_VARIANT), 222, "Mod::RECEIVED_DAMAGE_VARIANT") && ok;
    return ok;
}

auto testModSpecificDamageTakenValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::SLASH_SDT), 49, "Mod::SLASH_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::PIERCE_SDT), 50, "Mod::PIERCE_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::IMPACT_SDT), 51, "Mod::IMPACT_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::HTH_SDT), 52, "Mod::HTH_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::FIRE_SDT), 54, "Mod::FIRE_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_SDT), 55, "Mod::ICE_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_SDT), 56, "Mod::WIND_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_SDT), 57, "Mod::EARTH_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_SDT), 58, "Mod::THUNDER_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_SDT), 59, "Mod::WATER_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_SDT), 60, "Mod::LIGHT_SDT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_SDT), 61, "Mod::DARK_SDT") && ok;
    return ok;
}

auto testModNullDamageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::NULL_DAMAGE), 142, "Mod::NULL_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::NULL_PHYSICAL_DAMAGE), 416, "Mod::NULL_PHYSICAL_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::NULL_BREATH_DAMAGE), 143, "Mod::NULL_BREATH_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::NULL_MAGICAL_DAMAGE), 476, "Mod::NULL_MAGICAL_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::NULL_RANGED_DAMAGE), 239, "Mod::NULL_RANGED_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::FIRE_NULL), 467, "Mod::FIRE_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_NULL), 468, "Mod::ICE_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_NULL), 469, "Mod::WIND_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_NULL), 470, "Mod::EARTH_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::LTNG_NULL), 471, "Mod::LTNG_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_NULL), 472, "Mod::WATER_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_NULL), 473, "Mod::LIGHT_NULL") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_NULL), 474, "Mod::DARK_NULL") && ok;
    return ok;
}

auto testModAbsorbDamageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::ABSORB_DMG_CHANCE), 480, "Mod::ABSORB_DMG_CHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::PHYS_ABSORB), 512, "Mod::PHYS_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAGIC_ABSORB), 475, "Mod::MAGIC_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::FIRE_ABSORB), 459, "Mod::FIRE_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_ABSORB), 460, "Mod::ICE_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_ABSORB), 461, "Mod::WIND_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_ABSORB), 462, "Mod::EARTH_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::LTNG_ABSORB), 463, "Mod::LTNG_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_ABSORB), 464, "Mod::WATER_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_ABSORB), 465, "Mod::LIGHT_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_ABSORB), 466, "Mod::DARK_ABSORB") && ok;
    return ok;
}

auto testModTacticalParryAndInhibitTPValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::TACTICAL_PARRY), 486, "Mod::TACTICAL_PARRY") && ok;
    ok      = expectInt(static_cast<int>(Mod::INHIBIT_TP), 488, "Mod::INHIBIT_TP") && ok;
    return ok;
}

auto testModActionPowerMultiplierValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::POWER_MULTIPLIER_BASIC_ATTACK), 1173, "Mod::POWER_MULTIPLIER_BASIC_ATTACK") && ok;
    ok      = expectInt(static_cast<int>(Mod::POWER_MULTIPLIER_BASIC_RANGED), 1174, "Mod::POWER_MULTIPLIER_BASIC_RANGED") && ok;
    ok      = expectInt(static_cast<int>(Mod::POWER_MULTIPLIER_SPELL), 1175, "Mod::POWER_MULTIPLIER_SPELL") && ok;
    ok      = expectInt(static_cast<int>(Mod::POWER_MULTIPLIER_WEAPONSKILL), 1176, "Mod::POWER_MULTIPLIER_WEAPONSKILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::POWER_MULTIPLIER_JOB_ABILITY), 1177, "Mod::POWER_MULTIPLIER_JOB_ABILITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::POWER_MULTIPLIER_MOBSKILL), 1178, "Mod::POWER_MULTIPLIER_MOBSKILL") && ok;
    return ok;
}

auto testModCriticalHitRateValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::CRITHITRATE_ONLY_WEP), 141, "Mod::CRITHITRATE_ONLY_WEP") && ok;
    ok      = expectInt(static_cast<int>(Mod::CRITHITRATE), 165, "Mod::CRITHITRATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::CRITICAL_HIT_EVASION), 166, "Mod::CRITICAL_HIT_EVASION") && ok;
    return ok;
}

auto testModCriticalDamageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::CRIT_DMG_INCREASE), 421, "Mod::CRIT_DMG_INCREASE") && ok;
    ok      = expectInt(static_cast<int>(Mod::RANGED_CRIT_DMG_INCREASE), 964, "Mod::RANGED_CRIT_DMG_INCREASE") && ok;
    ok      = expectInt(static_cast<int>(Mod::CRIT_DEF_BONUS), 908, "Mod::CRIT_DEF_BONUS") && ok;
    return ok;
}

auto testModMagicCriticalValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::MAGIC_CRITHITRATE), 562, "Mod::MAGIC_CRITHITRATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAGIC_CRIT_DMG_INCREASE), 563, "Mod::MAGIC_CRIT_DMG_INCREASE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAGIC_CRITHITRATE_II), 1168, "Mod::MAGIC_CRITHITRATE_II") && ok;
    return ok;
}

auto testModFencerValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FENCER_TP_BONUS), 903, "Mod::FENCER_TP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::FENCER_CRITHITRATE), 904, "Mod::FENCER_CRITHITRATE") && ok;
    return ok;
}

auto testModSmiteAndGuardValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::SMITE), 898, "Mod::SMITE") && ok;
    ok      = expectInt(static_cast<int>(Mod::TACTICAL_GUARD), 899, "Mod::TACTICAL_GUARD") && ok;
    ok      = expectInt(static_cast<int>(Mod::GUARD_PERCENT), 976, "Mod::GUARD_PERCENT") && ok;
    return ok;
}

auto testModHasteAndSpellInterruptValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::HASTE_MAGIC), 167, "Mod::HASTE_MAGIC") && ok;
    ok      = expectInt(static_cast<int>(Mod::SPELLINTERRUPT), 168, "Mod::SPELLINTERRUPT") && ok;
    ok      = expectInt(static_cast<int>(Mod::TWOHAND_HASTE_ABILITY), 217, "Mod::TWOHAND_HASTE_ABILITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::HASTE_ABILITY), 383, "Mod::HASTE_ABILITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::HASTE_GEAR), 384, "Mod::HASTE_GEAR") && ok;
    return ok;
}

auto testModMovementSpeedValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_STACKABLE), 75, "Mod::MOVE_SPEED_STACKABLE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_GEAR_BONUS), 76, "Mod::MOVE_SPEED_GEAR_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_WEIGHT_PENALTY), 77, "Mod::MOVE_SPEED_WEIGHT_PENALTY") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_QUICKENING), 78, "Mod::MOVE_SPEED_QUICKENING") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_MAZURKA), 79, "Mod::MOVE_SPEED_MAZURKA") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_OVERRIDE), 169, "Mod::MOVE_SPEED_OVERRIDE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOUNT_MOVE), 972, "Mod::MOUNT_MOVE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_FLEE), 1085, "Mod::MOVE_SPEED_FLEE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_BOLTERS_ROLL), 1086, "Mod::MOVE_SPEED_BOLTERS_ROLL") && ok;
    ok      = expectInt(static_cast<int>(Mod::MOVE_SPEED_CHEER), 1087, "Mod::MOVE_SPEED_CHEER") && ok;
    return ok;
}

auto testModCastAndRecastValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::UFASTCAST), 407, "Mod::UFASTCAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::CURE_CAST_TIME), 519, "Mod::CURE_CAST_TIME") && ok;
    ok      = expectInt(static_cast<int>(Mod::ELEMENTAL_CELERITY), 901, "Mod::ELEMENTAL_CELERITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::HEALING_MAGIC_RECAST), 1183, "Mod::HEALING_MAGIC_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENFEEBLING_MAGIC_RECAST), 1184, "Mod::ENFEEBLING_MAGIC_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCING_MAGIC_RECAST), 1185, "Mod::ENHANCING_MAGIC_RECAST") && ok;
    return ok;
}

auto testModDelaySkillchainAndSwingValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::DELAY), 171, "Mod::DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::RANGED_DELAY), 172, "Mod::RANGED_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::MARTIAL_ARTS), 173, "Mod::MARTIAL_ARTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SKILLCHAINBONUS), 174, "Mod::SKILLCHAINBONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SKILLCHAINDMG), 175, "Mod::SKILLCHAINDMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAX_SWINGS), 978, "Mod::MAX_SWINGS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ADDITIONAL_SWING_CHANCE), 979, "Mod::ADDITIONAL_SWING_CHANCE") && ok;
    return ok;
}

auto testModMagicDamageAndMPCostValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::MAGIC_DAMAGE), 311, "Mod::MAGIC_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MP_COST_REDUCTION), 1197, "Mod::MP_COST_REDUCTION") && ok;
    return ok;
}

auto testModFoodValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FOOD_MACCP), 99, "Mod::FOOD_MACCP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_MACC_CAP), 100, "Mod::FOOD_MACC_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_HPP), 176, "Mod::FOOD_HPP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_HP_CAP), 177, "Mod::FOOD_HP_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_MPP), 178, "Mod::FOOD_MPP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_MP_CAP), 179, "Mod::FOOD_MP_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_ATTP), 180, "Mod::FOOD_ATTP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_ATT_CAP), 181, "Mod::FOOD_ATT_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_DEFP), 182, "Mod::FOOD_DEFP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_DEF_CAP), 183, "Mod::FOOD_DEF_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_ACCP), 184, "Mod::FOOD_ACCP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_ACC_CAP), 185, "Mod::FOOD_ACC_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_RATTP), 186, "Mod::FOOD_RATTP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_RATT_CAP), 187, "Mod::FOOD_RATT_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_RACCP), 188, "Mod::FOOD_RACCP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_RACC_CAP), 189, "Mod::FOOD_RACC_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_DURATION), 937, "Mod::FOOD_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_HP), 1130, "Mod::FOOD_HP") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOD_MP), 1131, "Mod::FOOD_MP") && ok;
    return ok;
}

auto testModKillerEffectValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::VERMIN_KILLER), 224, "Mod::VERMIN_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::BIRD_KILLER), 225, "Mod::BIRD_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::AMORPH_KILLER), 226, "Mod::AMORPH_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIZARD_KILLER), 227, "Mod::LIZARD_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::AQUAN_KILLER), 228, "Mod::AQUAN_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::PLANTOID_KILLER), 229, "Mod::PLANTOID_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::BEAST_KILLER), 230, "Mod::BEAST_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::UNDEAD_KILLER), 231, "Mod::UNDEAD_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::ARCANA_KILLER), 232, "Mod::ARCANA_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::DRAGON_KILLER), 233, "Mod::DRAGON_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEMON_KILLER), 234, "Mod::DEMON_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::EMPTY_KILLER), 235, "Mod::EMPTY_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::HUMANOID_KILLER), 236, "Mod::HUMANOID_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::LUMINIAN_KILLER), 237, "Mod::LUMINIAN_KILLER") && ok;
    ok      = expectInt(static_cast<int>(Mod::LUMINION_KILLER), 238, "Mod::LUMINION_KILLER") && ok;
    return ok;
}

auto testModEnfeebleResistanceValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::SLEEPRES), 240, "Mod::SLEEPRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::POISONRES), 241, "Mod::POISONRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::PARALYZERES), 242, "Mod::PARALYZERES") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLINDRES), 243, "Mod::BLINDRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::SILENCERES), 244, "Mod::SILENCERES") && ok;
    ok      = expectInt(static_cast<int>(Mod::VIRUSRES), 245, "Mod::VIRUSRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::PETRIFYRES), 246, "Mod::PETRIFYRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::BINDRES), 247, "Mod::BINDRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::CURSERES), 248, "Mod::CURSERES") && ok;
    ok      = expectInt(static_cast<int>(Mod::GRAVITYRES), 249, "Mod::GRAVITYRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::SLOWRES), 250, "Mod::SLOWRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::STUNRES), 251, "Mod::STUNRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHARMRES), 252, "Mod::CHARMRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::AMNESIARES), 253, "Mod::AMNESIARES") && ok;
    ok      = expectInt(static_cast<int>(Mod::LULLABYRES), 254, "Mod::LULLABYRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEATHRES), 255, "Mod::DEATHRES") && ok;
    ok      = expectInt(static_cast<int>(Mod::STATUSRES), 958, "Mod::STATUSRES") && ok;
    return ok;
}

auto testModEnfeebleMEVAValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::SLEEP_MEVA), 200, "Mod::SLEEP_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::POISON_MEVA), 201, "Mod::POISON_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::PARALYZE_MEVA), 202, "Mod::PARALYZE_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLIND_MEVA), 203, "Mod::BLIND_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::SILENCE_MEVA), 204, "Mod::SILENCE_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::VIRUS_MEVA), 205, "Mod::VIRUS_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::PETRIFY_MEVA), 206, "Mod::PETRIFY_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::BIND_MEVA), 207, "Mod::BIND_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::CURSE_MEVA), 208, "Mod::CURSE_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::GRAVITY_MEVA), 209, "Mod::GRAVITY_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::SLOW_MEVA), 210, "Mod::SLOW_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::STUN_MEVA), 211, "Mod::STUN_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHARM_MEVA), 212, "Mod::CHARM_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::AMNESIA_MEVA), 213, "Mod::AMNESIA_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::LULLABY_MEVA), 214, "Mod::LULLABY_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEATH_MEVA), 215, "Mod::DEATH_MEVA") && ok;
    ok      = expectInt(static_cast<int>(Mod::STATUS_MEVA), 216, "Mod::STATUS_MEVA") && ok;
    return ok;
}

auto testModStatusEffectImmunobreakValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::SLEEP_IMMUNOBREAK), 261, "Mod::SLEEP_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::POISON_IMMUNOBREAK), 262, "Mod::POISON_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::PARALYZE_IMMUNOBREAK), 263, "Mod::PARALYZE_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLIND_IMMUNOBREAK), 264, "Mod::BLIND_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::SILENCE_IMMUNOBREAK), 265, "Mod::SILENCE_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::PETRIFY_IMMUNOBREAK), 266, "Mod::PETRIFY_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::BIND_IMMUNOBREAK), 267, "Mod::BIND_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::GRAVITY_IMMUNOBREAK), 268, "Mod::GRAVITY_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::SLOW_IMMUNOBREAK), 269, "Mod::SLOW_IMMUNOBREAK") && ok;
    ok      = expectInt(static_cast<int>(Mod::ADDLE_IMMUNOBREAK), 270, "Mod::ADDLE_IMMUNOBREAK") && ok;
    return ok;
}

auto testModParalyzeMijinAndDualWieldValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::PARALYZE), 257, "Mod::PARALYZE") && ok;
    ok      = expectInt(static_cast<int>(Mod::MIJIN_RERAISE), 258, "Mod::MIJIN_RERAISE") && ok;
    ok      = expectInt(static_cast<int>(Mod::DUAL_WIELD), 259, "Mod::DUAL_WIELD") && ok;
    return ok;
}

auto testModifierAccessorsAndSetAmount() -> bool
{
    CModifier modifier(Mod::FASTCAST, -12);

    bool ok = true;
    ok      = expectInt(static_cast<int>(modifier.getModID()), static_cast<int>(Mod::FASTCAST), "modifier ID") && ok;
    ok      = expectInt(modifier.getModAmount(), -12, "modifier amount") && ok;

    modifier.setModAmount(23);
    ok = expectInt(modifier.getModAmount(), 23, "updated modifier amount") && ok;

    CModifier zeroAmount(Mod::FASTCAST);
    ok = expectInt(zeroAmount.getModAmount(), 0, "default modifier amount") && ok;
    return ok;
}

auto testPetModifierAccessorsAndSetAmount() -> bool
{
    CPetModifier modifier(Mod::STORETP, PetModType::Automaton, 15);

    bool ok = true;
    ok      = expectInt(static_cast<int>(modifier.getModID()), static_cast<int>(Mod::STORETP), "pet modifier ID") && ok;
    ok      = expectInt(modifier.getModAmount(), 15, "pet modifier amount") && ok;
    ok      = expectInt(static_cast<int>(modifier.getPetModType()), static_cast<int>(PetModType::Automaton), "pet modifier type") && ok;

    modifier.setModAmount(-3);
    ok = expectInt(modifier.getModAmount(), -3, "updated pet modifier amount") && ok;

    CPetModifier zeroAmount(Mod::STORETP, PetModType::Wyvern);
    ok = expectInt(zeroAmount.getModAmount(), 0, "default pet modifier amount") && ok;
    ok = expectInt(static_cast<int>(zeroAmount.getPetModType()), static_cast<int>(PetModType::Wyvern), "default pet modifier type") && ok;
    return ok;
}

} // namespace

auto runModifierSelfTests() -> bool
{
    bool ok = true;
    ok      = testPetModTypeValues() && ok;
    ok      = testModCoreValues() && ok;
    ok      = testModElementalMagicEvasionValues() && ok;
    ok      = testModCombatRatingValues() && ok;
    ok      = testModElementalMagicAttackAndAccuracyValues() && ok;
    ok      = testModWeaponSkillAttackPercentageAndSkillupValues() && ok;
    ok      = testModEvasionAndRecoveryValues() && ok;
    ok      = testModWeaponCombatSkillValues() && ok;
    ok      = testModAutomatonRangedAndDefensiveSkillValues() && ok;
    ok      = testModMagicSkillValues() && ok;
    ok      = testModSynthesisAndRidingSkillValues() && ok;
    ok      = testModFishingGearValues() && ok;
    ok      = testModBaseDamageTakenValues() && ok;
    ok      = testModExtendedDamageTakenValues() && ok;
    ok      = testModReceivedDamageCapValues() && ok;
    ok      = testModSpecificDamageTakenValues() && ok;
    ok      = testModNullDamageValues() && ok;
    ok      = testModAbsorbDamageValues() && ok;
    ok      = testModTacticalParryAndInhibitTPValues() && ok;
    ok      = testModActionPowerMultiplierValues() && ok;
    ok      = testModCriticalHitRateValues() && ok;
    ok      = testModCriticalDamageValues() && ok;
    ok      = testModMagicCriticalValues() && ok;
    ok      = testModFencerValues() && ok;
    ok      = testModSmiteAndGuardValues() && ok;
    ok      = testModHasteAndSpellInterruptValues() && ok;
    ok      = testModMovementSpeedValues() && ok;
    ok      = testModCastAndRecastValues() && ok;
    ok      = testModDelaySkillchainAndSwingValues() && ok;
    ok      = testModMagicDamageAndMPCostValues() && ok;
    ok      = testModFoodValues() && ok;
    ok      = testModKillerEffectValues() && ok;
    ok      = testModEnfeebleResistanceValues() && ok;
    ok      = testModEnfeebleMEVAValues() && ok;
    ok      = testModStatusEffectImmunobreakValues() && ok;
    ok      = testModParalyzeMijinAndDualWieldValues() && ok;
    ok      = testModifierAccessorsAndSetAmount() && ok;
    ok      = testPetModifierAccessorsAndSetAmount() && ok;
    return ok;
}
