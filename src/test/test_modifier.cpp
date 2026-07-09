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

auto testModFTPBonusValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FIRE_FTP_BONUS), 544, "Mod::FIRE_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_FTP_BONUS), 545, "Mod::ICE_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_FTP_BONUS), 546, "Mod::WIND_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_FTP_BONUS), 547, "Mod::EARTH_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_FTP_BONUS), 548, "Mod::THUNDER_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_FTP_BONUS), 549, "Mod::WATER_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_FTP_BONUS), 550, "Mod::LIGHT_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_FTP_BONUS), 551, "Mod::DARK_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ANY_FTP_BONUS), 1144, "Mod::ANY_FTP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::DAY_FTP_BONUS), 1145, "Mod::DAY_FTP_BONUS") && ok;
    return ok;
}

auto testModWarriorValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::DOUBLE_ATTACK), 288, "Mod::DOUBLE_ATTACK") && ok;
    ok      = expectInt(static_cast<int>(Mod::DOUBLE_ATTACK_DMG), 1038, "Mod::DOUBLE_ATTACK_DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::WARCRY_DURATION), 483, "Mod::WARCRY_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::BERSERK_POTENCY), 948, "Mod::BERSERK_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::BERSERK_DURATION), 954, "Mod::BERSERK_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::AGGRESSOR_DURATION), 955, "Mod::AGGRESSOR_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEFENDER_DURATION), 956, "Mod::DEFENDER_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_RESTRAINT), 1045, "Mod::ENHANCES_RESTRAINT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_BLOOD_RAGE), 1046, "Mod::ENHANCES_BLOOD_RAGE") && ok;
    return ok;
}

auto testModMonkValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::BOOST_EFFECT), 97, "Mod::BOOST_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHAKRA_MULT), 1026, "Mod::CHAKRA_MULT") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHAKRA_REMOVAL), 1027, "Mod::CHAKRA_REMOVAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUBTLE_BLOW), 289, "Mod::SUBTLE_BLOW") && ok;
    ok      = expectInt(static_cast<int>(Mod::COUNTER), 291, "Mod::COUNTER") && ok;
    ok      = expectInt(static_cast<int>(Mod::KICK_ATTACK_RATE), 292, "Mod::KICK_ATTACK_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::PERFECT_COUNTER_ATT), 428, "Mod::PERFECT_COUNTER_ATT") && ok;
    ok      = expectInt(static_cast<int>(Mod::COUNTER_DAMAGE), 1047, "Mod::COUNTER_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOOTWORK_ATT_BONUS), 429, "Mod::FOOTWORK_ATT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::COUNTERSTANCE_EFFECT), 543, "Mod::COUNTERSTANCE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DODGE_EFFECT), 552, "Mod::DODGE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::FOCUS_EFFECT), 561, "Mod::FOCUS_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ADDITIVE_GUARD), 1092, "Mod::ADDITIVE_GUARD") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUGMENTS_IMPETUS), 1097, "Mod::AUGMENTS_IMPETUS") && ok;
    return ok;
}

auto testModWhiteMageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::AFFLATUS_SOLACE), 293, "Mod::AFFLATUS_SOLACE") && ok;
    ok      = expectInt(static_cast<int>(Mod::AFFLATUS_MISERY), 294, "Mod::AFFLATUS_MISERY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUSPICE_EFFECT), 484, "Mod::AUSPICE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::AOE_NA), 524, "Mod::AOE_NA") && ok;
    ok      = expectInt(static_cast<int>(Mod::REGEN_MULTIPLIER), 838, "Mod::REGEN_MULTIPLIER") && ok;
    ok      = expectInt(static_cast<int>(Mod::CURE2MP_PERCENT), 860, "Mod::CURE2MP_PERCENT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DIVINE_BENISON), 910, "Mod::DIVINE_BENISON") && ok;
    ok      = expectInt(static_cast<int>(Mod::REGEN_BONUS), 989, "Mod::REGEN_BONUS") && ok;
    return ok;
}

auto testModBlackMageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::CLEAR_MIND), 295, "Mod::CLEAR_MIND") && ok;
    ok      = expectInt(static_cast<int>(Mod::CONSERVE_MP), 296, "Mod::CONSERVE_MP") && ok;
    ok      = expectInt(static_cast<int>(Mod::ELEMENTAL_MAGIC_RECAST), 1146, "Mod::ELEMENTAL_MAGIC_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_ELEMENTAL_SEAL), 1149, "Mod::ENHANCES_ELEMENTAL_SEAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ELEMENTAL_DEBUFF_EFFECT), 1150, "Mod::ELEMENTAL_DEBUFF_EFFECT") && ok;
    return ok;
}

auto testModRedMageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::BLINK), 299, "Mod::BLINK") && ok;
    ok      = expectInt(static_cast<int>(Mod::STONESKIN), 300, "Mod::STONESKIN") && ok;
    ok      = expectInt(static_cast<int>(Mod::PHALANX), 301, "Mod::PHALANX") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENF_MAG_POTENCY), 290, "Mod::ENF_MAG_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENF_MAG_DURATION), 1151, "Mod::ENF_MAG_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_SABOTEUR), 297, "Mod::ENHANCES_SABOTEUR") && ok;
    ok      = expectInt(static_cast<int>(Mod::PHALANX_RECEIVED), 1182, "Mod::PHALANX_RECEIVED") && ok;
    return ok;
}

