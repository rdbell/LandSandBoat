/*
===========================================================================

  Copyright (c) 2024 LandSandBoat Dev Teams

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

#include "trustutils.h"

#include "common/utils.h"

#include <algorithm>
#include <cstring>
#include <vector>

#include "battleutils.h"
#include "charutils.h"
#include "mobutils.h"
#include "zoneutils.h"

#include "grades.h"
#include "map/calculate_stats_capacity.h"
#include "map/trust_load_capacity.h"
#include "map/trust_stats_capacity.h"
#include "mob_spell_list.h"

#include "ai/ai_container.h"
#include "ai/controllers/trust_controller.h"
#include "ai/helpers/gambits_container.h"
#include "entities/mob_entity.h"
#include "entities/trust_entity.h"
#include "items/item_weapon.h"
#include "mobskill.h"
#include "status_effect_container.h"
#include "weapon_skill.h"
#include "zone_instance.h"

//
// Forward declarations
//

void BuildTrustData(uint32 TrustID);
auto LoadTrust(CCharEntity* PMaster, uint32 TrustID) -> CTrustEntity*;
void LoadTrustStatsAndSkills(CTrustEntity* PTrust);

// List of trusts that are essentially walking GEO bubbles that should not be targetable
static std::unordered_set<SpellID> passiveTrustIDs = {
    SpellID::Sakura,
    SpellID::Moogle,
    SpellID::Star_Sibyl,
    SpellID::Kuyin_Hathdenna,
    SpellID::Brygid,
    SpellID::Kupofried,
    SpellID::Cornelia,
};

struct TrustData
{
    uint32        trustID{};
    bool          isPassiveTrust{};
    uint32        pool{};
    look_t        look;        // appearance data
    std::string   name;        // script name string
    std::string   packet_name; // packet name string
    xi::Ecosystem EcoSystem{}; // ecosystem

    uint8  name_prefix{};
    uint8  modelSize{ 0 };
    float  modelHitboxSize{ 0.0f };
    uint16 m_Species{};

    uint8 mJob{};
    uint8 sJob{};
    float HPscale{}; // HP boost percentage
    float MPscale{}; // MP boost percentage

    uint8  cmbSkill{};
    uint16 cmbDmgMult{};
    uint16 cmbDelay{};
    uint8  baseSpeed{};
    uint8  animationSpeed{};

    // stat ranks
    uint8 strRank{};
    uint8 dexRank{};
    uint8 vitRank{};
    uint8 agiRank{};
    uint8 intRank{};
    uint8 mndRank{};
    uint8 chrRank{};
    uint8 attRank{};
    uint8 defRank{};
    uint8 evaRank{};
    uint8 accRank{};

    uint16 m_MobSkillList{};

    // magic stuff
    uint16 spellList{};

    // resists
    int16 slash_sdt{};
    int16 pierce_sdt{};
    int16 hth_sdt{};
    int16 impact_sdt{};

    int16 magical_sdt{};

    int16 fire_sdt{};
    int16 ice_sdt{};
    int16 wind_sdt{};
    int16 earth_sdt{};
    int16 thunder_sdt{};
    int16 water_sdt{};
    int16 light_sdt{};
    int16 dark_sdt{};

    int8 fire_res_rank{};
    int8 ice_res_rank{};
    int8 wind_res_rank{};
    int8 earth_res_rank{};
    int8 thunder_res_rank{};
    int8 water_res_rank{};
    int8 light_res_rank{};
    int8 dark_res_rank{};

    int8 paralyze_res_rank{};
    int8 bind_res_rank{};
    int8 silence_res_rank{};
    int8 slow_res_rank{};
    int8 poison_res_rank{};
    int8 light_sleep_res_rank{};
    int8 dark_sleep_res_rank{};
    int8 blind_res_rank{};
};

std::unordered_map<uint16, std::unique_ptr<TrustData>> g_PTrustData;

void trustutils::LoadTrustList()
{
    const auto rset = db::preparedStmt("SELECT "
                                       "spell_list.spellid, mob_pools.poolid "
                                       "FROM spell_list, mob_pools "
                                       "WHERE spell_list.spellid >= 896 AND mob_pools.poolid = (spell_list.spellid + 5000) ORDER BY spell_list.spellid");

    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            const auto trustSpellId = rset->get<uint32>(0);
            BuildTrustData(trustSpellId);
        }
    }
}

auto trustutils::SpawnTrust(CCharEntity* PMaster, uint32 TrustID) -> CTrustEntity*
{
    CTrustEntity* PTrust = LoadTrust(PMaster, TrustID);
    if (PTrust == nullptr)
    {
        return nullptr;
    }

    if (PMaster->PParty == nullptr)
    {
        PMaster->PParty = new CParty(PMaster);
    }

    PMaster->PTrusts.insert(PMaster->PTrusts.end(), PTrust);
    PMaster->StatusEffectContainer->CopyConfrontationEffect(PTrust);
    PTrust->setBattleID(PMaster->getBattleID());

    if (PMaster->PBattlefield)
    {
        PTrust->PBattlefield = PMaster->PBattlefield;
    }

    if (PMaster->PInstance)
    {
        PTrust->PInstance = PMaster->PInstance;
    }

    PMaster->loc.zone->InsertTRUST(PTrust);
    PTrust->Spawn();

    PMaster->PParty->ReloadParty();

    return PTrust;
}

void BuildTrustData(uint32 TrustID)
{
    const auto rset = db::preparedStmt("SELECT "
                                       "mob_pools.poolid, "
                                       "mob_pools.name, "
                                       "mob_pools.packet_name, "
                                       "mob_pools.modelid, "
                                       "mob_pools.speciesid, "
                                       "mob_pools.mJob, "
                                       "mob_pools.sJob, "
                                       "mob_pools.spellList, "
                                       "mob_pools.cmbSkill, "
                                       "mob_pools.cmbDelay, "
                                       "mob_pools.cmbDmgMult, "
                                       "mob_pools.name_prefix, "
                                       "mob_pools.skill_list_id, "
                                       "mob_pools.modelSize, "
                                       "mob_pools.modelHitboxSize, "
                                       "spell_list.spellid, "
                                       "mob_species_system.ecosystemID, "
                                       "(mob_species_system.HP / 100) AS HP, "
                                       "(mob_species_system.MP / 100) AS MP, "
                                       "mob_species_system.speed, "
                                       "mob_species_system.STR, "
                                       "mob_species_system.DEX, "
                                       "mob_species_system.VIT, "
                                       "mob_species_system.AGI, "
                                       "mob_species_system.INT, "
                                       "mob_species_system.MND, "
                                       "mob_species_system.CHR, "
                                       "mob_species_system.DEF, "
                                       "mob_species_system.ATT, "
                                       "mob_species_system.ACC, "
                                       "mob_species_system.EVA, "
                                       "mob_resistances.slash_sdt, mob_resistances.pierce_sdt, "
                                       "mob_resistances.h2h_sdt, mob_resistances.impact_sdt, "
                                       "mob_resistances.magical_sdt, "
                                       "mob_resistances.fire_sdt, mob_resistances.ice_sdt, "
                                       "mob_resistances.wind_sdt, mob_resistances.earth_sdt, "
                                       "mob_resistances.lightning_sdt, mob_resistances.water_sdt, "
                                       "mob_resistances.light_sdt, mob_resistances.dark_sdt, "
                                       "mob_resistances.fire_res_rank, mob_resistances.ice_res_rank, "
                                       "mob_resistances.wind_res_rank, mob_resistances.earth_res_rank, "
                                       "mob_resistances.lightning_res_rank, mob_resistances.water_res_rank, "
                                       "mob_resistances.light_res_rank, mob_resistances.dark_res_rank, "
                                       "mob_resistances.paralyze_res_rank, mob_resistances.bind_res_rank, "
                                       "mob_resistances.silence_res_rank, mob_resistances.slow_res_rank, "
                                       "mob_resistances.poison_res_rank, mob_resistances.light_sleep_res_rank, "
                                       "mob_resistances.dark_sleep_res_rank, mob_resistances.blind_res_rank "
                                       "FROM spell_list, mob_pools, mob_species_system, mob_resistances "
                                       "WHERE spell_list.spellid = ? "
                                       "AND (spell_list.spellid + 5000) = mob_pools.poolid "
                                       "AND mob_pools.resist_id = mob_resistances.resist_id "
                                       "AND mob_pools.speciesid = mob_species_system.speciesID "
                                       "ORDER BY spell_list.spellid",
                                       TrustID);

    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            auto data = std::make_unique<TrustData>();

            data->trustID = TrustID;

            if (passiveTrustIDs.contains(static_cast<SpellID>(data->trustID)))
            {
                data->isPassiveTrust = true;
            }

            data->pool        = rset->get<uint32>("poolid");
            data->name        = rset->get<std::string>("name");
            data->packet_name = rset->get<std::string>("packet_name");

            db::extractFromBlob(rset, "modelid", data->look);

            data->m_Species = rset->get<uint16>("speciesid");
            data->mJob      = rset->get<uint8>("mJob");
            data->sJob      = rset->get<uint8>("sJob");
            data->spellList = rset->get<uint16>("spellList");

            data->cmbSkill   = rset->get<uint8>("cmbSkill");
            data->cmbDelay   = rset->get<uint16>("cmbDelay");
            data->cmbDmgMult = rset->get<uint16>("cmbDmgMult");

            data->name_prefix    = rset->get<uint8>("name_prefix");
            data->m_MobSkillList = rset->get<uint16>("skill_list_id");

            data->modelSize       = rset->getOrDefault<uint8>("modelSize", 0);
            data->modelHitboxSize = std::max<float>(0.0f, rset->getOrDefault<float>("modelHitboxSize", 0) / 10.f);
            data->EcoSystem       = rset->get<xi::Ecosystem>("ecosystemID");
            data->HPscale         = rset->get<float>("HP");
            data->MPscale         = rset->get<float>("MP");

            data->baseSpeed      = 62;
            data->animationSpeed = 50;

            data->strRank = rset->get<uint8>("STR");
            data->dexRank = rset->get<uint8>("DEX");
            data->vitRank = rset->get<uint8>("VIT");
            data->agiRank = rset->get<uint8>("AGI");
            data->intRank = rset->get<uint8>("INT");
            data->mndRank = rset->get<uint8>("MND");
            data->chrRank = rset->get<uint8>("CHR");
            data->defRank = rset->get<uint8>("DEF");
            data->attRank = rset->get<uint8>("ATT");
            data->accRank = rset->get<uint8>("ACC");
            data->evaRank = rset->get<uint8>("EVA");

            // resistances
            data->slash_sdt  = rset->get<int16>("slash_sdt");
            data->pierce_sdt = rset->get<int16>("pierce_sdt");
            data->hth_sdt    = rset->get<int16>("h2h_sdt");
            data->impact_sdt = rset->get<int16>("impact_sdt");

            data->magical_sdt = rset->get<int16>("magical_sdt"); // Modifier 389, base 10000 stored as signed integer. Positives signify less damage.

            data->fire_sdt    = rset->get<int16>("fire_sdt");      // Modifier 54, base 10000 stored as signed integer. Positives signify less damage.
            data->ice_sdt     = rset->get<int16>("ice_sdt");       // Modifier 55, base 10000 stored as signed integer. Positives signify less damage.
            data->wind_sdt    = rset->get<int16>("wind_sdt");      // Modifier 56, base 10000 stored as signed integer. Positives signify less damage.
            data->earth_sdt   = rset->get<int16>("earth_sdt");     // Modifier 57, base 10000 stored as signed integer. Positives signify less damage.
            data->thunder_sdt = rset->get<int16>("lightning_sdt"); // Modifier 58, base 10000 stored as signed integer. Positives signify less damage.
            data->water_sdt   = rset->get<int16>("water_sdt");     // Modifier 59, base 10000 stored as signed integer. Positives signify less damage.
            data->light_sdt   = rset->get<int16>("light_sdt");     // Modifier 60, base 10000 stored as signed integer. Positives signify less damage.
            data->dark_sdt    = rset->get<int16>("dark_sdt");      // Modifier 61, base 10000 stored as signed integer. Positives signify less damage.

            data->fire_res_rank    = rset->get<int8>("fire_res_rank");
            data->ice_res_rank     = rset->get<int8>("ice_res_rank");
            data->wind_res_rank    = rset->get<int8>("wind_res_rank");
            data->earth_res_rank   = rset->get<int8>("earth_res_rank");
            data->thunder_res_rank = rset->get<int8>("lightning_res_rank");
            data->water_res_rank   = rset->get<int8>("water_res_rank");
            data->light_res_rank   = rset->get<int8>("light_res_rank");
            data->dark_res_rank    = rset->get<int8>("dark_res_rank");

            data->paralyze_res_rank    = rset->get<int8>("paralyze_res_rank");
            data->bind_res_rank        = rset->get<int8>("bind_res_rank");
            data->silence_res_rank     = rset->get<int8>("silence_res_rank");
            data->slow_res_rank        = rset->get<int8>("slow_res_rank");
            data->poison_res_rank      = rset->get<int8>("poison_res_rank");
            data->light_sleep_res_rank = rset->get<int8>("light_sleep_res_rank");
            data->dark_sleep_res_rank  = rset->get<int8>("dark_sleep_res_rank");
            data->blind_res_rank       = rset->get<int8>("blind_res_rank");

            g_PTrustData[TrustID] = std::move(data);
        }
    }
}

auto LoadTrust(CCharEntity* PMaster, uint32 TrustID) -> CTrustEntity*
{
    const auto itr = g_PTrustData.find(TrustID);
    if (itr == g_PTrustData.end())
    {
        ShowError(fmt::format("Could not look up trust data for id: {}", TrustID));
        return nullptr;
    }

    auto* trustData = itr->second.get();

    auto* PTrust = new CTrustEntity(PMaster, trustData->trustID, IsPassiveTrust{ trustData->isPassiveTrust });

    PTrust->loc              = PMaster->loc;
    PTrust->m_OwnerID.id     = PMaster->id;
    PTrust->m_OwnerID.targid = PMaster->targid;

    // Pure load policy (trust_load_capacity.h; slice 1614).
    // spawn me randomly around master
    PTrust->loc.p = nearPosition(PMaster->loc.p, trustloadhelpers::SpawnRadius(static_cast<int>(PMaster->PTrusts.size())), (float)M_PI);
    PTrust->look  = trustData->look;
    PTrust->name  = trustData->name;

    PTrust->m_Pool         = trustData->pool;
    PTrust->packetName     = trustData->packet_name;
    PTrust->m_name_prefix  = trustData->name_prefix;
    PTrust->m_Species      = trustData->m_Species;
    PTrust->m_MobSkillList = trustData->m_MobSkillList;
    PTrust->HPscale        = trustData->HPscale;
    PTrust->MPscale        = trustData->MPscale;
    PTrust->baseSpeed      = trustData->baseSpeed;
    PTrust->animationSpeed = trustData->animationSpeed;

    PTrust->UpdateSpeed();

    PTrust->status          = STATUS_TYPE::NORMAL;
    PTrust->modelSize       = trustData->modelSize;
    PTrust->modelHitboxSize = trustData->modelHitboxSize;
    PTrust->m_EcoSystem     = trustData->EcoSystem;

    PTrust->SetMJob(trustData->mJob);
    PTrust->SetSJob(trustData->sJob);

    // assume level matches master
    const uint8 masterMLvl = PMaster->GetMLevel();
    PTrust->SetMLevel(trustloadhelpers::MainLevel(masterMLvl));
    PTrust->SetSLevel(trustloadhelpers::SubLevel(masterMLvl));

    LoadTrustStatsAndSkills(PTrust);

    // Use Mob formulas to work out base "weapon" damage, but scale down to reasonable values.
    // TODO: Verify trust base damage.
    const uint16 finalDamage = trustloadhelpers::WeaponDamage(mobutils::GetBaseWeaponDamage(PTrust, SLOT_MAIN), trustData->cmbDmgMult);

    // Trust do not really have weapons, but they are modelled internally as
    // if they do.
    if (auto* mainWeapon = dynamic_cast<CItemWeapon*>(PTrust->m_Weapons[SLOT_MAIN]))
    {
        mainWeapon->setMaxHit(1);
        mainWeapon->setSkillType(trustData->cmbSkill);

        mainWeapon->setDamage(finalDamage);
        mainWeapon->setDelay(trustData->cmbDelay);
        mainWeapon->setBaseDelay(trustData->cmbDelay);

        // Compute DPS so rune/enchantment calculations that rely on getDPS() return meaningful values for trusts.
        if (const auto dps = trustloadhelpers::WeaponDPS(mainWeapon->getDamage(), mainWeapon->getDelay()); dps > 0.0)
        {
            mainWeapon->setDPS(dps);
        }
    }

    if (auto* subWeapon = dynamic_cast<CItemWeapon*>(PTrust->m_Weapons[SLOT_SUB]))
    {
        subWeapon->setDamage(finalDamage);
        subWeapon->setDelay(trustData->cmbDelay);
        subWeapon->setBaseDelay(trustData->cmbDelay);

        if (const auto dps = trustloadhelpers::WeaponDPS(subWeapon->getDamage(), subWeapon->getDelay()); dps > 0.0)
        {
            subWeapon->setDPS(dps);
        }
    }

    if (auto* rangedWeapon = dynamic_cast<CItemWeapon*>(PTrust->m_Weapons[SLOT_RANGED]))
    {
        rangedWeapon->setDamage(finalDamage);
        rangedWeapon->setDelay(trustData->cmbDelay);
        rangedWeapon->setBaseDelay(trustData->cmbDelay);

        if (const auto dps = trustloadhelpers::WeaponDPS(rangedWeapon->getDamage(), rangedWeapon->getDelay()); dps > 0.0)
        {
            rangedWeapon->setDPS(dps);
        }
    }

    if (auto* ammoWeapon = dynamic_cast<CItemWeapon*>(PTrust->m_Weapons[SLOT_AMMO]))
    {
        ammoWeapon->setDamage(finalDamage);
        ammoWeapon->setDelay(trustData->cmbDelay);
        ammoWeapon->setBaseDelay(trustData->cmbDelay);

        if (const auto dps = trustloadhelpers::WeaponDPS(ammoWeapon->getDamage(), ammoWeapon->getDelay()); dps > 0.0)
        {
            ammoWeapon->setDPS(dps);
        }
    }

    // NOTE: Trusts don't really have weapons, and they don't really have combat skills. They only have
    // a damage type, and whether or not they are multi-hit. We handle this wrong everywhere.
    // To give any Trust multi-hit, you need to give them cmbSkill == SKILL_HAND_TO_HAND (1).
    if (trustloadhelpers::DualWieldFromCmbSkill(trustData->cmbSkill))
    {
        PTrust->m_dualWield = true;
    }

    if (auto* spellList = mobSpellList::GetMobSpellList(trustData->spellList); spellList != nullptr)
    {
        mobutils::SetSpellList(PTrust, trustData->spellList);
    }

    return PTrust;
}

void LoadTrustStatsAndSkills(CTrustEntity* PTrust)
{
    // Pure policy via truststatshelpers + calculatestatshelpers (slice 1611).
    if (settings::get<uint8>("main.ENABLE_TRUST_ALTER_EGO_EXPO") > 0) // Alter Ego Expo HPP/MPP +50%, All Status Resistance +25%
    {
        PTrust->addModifier(Mod::HPP, truststatshelpers::ExpoHPP);
        PTrust->addModifier(Mod::MPP, truststatshelpers::ExpoMPP);
        PTrust->addModifier(Mod::STATUSRES, truststatshelpers::ExpoStatusRes);
    }

    // add mob pool mods ahead of applying stats
    mobutils::AddSqlModifiers(PTrust);

    JOBTYPE mJob = PTrust->GetMJob();
    JOBTYPE sJob = PTrust->GetSJob();
    uint8   mLvl = PTrust->GetMLevel();
    uint8   sLvl = PTrust->GetSLevel();

    // HP/MP ========================
    // Same system as charutils, modified for mob_species HPscale/MPscale grades.
    // http://ffxi-stat-calc.sourceforge.net/cgi-bin/ffxistats.cgi?mode=document

    float raceStat  = 0;
    float jobStat   = 0;
    float sJobStat  = 0;
    int32 bonusStat = 0;

    uint8 grade = 0;

    const int32 mainLevelOver30     = calculatestatshelpers::MainLevelOver30(mLvl);
    const int32 mainLevelUpTo60     = calculatestatshelpers::MainLevelUpTo60(mLvl);
    const int32 mainLevelOver60To75 = calculatestatshelpers::MainLevelOver60To75(mLvl);
    const int32 mainLevelOver75     = calculatestatshelpers::MainLevelOver75(mLvl);
    const int32 mainLevelOver10           = calculatestatshelpers::MainLevelOver10(mLvl);
    const int32 mainLevelOver50andUnder60 = calculatestatshelpers::MainLevelOver50AndUnder60(mLvl);
    const int32 mainLevelOver60           = calculatestatshelpers::MainLevelOver60(mLvl);
    const int32 subLevelOver10            = calculatestatshelpers::SubLevelOver10(sLvl);
    const int32 subLevelOver30            = calculatestatshelpers::SubLevelOver30(sLvl);

    grade = truststatshelpers::ScaleToGrade(PTrust->HPscale);
    raceStat = calculatestatshelpers::ComposeHPScale(
        grade::GetHPScale(grade, calculatestatshelpers::BaseValueColumn),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleTo60Column),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleOver30Column),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleOver60Column),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleOver75Column),
        mainLevelUpTo60,
        mainLevelOver30,
        mainLevelOver60To75,
        mainLevelOver75);

    grade = grade::GetJobGrade(mJob, 0);
    jobStat = calculatestatshelpers::ComposeHPScale(
        grade::GetHPScale(grade, calculatestatshelpers::BaseValueColumn),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleTo60Column),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleOver30Column),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleOver60Column),
        grade::GetHPScale(grade, calculatestatshelpers::ScaleOver75Column),
        mainLevelUpTo60,
        mainLevelOver30,
        mainLevelOver60To75,
        mainLevelOver75);

    bonusStat = calculatestatshelpers::BonusHPStat(mainLevelOver10, mainLevelOver50andUnder60);

    if (sLvl > 0)
    {
        grade = grade::GetJobGrade(sJob, 0);
        sJobStat = truststatshelpers::ComposeTrustSubJobHP(
            grade::GetHPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetHPScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetHPScale(grade, calculatestatshelpers::ScaleOver30Column),
            sLvl,
            subLevelOver30,
            subLevelOver10);
    }

    PTrust->health.maxhp = truststatshelpers::FinalTrustMaxHP(
        raceStat,
        jobStat,
        bonusStat,
        sJobStat,
        truststatshelpers::ClampAlterEgoMultiplier(settings::get<float>("map.ALTER_EGO_HP_MULTIPLIER")));

    // MP
    raceStat = 0;
    jobStat  = 0;
    sJobStat = 0;

    grade = truststatshelpers::ScaleToGrade(PTrust->MPscale);

    if (grade::GetJobGrade(mJob, 1) == 0)
    {
        if (grade::GetJobGrade(sJob, 1) != 0 && sLvl > 0)
        {
            raceStat = calculatestatshelpers::ComposeSubJobMP(
                grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
                grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
                sLvl,
                static_cast<float>(settings::get<uint8>("map.SJ_MP_DIVISOR")));
        }
    }
    else
    {
        raceStat = calculatestatshelpers::ComposeMPScale(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetMPScale(grade, calculatestatshelpers::MPScaleOver60),
            mainLevelUpTo60,
            mainLevelOver60);
    }

    grade = grade::GetJobGrade(mJob, 1);
    if (grade > 0)
    {
        jobStat = calculatestatshelpers::ComposeMPScale(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetMPScale(grade, calculatestatshelpers::MPScaleOver60),
            mainLevelUpTo60,
            mainLevelOver60);
    }

    if (sLvl > 0)
    {
        grade    = grade::GetJobGrade(sJob, 1);
        sJobStat = truststatshelpers::ComposeTrustSubJobMP(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column));
    }

    PTrust->health.maxmp = truststatshelpers::FinalTrustMaxMP(
        raceStat,
        jobStat,
        sJobStat,
        truststatshelpers::ClampAlterEgoMultiplier(settings::get<float>("map.ALTER_EGO_MP_MULTIPLIER")));

    PTrust->health.tp = 0;
    PTrust->UpdateHealth();
    PTrust->health.hp = PTrust->GetMaxHP();
    PTrust->health.mp = PTrust->GetMaxMP();

    // Stats ========================
    const uint16 fSTR = mobutils::GetBaseToRank(PTrust->strRank, mLvl);
    const uint16 fDEX = mobutils::GetBaseToRank(PTrust->dexRank, mLvl);
    const uint16 fVIT = mobutils::GetBaseToRank(PTrust->vitRank, mLvl);
    const uint16 fAGI = mobutils::GetBaseToRank(PTrust->agiRank, mLvl);
    const uint16 fINT = mobutils::GetBaseToRank(PTrust->intRank, mLvl);
    const uint16 fMND = mobutils::GetBaseToRank(PTrust->mndRank, mLvl);
    const uint16 fCHR = mobutils::GetBaseToRank(PTrust->chrRank, mLvl);

    const uint16 mSTR = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 2), mLvl);
    const uint16 mDEX = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 3), mLvl);
    const uint16 mVIT = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 4), mLvl);
    const uint16 mAGI = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 5), mLvl);
    const uint16 mINT = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 6), mLvl);
    const uint16 mMND = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 7), mLvl);
    const uint16 mCHR = mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetMJob(), 8), mLvl);

    const uint16 sSTR = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 2), sLvl), sLvl);
    const uint16 sDEX = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 3), sLvl), sLvl);
    const uint16 sVIT = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 4), sLvl), sLvl);
    const uint16 sAGI = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 5), sLvl), sLvl);
    const uint16 sINT = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 6), sLvl), sLvl);
    const uint16 sMND = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 7), sLvl), sLvl);
    const uint16 sCHR = truststatshelpers::ScaleSubJobStat(mobutils::GetBaseToRank(grade::GetJobGrade(PTrust->GetSJob(), 8), sLvl), sLvl);

    const float statMultiplier = truststatshelpers::ClampAlterEgoMultiplier(settings::get<float>("map.ALTER_EGO_STAT_MULTIPLIER"));
    PTrust->stats.STR          = truststatshelpers::FinalTrustStat(fSTR, mSTR, sSTR, statMultiplier);
    PTrust->stats.DEX          = truststatshelpers::FinalTrustStat(fDEX, mDEX, sDEX, statMultiplier);
    PTrust->stats.VIT          = truststatshelpers::FinalTrustStat(fVIT, mVIT, sVIT, statMultiplier);
    PTrust->stats.AGI          = truststatshelpers::FinalTrustStat(fAGI, mAGI, sAGI, statMultiplier);
    PTrust->stats.INT          = truststatshelpers::FinalTrustStat(fINT, mINT, sINT, statMultiplier);
    PTrust->stats.MND          = truststatshelpers::FinalTrustStat(fMND, mMND, sMND, statMultiplier);
    PTrust->stats.CHR          = truststatshelpers::FinalTrustStat(fCHR, mCHR, sCHR, statMultiplier);

    // Skills =======================
    const uint8  skillLvl   = truststatshelpers::SkillCapLevel(mLvl);
    const float  skillMult  = settings::get<float>("map.ALTER_EGO_SKILL_MULTIPLIER");
    for (int i = SKILL_DIVINE_MAGIC; i <= SKILL_BLUE_MAGIC; i++)
    {
        uint16 maxSkill = battleutils::GetMaxSkill((SKILLTYPE)i, mJob, skillLvl);
        if (maxSkill != 0)
        {
            PTrust->WorkingSkills.skill[i] = truststatshelpers::ApplySkillMultiplier(maxSkill, skillMult);
        }
        else // if the mob is WAR/BLM and can cast spell
        {
            // set skill as high as main level, so their spells won't get resisted
            uint16 maxSubSkill = battleutils::GetMaxSkill((SKILLTYPE)i, sJob, skillLvl);

            if (maxSubSkill != 0)
            {
                PTrust->WorkingSkills.skill[i] = truststatshelpers::ApplySkillMultiplier(maxSubSkill, skillMult);
            }
        }
    }

    for (int i = SKILL_HAND_TO_HAND; i <= SKILL_STAFF; i++)
    {
        uint16 maxSkill = battleutils::GetMaxSkill((SKILLTYPE)i, skillLvl);
        if (maxSkill != 0)
        {
            PTrust->WorkingSkills.skill[i] = truststatshelpers::ApplySkillMultiplier(maxSkill, skillMult);
        }
    }

    PTrust->addModifier(Mod::DEF, mobutils::GetBaseSkill(PTrust, PTrust->defRank));
    PTrust->addModifier(Mod::EVA, mobutils::GetBaseSkill(PTrust, PTrust->evaRank));
    PTrust->addModifier(Mod::ATT, mobutils::GetBaseSkill(PTrust, PTrust->attRank));
    PTrust->addModifier(Mod::ACC, mobutils::GetBaseSkill(PTrust, PTrust->accRank));

    PTrust->addModifier(Mod::RATT, mobutils::GetBaseSkill(PTrust, PTrust->attRank));
    PTrust->addModifier(Mod::RACC, mobutils::GetBaseSkill(PTrust, PTrust->accRank));

    // Natural magic evasion
    PTrust->addModifier(Mod::MEVA, mobutils::GetMagicEvasion(PTrust));

    // Add traits for sub and main
    battleutils::AddTraits(PTrust, traits::GetTraits(mJob), mLvl);
    battleutils::AddTraits(PTrust, traits::GetTraits(sJob), sLvl);

    mobutils::SetupJob(PTrust);

    // Skills
    using namespace gambits;
    auto* controller = dynamic_cast<CTrustController*>(PTrust->PAI->GetController());

    if (!controller)
    {
        ShowWarning("trustutils::LoadTrustStatsAndSkills() - Trust Controller was null.");
        return;
    }

    // Default TP selectors
    controller->m_GambitsContainer->tp_trigger = G_TP_TRIGGER::ASAP;
    controller->m_GambitsContainer->tp_select  = G_SELECT::RANDOM;

    auto skillList = battleutils::GetMobSkillList(PTrust->m_MobSkillList);
    for (uint16 skill_id : skillList)
    {
        TrustSkill_t skill;
        if (skill_id <= 255) // Player WSs
        {
            CWeaponSkill* PWeaponSkill = battleutils::GetWeaponSkill(skill_id);
            if (!PWeaponSkill)
            {
                ShowWarning("LoadTrustStatsAndSkills: Error loading WeaponSkill id %d for trust %s", skill_id, PTrust->name);
                break;
            }

            skill = TrustSkill_t{
                G_REACTION::WS,
                skill_id,
                PWeaponSkill->getPrimarySkillchain(),
                PWeaponSkill->getSecondarySkillchain(),
                PWeaponSkill->getTertiarySkillchain(),
                battleutils::isValidSelfTargetWeaponskill(skill_id) ? TARGET_SELF : TARGET_ENEMY,
            };
        }
        else // MobSkills
        {
            CMobSkill* PMobSkill = battleutils::GetMobSkill(skill_id);
            if (!PMobSkill)
            {
                ShowWarning("LoadTrustStatsAndSkills: Error loading MobSkill id %d for trust %s", skill_id, PTrust->name);
                break;
            }
            skill = {
                G_REACTION::MS,
                skill_id,
                PMobSkill->getPrimarySkillchain(),
                PMobSkill->getSecondarySkillchain(),
                PMobSkill->getTertiarySkillchain(),
                static_cast<TARGETTYPE>(PMobSkill->getValidTargets()),
            };

            controller->m_GambitsContainer->tp_skills.emplace_back(skill);
        }

        // Pure Lv3 SC TP skill gate (trust_load_capacity.h; slice 1615).
        // Note: MS path may have already emplaced above; existing count is post-MS size.
        if (trustloadhelpers::CanUseTPSkill(
                PTrust->GetMLevel(),
                skill.primary,
                skill.secondary,
                skill.tertiary,
                static_cast<int>(controller->m_GambitsContainer->tp_skills.size())))
        {
            controller->m_GambitsContainer->tp_skills.emplace_back(skill);
        }
    }
}
