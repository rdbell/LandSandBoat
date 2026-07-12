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
#include "hit_count_capacity.h"
#include "defense_gates_capacity.h"
#include "attack.h"
#include "battleutils.h"
#include "ai/ai_container.h"
#include "charutils.h"
#include "common/settings.h"
#include "common/utils.h"
#include "common/xirand.h"
#include "entities/automaton_entity.h"
#include "entities/mob_entity.h"
#include "entities/pet_entity.h"
#include "items/item_equipment.h"
#include "items/item_weapon.h"
#include "lua/luautils.h"
#include "mob_modifier.h"
#include "status_effect_container.h"

namespace attackutils
{
namespace
{

using namespace defensegateshelpers;

auto IsMobPetOrTrust(const CBattleEntity* PEntity) -> bool
{
    return PEntity->objtype == TYPE_MOB || PEntity->objtype == TYPE_PET || PEntity->objtype == TYPE_TRUST;
}

auto IsAutomaton(const CBattleEntity* PEntity) -> bool
{
    return PEntity->objtype == TYPE_PET &&
           static_cast<const CPetEntity*>(PEntity)->getPetType() == PET_TYPE::AUTOMATON;
}

// Pet/Trust inherit CMobEntity; PC/NPC have no mob mods.
auto DefenderMobMod(CBattleEntity* PEntity, const uint16 type) -> int16
{
    if (!IsMobPetOrTrust(PEntity))
    {
        return 0;
    }
    return static_cast<CMobEntity*>(PEntity)->getMobMod(type);
}

// PC: main weapon skill + iLvlSkill; non-PC: GetMaxSkill(A+, mainLvl).
// Mirrors calculateParryRate/calculateGuardRate attacker skill assembly.
auto AssembledAttackerWeaponSkill(CBattleEntity* PAttacker) -> double
{
    if (PAttacker->objtype == TYPE_PC)
    {
        if (auto* weapon = dynamic_cast<CItemWeapon*>(PAttacker->m_Weapons[SLOT_MAIN]))
        {
            return static_cast<double>(PAttacker->GetSkill(weapon->getSkillType())) +
                   static_cast<double>(weapon->getILvlSkill());
        }
        // getWeaponSkillType returns 0 with empty main; getILvlSkill returns 0.
        return static_cast<double>(PAttacker->GetSkill(SKILL_NONE));
    }
    return static_cast<double>(battleutils::GetMaxSkill(SkillRankAPlus, PAttacker->GetMLevel()));
}

// Block-path attacker skill: assume H2H, else main weapon skill type; no iLvl.
// Mirrors calculateBlockRate: isUsingH2H → H2H else getWeaponSkillType(MAIN).
auto AssembledBlockAttackerSkill(CBattleEntity* PAttacker) -> double
{
    SKILLTYPE skillType = SKILL_HAND_TO_HAND;
    if (auto* weapon = dynamic_cast<CItemWeapon*>(PAttacker->m_Weapons[SLOT_MAIN]))
    {
        if (weapon->getSkillType() != SKILL_HAND_TO_HAND)
        {
            skillType = static_cast<SKILLTYPE>(weapon->getSkillType());
        }
    }
    // Bare hands (no main weapon): isUsingH2H is true for PC → H2H skill.
    return static_cast<double>(PAttacker->GetSkill(skillType));
}

auto AssembledParryDefenderSkill(CBattleEntity* PDefender) -> double
{
    if (PDefender->objtype == TYPE_PC)
    {
        double skill = static_cast<double>(PDefender->GetSkill(SKILL_PARRY));
        skill += static_cast<double>(PDefender->getMod(Mod::PARRY));
        if (auto* weapon = dynamic_cast<CItemWeapon*>(PDefender->m_Weapons[SLOT_MAIN]))
        {
            skill += static_cast<double>(weapon->getILvlParry());
        }
        return skill;
    }
    return static_cast<double>(battleutils::GetMaxSkill(SkillRankAPlus, PDefender->GetMLevel()));
}

auto AssembledGuardDefenderSkill(CBattleEntity* PDefender) -> double
{
    if (PDefender->objtype == TYPE_PC)
    {
        double skill = static_cast<double>(PDefender->GetSkill(SKILL_GUARD));
        skill += static_cast<double>(PDefender->getMod(Mod::GUARD));
        // getILvlParry also covers H2H guard.
        if (auto* weapon = dynamic_cast<CItemWeapon*>(PDefender->m_Weapons[SLOT_MAIN]))
        {
            skill += static_cast<double>(weapon->getILvlParry());
        }
        return skill;
    }
    return static_cast<double>(battleutils::GetMaxSkill(SkillRankAPlus, PDefender->GetMLevel()));
}

auto DefenseRoll1to10000() -> int
{
    // xirand max is exclusive; Lua math.random(1, 10000) is inclusive.
    return xirand::GetRandomNumber(1, DefenseRollMax + 1);
}

} // namespace

/************************************************************************
 *                                                                       *
 *  Multihit calculator.                                                 *
 *                                                                       *
 ************************************************************************/
uint8 getHitCount(uint8 hits)
{
    return hitcounthelpers::GetHitCount(hits, static_cast<std::uint8_t>(xirand::GetRandomNumber(100)));
}

bool IsParried(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    if (PAttacker == nullptr || PDefender == nullptr)
    {
        return false;
    }

    const bool isFacing         = facing(PDefender->loc.p, PAttacker->loc.p, FacingCone);
    const bool isEngaged        = PDefender->PAI != nullptr && PDefender->PAI->IsEngaged();
    const bool hasPreventAction = PDefender->StatusEffectContainer->HasPreventActionEffect(true);

    bool abilityEligible = false;
    if (PDefender->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(PDefender);
        const bool rankNonZero = PChar->RealSkills.rank[SKILL_PARRY] != 0;
        auto*      mainWeapon  = dynamic_cast<CItemWeapon*>(PDefender->m_Weapons[SLOT_MAIN]);
        const bool hasMain     = mainWeapon != nullptr;
        const bool mainIsH2H   = hasMain && mainWeapon->getSkillType() == SKILL_HAND_TO_HAND;
        abilityEligible        = PCParryAbilityEligible(rankNonZero, hasMain, mainIsH2H);
    }
    else
    {
        abilityEligible = NonPCParryAbilityEligible(DefenderMobMod(PDefender, MOBMOD_CAN_PARRY) > 0);
    }

    if (!CanParry(isFacing, isEngaged, hasPreventAction, abilityEligible))
    {
        return false;
    }

    double issekiganPower = 0.0;
    if (auto* effect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Issekigan))
    {
        issekiganPower = static_cast<double>(effect->GetPower());
    }

