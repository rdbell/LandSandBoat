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

#include "mobutils.h"

#include "common/database.h"
#include "common/logging.h"
#include "common/utils.h"

#include "map/base_to_rank_capacity.h"
#include "map/mob_base_capacity.h"
#include "map/mob_base_skill_capacity.h"
#include "map/mob_hp_capacity.h"
#include "map/mob_setup_capacity.h"
#include "map/mob_stats_product_capacity.h"
#include "map/mob_weapon_damage_capacity.h"
#include "map/sub_job_stats_capacity.h"

#include "action/action.h"
#include "ai/ai_container.h"
#include "battlefield.h"
#include "battleutils.h"
#include "grades.h"
#include "instance.h"
#include "items/item_weapon.h"
#include "lua/luautils.h"
#include "map_engine.h"
#include "mob_modifier.h"
#include "mob_spell_container.h"
#include "mob_spell_list.h"
#include "packets/s2c/0x028_battle2.h"
#include "status_effect_container.h"
#include "trait.h"
#include "zone_entities.h"
#include "zoneutils.h"
#include <vector>

namespace mobutils
{

ModsMap_t mobSpeciesModsList;
ModsMap_t mobPoolModsList;
ModsMap_t mobSpawnModsList;

/************************************************************************
 *                                                                       *
 *  Calculate mob's initial base weapon damage without modifiers         *
 *                                                                       *
 ************************************************************************/

uint16 GetBaseWeaponDamage(CMobEntity* PMob, uint16 slot)
{
    // https://docs.google.com/spreadsheets/d/1YBoveP-weMdidrirY-vPDzHyxbEI2ryECINlfCnFkLI/edit?pli=1&gid=1743955268#gid=1743955268
    // Basic base damage formulas for reference:
    // Normal Mobs(Non H2H): (Level * Multiplier) + Offset
    // Normal MNK mobs     : (Level * Multiplier(Default: 1.0000)) + Offset (Auto attacks get a penalty multiplier)
    // "Special" MNK mobs  : (Level + Offset) * Multiplier(1.6667) (Auto attacks get a penalty multiplier)

    // NOTE: Multipliers and damage modifiers are handled in battlentity::GetMainWeaponDmg(), battlentity::GetRangedWeaponDmg().
    // Pure region/beginner offset plan (mob_weapon_damage_capacity.h; slice 1601).
    // setDamage returns level only; offsets stored as MOBMOD for later application.
    (void)slot;

    const auto regionID  = static_cast<std::uint8_t>(PMob->loc.zone->GetRegionID());
    const auto mobZoneId = static_cast<std::uint16_t>(PMob->getZone());
    const auto mobLvl    = static_cast<std::uint16_t>(PMob->GetMLevel());
    // Exact equality matches upstream (Voidwatch etc. may combine type bits).
    const bool isNM = PMob->m_Type == MOBTYPE_NOTORIOUS;

    const auto plan = mobweapondamagehelpers::PlanBaseWeaponDamage(regionID, mobLvl, mobZoneId, isNM);
    const auto damage       = std::get<0>(plan);
    const auto offset       = std::get<1>(plan);
    const auto rangedOffset = std::get<2>(plan);

    // Set default offsets. Will be calculated in battlentity::GetMainWeaponDmg()
    PMob->setMobMod(MOBMOD_DAMAGE_OFFSET, offset);
    PMob->setMobMod(MOBMOD_RANGED_DAMAGE_OFFSET, rangedOffset);
    return damage;
}

// Get base skill rankings for ACC/ATT/EVA/MEVA
uint16 GetBaseSkill(CMobEntity* PMob, uint8 rank)
{
    // Pure rank→skill/job proxy (mob_base_skill_capacity.h; slice 1599).
    std::uint16_t skillType = 0;
    std::uint8_t  job       = 0;
    if (!mobbaseskillhelpers::BaseSkillProxy(rank, skillType, job))
    {
        ShowError("mobutils::GetBaseSkill rank (%d) is out of bounds for mob (%u) ", rank, PMob->id);
        return 0;
    }
    return battleutils::GetMaxSkill(static_cast<SKILLTYPE>(skillType), static_cast<JOBTYPE>(job), PMob->GetMLevel());
}

uint16 GetMagicEvasion(CMobEntity* PMob)
{
    // Pure trust/mob cap rank + level clamp (slice 1599).
    const auto mlvl = mobbaseskillhelpers::CapMagicEvasionLevel(PMob->GetMLevel());
    const auto rank = mobbaseskillhelpers::MagicEvasionCapRank(PMob->objtype == TYPE_TRUST);
    return battleutils::GetMaxSkill(rank, mlvl);
}

/************************************************************************
 *                                                                       *
 *  Base value for defense and evasion                                   *
 *                                                                       *
 ************************************************************************/

uint16 GetBaseDefEva(CMobEntity* PMob, uint8 rank)
{
    // See: https://w.atwiki.jp/studiogobli/pages/25.html
    // Pure f(Lv, rank) (mob_base_capacity.h; slice 1598).
    return mobbasehelpers::GetBaseDefEva(rank, PMob->GetMLevel());
}

/************************************************************************
 *                                                                       *
 *  Base value for stat calculations                                     *
 *                                                                       *
 ************************************************************************/

uint16 GetBaseToRank(uint8 rank, uint16 lvl)
{
    // Pure shared base-to-rank (base_to_rank_capacity.h; slice 1594).
    return basetorankhelpers::GetBaseToRank(rank, lvl);
}

/************************************************************************
 *                                                                       *
 *  Calculation for subjob stats                                         *
 *                                                                       *
 ************************************************************************/
uint16 GetSubJobStats(uint8 rank, uint16 level, uint16 stat)
{
    // Pure subjob stat scaling (sub_job_stats_capacity.h; slice 1597).
    return subjobstatshelpers::GetSubJobStats(rank, level, stat);
}

/************************************************************************
 *                                                                       *
 *  Checks if the mob is in any Original/RoZ zone                        *
 *                                                                       *
 ************************************************************************/
bool CheckSubJobZone(CMobEntity* PMob)
{
    // Pure Original/RoZ zone membership (mob_base_capacity.h; slice 1598).
    return mobbasehelpers::IsSubJobZone(static_cast<uint16>(PMob->getZone()));
}

/************************************************************************
 *                                                                       *
 *  Calculate base mob HP from job grades and levels                     *
 *                                                                       *
 ************************************************************************/
static uint32 CalculateBaseMobHP(uint8 mLvl, uint8 baseHP, uint8 jobScale, uint8 scaleXHP)
{
    // Pure main-job HP ladder (mob_hp_capacity.h; slice 1600).
    return mobhphelpers::CalculateBaseMobHP(mLvl, baseHP, jobScale, scaleXHP);
}

/************************************************************************
 *                                                                       *
 *  Calculate subjob HP contribution                                     *
 *                                                                       *
 ************************************************************************/
static uint32 CalculateSubjobHP(uint8 mLvl, uint8 sjJobScale, uint8 sjScaleXHP)
{
    // Pure subjob HP contribution (mob_hp_capacity.h; slice 1600).
    return mobhphelpers::CalculateSubjobHP(mLvl, sjJobScale, sjScaleXHP);
}

/************************************************************************
 *                                                                       *
 *  Calculate mob stats                                                  *
 *                                                                       *
 ************************************************************************/
void CalculateMobStats(CMobEntity* PMob, bool recover)
{
    // Reset modifiers to base values to prevent stacking
    PMob->restoreModifiers();
    PMob->restoreMobModifiers();

    if (recover)
    {
        // Clear status effects only when fully recovering
        PMob->StatusEffectContainer->KillAllStatusEffect();
    }

    bool      isNM     = PMob->m_Type & MOBTYPE_NOTORIOUS;
    JOBTYPE   mJob     = PMob->GetMJob();
    JOBTYPE   sJob     = PMob->GetSJob();
    uint8     mLvl     = PMob->GetMLevel();
    uint8     sLvl     = PMob->GetSLevel();
    ZONE_TYPE zoneType = PMob->loc.zone->GetTypeMask();

    uint8 mJobGrade = 0; // main jobs grade
    uint8 sJobGrade = 0; // subjobs grade

    if (recover == true)
    {
        if (PMob->HPmodifier == 0)
        {
            // HP Calculations
            mJobGrade = grade::GetJobGrade(mJob, 0);
            sJobGrade = grade::GetJobGrade(sJob, 0);

            // 1. Retrieve HP scaling values from job grades
            // Index 0: Base HP
            // Index 1: Job scaling
            // Index 2: Modifier scale
            uint8 BaseHP     = grade::GetMobHPScale(mJobGrade, 0);
            uint8 JobScale   = grade::GetMobHPScale(mJobGrade, 1);
            uint8 ScaleXHP   = grade::GetMobHPScale(mJobGrade, 2);
            uint8 sjJobScale = grade::GetMobHPScale(sJobGrade, 1);
            uint8 sjScaleXHP = grade::GetMobHPScale(sJobGrade, 2);

            // 2–5. Pure base+sub HP combine and optional pet scale (mob_stats_product_capacity.h; slice 1620).
            const uint32 baseMobHP = CalculateBaseMobHP(mLvl, BaseHP, JobScale, ScaleXHP);
            const uint32 sjHP      = CalculateSubjobHP(mLvl, sjJobScale, sjScaleXHP);
            const uint32 mobHP     = mobstatsproducthelpers::CombineMobHP(baseMobHP, sjHP, PMob->PMaster != nullptr);
            PMob->health.maxhp     = static_cast<int16>(mobHP);
        }
        else
        {
            PMob->health.maxhp = PMob->HPmodifier;
        }

        // Apply NM/Mob HP multiplier from settings (pure clamp+product; slice 1620).
        {
            const auto key = isNM ? "map.NM_HP_MULTIPLIER" : "map.MOB_HP_MULTIPLIER";
            PMob->health.maxhp = mobstatsproducthelpers::ApplyHPMultiplier(PMob->health.maxhp, settings::get<float>(key));
        }

        // MP Calculations (pure job gate + formula; slice 1620).
        bool hasMp = mobstatsproducthelpers::JobHasMP(static_cast<uint8>(mJob), static_cast<uint8>(sJob));
        if (PMob->getMobMod(MOBMOD_MP_BASE))
        {
            hasMp = true;
        }

        if (hasMp)
        {
            const auto scale = mobstatsproducthelpers::ResolveMPScale(PMob->MPscale, static_cast<int16>(PMob->getMobMod(MOBMOD_MP_BASE)));

            if (PMob->MPmodifier == 0)
            {
                PMob->health.maxmp = mobstatsproducthelpers::CalculateMobMaxMP(mLvl, scale);
            }
            else
            {
                PMob->health.maxmp = PMob->MPmodifier;
            }

            const auto mpKey = isNM ? "map.NM_MP_MULTIPLIER" : "map.MOB_MP_MULTIPLIER";
            PMob->health.maxmp = mobstatsproducthelpers::ApplyMPMultiplier(PMob->health.maxmp, settings::get<float>(mpKey));
        }
    }

    ((CItemWeapon*)PMob->m_Weapons[SLOT_MAIN])->setDamage(GetBaseWeaponDamage(PMob, SLOT_MAIN));
    ((CItemWeapon*)PMob->m_Weapons[SLOT_RANGED])->setDamage(GetBaseWeaponDamage(PMob, SLOT_RANGED));

    // reduce weapon delay of MNK
    if (PMob->GetMJob() == JOB_MNK)
    {
        ((CItemWeapon*)PMob->m_Weapons[SLOT_MAIN])->resetDelay();
    }

    uint16 fSTR = GetBaseToRank(PMob->strRank, mLvl);
    uint16 fDEX = GetBaseToRank(PMob->dexRank, mLvl);
    uint16 fVIT = GetBaseToRank(PMob->vitRank, mLvl);
    uint16 fAGI = GetBaseToRank(PMob->agiRank, mLvl);
    uint16 fINT = GetBaseToRank(PMob->intRank, mLvl);
    uint16 fMND = GetBaseToRank(PMob->mndRank, mLvl);
    uint16 fCHR = GetBaseToRank(PMob->chrRank, mLvl);

    uint16 mSTR = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 2), mLvl);
    uint16 mDEX = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 3), mLvl);
    uint16 mVIT = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 4), mLvl);
    uint16 mAGI = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 5), mLvl);
    uint16 mINT = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 6), mLvl);
    uint16 mMND = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 7), mLvl);
    uint16 mCHR = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 8), mLvl);

    uint16 sSTR = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 2), sLvl);
    uint16 sDEX = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 3), sLvl);
    uint16 sVIT = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 4), sLvl);
    uint16 sAGI = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 5), sLvl);
    uint16 sINT = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 6), sLvl);
    uint16 sMND = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 7), sLvl);
    uint16 sCHR = GetBaseToRank(grade::GetJobGrade(PMob->GetSJob(), 8), sLvl);

    // Each subjob stat is determined by where the mob is located and what level the mob is.
    // Each rank has their own formula as shown in GetSubJobStats
    // Sub-level 50 monsters implemented in "Chains of Promathia" and onward (i.e. "Wings of the Goddess" as well) will use rank/2 at all levels.
    // Note: Subjob Level will ALWAYS = Main Job Level but we use sLvl so it makes it easier to know what stat we are calculating
    if (CheckSubJobZone(PMob) && (sLvl < 50))
    {
        sSTR = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 2), sLvl, sSTR);
        sDEX = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 3), sLvl, sDEX);
        sVIT = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 4), sLvl, sVIT);
        sAGI = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 5), sLvl, sAGI);
        sINT = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 6), sLvl, sINT);
        sMND = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 7), sLvl, sMND);
        sCHR = GetSubJobStats(grade::GetJobGrade(PMob->GetSJob(), 8), sLvl, sCHR);
    }
    else
    {
        // Pure subjob half for non-CoP path (mob_stats_product_capacity.h; slice 1620).
        sSTR = mobstatsproducthelpers::HalveSubJobStat(sSTR);
        sDEX = mobstatsproducthelpers::HalveSubJobStat(sDEX);
        sAGI = mobstatsproducthelpers::HalveSubJobStat(sAGI);
        sINT = mobstatsproducthelpers::HalveSubJobStat(sINT);
        sMND = mobstatsproducthelpers::HalveSubJobStat(sMND);
        sCHR = mobstatsproducthelpers::HalveSubJobStat(sCHR);
        sVIT = mobstatsproducthelpers::HalveSubJobStat(sVIT);
    }

    // [stat] = floor[family] + floor[main] + floor[sub] then settings mult (slice 1620).
    PMob->stats.STR = mobstatsproducthelpers::SumStat(fSTR, mSTR, sSTR);
    PMob->stats.DEX = mobstatsproducthelpers::SumStat(fDEX, mDEX, sDEX);
    PMob->stats.VIT = mobstatsproducthelpers::SumStat(fVIT, mVIT, sVIT);
    PMob->stats.AGI = mobstatsproducthelpers::SumStat(fAGI, mAGI, sAGI);
    PMob->stats.INT = mobstatsproducthelpers::SumStat(fINT, mINT, sINT);
    PMob->stats.MND = mobstatsproducthelpers::SumStat(fMND, mMND, sMND);
    PMob->stats.CHR = mobstatsproducthelpers::SumStat(fCHR, mCHR, sCHR);

    const auto statKey        = isNM ? "map.NM_STAT_MULTIPLIER" : "map.MOB_STAT_MULTIPLIER";
    const auto statMultiplier = settings::get<float>(statKey);
    PMob->stats.STR           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.STR, statMultiplier);
    PMob->stats.DEX           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.DEX, statMultiplier);
    PMob->stats.VIT           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.VIT, statMultiplier);
    PMob->stats.AGI           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.AGI, statMultiplier);
    PMob->stats.INT           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.INT, statMultiplier);
    PMob->stats.MND           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.MND, statMultiplier);
    PMob->stats.CHR           = mobstatsproducthelpers::ApplyStatMultiplier(PMob->stats.CHR, statMultiplier);

    // special case, give spell list to my pet
    if (PMob->getMobMod(MOBMOD_PET_SPELL_LIST) && PMob->PPet != nullptr)
    {
        // Stubborn_Dredvodd
        CMobEntity* PPet = (CMobEntity*)PMob->PPet;

        // give pet spell list
        PPet->m_SpellListContainer = mobSpellList::GetMobSpellList(PMob->getMobMod(MOBMOD_PET_SPELL_LIST));
    }

    if (PMob->getMobMod(MOBMOD_SPELL_LIST))
    {
        PMob->m_SpellListContainer = mobSpellList::GetMobSpellList(PMob->getMobMod(MOBMOD_SPELL_LIST));
    }

    // cap all stats for mLvl / job (level clamp pure; slice 1622)
    const auto skillLvl = mobsetuphelpers::CapSkillLevel(mLvl);
    for (int i = SKILL_DIVINE_MAGIC; i <= SKILL_BLUE_MAGIC; i++)
    {
        uint16 maxSkill = battleutils::GetMaxSkill((SKILLTYPE)i, PMob->GetMJob(), skillLvl);
        if (maxSkill != 0)
        {
            PMob->WorkingSkills.skill[i] = maxSkill;
        }
        else // if the mob is WAR/BLM and can cast spell
        {
            // set skill as high as main level, so their spells won't get resisted
            uint16 maxSubSkill = battleutils::GetMaxSkill((SKILLTYPE)i, PMob->GetSJob(), skillLvl);

            if (maxSubSkill != 0)
            {
                PMob->WorkingSkills.skill[i] = maxSubSkill;
            }
        }
    }
    for (int i = SKILL_HAND_TO_HAND; i <= SKILL_STAFF; i++)
    {
        uint16 maxSkill = battleutils::GetMaxSkill(3, skillLvl);
        if (maxSkill != 0)
        {
            PMob->WorkingSkills.skill[i] = maxSkill;
        }
    }

    PMob->addModifier(Mod::DEF, GetBaseDefEva(PMob, PMob->defRank));                         // Base Defense for all mobs
    PMob->addModifier(Mod::EVA, GetBaseDefEva(PMob, JobSkillRankToBaseEvaRank(mJob, sJob))); // Evasion is based off the highest job rank. // TODO: add family bonuses (colibri has static evasion+ porrogos have % boost.)
    PMob->addModifier(Mod::ATT, GetBaseSkill(PMob, PMob->attRank));                          // Base Attack for all mobs is Rank A+ but pull from DB for specific cases
    PMob->addModifier(Mod::ACC, GetBaseSkill(PMob, PMob->accRank));                          // Base Accuracy for all mobs is Rank A+ but pull from DB for specific cases
    PMob->addModifier(Mod::RATT, GetBaseSkill(PMob, PMob->attRank));                         // Base Ranged Attack for all mobs is Rank A+ but pull from DB for specific cases
    PMob->addModifier(Mod::RACC, GetBaseSkill(PMob, PMob->accRank));                         // Base Ranged Accuracy for all mobs is Rank A+ but pull from DB for specific cases

    // Parry/guard skill assignment (pure gates; slice 1623).
    // MOBMOD_CAN_PARRY uses the mod value as the rank (Dynamis beastmen, etc.).
    if (mobsetuphelpers::ShouldAssignParrySkill(PMob->getMobMod(MOBMOD_CAN_PARRY)))
    {
        PMob->WorkingSkills.skill[SKILL_PARRY] = GetBaseSkill(PMob, static_cast<uint8>(PMob->getMobMod(MOBMOD_CAN_PARRY)));
    }
    // Assume base guard for MNK and PUP mobs is the same as parry (Rank C).
    if (mobsetuphelpers::ShouldAssignGuardSkill(static_cast<uint8>(PMob->GetMJob()), PMob->getMobMod(MOBMOD_CANNOT_GUARD)))
    {
        PMob->WorkingSkills.skill[SKILL_GUARD] = GetBaseSkill(PMob, mobsetuphelpers::GuardSkillRank);
    }

    // natural magic evasion
    PMob->addModifier(Mod::MEVA, GetMagicEvasion(PMob));

    // add traits for sub and main
    battleutils::AddTraits(PMob, traits::GetTraits(mJob), mLvl);
    battleutils::AddTraits(PMob, traits::GetTraits(PMob->GetSJob()), PMob->GetSLevel());

    // Max [HP/MP] Boost traits
    PMob->UpdateHealth();

    if (recover)
    {
        PMob->health.tp = 0;
        PMob->health.hp = PMob->GetMaxHP();
        PMob->health.mp = PMob->GetMaxMP();
    }

    SetupJob(PMob);

    // If a mob is going to dual wield, then it needs to have a sub slot.
    // Assume it is the same damage as the main slot.
    // Ordering matters. This has to come after SetupJob (slice 1622 pure product).
    static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_SUB])
        ->setDamage(mobsetuphelpers::DualWieldSubDamage(PMob->IsDualWielding(), GetBaseWeaponDamage(PMob, SLOT_MAIN)));

    SetupRoaming(PMob);

    // All beastmen drop gil (pure default entry; slice 1622).
    if (PMob->m_EcoSystem == xi::Ecosystem::Beastmen)
    {
        const auto gil = mobsetuphelpers::BeastmenGilBonusDefault();
        PMob->defaultMobMod(static_cast<MOBMODIFIER>(gil.mod), gil.value);
    }

    if (PMob->PMaster != nullptr)
    {
        SetupPetSkills(PMob);
    }

    PMob->m_Behavior |= PMob->getMobMod(MOBMOD_BEHAVIOR);

    if (PMob->m_Type & MOBTYPE_BATTLEFIELD)
    {
        SetupBattlefieldMob(PMob);
    }

    if (PMob->m_Type & MOBTYPE_NOTORIOUS)
    {
        const auto nm = mobsetuphelpers::NotoriousNoDespawn();
        PMob->setMobMod(static_cast<MOBMODIFIER>(nm.mod), nm.value);
    }

    if (zoneType & ZONE_TYPE::INSTANCED)
    {
        SetupDungeonInstanceMob(PMob);
    }

    if (PMob->m_Type & MOBTYPE_EVENT)
    {
        SetupEventMob(PMob);
    }

    if (PMob->CanStealGil())
    {
        PMob->ResetGilPurse();
    }

    // Check for possible miss-setups
    if (PMob->getMobMod(MOBMOD_SPECIAL_SKILL) != 0 && PMob->getMobMod(MOBMOD_SPECIAL_COOL) == 0)
    {
        ShowError("mobutils::CalculateMobStats Mob (%s, %d) with special skill but no cool down set!", PMob->getName(), PMob->id);
    }

    if (PMob->SpellContainer->HasSpells() && PMob->getMobMod(MOBMOD_MAGIC_COOL) == 0)
    {
        ShowError("mobutils::CalculateMobStats Mob (%s, %d) with magic but no cool down set!", PMob->getName(), PMob->id);
    }

    if (PMob->getMobMod(MOBMOD_DETECTION) == 0)
    {
        ShowError("mobutils::CalculateMobStats Mob (%s, %d, %d) has no detection methods!", PMob->getName(), PMob->id, PMob->m_Species);
    }
}