auto testModThiefValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FLEE_DURATION), 93, "Mod::FLEE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::STEAL), 298, "Mod::STEAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::DESPOIL), 896, "Mod::DESPOIL") && ok;
    ok      = expectInt(static_cast<int>(Mod::PERFECT_DODGE), 883, "Mod::PERFECT_DODGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::TRIPLE_ATTACK), 302, "Mod::TRIPLE_ATTACK") && ok;
    ok      = expectInt(static_cast<int>(Mod::TRIPLE_ATTACK_DMG), 1039, "Mod::TRIPLE_ATTACK_DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::TREASURE_HUNTER), 303, "Mod::TREASURE_HUNTER") && ok;
    ok      = expectInt(static_cast<int>(Mod::TREASURE_HUNTER_PROC), 1048, "Mod::TREASURE_HUNTER_PROC") && ok;
    ok      = expectInt(static_cast<int>(Mod::TREASURE_HUNTER_CAP), 1049, "Mod::TREASURE_HUNTER_CAP") && ok;
    ok      = expectInt(static_cast<int>(Mod::SNEAK_ATK_DEX), 830, "Mod::SNEAK_ATK_DEX") && ok;
    ok      = expectInt(static_cast<int>(Mod::TRICK_ATK_AGI), 520, "Mod::TRICK_ATK_AGI") && ok;
    ok      = expectInt(static_cast<int>(Mod::MUG_EFFECT), 835, "Mod::MUG_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ACC_COLLAB_EFFECT), 884, "Mod::ACC_COLLAB_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::HIDE_DURATION), 885, "Mod::HIDE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::GILFINDER), 897, "Mod::GILFINDER") && ok;
    return ok;
}

auto testModPaladinValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::HOLY_CIRCLE_DURATION), 857, "Mod::HOLY_CIRCLE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::HOLY_CIRCLE_POTENCY), 1141, "Mod::HOLY_CIRCLE_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::RAMPART_DURATION), 92, "Mod::RAMPART_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ABSORB_PHYSDMG_TO_MP), 426, "Mod::ABSORB_PHYSDMG_TO_MP") && ok;
    ok      = expectInt(static_cast<int>(Mod::SHIELD_MASTERY_TP), 485, "Mod::SHIELD_MASTERY_TP") && ok;
    ok      = expectInt(static_cast<int>(Mod::SENTINEL_EFFECT), 837, "Mod::SENTINEL_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::SHIELD_DEF_BONUS), 905, "Mod::SHIELD_DEF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::COVER_TO_MP), 965, "Mod::COVER_TO_MP") && ok;
    ok      = expectInt(static_cast<int>(Mod::COVER_MAGIC_AND_RANGED), 966, "Mod::COVER_MAGIC_AND_RANGED") && ok;
    ok      = expectInt(static_cast<int>(Mod::COVER_DURATION), 967, "Mod::COVER_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_CHIVALRY), 1061, "Mod::ENHANCES_CHIVALRY") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_DIVINE_EMBLEM), 1062, "Mod::ENHANCES_DIVINE_EMBLEM") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_FEALTY), 1063, "Mod::ENHANCES_FEALTY") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_IRON_WILL), 1064, "Mod::ENHANCES_IRON_WILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_GUARDIAN), 1065, "Mod::ENHANCES_GUARDIAN") && ok;
    ok      = expectInt(static_cast<int>(Mod::PALISADE_BLOCK_BONUS), 1066, "Mod::PALISADE_BLOCK_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::REPRISAL_BLOCK_BONUS), 1067, "Mod::REPRISAL_BLOCK_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::REPRISAL_SPIKES_BONUS), 1068, "Mod::REPRISAL_SPIKES_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SHIELD_BARRIER), 1082, "Mod::SHIELD_BARRIER") && ok;
    return ok;
}

auto testModDarkKnightValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::ARCANE_CIRCLE_DURATION), 858, "Mod::ARCANE_CIRCLE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ARCANE_CIRCLE_POTENCY), 1069, "Mod::ARCANE_CIRCLE_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::SOULEATER_EFFECT), 96, "Mod::SOULEATER_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::SOULEATER_EFFECT_II), 53, "Mod::SOULEATER_EFFECT_II") && ok;
    ok      = expectInt(static_cast<int>(Mod::DESPERATE_BLOWS), 906, "Mod::DESPERATE_BLOWS") && ok;
    ok      = expectInt(static_cast<int>(Mod::STALWART_SOUL), 907, "Mod::STALWART_SOUL") && ok;
    ok      = expectInt(static_cast<int>(Mod::DREAD_SPIKES_EFFECT), 998, "Mod::DREAD_SPIKES_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_MAGIC_CAST), 1071, "Mod::DARK_MAGIC_CAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_MAGIC_DURATION), 1072, "Mod::DARK_MAGIC_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_BLOOD_WEAPON), 1070, "Mod::ENHANCES_BLOOD_WEAPON") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_DARK_SEAL), 1073, "Mod::ENHANCES_DARK_SEAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_DIABOLIC_EYE), 275, "Mod::ENHANCES_DIABOLIC_EYE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_NETHER_VOID), 1083, "Mod::ENHANCES_NETHER_VOID") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_MUTED_SOUL), 1084, "Mod::ENHANCES_MUTED_SOUL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_ABSORB_EFFECTS), 1136, "Mod::ENHANCES_ABSORB_EFFECTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUGMENTS_ABSORB), 1137, "Mod::AUGMENTS_ABSORB") && ok;
    ok      = expectInt(static_cast<int>(Mod::ABSORB_EFFECT_DURATION), 1138, "Mod::ABSORB_EFFECT_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUGMENTS_ABSORB_TP), 1153, "Mod::AUGMENTS_ABSORB_TP") && ok;
    return ok;
}

