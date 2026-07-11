/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

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

#include "attackutils.h"
#include "utils/attackutils_capacity.h"
#include "attack.h"
#include "battleutils.h"
#include "common/utils.h"
#include "items/item_weapon.h"
#include "lua/luautils.h"
#include "status_effect_container.h"

namespace attackutils
{

/************************************************************************
 *                                                                       *
 *  Multihit calculator.                                                 *
 *                                                                       *
 ************************************************************************/
uint8 getHitCount(uint8 hits)
{
    uint8 distribution = xirand::GetRandomNumber(100);
    uint8 num          = 1;

    switch (hits)
    {
        case 0:
            break;
        case 1:
            break;
        case 2: // cdf = 55,100
            if (distribution < 55)
            {
                break;
            }
            else
            {
                num += 1;
                break;
            }
            break;
        case 3: // cdf = 30,80,100
            if (distribution < 30)
            {
                break;
            }
            else if (distribution < 80)
            {
                num += 1;
                break;
            }
            else
            {
                num += 2;
                break;
            }
            break;
        case 4: // cdf = 20,50,80,100
            if (distribution < 20)
            {
                break;
            }
            else if (distribution < 50)
            {
                num += 1;
                break;
            }
            else if (distribution < 80)
            {
                num += 2;
                break;
            }
            else
            {
                num += 3;
                break;
            }
            break;
        case 5: // cdf = 10,30,60,90,100
            if (distribution < 10)
            {
                break;
            }
            else if (distribution < 30)
            {
                num += 1;
                break;
            }
            else if (distribution < 60)
            {
                num += 2;
                break;
            }
            else if (distribution < 90)
            {
                num += 3;
                break;
            }
            else
            {
                num += 4;
                break;
            }
            break;
        case 6: // cdf = 10,30,50,70,90,100
            if (distribution < 10)
            {
                break;
            }
            else if (distribution < 30)
            {
                num += 1;
                break;
            }
            else if (distribution < 50)
            {
                num += 2;
                break;
            }
            else if (distribution < 70)
            {
                num += 3;
                break;
            }
            else if (distribution < 90)
            {
                num += 4;
                break;
            }
            else
            {
                num += 5;
                break;
            }
            break;
        case 7: // cdf = 5,20,45,70,85,95,100
            if (distribution < 5)
            {
                break;
            }
            else if (distribution < 20)
            {
                num += 1;
                break;
            }
            else if (distribution < 45)
            {
                num += 2;
                break;
            }
            else if (distribution < 70)
            {
                num += 3;
                break;
            }
            else if (distribution < 85)
            {
                num += 4;
                break;
            }
            else if (distribution < 95)
            {
                num += 5;
                break;
            }
            else
            {
                num += 6;
                break;
            }
            break;
        case 8: // cdf = 5,20,45,70,85,95,98,100
            if (distribution < 5)
            {
                break;
            }
            else if (distribution < 20)
            {
                num += 1;
                break;
            }
            else if (distribution < 45)
            {
                num += 2;
                break;
            }
            else if (distribution < 70)
            {
                num += 3;
                break;
            }
            else if (distribution < 85)
            {
                num += 4;
                break;
            }
            else if (distribution < 95)
            {
                num += 5;
                break;
            }
            else if (distribution < 98)
            {
                num += 6;
                break;
            }
            else
            {
                num += 7;
                break;
            }
            break;
    }
    return std::min<uint8>(num, 8); // no more than eight hits per attack
}

bool IsParried(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    return luautils::callGlobal<bool>("xi.combat.physical.isParried", PDefender, PAttacker);
}

bool IsGuarded(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    return luautils::callGlobal<bool>("xi.combat.physical.isGuarded", PDefender, PAttacker);
}

bool IsBlocked(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    return luautils::callGlobal<bool>("xi.combat.physical.isBlocked", PDefender, PAttacker);
}

/************************************************************************
 *                                                                       *
 *  Check for damage multiplier, relic weapons etc.                      *
 *                                                                       *
 ************************************************************************/
uint32 CheckForDamageMultiplier(CCharEntity* PChar, CItemWeapon* PWeapon, uint32 damage, PHYSICAL_ATTACK_TYPE attackType, uint8 weaponSlot, bool allowProc)
{
    if (attackutilshelpers::ShouldRejectNullWeapon(PWeapon == nullptr))
    {
        return damage;
    }

    uint32 originalDamage    = damage;
    int16  occ_do_triple_dmg = 0;
    int16  occ_do_double_dmg = 0;
    const auto attackTypeU8 = static_cast<uint8>(attackType);

    if (attackutilshelpers::ShouldUseRangedRemOcc(attackTypeU8))
    {
        occ_do_triple_dmg = attackutilshelpers::RemOccChance(PChar->getMod(Mod::REM_OCC_DO_TRIPLE_DMG_RANGED));
        occ_do_double_dmg = attackutilshelpers::RemOccChance(PChar->getMod(Mod::REM_OCC_DO_DOUBLE_DMG_RANGED));
    }
    else if (attackutilshelpers::ShouldUseMainHandRemOcc(attackTypeU8, weaponSlot))
    {
        occ_do_triple_dmg = attackutilshelpers::RemOccChance(PChar->getMod(Mod::REM_OCC_DO_TRIPLE_DMG));
        occ_do_double_dmg = attackutilshelpers::RemOccChance(PChar->getMod(Mod::REM_OCC_DO_DOUBLE_DMG));
    }

    const float occ_extra_dmg = attackutilshelpers::OccExtraDmgMultiplier(
        battleutils::GetScaledItemModifier(PChar, PWeapon, Mod::OCC_DO_EXTRA_DMG));
    const int16 occ_extra_dmg_chance = attackutilshelpers::OccExtraDmgChance(
        battleutils::GetScaledItemModifier(PChar, PWeapon, Mod::EXTRA_DMG_CHANCE));

    // Preserve exclusive short-circuit RNG order of the allowProc ladder.
    if (allowProc)
    {
        if (occ_extra_dmg > 3.0f &&
            attackutilshelpers::RollChancePercent(occ_extra_dmg_chance, xirand::GetRandomNumber(100)))
        {
            return attackutilshelpers::ApplyOccProcDamage(
                damage, attackutilshelpers::OccProcResult::ExtraDamage, occ_extra_dmg);
        }
        if (attackutilshelpers::RollChancePercent(occ_do_triple_dmg, xirand::GetRandomNumber(100)))
        {
            return attackutilshelpers::ApplyOccProcDamage(
                damage, attackutilshelpers::OccProcResult::RemTriple, occ_extra_dmg);
        }
        if (occ_extra_dmg > 2.0f &&
            attackutilshelpers::RollChancePercent(occ_extra_dmg_chance, xirand::GetRandomNumber(100)))
        {
            return attackutilshelpers::ApplyOccProcDamage(
                damage, attackutilshelpers::OccProcResult::ExtraDamage, occ_extra_dmg);
        }
        if (attackutilshelpers::RollChancePercent(occ_do_double_dmg, xirand::GetRandomNumber(100)))
        {
            return attackutilshelpers::ApplyOccProcDamage(
                damage, attackutilshelpers::OccProcResult::RemDouble, occ_extra_dmg);
        }
        if (occ_extra_dmg > 0 &&
            attackutilshelpers::RollChancePercent(occ_extra_dmg_chance, xirand::GetRandomNumber(100)))
        {
            return attackutilshelpers::ApplyOccProcDamage(
                damage, attackutilshelpers::OccProcResult::ExtraDamage, occ_extra_dmg);
        }
    }

    switch (attackType)
    {
        case PHYSICAL_ATTACK_TYPE::ZANSHIN:
            if (attackutilshelpers::ShouldApplyZanshinDoubleDamage(
                    attackTypeU8,
                    attackutilshelpers::RollRatePercent(PChar->getMod(Mod::ZANSHIN_DOUBLE_DAMAGE), xirand::GetRandomNumber(100))))
            {
                return attackutilshelpers::ApplyTypeDoubleDamage(originalDamage, 2);
            }
            break;
        case PHYSICAL_ATTACK_TYPE::TRIPLE:
            if (attackutilshelpers::ShouldApplyTATripleDamage(
                    attackTypeU8,
                    attackutilshelpers::RollRatePercent(PChar->getMod(Mod::TA_TRIPLE_DMG_RATE), xirand::GetRandomNumber(100))))
            {
                return attackutilshelpers::ApplyTypeDoubleDamage(originalDamage, 3);
            }
            break;
        case PHYSICAL_ATTACK_TYPE::DOUBLE:
            if (attackutilshelpers::ShouldApplyDADoubleDamage(
                    attackTypeU8,
                    attackutilshelpers::RollRatePercent(PChar->getMod(Mod::DA_DOUBLE_DMG_RATE), xirand::GetRandomNumber(100))))
            {
                return attackutilshelpers::ApplyTypeDoubleDamage(originalDamage, 2);
            }
            break;
        case PHYSICAL_ATTACK_TYPE::RAPID_SHOT:
            if (attackutilshelpers::ShouldApplyRapidShotDoubleDamage(
                    attackTypeU8,
                    attackutilshelpers::RollRatePercent(PChar->getMod(Mod::RAPID_SHOT_DOUBLE_DAMAGE), xirand::GetRandomNumber(100))))
            {
                return attackutilshelpers::ApplyTypeDoubleDamage(originalDamage, 2);
            }
            break;
        case PHYSICAL_ATTACK_TYPE::SAMBA:
            if (attackutilshelpers::ShouldApplySambaDoubleDamage(
                    attackTypeU8,
                    attackutilshelpers::RollRatePercent(PChar->getMod(Mod::SAMBA_DOUBLE_DAMAGE), xirand::GetRandomNumber(100))))
            {
                return attackutilshelpers::ApplyTypeDoubleDamage(originalDamage, 2);
            }
            break;
        default:
            break;
    }
    return originalDamage;
}

} // namespace attackutils