void SetupRangedAttack(CMobEntity* PMob)
{
    // Pure ranged defaults (mob_setup_capacity.h; slice 1623).
    const auto plan = mobsetuphelpers::PlanSetupRangedAttack();
    for (const auto& entry : plan.mods)
    {
        PMob->defaultMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
    }
    PMob->PAI->GetController()->SetRangedAttackEnabled(true);
    static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_RANGED])->setBaseDelay(plan.rangedBaseDelay);
}

void SetupJob(CMobEntity* PMob)
{
    // Pure job/family mod plan (mob_setup_capacity.h; slice 1621).
    const auto mJob           = static_cast<uint8>(PMob->GetMJob());
    const auto sJob           = static_cast<uint8>(PMob->GetSJob());
    const auto mainJobMPGrade = grade::GetJobGrade(PMob->GetMJob(), 1);
    const auto plan           = mobsetuphelpers::BuildSetupJobPlan(
        mJob, sJob, mainJobMPGrade, PMob->m_Family, PMob->m_EcoSystem == xi::Ecosystem::Beastmen);

    for (const auto& entry : plan.mods)
    {
        if (entry.kind == mobsetuphelpers::MobModApplyKind::Force)
        {
            PMob->setMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
        }
        else
        {
            PMob->defaultMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
        }
    }
    if (plan.callSetupRangedAttack)
    {
        SetupRangedAttack(PMob);
    }
}