auto testModBeastmasterValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::TAME), 304, "Mod::TAME") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHARM_TIME), 360, "Mod::CHARM_TIME") && ok;
    ok      = expectInt(static_cast<int>(Mod::FAMILIAR_BONUS), 1169, "Mod::FAMILIAR_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::REWARD_HP_BONUS), 364, "Mod::REWARD_HP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::CHARM_CHANCE), 391, "Mod::CHARM_CHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::FERAL_HOWL_DURATION), 503, "Mod::FERAL_HOWL_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUG_LEVEL_RANGE), 564, "Mod::JUG_LEVEL_RANGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::CALL_BEAST_DELAY), 273, "Mod::CALL_BEAST_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::SIC_READY_RECAST), 1052, "Mod::SIC_READY_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::TANDEM_STRIKE_POWER), 271, "Mod::TANDEM_STRIKE_POWER") && ok;
    ok      = expectInt(static_cast<int>(Mod::TANDEM_BLOW_POWER), 272, "Mod::TANDEM_BLOW_POWER") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_MONSTER_CORRELATION), 1155, "Mod::ENHANCES_MONSTER_CORRELATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_SPUR), 1157, "Mod::ENHANCES_SPUR") && ok;
    return ok;
}

auto testModBardValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::MINNE_EFFECT), 433, "Mod::MINNE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MINUET_EFFECT), 434, "Mod::MINUET_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::PAEON_EFFECT), 435, "Mod::PAEON_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::REQUIEM_EFFECT), 436, "Mod::REQUIEM_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::THRENODY_EFFECT), 437, "Mod::THRENODY_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MADRIGAL_EFFECT), 438, "Mod::MADRIGAL_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAMBO_EFFECT), 439, "Mod::MAMBO_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::LULLABY_EFFECT), 440, "Mod::LULLABY_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ETUDE_EFFECT), 441, "Mod::ETUDE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::BALLAD_EFFECT), 442, "Mod::BALLAD_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MARCH_EFFECT), 443, "Mod::MARCH_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::FINALE_EFFECT), 444, "Mod::FINALE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::CAROL_EFFECT), 445, "Mod::CAROL_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAZURKA_EFFECT), 446, "Mod::MAZURKA_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ELEGY_EFFECT), 447, "Mod::ELEGY_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::PRELUDE_EFFECT), 448, "Mod::PRELUDE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::HYMNUS_EFFECT), 449, "Mod::HYMNUS_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::VIRELAI_EFFECT), 450, "Mod::VIRELAI_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::SCHERZO_EFFECT), 451, "Mod::SCHERZO_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ALL_SONGS_EFFECT), 452, "Mod::ALL_SONGS_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAXIMUM_SONGS_BONUS), 453, "Mod::MAXIMUM_SONGS_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SONG_DURATION_BONUS), 454, "Mod::SONG_DURATION_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SONG_SPELLCASTING_TIME), 455, "Mod::SONG_SPELLCASTING_TIME") && ok;
    ok      = expectInt(static_cast<int>(Mod::SONG_RECAST_DELAY), 833, "Mod::SONG_RECAST_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUGMENT_SONG_STAT), 1003, "Mod::AUGMENT_SONG_STAT") && ok;
    return ok;
}

auto testModRangerValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::CAMOUFLAGE_DURATION), 98, "Mod::CAMOUFLAGE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::RECYCLE), 305, "Mod::RECYCLE") && ok;
    ok      = expectInt(static_cast<int>(Mod::SNAPSHOT), 365, "Mod::SNAPSHOT") && ok;
    ok      = expectInt(static_cast<int>(Mod::RAPID_SHOT), 359, "Mod::RAPID_SHOT") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIDESCAN), 340, "Mod::WIDESCAN") && ok;
    ok      = expectInt(static_cast<int>(Mod::BARRAGE_ACC), 420, "Mod::BARRAGE_ACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::BARRAGE_COUNT), 138, "Mod::BARRAGE_COUNT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DOUBLE_SHOT_RATE), 422, "Mod::DOUBLE_SHOT_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::VELOCITY_SNAPSHOT_BONUS), 423, "Mod::VELOCITY_SNAPSHOT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::VELOCITY_RATT_BONUS), 424, "Mod::VELOCITY_RATT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SHADOW_BIND_EXT), 425, "Mod::SHADOW_BIND_EXT") && ok;
    ok      = expectInt(static_cast<int>(Mod::SCAVENGE_EFFECT), 312, "Mod::SCAVENGE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::SHARPSHOT), 314, "Mod::SHARPSHOT") && ok;
    ok      = expectInt(static_cast<int>(Mod::TRUE_SHOT_EFFECT), 1053, "Mod::TRUE_SHOT_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DEAD_AIM_EFFECT), 1054, "Mod::DEAD_AIM_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::BOUNTY_SHOT_TH_BONUS), 826, "Mod::BOUNTY_SHOT_TH_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::RETAIN_CAMOUFLAGE), 1189, "Mod::RETAIN_CAMOUFLAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::RETAIN_UNLIMITED_SHOT), 1190, "Mod::RETAIN_UNLIMITED_SHOT") && ok;
    ok      = expectInt(static_cast<int>(Mod::RA_IGNORE_LVL_DIFF), 1191, "Mod::RA_IGNORE_LVL_DIFF") && ok;
    return ok;
}