    const double rate = ParryRate(
        AssembledParryDefenderSkill(PDefender),
        AssembledAttackerWeaponSkill(PAttacker),
        issekiganPower,
        static_cast<double>(PDefender->getMod(Mod::INQUARTATA)));
    const int  roll    = DefenseRoll1to10000();
    const bool parried = defensegateshelpers::IsParried(true, rate, roll);

    if (parried)
    {
        const auto recoveryMod = static_cast<std::int16_t>(PDefender->getMod(Mod::PARRY_HP_RECOVERY));
        if (ShouldApplyParryHPRecovery(true, recoveryMod, PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::CurseIi)))
        {
            PDefender->addHP(recoveryMod);
        }
        if (PDefender->objtype == TYPE_PC)
        {
            battleutils::HandleTacticalParry(PDefender);
        }
    }

    if (ShouldTryDefensiveSkillUp(
            PDefender->objtype == TYPE_PC,
            parried,
            settings::get<bool>("map.DEFENSIVE_OLD_SKILLUP_STYLE")))
    {
        charutils::TrySkillUP(static_cast<CCharEntity*>(PDefender), SKILL_PARRY, PAttacker->GetMLevel());
    }

    return parried;
}

bool IsGuarded(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    if (PAttacker == nullptr || PDefender == nullptr)
    {
        return false;
    }

    const bool isFacing         = facing(PDefender->loc.p, PAttacker->loc.p, FacingCone);
    const bool isEngaged        = PDefender->PAI != nullptr && PDefender->PAI->IsEngaged();
    const bool hasPreventAction = PDefender->StatusEffectContainer->HasPreventActionEffect(true);

    bool abilityEligible = false;
    if (PDefender->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(PDefender);
        const bool rankPositive = PChar->RealSkills.rank[SKILL_GUARD] > 0;
        auto*      mainWeapon   = dynamic_cast<CItemWeapon*>(PDefender->m_Weapons[SLOT_MAIN]);
        const bool hasMain      = mainWeapon != nullptr;
        const bool mainIsH2H    = hasMain && mainWeapon->getSkillType() == SKILL_HAND_TO_HAND;
        abilityEligible         = PCGuardAbilityEligible(rankPositive, hasMain, mainIsH2H);
    }
    else
    {
        const bool isMNKOrPUP = PDefender->GetMJob() == JOB_MNK || PDefender->GetMJob() == JOB_PUP;
        abilityEligible       = NonPCGuardAbilityEligible(
            IsMobPetOrTrust(PDefender),
            isMNKOrPUP,
            DefenderMobMod(PDefender, MOBMOD_CANNOT_GUARD) != 0);
    }

    if (!CanGuard(isFacing, isEngaged, hasPreventAction, abilityEligible))
    {
        return false;
    }

    const double rate = GuardRate(
        AssembledGuardDefenderSkill(PDefender),
        AssembledAttackerWeaponSkill(PAttacker),
        static_cast<double>(PDefender->getMod(Mod::ADDITIVE_GUARD)));
    const int  roll    = DefenseRoll1to10000();
    const bool guarded = defensegateshelpers::IsGuarded(true, rate, roll);

    if (guarded && PDefender->objtype == TYPE_PC)
    {
        battleutils::HandleTacticalGuard(PDefender);
    }

    if (ShouldTryDefensiveSkillUp(
            PDefender->objtype == TYPE_PC,
            guarded,
            settings::get<bool>("map.DEFENSIVE_OLD_SKILLUP_STYLE")))
    {
        charutils::TrySkillUP(static_cast<CCharEntity*>(PDefender), SKILL_GUARD, PAttacker->GetMLevel());
    }

    return guarded;
}