void SetupRoaming(CMobEntity* PMob)
{
    // Pure roaming plan (mob_setup_capacity.h; slice 1621).
    const auto plan = mobsetuphelpers::PlanSetupRoaming(
        PMob->m_EcoSystem == xi::Ecosystem::Beastmen, static_cast<uint16>(PMob->m_roamFlags));
    for (const auto& entry : plan.mods)
    {
        if (entry.kind == mobsetuphelpers::MobModApplyKind::Force)
        {
            PMob->setMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
        }
        else
        {
            PMob->defaultMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
        }
    }
    if (plan.specialFlagsOR != 0)
    {
        PMob->m_specialFlags |= plan.specialFlagsOR;
    }
    if (plan.setMaxRoam)
    {
        PMob->m_maxRoamDistance = plan.maxRoamDistance;
    }
}

void SetupPetSkills(CMobEntity* PMob)
{
    // Pure species → skill list (mob_setup_capacity.h; slice 1621).
    const auto skillListId = mobsetuphelpers::PetSkillListID(PMob->m_Species);
    if (skillListId != 0)
    {
        PMob->setMobMod(MOBMOD_SKILL_LIST, skillListId);
    }
}

uint8 JobSkillRankToBaseEvaRank(JOBTYPE mjob, JOBTYPE sjob)
{
    // Pure skill-rank → base eva rank map (mob_base_skill_capacity.h; slice 1599).
    const uint8 mainEvasionSkillRank = battleutils::GetSkillRank(SKILL_EVASION, mjob);
    const uint8 subEvasionSkillRank  = battleutils::GetSkillRank(SKILL_EVASION, sjob);
    const auto  best                 = mobbaseskillhelpers::BestEvasionSkillRank(mainEvasionSkillRank, subEvasionSkillRank, sjob == JOB_NON);
    const auto  mapped               = mobbaseskillhelpers::JobSkillRankToBaseEvaRank(best);
    if (mapped == 0)
    {
        ShowError("JobSkillRankToBaseEvaRank: rank not implemented. Job SKILL_EVASION rank is likely not valid or no longer exists (A- rank in particular.)");
        return 3; // Give them C rank as a fallback.
    }
    return mapped;
};