auto testModSamuraiValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::WARDING_CIRCLE_DURATION), 95, "Mod::WARDING_CIRCLE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::WARDING_CIRCLE_POTENCY), 1143, "Mod::WARDING_CIRCLE_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::MEDITATE_DURATION), 94, "Mod::MEDITATE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ZANSHIN), 306, "Mod::ZANSHIN") && ok;
    ok      = expectInt(static_cast<int>(Mod::THIRD_EYE_COUNTER_RATE), 508, "Mod::THIRD_EYE_COUNTER_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::THIRD_EYE_RETENTION_RATE), 839, "Mod::THIRD_EYE_RETENTION_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::THIRD_EYE_BONUS), 1055, "Mod::THIRD_EYE_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SENGIKORI_SC_DMG_DEBUFF), 1088, "Mod::SENGIKORI_SC_DMG_DEBUFF") && ok;
    ok      = expectInt(static_cast<int>(Mod::SENGIKORI_MB_DMG_DEBUFF), 1089, "Mod::SENGIKORI_MB_DMG_DEBUFF") && ok;
    ok      = expectInt(static_cast<int>(Mod::SENGIKORI_BONUS), 1090, "Mod::SENGIKORI_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::HASSO_ZANSHIN_BONUS), 1187, "Mod::HASSO_ZANSHIN_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SEIGAN_COUNTER_BONUS), 1188, "Mod::SEIGAN_COUNTER_BONUS") && ok;
    return ok;
}

auto testModNinjaValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::UTSUSEMI), 307, "Mod::UTSUSEMI") && ok;
    ok      = expectInt(static_cast<int>(Mod::UTSUSEMI_BONUS), 900, "Mod::UTSUSEMI_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::NINJA_TOOL), 308, "Mod::NINJA_TOOL") && ok;
    ok      = expectInt(static_cast<int>(Mod::NIN_NUKE_BONUS_INNIN), 223, "Mod::NIN_NUKE_BONUS_INNIN") && ok;
    ok      = expectInt(static_cast<int>(Mod::NIN_NUKE_BONUS_GEAR), 522, "Mod::NIN_NUKE_BONUS_GEAR") && ok;
    ok      = expectInt(static_cast<int>(Mod::DAKEN), 911, "Mod::DAKEN") && ok;
    ok      = expectInt(static_cast<int>(Mod::NINJUTSU_DURATION), 1000, "Mod::NINJUTSU_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_SANGE), 1091, "Mod::ENHANCES_SANGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_FUTAE), 1148, "Mod::ENHANCES_FUTAE") && ok;
    ok      = expectInt(static_cast<int>(Mod::UTSUSEMI_AOE), 1179, "Mod::UTSUSEMI_AOE") && ok;
    ok      = expectInt(static_cast<int>(Mod::YONIN_UTSUSEMI_ENMITY), 1192, "Mod::YONIN_UTSUSEMI_ENMITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::SANGE_MULTI_HIT), 1193, "Mod::SANGE_MULTI_HIT") && ok;
    return ok;
}

auto testModDragoonValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::ANCIENT_CIRCLE_DURATION), 859, "Mod::ANCIENT_CIRCLE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ANCIENT_CIRCLE_POTENCY), 1142, "Mod::ANCIENT_CIRCLE_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUMP_TP_BONUS), 361, "Mod::JUMP_TP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUMP_SPIRIT_TP_BONUS), 285, "Mod::JUMP_SPIRIT_TP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUMP_ATT_BONUS), 362, "Mod::JUMP_ATT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUMP_SOUL_SPIRIT_ATT_BONUS), 286, "Mod::JUMP_SOUL_SPIRIT_ATT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUMP_ACC_BONUS), 936, "Mod::JUMP_ACC_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::JUMP_DOUBLE_ATTACK), 888, "Mod::JUMP_DOUBLE_ATTACK") && ok;
    ok      = expectInt(static_cast<int>(Mod::HIGH_JUMP_ENMITY_REDUCTION), 363, "Mod::HIGH_JUMP_ENMITY_REDUCTION") && ok;
    ok      = expectInt(static_cast<int>(Mod::FORCE_JUMP_CRIT), 828, "Mod::FORCE_JUMP_CRIT") && ok;
    ok      = expectInt(static_cast<int>(Mod::WYVERN_EFFECTIVE_BREATH), 829, "Mod::WYVERN_EFFECTIVE_BREATH") && ok;
    ok      = expectInt(static_cast<int>(Mod::WYVERN_SUBJOB_TRAITS), 974, "Mod::WYVERN_SUBJOB_TRAITS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WYVERN_BREATH_MACC), 986, "Mod::WYVERN_BREATH_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::WYVERN_LVL_BONUS), 1043, "Mod::WYVERN_LVL_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WYVERN_ATTRIBUTE_DA), 1056, "Mod::WYVERN_ATTRIBUTE_DA") && ok;
    ok      = expectInt(static_cast<int>(Mod::WYVERN_SHOW_READYING), 1195, "Mod::WYVERN_SHOW_READYING") && ok;
    ok      = expectInt(static_cast<int>(Mod::DRAGOON_BREATH_RECAST), 1057, "Mod::DRAGOON_BREATH_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCE_DEEP_BREATHING), 283, "Mod::ENHANCE_DEEP_BREATHING") && ok;
    ok      = expectInt(static_cast<int>(Mod::UNCAPPED_WYVERN_BREATH), 284, "Mod::UNCAPPED_WYVERN_BREATH") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_STRAFE), 282, "Mod::ENHANCES_STRAFE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_SPIRIT_LINK), 281, "Mod::ENHANCES_SPIRIT_LINK") && ok;
    return ok;
}