bool IsBlocked(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    if (PAttacker == nullptr || PDefender == nullptr)
    {
        return false;
    }

    const bool isFacing         = facing(PDefender->loc.p, PAttacker->loc.p, FacingCone);
    const bool hasPreventAction = PDefender->StatusEffectContainer->HasPreventActionEffect(true);

    bool abilityEligible = false;
    bool hasShield       = false;
    int  shieldSize      = 0;

    if (PDefender->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(PDefender);
        const bool rankPositive = PChar->RealSkills.rank[SKILL_SHIELD] > 0;
        if (auto* equip = dynamic_cast<CItemEquipment*>(PChar->getEquip(SLOT_SUB)))
        {
            if (equip->IsShield())
            {
                hasShield  = true;
                shieldSize = static_cast<int>(equip->getShieldSize());
            }
        }
        abilityEligible = PCBlockAbilityEligible(rankPositive, hasShield);
    }
    else
    {
        abilityEligible = NonPCBlockAbilityEligible(
            IsMobPetOrTrust(PDefender),
            DefenderMobMod(PDefender, MOBMOD_CAN_SHIELD_BLOCK) > 0);
    }

    if (!CanBlock(isFacing, hasPreventAction, abilityEligible))
    {
        return false;
    }

    const double attackSkill = AssembledBlockAttackerSkill(PAttacker);
    const double palisade    = static_cast<double>(PDefender->getMod(Mod::PALISADE_BLOCK_BONUS));
    const bool   hasReprisal = PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Reprisal);
    const bool   reprisalBonus = PDefender->getMod(Mod::REPRISAL_BLOCK_BONUS) > 0;

    double rate = 0.0;
    if (PDefender->objtype == TYPE_PC)
    {
        rate = BlockRatePC(
            hasShield,
            shieldSize,
            static_cast<double>(PDefender->GetSkill(SKILL_SHIELD)),
            attackSkill,
            palisade,
            hasReprisal,
            reprisalBonus);
    }
    else if (IsAutomaton(PDefender))
    {
        rate = BlockRateAutomaton(
            DefenderMobMod(PDefender, MOBMOD_CAN_SHIELD_BLOCK) > 0,
            static_cast<double>(PDefender->getMod(Mod::SHIELDBLOCKRATE)),
            static_cast<double>(PDefender->GetSkill(SKILL_AUTOMATON_MELEE)),
            attackSkill);
    }
    else
    {
        // Mob/trust (and non-automaton pet): max shield skill for job/level.
        // Trust clamps main level to 99 (Lua TODO for ilvl trusts).
        const uint8 level = PDefender->objtype == TYPE_TRUST
                                ? static_cast<uint8>(std::min(static_cast<int>(PDefender->GetMLevel()), 99))
                                : PDefender->GetMLevel();
        const double blockSkill = static_cast<double>(
            battleutils::GetMaxSkill(SKILL_SHIELD, PDefender->GetMJob(), level));
        rate = BlockRateMob(
            DefenderMobMod(PDefender, MOBMOD_CAN_SHIELD_BLOCK) > 0,
            static_cast<double>(PDefender->getMod(Mod::SHIELDBLOCKRATE)),
            blockSkill,
            attackSkill,
            palisade,
            hasReprisal,
            reprisalBonus);
    }

    const int  roll    = DefenseRoll1to10000();
    const bool blocked = defensegateshelpers::IsBlocked(true, rate, roll);

    if (ShouldTryDefensiveSkillUp(
            PDefender->objtype == TYPE_PC,
            blocked,
            settings::get<bool>("map.DEFENSIVE_OLD_SKILLUP_STYLE")))
    {
        charutils::TrySkillUP(static_cast<CCharEntity*>(PDefender), SKILL_SHIELD, PAttacker->GetMLevel());
    }

    return blocked;
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