void SetupBattlefieldMob(CMobEntity* PMob)
{
    // Pure battlefield plan (mob_setup_capacity.h; slice 1622).
    const auto plan = mobsetuphelpers::PlanSetupBattlefield(
        PMob->PBattlefield == nullptr, PMob->m_bcnmID, static_cast<int16>(PMob->m_battlefieldID));
    for (const auto& entry : plan.alwaysMods)
    {
        PMob->setMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
    }
    PMob->SetDespawnTime(0s);
    if (PMob->PBattlefield != nullptr)
    {
        return;
    }
    for (const auto& entry : plan.notInBFMods)
    {
        PMob->setMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
    }
    if (plan.setMaxRoam)
    {
        PMob->m_maxRoamDistance = plan.maxRoamDistance;
    }
}

void SetupEventMob(CMobEntity* PMob)
{
    // Pure event plan (mob_setup_capacity.h; slice 1622).
    const auto plan = mobsetuphelpers::PlanSetupEvent();
    PMob->m_roamFlags |= plan.roamFlagsOR;
    for (const auto& entry : plan.mods)
    {
        PMob->setMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
    }
    if (plan.setMaxRoam)
    {
        PMob->m_maxRoamDistance = plan.maxRoamDistance;
    }
}

void SetupDungeonInstanceMob(CMobEntity* PMob)
{
    // Pure dungeon plan (mob_setup_capacity.h; slice 1622).
    const auto plan = mobsetuphelpers::PlanSetupDungeon(static_cast<uint16>(PMob->getZone()));
    for (const auto& entry : plan.mods)
    {
        PMob->setMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
    }
    if (plan.resetPosToSpawn)
    {
        PMob->loc.p = PMob->m_SpawnPoint;
    }
    if (plan.clearDespawnTime)
    {
        PMob->SetDespawnTime(0s);
    }
}

void RecalculateSpellContainer(CMobEntity* PMob)
{
    // clear spell list
    PMob->SpellContainer->ClearSpells();

    // insert the rest of the spells
    for (const auto spellId : PMob->m_SpellListContainer->GetEligibleSpells(PMob->GetMLevel()))
    {
        PMob->SpellContainer->AddSpell(spellId);
    }
}