auto testModSummonerValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::AVATAR_PERPETUATION), 371, "Mod::AVATAR_PERPETUATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::WEATHER_REDUCTION), 372, "Mod::WEATHER_REDUCTION") && ok;
    ok      = expectInt(static_cast<int>(Mod::DAY_REDUCTION), 373, "Mod::DAY_REDUCTION") && ok;
    ok      = expectInt(static_cast<int>(Mod::PERPETUATION_REDUCTION), 346, "Mod::PERPETUATION_REDUCTION") && ok;
    ok      = expectInt(static_cast<int>(Mod::HALF_PERPETUATION_CARBUNCLE), 356, "Mod::HALF_PERPETUATION_CARBUNCLE") && ok;
    ok      = expectInt(static_cast<int>(Mod::HALF_PERPETUATION_DAY), 1170, "Mod::HALF_PERPETUATION_DAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::HALF_PERPETUATION_WEATHER), 1171, "Mod::HALF_PERPETUATION_WEATHER") && ok;
    ok      = expectInt(static_cast<int>(Mod::BP_DELAY), 357, "Mod::BP_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_ELEMENTAL_SIPHON), 540, "Mod::ENHANCES_ELEMENTAL_SIPHON") && ok;
    ok      = expectInt(static_cast<int>(Mod::BP_DELAY_II), 541, "Mod::BP_DELAY_II") && ok;
    ok      = expectInt(static_cast<int>(Mod::BP_DAMAGE), 126, "Mod::BP_DAMAGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLOOD_BOON), 913, "Mod::BLOOD_BOON") && ok;
    ok      = expectInt(static_cast<int>(Mod::AVATARS_FAVOR_ENHANCE), 1154, "Mod::AVATARS_FAVOR_ENHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::AVATAR_LVL_BONUS), 1040, "Mod::AVATAR_LVL_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::CARBUNCLE_LVL_BONUS), 1041, "Mod::CARBUNCLE_LVL_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::CAIT_SITH_LVL_BONUS), 1042, "Mod::CAIT_SITH_LVL_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_MANA_CEDE), 74, "Mod::ENHANCES_MANA_CEDE") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUMMONING_MAGIC_CAST), 1078, "Mod::SUMMONING_MAGIC_CAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::SPIRIT_CAST_REDUCTION), 140, "Mod::SPIRIT_CAST_REDUCTION") && ok;
    return ok;
}

auto testModBlueMageValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::BLUE_POINTS), 309, "Mod::BLUE_POINTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLUE_LEARN_CHANCE), 945, "Mod::BLUE_LEARN_CHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLUE_JOB_TRAIT_BONUS), 1058, "Mod::BLUE_JOB_TRAIT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLUE_MAGIC_EFFECT), 1059, "Mod::BLUE_MAGIC_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_BURST_AFFINITY), 1139, "Mod::ENHANCES_BURST_AFFINITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_CHAIN_AFFINITY), 1140, "Mod::ENHANCES_CHAIN_AFFINITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLUE_MAGIC_RECAST), 1147, "Mod::BLUE_MAGIC_RECAST") && ok;
    return ok;
}