/* Gets the available spells for the specified monster.
 */
void GetAvailableSpells(CMobEntity* PMob)
{
    // make sure the mob actually has a spell list
    if (PMob->m_SpellListContainer == nullptr)
    {
        return;
    }

    // catch all non-defaulted spell chances (pure defaults; slice 1622)
    for (const auto& entry : mobsetuphelpers::AvailableSpellsDefaultMods())
    {
        PMob->defaultMobMod(static_cast<MOBMODIFIER>(entry.mod), entry.value);
    }

    RecalculateSpellContainer(PMob);

    // make sure mob has mp to cast spells
    if (PMob->health.maxmp == 0 && PMob->SpellContainer != nullptr && PMob->SpellContainer->HasMPSpells())
    {
        ShowError("mobutils::GetAvailableSpells Mob (%u) has no mp for casting spells!", PMob->id);
    }
}

void SetSpellList(CMobEntity* PMob, uint16 spellList)
{
    PMob->m_SpellListContainer = mobSpellList::GetMobSpellList(spellList);
    RecalculateSpellContainer(PMob);
}

// Go host pure half: mobutils.ApplyInitializeMob / PlanInitializeMobDefaultMods (6408).
void InitializeMob(CMobEntity* PMob)
{
    // add special mob mods
    PMob->defaultMobMod(MOBMOD_SKILL_LIST, PMob->m_MobSkillList);
    PMob->defaultMobMod(MOBMOD_LINK_RADIUS, 10);
    PMob->defaultMobMod(MOBMOD_SIGHT_RANGE, (int16)CMobEntity::sight_range);
    PMob->defaultMobMod(MOBMOD_SOUND_RANGE, (int16)CMobEntity::sound_range);
    PMob->defaultMobMod(MOBMOD_MAGIC_RANGE, (int16)CMobEntity::magic_range);

    battleutils::addEcosystemKillerEffects(PMob);

    if (PMob->m_maxLevel == 0 && PMob->m_minLevel == 0)
    {
        if (PMob->getZone() >= 1 && PMob->getZone() <= 252)
        {
            ShowError("Mob %s level is 0! zoneid %d, poolid %d", PMob->getName(), PMob->getZone(), PMob->m_Pool);
        }
    }
}

/*
Loads up mob mods from mob_pool_mods and mob_species_mods table. This will allow you to change
a mobs regen rate, magic defense, triple attack rate from a table instead of hardcoding it.

Usage:

    Evil weapons have a magic defense boost. So pop that into mob_species_mods table.
    Goblin Diggers have a vermin killer trait, so find its poolid and put it in mod_pool_mods table.
*/
// Go host pure half: mobutils.ApplyLoadSqlModifiers / SqlModifiersStore (6409).
void LoadSqlModifiers()
{
    // load family mods
    auto rset = db::preparedStmt("SELECT speciesid, modid, value, is_mob_mod "
                                 "FROM mob_species_mods");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        ModsList_t* speciesMods = GetMobSpeciesMods(rset->get<uint16>("speciesid"), true);

        auto* mod = new CModifier(rset->get<Mod>("modid"));
        mod->setModAmount(rset->get<int16>("value"));

        if (rset->get<bool>("is_mob_mod"))
        {
            speciesMods->mobMods.emplace_back(mod);
        }
        else
        {
            speciesMods->mods.emplace_back(mod);
        }
    }

    // load pool mods
    rset = db::preparedStmt("SELECT poolid, modid, value, is_mob_mod "
                            "FROM mob_pool_mods");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        const auto  pool     = rset->get<uint16>("poolid");
        ModsList_t* poolMods = GetMobPoolMods(pool, true);

        const auto id = rset->get<Mod>("modid");

        auto* mod = new CModifier(id);
        mod->setModAmount(rset->get<int16>("value"));

        if (rset->get<bool>("is_mob_mod"))
        {
            poolMods->mobMods.emplace_back(mod);
        }
        else
        {
            poolMods->mods.emplace_back(mod);
        }
    }
}

void Cleanup()
{
    // Manually delete and clean up pointers
    for (auto spawnMod : mobSpawnModsList)
    {
        if (spawnMod.second)
        {
            for (auto mobMods : spawnMod.second->mobMods)
            {
                destroy(mobMods);
            }

            for (auto mods : spawnMod.second->mods)
            {
                destroy(mods);
            }
            destroy(spawnMod.second);
        }
    }
    mobSpawnModsList.clear();

    for (auto mobSpeciesMods : mobSpeciesModsList)
    {
        if (mobSpeciesMods.second)
        {
            for (auto mobMods : mobSpeciesMods.second->mobMods)
            {
                destroy(mobMods);
            }

            for (auto mods : mobSpeciesMods.second->mods)
            {
                destroy(mods);
            }

            destroy(mobSpeciesMods.second);
        }
    }
    mobSpeciesModsList.clear();

    for (auto mobPoolMods : mobPoolModsList)
    {
        if (mobPoolMods.second)
        {
            for (auto mobMods : mobPoolMods.second->mobMods)
            {
                destroy(mobMods);
            }

            for (auto mods : mobPoolMods.second->mods)
            {
                destroy(mods);
            }
            destroy(mobPoolMods.second);
        }
    }
    mobPoolModsList.clear();
}

ModsList_t* GetMobSpeciesMods(uint16 speciesId, bool create)
{
    if (mobSpeciesModsList[speciesId])
    {
        return mobSpeciesModsList[speciesId];
    }

    if (create)
    {
        // create new one
        ModsList_t* mods = new ModsList_t;
        mods->id         = speciesId;

        mobSpeciesModsList[speciesId] = mods;

        return mods;
    }

    return nullptr;
}

ModsList_t* GetMobPoolMods(uint32 poolId, bool create)
{
    if (mobPoolModsList[poolId])
    {
        return mobPoolModsList[poolId];
    }

    if (create)
    {
        // create new one
        ModsList_t* mods = new ModsList_t;
        mods->id         = poolId;

        mobPoolModsList[poolId] = mods;

        return mods;
    }

    return nullptr;
}

ModsList_t* GetMobSpawnMods(uint32 mobId, bool create)
{
    if (mobSpawnModsList[mobId])
    {
        return mobSpawnModsList[mobId];
    }

    if (create)
    {
        // create new one
        ModsList_t* mods = new ModsList_t;
        mods->id         = mobId;

        mobSpawnModsList[mobId] = mods;

        return mods;
    }

    return nullptr;
}

// Go host pure half: mobutils.ApplyAddSqlModifiers (slice 6409).
void AddSqlModifiers(CMobEntity* PMob)
{
    // find my species mods
    ModsList_t* PSpeciesMods = GetMobSpeciesMods(PMob->m_Species);

    if (PSpeciesMods != nullptr)
    {
        // add them
        for (auto& mod : PSpeciesMods->mods)
        {
            PMob->addModifier(mod->getModID(), mod->getModAmount());
        }
        // TODO: don't store mobmods in a CModifier
        for (auto& mobMod : PSpeciesMods->mobMods)
        {
            PMob->setMobMod(static_cast<uint16>(mobMod->getModID()), mobMod->getModAmount());
        }
    }

    // find my pools mods
    ModsList_t* PPoolMods = GetMobPoolMods(PMob->m_Pool);

    if (PPoolMods != nullptr)
    {
        // add them
        for (auto& mod : PPoolMods->mods)
        {
            PMob->addModifier(mod->getModID(), mod->getModAmount());
        }

        for (auto& mobMod : PPoolMods->mobMods)
        {
            PMob->setMobMod(static_cast<uint16>(mobMod->getModID()), mobMod->getModAmount());
        }
    }

    // find my IDs mods
    ModsList_t* PSpawnMods = GetMobSpawnMods(PMob->id);

    if (PSpawnMods != nullptr)
    {
        // add them
        for (auto& mod : PSpawnMods->mods)
        {
            PMob->addModifier(mod->getModID(), mod->getModAmount());
        }

        for (auto& mobMod : PSpawnMods->mobMods)
        {
            PMob->setMobMod(static_cast<uint16>(mobMod->getModID()), mobMod->getModAmount());
        }
    }
}

auto InstantiateAlly(uint32 groupid, uint16 zoneID, CInstance* instance) -> CMobEntity*
{
    CMobEntity* PMob = nullptr;

    const auto rset = db::preparedStmt("SELECT zoneid, mob_groups.name, packet_name, respawntime, "
                                       "spawntype, dropid, mob_groups.HP, mob_groups.MP, "
                                       "mob_spawn_points.minLevel, mob_spawn_points.maxLevel, modelid, mJob, "
                                       "sJob, cmbSkill, cmbDmgMult, cmbDelay, "
                                       "behavior, links, mobType, immunity, "
                                       "ecosystemID, speed, STR, "
                                       "DEX, VIT, AGI, `INT`, "
                                       "MND, CHR, EVA, DEF, "
                                       "ATT, ACC, slash_sdt, pierce_sdt, "
                                       "h2h_sdt, impact_sdt, magical_sdt, "
                                       "fire_sdt, ice_sdt, wind_sdt, earth_sdt, lightning_sdt, water_sdt, light_sdt, dark_sdt, "
                                       "fire_res_rank, ice_res_rank, wind_res_rank, earth_res_rank, lightning_res_rank, water_res_rank, light_res_rank, dark_res_rank, "
                                       "paralyze_res_rank, bind_res_rank, silence_res_rank, slow_res_rank, poison_res_rank, light_sleep_res_rank, dark_sleep_res_rank, blind_res_rank, "
                                       "Element, "
                                       "mob_pools.speciesid, name_prefix, entityFlags, animationsub, "
                                       "(mob_species_system.HP / 100) AS hp_scale, (mob_species_system.MP / 100) AS mp_scale, hasSpellScript, spellList, "
                                       "mob_groups.poolid, allegiance, namevis, aggro, "
                                       "mob_pools.skill_list_id, mob_pools.true_detection, mob_species_system.detects, "
                                       "mob_pools.modelSize, mob_pools.modelHitboxSize "
                                       "FROM mob_groups INNER JOIN mob_spawn_points ON mob_groups.groupid = mob_spawn_points.groupid "
                                       "INNER JOIN mob_pools ON mob_groups.poolid = mob_pools.poolid "
                                       "INNER JOIN mob_resistances ON mob_pools.resist_id = mob_resistances.resist_id "
                                       "INNER JOIN mob_species_system ON mob_pools.speciesid = mob_species_system.speciesID "
                                       "WHERE mob_groups.groupid = ? AND mob_groups.zoneid = ?",
                                       groupid,
                                       zoneID);
    FOR_DB_SINGLE_RESULT(rset)
    {
        PMob            = new CMobEntity();
        PMob->PInstance = instance;

        PMob->name.insert(0, rset->get<std::string>("name"));
        PMob->packetName.insert(0, rset->get<std::string>("packet_name"));

        PMob->m_RespawnTime = std::chrono::seconds(rset->get<uint32>("respawntime"));
        PMob->m_SpawnType   = rset->get<SPAWNTYPE>("spawntype");
        PMob->m_DropID      = rset->get<uint32>("dropid");

        PMob->HPmodifier = rset->get<uint32>("HP");
        PMob->MPmodifier = rset->get<uint32>("MP");

        PMob->m_minLevel = rset->get<uint8>("minLevel");
        PMob->m_maxLevel = rset->get<uint8>("maxLevel");

        uint16 sqlModelID[10];
        db::extractFromBlob(rset, "modelid", sqlModelID);
        PMob->look = look_t(sqlModelID);

        PMob->SetMJob(rset->get<uint8>("mJob"));
        PMob->SetSJob(rset->get<uint8>("sJob"));

        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setMaxHit(1);
        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setSkillType(rset->get<uint8>("cmbSkill"));
        PMob->m_dmgMult = rset->get<uint16>("cmbDmgMult");
        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setDelay(rset->get<uint16>("cmbDelay"));
        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setBaseDelay(rset->get<uint16>("cmbDelay"));

        PMob->m_Behavior  = rset->get<uint16>("behavior");
        PMob->m_Link      = rset->get<uint8>("links");
        PMob->m_Type      = rset->get<uint8>("mobType");
        PMob->m_Immunity  = rset->get<IMMUNITY>("immunity");
        PMob->m_EcoSystem = rset->get<xi::Ecosystem>("ecosystemID");

        PMob->baseSpeed      = rset->get<uint8>("speed"); // Overwrites baseentity.cpp's defined baseSpeed
        PMob->animationSpeed = rset->get<uint8>("speed"); // Overwrites baseentity.cpp's defined animationSpeed
        PMob->UpdateSpeed();

        PMob->strRank = rset->get<uint8>("STR");
        PMob->dexRank = rset->get<uint8>("DEX");
        PMob->vitRank = rset->get<uint8>("VIT");
        PMob->agiRank = rset->get<uint8>("AGI");
        PMob->intRank = rset->get<uint8>("INT");
        PMob->mndRank = rset->get<uint8>("MND");
        PMob->chrRank = rset->get<uint8>("CHR");
        PMob->evaRank = rset->get<uint8>("EVA");
        PMob->defRank = rset->get<uint8>("DEF");
        PMob->attRank = rset->get<uint8>("ATT");
        PMob->accRank = rset->get<uint8>("ACC");

        PMob->setModifier(Mod::SLASH_SDT, rset->get<int16>("slash_sdt"));
        PMob->setModifier(Mod::PIERCE_SDT, rset->get<int16>("pierce_sdt"));
        PMob->setModifier(Mod::HTH_SDT, rset->get<int16>("h2h_sdt"));
        PMob->setModifier(Mod::IMPACT_SDT, rset->get<int16>("impact_sdt"));

        PMob->setModifier(Mod::UDMGMAGIC, rset->get<int16>("magical_sdt")); // Modifier 389, base 10000 stored as signed integer. Positives signify less damage.

        PMob->setModifier(Mod::FIRE_SDT, rset->get<int16>("fire_sdt"));         // Modifier 54, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::ICE_SDT, rset->get<int16>("ice_sdt"));           // Modifier 55, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::WIND_SDT, rset->get<int16>("wind_sdt"));         // Modifier 56, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::EARTH_SDT, rset->get<int16>("earth_sdt"));       // Modifier 57, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::THUNDER_SDT, rset->get<int16>("lightning_sdt")); // Modifier 58, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::WATER_SDT, rset->get<int16>("water_sdt"));       // Modifier 59, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::LIGHT_SDT, rset->get<int16>("light_sdt"));       // Modifier 60, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::DARK_SDT, rset->get<int16>("dark_sdt"));         // Modifier 61, base 10000 stored as signed integer. Positives signify less damage.

        PMob->setModifier(Mod::FIRE_RES_RANK, rset->get<int8>("fire_res_rank"));
        PMob->setModifier(Mod::ICE_RES_RANK, rset->get<int8>("ice_res_rank"));
        PMob->setModifier(Mod::WIND_RES_RANK, rset->get<int8>("wind_res_rank"));
        PMob->setModifier(Mod::EARTH_RES_RANK, rset->get<int8>("earth_res_rank"));
        PMob->setModifier(Mod::THUNDER_RES_RANK, rset->get<int8>("lightning_res_rank"));
        PMob->setModifier(Mod::WATER_RES_RANK, rset->get<int8>("water_res_rank"));
        PMob->setModifier(Mod::LIGHT_RES_RANK, rset->get<int8>("light_res_rank"));
        PMob->setModifier(Mod::DARK_RES_RANK, rset->get<int8>("dark_res_rank"));

        PMob->setModifier(Mod::PARALYZE_RES_RANK, rset->get<int8>("paralyze_res_rank"));
        PMob->setModifier(Mod::BIND_RES_RANK, rset->get<int8>("bind_res_rank"));
        PMob->setModifier(Mod::SILENCE_RES_RANK, rset->get<int8>("silence_res_rank"));
        PMob->setModifier(Mod::SLOW_RES_RANK, rset->get<int8>("slow_res_rank"));
        PMob->setModifier(Mod::POISON_RES_RANK, rset->get<int8>("poison_res_rank"));
        PMob->setModifier(Mod::LIGHT_SLEEP_RES_RANK, rset->get<int8>("light_sleep_res_rank"));
        PMob->setModifier(Mod::DARK_SLEEP_RES_RANK, rset->get<int8>("dark_sleep_res_rank"));
        PMob->setModifier(Mod::BLIND_RES_RANK, rset->get<int8>("blind_res_rank"));

        PMob->m_Element     = rset->get<uint8>("Element");
        PMob->m_Species     = rset->get<uint16>("speciesid");
        PMob->m_name_prefix = rset->get<uint8>("name_prefix");
        PMob->m_flags       = rset->get<uint32>("entityFlags");

        // Special sub animation for Mob (yovra, jailer of love, phuabo)
        // yovra 1: On top/in the sky, 2: , 3: On top/in the sky
        // phuabo 1: Underwater, 2: Out of the water, 3: Goes back underwater
        PMob->animationsub = rset->get<uint32>("animationsub");

        // Setup HP / MP Stat Percentage Boost
        PMob->HPscale = rset->get<float>("hp_scale");
        PMob->MPscale = rset->get<float>("mp_scale");

        PMob->m_SpellListContainer = mobSpellList::GetMobSpellList(rset->get<uint16>("spellList"));

        PMob->m_Pool = rset->get<uint32>("poolid");

        PMob->allegiance      = rset->get<ALLEGIANCE_TYPE>("allegiance");
        PMob->namevis         = rset->get<uint8>("namevis");
        PMob->modelHitboxSize = std::max<float>(0.0f, rset->getOrDefault<float>("modelHitboxSize", 0) / 10.f);
        PMob->modelSize       = rset->getOrDefault<uint8>("modelSize", 0);
        PMob->m_Aggro         = rset->get<bool>("aggro");
        PMob->m_MobSkillList  = rset->get<uint16>("skill_list_id");
        PMob->m_TrueDetection = rset->get<bool>("true_detection");
        PMob->setMobMod(MOBMOD_DETECTION, rset->get<int16>("detects"));

        if (instance)
        {
            instance->AssignDynamicTargIDandLongID(PMob);
            instance->InsertMOB(PMob);
        }
        else if (CZone* PZone = zoneutils::GetZone(zoneID))
        {
            PZone->GetZoneEntities()->AssignDynamicTargIDandLongID(PMob);
            PZone->GetZoneEntities()->InsertMOB(PMob);
        }
        else
        {
            ShowError("Mobutils::InstantiateAlly failed to get zone from zoneutils::GetZone(zoneID)");
        }

        // Ensure dynamic targid is released on death
        PMob->m_bReleaseTargIDOnDisappear = true;

        // must be here first to define mobmods
        mobutils::InitializeMob(PMob);

        luautils::OnEntityLoad(PMob);

        luautils::OnMobInitialize(PMob);
        if (CZone* PZone = zoneutils::GetZone(zoneID))
        {
            PZone->FindPartyForMob(PMob);
        }
        luautils::ApplyMixins(PMob);
        luautils::ApplyZoneMixins(PMob);

        PMob->saveModifiers();
        PMob->saveMobModifiers();
    }

    return PMob;
}