auto testModCorsairValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::EXP_BONUS), 382, "Mod::EXP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_RANGE), 528, "Mod::ROLL_RANGE") && ok;
    ok      = expectInt(static_cast<int>(Mod::JOB_BONUS_CHANCE), 542, "Mod::JOB_BONUS_CHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::RANDOM_DEAL_BONUS), 220, "Mod::RANDOM_DEAL_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::TRIPLE_SHOT_RATE), 999, "Mod::TRIPLE_SHOT_RATE") && ok;
    ok      = expectInt(static_cast<int>(Mod::QUICK_DRAW_RECAST), 1060, "Mod::QUICK_DRAW_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::DMG_REFLECT), 316, "Mod::DMG_REFLECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_ROGUES), 317, "Mod::ROLL_ROGUES") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_GALLANTS), 318, "Mod::ROLL_GALLANTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_CHAOS), 319, "Mod::ROLL_CHAOS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_BEAST), 320, "Mod::ROLL_BEAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_CHORAL), 321, "Mod::ROLL_CHORAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_HUNTERS), 322, "Mod::ROLL_HUNTERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_SAMURAI), 323, "Mod::ROLL_SAMURAI") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_NINJA), 324, "Mod::ROLL_NINJA") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_DRACHEN), 325, "Mod::ROLL_DRACHEN") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_EVOKERS), 326, "Mod::ROLL_EVOKERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_MAGUS), 327, "Mod::ROLL_MAGUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_CORSAIRS), 328, "Mod::ROLL_CORSAIRS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_PUPPET), 329, "Mod::ROLL_PUPPET") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_DANCERS), 330, "Mod::ROLL_DANCERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_SCHOLARS), 331, "Mod::ROLL_SCHOLARS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_BOLTERS), 869, "Mod::ROLL_BOLTERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_CASTERS), 870, "Mod::ROLL_CASTERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_COURSERS), 871, "Mod::ROLL_COURSERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_BLITZERS), 872, "Mod::ROLL_BLITZERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_TACTICIANS), 873, "Mod::ROLL_TACTICIANS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_ALLIES), 874, "Mod::ROLL_ALLIES") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_MISERS), 875, "Mod::ROLL_MISERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_COMPANIONS), 876, "Mod::ROLL_COMPANIONS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_AVENGERS), 877, "Mod::ROLL_AVENGERS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_NATURALISTS), 878, "Mod::ROLL_NATURALISTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ROLL_RUNEISTS), 879, "Mod::ROLL_RUNEISTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::BUST), 332, "Mod::BUST") && ok;
    ok      = expectInt(static_cast<int>(Mod::QUICK_DRAW_DMG), 411, "Mod::QUICK_DRAW_DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::QUICK_DRAW_DMG_PERCENT), 834, "Mod::QUICK_DRAW_DMG_PERCENT") && ok;
    ok      = expectInt(static_cast<int>(Mod::QUICK_DRAW_MACC), 191, "Mod::QUICK_DRAW_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::PHANTOM_ROLL), 881, "Mod::PHANTOM_ROLL") && ok;
    ok      = expectInt(static_cast<int>(Mod::PHANTOM_DURATION), 882, "Mod::PHANTOM_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::PHANTOM_RECAST), 1076, "Mod::PHANTOM_RECAST") && ok;
    return ok;
}

auto testModPuppetmasterValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::AUTO_MAB_COEFFICIENT), 157, "Mod::AUTO_MAB_COEFFICIENT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MANEUVER_BONUS), 504, "Mod::MANEUVER_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::OVERLOAD_THRESH), 505, "Mod::OVERLOAD_THRESH") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_DECISION_DELAY), 842, "Mod::AUTO_DECISION_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_SHIELD_BASH_DELAY), 843, "Mod::AUTO_SHIELD_BASH_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_MAGIC_COOLDOWN), 844, "Mod::AUTO_MAGIC_COOLDOWN") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_HEALING_DELAY), 845, "Mod::AUTO_HEALING_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_HEALING_THRESHOLD), 846, "Mod::AUTO_HEALING_THRESHOLD") && ok;
    ok      = expectInt(static_cast<int>(Mod::BURDEN_DECAY), 847, "Mod::BURDEN_DECAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_SHIELD_BASH_SLOW), 848, "Mod::AUTO_SHIELD_BASH_SLOW") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_TP_EFFICIENCY), 849, "Mod::AUTO_TP_EFFICIENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_SCAN_RESISTS), 850, "Mod::AUTO_SCAN_RESISTS") && ok;
    ok      = expectInt(static_cast<int>(Mod::REPAIR_EFFECT), 853, "Mod::REPAIR_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::REPAIR_POTENCY), 854, "Mod::REPAIR_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::PREVENT_OVERLOAD), 855, "Mod::PREVENT_OVERLOAD") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUPPRESS_OVERLOAD), 125, "Mod::SUPPRESS_OVERLOAD") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_STEAM_JACKET_REDUCTION), 939, "Mod::AUTO_STEAM_JACKET_REDUCTION") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_SCHURZEN), 940, "Mod::AUTO_SCHURZEN") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_EQUALIZER), 941, "Mod::AUTO_EQUALIZER") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_PERFORMANCE_BOOST), 942, "Mod::AUTO_PERFORMANCE_BOOST") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_ANALYZER), 943, "Mod::AUTO_ANALYZER") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_ELEM_CAPACITY), 987, "Mod::AUTO_ELEM_CAPACITY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_RANGED_DELAY), 1001, "Mod::AUTO_RANGED_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTO_RANGED_DAMAGEP), 1002, "Mod::AUTO_RANGED_DAMAGEP") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUTOMATON_LVL_BONUS), 1044, "Mod::AUTOMATON_LVL_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::VOLT_GUN_POTENCY), 1198, "Mod::VOLT_GUN_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::TACTICAL_SWITCH_TP_BONUS), 1199, "Mod::TACTICAL_SWITCH_TP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::OVERDRIVE_BONUS_DURATION), 1200, "Mod::OVERDRIVE_BONUS_DURATION") && ok;
    return ok;
}

auto testModDancerValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FINISHING_MOVES), 333, "Mod::FINISHING_MOVES") && ok;
    ok      = expectInt(static_cast<int>(Mod::SAMBA_DURATION), 490, "Mod::SAMBA_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::WALTZ_POTENCY), 491, "Mod::WALTZ_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::JIG_DURATION), 492, "Mod::JIG_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::VFLOURISH_MACC), 493, "Mod::VFLOURISH_MACC") && ok;
    ok      = expectInt(static_cast<int>(Mod::STEP_FINISH), 494, "Mod::STEP_FINISH") && ok;
    ok      = expectInt(static_cast<int>(Mod::STEP_ACCURACY), 403, "Mod::STEP_ACCURACY") && ok;
    ok      = expectInt(static_cast<int>(Mod::WALTZ_DELAY), 497, "Mod::WALTZ_DELAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::SAMBA_PDURATION), 498, "Mod::SAMBA_PDURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::REVERSE_FLOURISH_EFFECT), 836, "Mod::REVERSE_FLOURISH_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAX_FINISHING_MOVE_BONUS), 988, "Mod::MAX_FINISHING_MOVE_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WALTZ_COST), 139, "Mod::WALTZ_COST") && ok;
    ok      = expectInt(static_cast<int>(Mod::STEP_TP_CONSUMED), 1077, "Mod::STEP_TP_CONSUMED") && ok;
    return ok;
}