auto InstantiateDynamicMob(uint32 groupid, uint16 groupZoneId, uint16 targetZoneId) -> CMobEntity*
{
    auto* PMob = new CMobEntity();

    const auto rset = db::preparedStmt("SELECT zoneid, mob_groups.name, packet_name, respawntime, "
                                       "spawntype, dropid, mob_groups.HP, mob_groups.MP, "
                                       "modelid, mJob, "
                                       "sJob, cmbSkill, cmbDmgMult, cmbDelay, "
                                       "behavior, links, mobType, immunity, "
                                       "ecosystemID, speed, STR, "
                                       "DEX, VIT, AGI, `INT`, "
                                       "MND, CHR, EVA, DEF, "
                                       "ATT, ACC, slash_sdt, pierce_sdt, "
                                       "h2h_sdt, impact_sdt, magical_sdt, fire_sdt, "
                                       "ice_sdt, wind_sdt, earth_sdt, lightning_sdt, "
                                       "water_sdt, light_sdt, dark_sdt, fire_res_rank, "
                                       "ice_res_rank, wind_res_rank, earth_res_rank, lightning_res_rank, "
                                       "water_res_rank, light_res_rank, dark_res_rank, Element, "
                                       "mob_pools.speciesid, name_prefix, entityFlags, animationsub, "
                                       "(mob_species_system.HP / 100) AS hp_scale, (mob_species_system.MP / 100) AS mp_scale, hasSpellScript, spellList, "
                                       "mob_groups.poolid, allegiance, namevis, aggro, "
                                       "mob_pools.modelSize, mob_pools.modelHitboxSize, "
                                       "mob_pools.skill_list_id, mob_pools.true_detection, mob_species_system.detects "
                                       "FROM mob_groups INNER JOIN mob_pools ON mob_groups.poolid = mob_pools.poolid "
                                       "INNER JOIN mob_resistances ON mob_pools.resist_id = mob_resistances.resist_id "
                                       "INNER JOIN mob_species_system ON mob_pools.speciesid = mob_species_system.speciesID "
                                       "WHERE mob_groups.groupid = ? AND mob_groups.zoneid = ?",
                                       groupid,
                                       groupZoneId);
    FOR_DB_SINGLE_RESULT(rset)
    {
        PMob->name.insert(0, rset->get<std::string>("name"));
        PMob->packetName.insert(0, rset->get<std::string>("packet_name"));

        PMob->m_RespawnTime = std::chrono::seconds(rset->get<uint32>("respawntime"));
        PMob->m_SpawnType   = rset->get<SPAWNTYPE>("spawntype");
        PMob->m_DropID      = rset->get<uint32>("dropid");

        PMob->HPmodifier = rset->get<uint32>("HP");
        PMob->MPmodifier = rset->get<uint32>("MP");

        uint16 sqlModelID[10];
        db::extractFromBlob(rset, "modelid", sqlModelID);
        PMob->look = look_t(sqlModelID);

        PMob->SetMJob(rset->get<uint8>("mJob"));
        PMob->SetSJob(rset->get<uint8>("sJob"));

        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setMaxHit(1);
        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setSkillType(rset->get<uint8>("cmbSkill"));
        PMob->m_dmgMult = rset->get<uint16>("cmbDmgMult");
        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setDelay(rset->get<uint16>("cmbDelay"));
        static_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN])->setBaseDelay(rset->get<uint16>("cmbDelay"));

        PMob->m_Behavior  = rset->get<uint16>("behavior");
        PMob->m_Link      = rset->get<uint8>("links");
        PMob->m_Type      = rset->get<uint8>("mobType");
        PMob->m_Immunity  = rset->get<IMMUNITY>("immunity");
        PMob->m_EcoSystem = rset->get<xi::Ecosystem>("ecosystemID");

        PMob->baseSpeed      = rset->get<uint8>("speed"); // Overwrites baseentity.cpp's defined baseSpeed
        PMob->animationSpeed = rset->get<uint8>("speed"); // Overwrites baseentity.cpp's defined animationSpeed
        PMob->UpdateSpeed();

        PMob->strRank = rset->get<uint8>("STR");
        PMob->dexRank = rset->get<uint8>("DEX");
        PMob->vitRank = rset->get<uint8>("VIT");
        PMob->agiRank = rset->get<uint8>("AGI");
        PMob->intRank = rset->get<uint8>("INT");
        PMob->mndRank = rset->get<uint8>("MND");
        PMob->chrRank = rset->get<uint8>("CHR");
        PMob->evaRank = rset->get<uint8>("EVA");
        PMob->defRank = rset->get<uint8>("DEF");
        PMob->attRank = rset->get<uint8>("ATT");
        PMob->accRank = rset->get<uint8>("ACC");

        PMob->setModifier(Mod::SLASH_SDT, rset->get<int16>("slash_sdt"));
        PMob->setModifier(Mod::PIERCE_SDT, rset->get<int16>("pierce_sdt"));
        PMob->setModifier(Mod::HTH_SDT, rset->get<int16>("h2h_sdt"));
        PMob->setModifier(Mod::IMPACT_SDT, rset->get<int16>("impact_sdt"));

        PMob->setModifier(Mod::UDMGMAGIC, rset->get<int16>("magical_sdt")); // Modifier 389, base 10000 stored as signed integer. Positives signify less damage.

        PMob->setModifier(Mod::FIRE_SDT, rset->get<int16>("fire_sdt"));         // Modifier 54, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::ICE_SDT, rset->get<int16>("ice_sdt"));           // Modifier 55, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::WIND_SDT, rset->get<int16>("wind_sdt"));         // Modifier 56, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::EARTH_SDT, rset->get<int16>("earth_sdt"));       // Modifier 57, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::THUNDER_SDT, rset->get<int16>("lightning_sdt")); // Modifier 58, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::WATER_SDT, rset->get<int16>("water_sdt"));       // Modifier 59, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::LIGHT_SDT, rset->get<int16>("light_sdt"));       // Modifier 60, base 10000 stored as signed integer. Positives signify less damage.
        PMob->setModifier(Mod::DARK_SDT, rset->get<int16>("dark_sdt"));         // Modifier 61, base 10000 stored as signed integer. Positives signify less damage.

        PMob->setModifier(Mod::FIRE_RES_RANK, rset->get<int8>("fire_res_rank"));
        PMob->setModifier(Mod::ICE_RES_RANK, rset->get<int8>("ice_res_rank"));
        PMob->setModifier(Mod::WIND_RES_RANK, rset->get<int8>("wind_res_rank"));
        PMob->setModifier(Mod::EARTH_RES_RANK, rset->get<int8>("earth_res_rank"));
        PMob->setModifier(Mod::THUNDER_RES_RANK, rset->get<int8>("lightning_res_rank"));
        PMob->setModifier(Mod::WATER_RES_RANK, rset->get<int8>("water_res_rank"));
        PMob->setModifier(Mod::LIGHT_RES_RANK, rset->get<int8>("light_res_rank"));
        PMob->setModifier(Mod::DARK_RES_RANK, rset->get<int8>("dark_res_rank"));

        PMob->m_Element     = rset->get<uint8>("Element");
        PMob->m_Species     = rset->get<uint16>("speciesid");
        PMob->m_name_prefix = rset->get<uint8>("name_prefix");
        PMob->m_flags       = rset->get<uint32>("entityFlags");

        PMob->animationsub = rset->get<uint32>("animationsub");

        // Setup HP / MP Stat Percentage Boost
        PMob->HPscale = rset->get<float>("hp_scale");
        PMob->MPscale = rset->get<float>("mp_scale");

        PMob->m_SpellListContainer = mobSpellList::GetMobSpellList(rset->get<uint16>("spellList"));

        PMob->m_Pool = rset->get<uint32>("poolid");

        PMob->allegiance      = rset->get<ALLEGIANCE_TYPE>("allegiance");
        PMob->namevis         = rset->get<uint8>("namevis");
        PMob->modelHitboxSize = std::max<float>(0.0f, rset->getOrDefault<float>("modelHitboxSize", 0) / 10.f);
        PMob->modelSize       = rset->getOrDefault<uint8>("modelSize", 0);
        PMob->m_Aggro         = rset->get<bool>("aggro");
        PMob->m_MobSkillList  = rset->get<uint16>("skill_list_id");
        PMob->m_TrueDetection = rset->get<bool>("true_detection");
        PMob->setMobMod(MOBMOD_DETECTION, rset->get<int16>("detects"));

        mobutils::InitializeMob(PMob);
        mobutils::AddSqlModifiers(PMob);
    }

    return PMob;
}

void WeaknessTrigger(CBaseEntity* PTarget, WeaknessType level)
{
    ActionAnimation animationID = ActionAnimation::None;
    switch (level)
    {
        case WeaknessType::RED:
            animationID = ActionAnimation::RedTrigger;
            break;
        case WeaknessType::YELLOW:
            animationID = ActionAnimation::YellowTrigger;
            break;
        case WeaknessType::BLUE:
            animationID = ActionAnimation::BlueTrigger;
            break;
        case WeaknessType::WHITE:
            animationID = ActionAnimation::WhiteTrigger;
            break;
    }
    // TODO: Weakness Triggers are actually MAGIC_SCHEDULOR + Terror flag...
    action_t action{
        .actorId    = PTarget->id,
        .actiontype = ActionCategory::MobSkillFinish,
        .targets    = {
            {
                .actorId = PTarget->id,
                .results = {
                    {
                        .animation = animationID,
                        .param     = 2582,
                    },
                },
            },
        },
    };

    PTarget->loc.zone->PushPacket(PTarget, CHAR_INRANGE, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
}

}; // namespace mobutils