auto testModScholarValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::BLACK_MAGIC_COST), 393, "Mod::BLACK_MAGIC_COST") && ok;
    ok      = expectInt(static_cast<int>(Mod::WHITE_MAGIC_COST), 394, "Mod::WHITE_MAGIC_COST") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLACK_MAGIC_CAST), 395, "Mod::BLACK_MAGIC_CAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::WHITE_MAGIC_CAST), 396, "Mod::WHITE_MAGIC_CAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::BLACK_MAGIC_RECAST), 397, "Mod::BLACK_MAGIC_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::WHITE_MAGIC_RECAST), 398, "Mod::WHITE_MAGIC_RECAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::ALACRITY_CELERITY_EFFECT), 399, "Mod::ALACRITY_CELERITY_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_ARTS_EFFECT), 334, "Mod::LIGHT_ARTS_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_ARTS_EFFECT), 335, "Mod::DARK_ARTS_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_ARTS_SKILL), 336, "Mod::LIGHT_ARTS_SKILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_ARTS_SKILL), 337, "Mod::DARK_ARTS_SKILL") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_ARTS_REGEN), 338, "Mod::LIGHT_ARTS_REGEN") && ok;
    ok      = expectInt(static_cast<int>(Mod::REGEN_DURATION), 339, "Mod::REGEN_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::HELIX_EFFECT), 478, "Mod::HELIX_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::HELIX_DURATION), 477, "Mod::HELIX_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::STORMSURGE_EFFECT), 400, "Mod::STORMSURGE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUBLIMATION_BONUS), 401, "Mod::SUBLIMATION_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::GRIMOIRE_SPELLCASTING), 489, "Mod::GRIMOIRE_SPELLCASTING") && ok;
    ok      = expectInt(static_cast<int>(Mod::STRATAGEM_RECAST), 1159, "Mod::STRATAGEM_RECAST") && ok;
    return ok;
}

auto testModGeomancerValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::CARDINAL_CHANT), 959, "Mod::CARDINAL_CHANT") && ok;
    ok      = expectInt(static_cast<int>(Mod::CARDINAL_CHANT_BONUS), 1132, "Mod::CARDINAL_CHANT_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::INDI_DURATION), 960, "Mod::INDI_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::GEOMANCY_BONUS), 961, "Mod::GEOMANCY_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIDENED_COMPASS), 962, "Mod::WIDENED_COMPASS") && ok;
    ok      = expectInt(static_cast<int>(Mod::MENDING_HALATION), 968, "Mod::MENDING_HALATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::RADIAL_ARCANA), 969, "Mod::RADIAL_ARCANA") && ok;
    ok      = expectInt(static_cast<int>(Mod::CURATIVE_RECANTATION), 970, "Mod::CURATIVE_RECANTATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::PRIMEVAL_ZEAL), 971, "Mod::PRIMEVAL_ZEAL") && ok;
    ok      = expectInt(static_cast<int>(Mod::FULL_CIRCLE), 1025, "Mod::FULL_CIRCLE") && ok;
    ok      = expectInt(static_cast<int>(Mod::BOLSTER_EFFECT), 1028, "Mod::BOLSTER_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIFE_CYCLE_EFFECT), 1029, "Mod::LIFE_CYCLE_EFFECT") && ok;
    ok      = expectInt(static_cast<int>(Mod::AURA_SIZE), 1030, "Mod::AURA_SIZE") && ok;
    return ok;
}

auto testModEnspellAndSpikesValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::ENSPELL), 341, "Mod::ENSPELL") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENSPELL_DMG), 343, "Mod::ENSPELL_DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENSPELL_DMG_BONUS), 432, "Mod::ENSPELL_DMG_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENSPELL_CHANCE), 856, "Mod::ENSPELL_CHANCE") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENSPELL_DMG_PCT), 1195, "Mod::ENSPELL_DMG_PCT") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENSPELL_DURATION), 938, "Mod::ENSPELL_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::SPIKES), 342, "Mod::SPIKES") && ok;
    ok      = expectInt(static_cast<int>(Mod::SPIKES_DMG), 344, "Mod::SPIKES_DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::SPIKES_DMG_BONUS), 1079, "Mod::SPIKES_DMG_BONUS") && ok;
    return ok;
}

auto testModTPValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::TP_BONUS), 345, "Mod::TP_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::SAVETP), 880, "Mod::SAVETP") && ok;
    ok      = expectInt(static_cast<int>(Mod::CONSERVE_TP), 944, "Mod::CONSERVE_TP") && ok;
    return ok;
}

auto testModRuneFencerValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::INQUARTATA), 963, "Mod::INQUARTATA") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_BATTUTA), 1004, "Mod::ENHANCES_BATTUTA") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_ELEMENTAL_SFORZO), 1005, "Mod::ENHANCES_ELEMENTAL_SFORZO") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_SLEIGHT_OF_SWORD), 1006, "Mod::ENHANCES_SLEIGHT_OF_SWORD") && ok;
    ok      = expectInt(static_cast<int>(Mod::ENHANCES_INSPIRATION), 1007, "Mod::ENHANCES_INSPIRATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::SWORDPLAY), 1008, "Mod::SWORDPLAY") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIEMENT), 1009, "Mod::LIEMENT") && ok;
    ok      = expectInt(static_cast<int>(Mod::VALIANCE_VALLATION_DURATION), 1010, "Mod::VALIANCE_VALLATION_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::PFLUG), 1011, "Mod::PFLUG") && ok;
    ok      = expectInt(static_cast<int>(Mod::VIVACIOUS_PULSE_POTENCY), 1012, "Mod::VIVACIOUS_PULSE_POTENCY") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUGMENTS_VIVACIOUS_PULSE), 1013, "Mod::AUGMENTS_VIVACIOUS_PULSE") && ok;
    ok      = expectInt(static_cast<int>(Mod::RAYKE_DURATION), 1014, "Mod::RAYKE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::ODYLLIC_SUBTERFUGE_DURATION), 1015, "Mod::ODYLLIC_SUBTERFUGE_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::SWIPE), 1016, "Mod::SWIPE") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIEMENT_DURATION), 1017, "Mod::LIEMENT_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::GAMBIT_DURATION), 1018, "Mod::GAMBIT_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::EMBOLDEN_DURATION), 1019, "Mod::EMBOLDEN_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIEMENT_EXTENDS_TO_AREA), 1020, "Mod::LIEMENT_EXTENDS_TO_AREA") && ok;
    ok      = expectInt(static_cast<int>(Mod::INSPIRATION_FAST_CAST), 1021, "Mod::INSPIRATION_FAST_CAST") && ok;
    ok      = expectInt(static_cast<int>(Mod::PARRY_SPIKES), 1022, "Mod::PARRY_SPIKES") && ok;
    ok      = expectInt(static_cast<int>(Mod::PARRY_SPIKES_DMG), 1023, "Mod::PARRY_SPIKES_DMG") && ok;
    ok      = expectInt(static_cast<int>(Mod::SPECIAL_ATTACK_EVASION), 1024, "Mod::SPECIAL_ATTACK_EVASION") && ok;
    ok      = expectInt(static_cast<int>(Mod::AUGMENTS_SLEIGHT_OF_SWORD), 277, "Mod::AUGMENTS_SLEIGHT_OF_SWORD") && ok;
    return ok;
}

auto testModElementalStaffAndAffinityPerpValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::FIRE_STAFF_BONUS), 347, "Mod::FIRE_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_STAFF_BONUS), 348, "Mod::ICE_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_STAFF_BONUS), 349, "Mod::WIND_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_STAFF_BONUS), 350, "Mod::EARTH_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_STAFF_BONUS), 351, "Mod::THUNDER_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_STAFF_BONUS), 352, "Mod::WATER_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_STAFF_BONUS), 353, "Mod::LIGHT_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_STAFF_BONUS), 354, "Mod::DARK_STAFF_BONUS") && ok;
    ok      = expectInt(static_cast<int>(Mod::FIRE_AFFINITY_PERP), 553, "Mod::FIRE_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::ICE_AFFINITY_PERP), 554, "Mod::ICE_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::WIND_AFFINITY_PERP), 555, "Mod::WIND_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::EARTH_AFFINITY_PERP), 556, "Mod::EARTH_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::THUNDER_AFFINITY_PERP), 557, "Mod::THUNDER_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::WATER_AFFINITY_PERP), 558, "Mod::WATER_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::LIGHT_AFFINITY_PERP), 559, "Mod::LIGHT_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::DARK_AFFINITY_PERP), 560, "Mod::DARK_AFFINITY_PERP") && ok;
    ok      = expectInt(static_cast<int>(Mod::ADDS_WEAPONSKILL), 355, "Mod::ADDS_WEAPONSKILL") && ok;
    return ok;
}

auto testModStealthAndDurationValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::STEALTH), 358, "Mod::STEALTH") && ok;
    ok      = expectInt(static_cast<int>(Mod::SNEAK_DURATION), 946, "Mod::SNEAK_DURATION") && ok;
    ok      = expectInt(static_cast<int>(Mod::INVISIBLE_DURATION), 947, "Mod::INVISIBLE_DURATION") && ok;
    return ok;
}

auto testModWeaponDamageRatingValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(Mod::DMG_RATING), 287, "Mod::DMG_RATING") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAIN_DMG_RATING), 366, "Mod::MAIN_DMG_RATING") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUB_DMG_RATING), 367, "Mod::SUB_DMG_RATING") && ok;
    ok      = expectInt(static_cast<int>(Mod::RANGED_DMG_RATING), 376, "Mod::RANGED_DMG_RATING") && ok;
    ok      = expectInt(static_cast<int>(Mod::MAIN_DMG_RANK), 377, "Mod::MAIN_DMG_RANK") && ok;
    ok      = expectInt(static_cast<int>(Mod::SUB_DMG_RANK), 378, "Mod::SUB_DMG_RANK") && ok;
    ok      = expectInt(static_cast<int>(Mod::RANGED_DMG_RANK), 379, "Mod::RANGED_DMG_RANK") && ok;
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
    ok      = testModFTPBonusValues() && ok;
    ok      = testModWarriorValues() && ok;
    ok      = testModMonkValues() && ok;
    ok      = testModWhiteMageValues() && ok;
    ok      = testModBlackMageValues() && ok;
    ok      = testModRedMageValues() && ok;
    ok      = testModThiefValues() && ok;
    ok      = testModPaladinValues() && ok;
    ok      = testModDarkKnightValues() && ok;
    ok      = testModBeastmasterValues() && ok;
    ok      = testModBardValues() && ok;
    ok      = testModRangerValues() && ok;
    ok      = testModSamuraiValues() && ok;
    ok      = testModNinjaValues() && ok;
    ok      = testModDragoonValues() && ok;
    ok      = testModSummonerValues() && ok;
    ok      = testModBlueMageValues() && ok;
    ok      = testModCorsairValues() && ok;
    ok      = testModPuppetmasterValues() && ok;
    ok      = testModDancerValues() && ok;
    ok      = testModScholarValues() && ok;
    ok      = testModGeomancerValues() && ok;
    ok      = testModEnspellAndSpikesValues() && ok;
    ok      = testModTPValues() && ok;
    ok      = testModRuneFencerValues() && ok;
    ok      = testModElementalStaffAndAffinityPerpValues() && ok;
    ok      = testModStealthAndDurationValues() && ok;
    ok      = testModWeaponDamageRatingValues() && ok;
    ok      = testModifierAccessorsAndSetAmount() && ok;
    ok      = testPetModifierAccessorsAndSetAmount() && ok;
    return ok;
}
