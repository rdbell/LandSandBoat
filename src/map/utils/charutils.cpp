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

#include "common/logging.h"
#include "common/macros.h"
#include "common/settings.h"
#include "common/timer.h"
#include "common/utils.h"
#include "common/vana_time.h"

#include <array>
#include <chrono>

#include "lua/luautils.h"

#include "ai/ai_container.h"
#include "ai/states/attack_state.h"
#include "ai/states/item_state.h"
#include "ai/states/range_state.h"

#include "packets/char_status.h"
#include "packets/char_sync.h"
#include "packets/s2c/0x009_message.h"
#include "packets/s2c/0x00b_logout.h"
#include "packets/s2c/0x01b_job_info.h"
#include "packets/s2c/0x01d_item_same.h"
#include "packets/s2c/0x01e_item_num.h"
#include "packets/s2c/0x01f_item_list.h"
#include "packets/s2c/0x020_item_attr.h"
#include "packets/s2c/0x026_item_subcontainer.h"
#include "packets/s2c/0x02d_battle_message2.h"
#include "packets/s2c/0x04f_equip_clear.h"
#include "packets/s2c/0x050_equip_list.h"
#include "packets/s2c/0x051_grap_list.h"
#include "packets/s2c/0x053_systemmes.h"
#include "packets/s2c/0x055_scenarioitem.h"
#include "packets/s2c/0x061_clistatus.h"
#include "packets/s2c/0x062_clistatus2.h"
#include "packets/s2c/0x0ac_command_data.h"
#include "packets/s2c/0x0ad_dungeon.h"
#include "packets/s2c/0x0e0_group_comlink.h"
#include "packets/s2c/0x119_abil_recast.h"

#include "ability.h"
#include "alliance.h"
#include "conquest_system.h"
#include "grades.h"
#include "ipc_client.h"
#include "item_container.h"
#include "items.h"
#include "latent_effect_container.h"
#include "linkshell.h"
#include "map_networking.h"
#include "mob_modifier.h"
#include "nominate_manager.h"
#include "recast_container.h"
#include "roe.h"
#include "spell.h"
#include "status_effect_container.h"
#include "trade_container.h"
#include "trait.h"
#include "treasure_pool.h"
#include "unitychat.h"
#include "universal_container.h"
#include "weapon_skill.h"
#include "zone.h"

#include "entities/automaton_entity.h"
#include "entities/char_entity.h"
#include "entities/mob_entity.h"
#include "entities/pet_entity.h"

#include "battleutils.h"
#include "blueutils.h"
#include "charutils.h"
#include "capacity_distribute_capacity.h"
#include "exp_distribute_capacity.h"
#include "exp_award_capacity.h"
#include "exp_loss_capacity.h"
#include "skill_up_capacity.h"
#include "calculate_stats_capacity.h"
#include "distribute_gil_capacity.h"
#include "treasure_hunter_drop_capacity.h"
#include "building_skills_capacity.h"
#include "check_equipment_capacity.h"
#include "capacity_award_capacity.h"
#include "weapon_skill_roster_capacity.h"
#include "weapon_style_update_capacity.h"
#include "ability_table_capacity.h"
#include "armor_style_update_capacity.h"
#include "pet_ability_table_capacity.h"
#include "keyitem_spell_capacity.h"
#include "equip_item_finalize_capacity.h"
#include "equip_item_success_capacity.h"
#include "equip_armor_direct_restrictions_capacity.h"
#include "equip_armor_ammo_look_capacity.h"
#include "equip_armor_main_look_capacity.h"
#include "equip_armor_main_sub_capacity.h"
#include "equip_armor_ranged_compatibility_capacity.h"
#include "equip_armor_ranged_look_capacity.h"
#include "equip_armor_removed_look_capacity.h"
#include "equip_armor_reverse_restrictions_capacity.h"
#include "equip_armor_sub_capacity.h"
#include "equip_armor_sub_look_capacity.h"
#include "equip_armor_target_look_capacity.h"
#include "unequip_armor_look_capacity.h"
#include "unequip_main_attack_timer_capacity.h"
#include "unequip_main_sub_look_capacity.h"
#include "unequip_item_recast_capacity.h"
#include "unequip_item_unlock_capacity.h"
#include "unequip_recalculate_capacity.h"
#include "unequip_removed_armor_look_capacity.h"
#include "unequip_script_flags_capacity.h"
#include "unequip_ranged_look_capacity.h"
#include "unequip_sub_look_capacity.h"
#include "unequip_sub_state_capacity.h"
#include "unequip_weapon_finalize_capacity.h"
#include "equip_policy_capacity.h"
#include "trade_item_capacity.h"
#include "style_update_capacity.h"
#include "style_lock_transition_capacity.h"
#include "inventory_move_capacity.h"
#include "lockstyle_removed_look_capacity.h"
#include "misc_progress_capacity.h"
#include "entity_spawn_capacity.h"
#include "zone_out_capacity.h"
#include "enums/item_lockflg.h"
#include "items/transactions/synth.h"
#include "itemutils.h"
#include "job_points.h"
#include "map_engine.h"
#include "petutils.h"
#include "puppetutils.h"
#include "synthutils.h"
#include "zoneutils.h"

#include "enums/key_items.h"
#include "enums/quest_log.h"
#include "items/item_furnishing.h"
#include "items/item_linkshell.h"
#include "packets/s2c/0x029_battle_message.h"
#include "packets/s2c/0x044_extended_job_blu.h"
#include "packets/s2c/0x044_extended_job_mon.h"
#include "packets/s2c/0x044_extended_job_pup.h"
#include "packets/s2c/0x056_mission.h"
#include "packets/s2c/0x056_mission_other.h"
#include "packets/s2c/0x056_mission_tvr.h"
#include "packets/s2c/0x05e_conquest.h"
#include "packets/s2c/0x063_miscdata_job_points.h"
#include "packets/s2c/0x063_miscdata_merits.h"
#include "packets/s2c/0x063_miscdata_monstrosity.h"
#include "packets/s2c/0x063_miscdata_unity.h"
#include "packets/s2c/0x075_battlefield.h"
#include "packets/s2c/0x0df_group_attr.h"
#include "packets/s2c/0x110_unity.h"
#include "packets/s2c/0x111_roe_activelog.h"
#include "packets/s2c/0x112_roe_log.h"

/************************************************************************
 *                                                                       *
 *  Experience tables                                                    *
 *                                                                       *
 ************************************************************************/

// Number of rows in the exp table
static constexpr int32                               ExpTableRowCount = 60;
std::array<std::array<uint16, 20>, ExpTableRowCount> g_ExpTable;
std::array<uint16, 100>                              g_ExpPerLevel;

std::vector<std::pair<uint16, EMobDifficulty>> ExpToDifficultyTable = {};
// Eventually loaded as something like...
/*
    //  { EXP value, check result }
    { 400, EMobDifficulty::IncrediblyTough },
    { 350, EMobDifficulty::VeryTough },
    { 220, EMobDifficulty::Tough },
    { 200, EMobDifficulty::EvenMatch },
    { 160, EMobDifficulty::DecentChallenge },
    { 60, EMobDifficulty::EasyPrey },
*/

std::pair<uint16, uint8> IncrediblyEasyPreyCheck = { 1, 56 };

// { EXP value, mob level }
// { 1, 56 }
// Must gain more than 1 exp but less than the lowest of ExpToDifficultyTable and greater than or equal to mob level

namespace
{

// Key items granting an increase to the rate of skillups
const std::set skillupIncreaseKeyItems = {
    KeyItem::RHAPSODY_IN_WHITE,
    KeyItem::RHAPSODY_IN_CRIMSON,
    KeyItem::RHAPSODY_IN_FUCHSIA
};

// Key items granting an increase to earned experience points
const std::set experienceBonusKeyItems = {
    KeyItem::RHAPSODY_IN_WHITE,
    KeyItem::RHAPSODY_IN_UMBER,
    KeyItem::RHAPSODY_IN_AZURE,
    KeyItem::RHAPSODY_IN_CRIMSON,
    KeyItem::RHAPSODY_IN_EMERALD,
    KeyItem::RHAPSODY_IN_MAUVE,
};

// Key items granting an increase to earned capacity points
const std::set capacityBonusKeyItems = {
    KeyItem::RHAPSODY_IN_FUCHSIA,
    KeyItem::RHAPSODY_IN_PUCE,
    KeyItem::RHAPSODY_IN_OCHRE,
};

// Key items reducing the time for traverser stones
const std::set traverserStoneReductionKeyItems = {
    KeyItem::AZURE_ABYSSITE_OF_CELERITY,
    KeyItem::CRIMSON_ABYSSITE_OF_CELERITY,
    KeyItem::IVORY_ABYSSITE_OF_CELERITY
};

bool characterPersistenceSuppressedForTests = false;

} // namespace

namespace charutils
{

void SetCharacterPersistenceSuppressedForTests(const bool suppressed)
{
    characterPersistenceSuppressedForTests = suppressed;
}

bool IsCharacterPersistenceSuppressedForTests()
{
    return characterPersistenceSuppressedForTests;
}

/************************************************************************
 *                                                                       *
 *  Calculation of stats of characters                                   *
 *                                                                       *
 ************************************************************************/

void CalculateStats(CCharEntity* PChar)
{
    float raceStat  = 0; // The final HP number for a race-based level.
    float jobStat   = 0; // Estimate HP level for the level based on the primary profession.
    float sJobStat  = 0; // HP final number for a level based on a secondary profession.
    int32 bonusStat = 0; // HP bonus number that is added subject to some conditions.

    uint8 grade = 0;

    uint8      mlvl        = PChar->GetMLevel();
    uint8      slvl        = PChar->GetSLevel();
    JOBTYPE    mjob        = PChar->GetMJob();
    JOBTYPE    sjob        = PChar->GetSJob();
    MERIT_TYPE statMerit[] = { MERIT_STR, MERIT_DEX, MERIT_VIT, MERIT_AGI, MERIT_INT, MERIT_MND, MERIT_CHR };

    // We have to make sure we don't leave the job as JOB_MON - we CANNOT generate stats for it.
    {
        const auto [nm, ns] = calculatestatshelpers::NormalizeMonJob(static_cast<uint8>(mjob), static_cast<uint8>(sjob));
        mjob               = static_cast<JOBTYPE>(nm);
        sjob               = static_cast<JOBTYPE>(ns);
    }

    // NOTE: Monstrosity (MON) is treated as its own job, but each species is it's own
    //     : combination of main/sub job for stats, traits and abilities.
    if (PChar->m_PMonstrosity != nullptr)
    {
        mjob = PChar->m_PMonstrosity->MainJob;
        sjob = PChar->m_PMonstrosity->SubJob;
        mlvl = PChar->m_PMonstrosity->levels[PChar->m_PMonstrosity->MonstrosityId];
        slvl = mlvl;
    }

    const uint8 race = calculatestatshelpers::RaceGroupFromCharRace(PChar->look.race);

    // HP Calculation from Main Job

    const int32 mainLevelOver30     = calculatestatshelpers::MainLevelOver30(mlvl);
    const int32 mainLevelUpTo60     = calculatestatshelpers::MainLevelUpTo60(mlvl);
    const int32 mainLevelOver60To75 = calculatestatshelpers::MainLevelOver60To75(mlvl);
    const int32 mainLevelOver75     = calculatestatshelpers::MainLevelOver75(mlvl);

    // Calculation of the bonus amount of HP

    const int32 mainLevelOver10           = calculatestatshelpers::MainLevelOver10(mlvl);
    const int32 mainLevelOver50andUnder60 = calculatestatshelpers::MainLevelOver50AndUnder60(mlvl);
    const int32 mainLevelOver60           = calculatestatshelpers::MainLevelOver60(mlvl);

    // HP calculation of an additional profession

    const int32 subLevelOver10 = calculatestatshelpers::SubLevelOver10(slvl);
    const int32 subLevelOver30 = calculatestatshelpers::SubLevelOver30(slvl);

    // Calculate Racestat Jobstat Bonusstat Sjobstat
    // Calculation of race

    grade = grade::GetRaceGrades(race, 0);

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

    // Calculation on Main Job
    grade = grade::GetJobGrade(mjob, 0);

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

    // Calculation of bonus HP.
    bonusStat = calculatestatshelpers::BonusHPStat(mainLevelOver10, mainLevelOver50andUnder60);

    // Calculation on Support Job
    if (slvl > 0)
    {
        grade = grade::GetJobGrade(sjob, 0);

        sJobStat = calculatestatshelpers::ComposeSubJobHP(
            grade::GetHPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetHPScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetHPScale(grade, calculatestatshelpers::ScaleOver30Column),
            slvl,
            subLevelOver30,
            subLevelOver10);
    }

    uint16 MeritBonus   = PChar->PMeritPoints->GetMeritValue(MERIT_MAX_HP, PChar);
    PChar->health.maxhp = calculatestatshelpers::FinalMaxHP(raceStat, jobStat, bonusStat, sJobStat, MeritBonus);

    // The beginning of the MP

    raceStat = 0;
    jobStat  = 0;
    sJobStat = 0;

    // Calculation of the MP race.
    grade = grade::GetRaceGrades(race, 1);

    // If Main Job has no MP rating, we calculate a racial bonus based on the level of the subjob level (provided that he has a MP rating)
    if (calculatestatshelpers::ShouldUseSubLevelForRaceMP(grade::GetJobGrade(mjob, 1), grade::GetJobGrade(sjob, 1), slvl))
    {
        raceStat = calculatestatshelpers::ComposeSubJobMP(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
            slvl,
            settings::get<float>("map.SJ_MP_DIVISOR")); // TODO: Here is a mistake (parity note retained)
    }
    else if (grade::GetJobGrade(mjob, 1) != 0)
    {
        // Calculation of a normal racial bonus
        raceStat = calculatestatshelpers::ComposeMPScale(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetMPScale(grade, calculatestatshelpers::MPScaleOver60),
            mainLevelUpTo60,
            mainLevelOver60);
    }

    // Main Job
    grade = grade::GetJobGrade(mjob, 1);
    if (grade > 0)
    {
        jobStat = calculatestatshelpers::ComposeMPScale(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetMPScale(grade, calculatestatshelpers::MPScaleOver60),
            mainLevelUpTo60,
            mainLevelOver60);
    }

    // Subjob
    if (slvl > 0)
    {
        grade    = grade::GetJobGrade(sjob, 1);
        sJobStat = calculatestatshelpers::ComposeSubJobMP(
            grade::GetMPScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetMPScale(grade, calculatestatshelpers::ScaleTo60Column),
            slvl,
            settings::get<float>("map.SJ_MP_DIVISOR"));
    }

    MeritBonus          = PChar->PMeritPoints->GetMeritValue(MERIT_MAX_MP, PChar);
    PChar->health.maxmp = calculatestatshelpers::FinalMaxMP(raceStat, jobStat, sJobStat, MeritBonus); // MP calculation result

    // Start calculating Stats

    uint8 counter = 0;

    for (uint8 StatIndex = 2; StatIndex <= 8; ++StatIndex)
    {
        // Calculation of race
        grade    = grade::GetRaceGrades(race, StatIndex);
        raceStat = calculatestatshelpers::ComposeStatScale(
            grade::GetStatScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetStatScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetStatScale(grade, calculatestatshelpers::MPScaleOver60),
            grade::GetStatScale(grade, calculatestatshelpers::StatScaleOver75),
            mainLevelUpTo60,
            mainLevelOver60,
            mainLevelOver75,
            mlvl);

        // Calculation by profession
        grade   = grade::GetJobGrade(mjob, StatIndex);
        jobStat = calculatestatshelpers::ComposeStatScale(
            grade::GetStatScale(grade, calculatestatshelpers::BaseValueColumn),
            grade::GetStatScale(grade, calculatestatshelpers::ScaleTo60Column),
            grade::GetStatScale(grade, calculatestatshelpers::MPScaleOver60),
            grade::GetStatScale(grade, calculatestatshelpers::StatScaleOver75),
            mainLevelUpTo60,
            mainLevelOver60,
            mainLevelOver75,
            mlvl);

        // Calculation for an additional profession
        if (slvl > 0)
        {
            grade    = grade::GetJobGrade(sjob, StatIndex);
            sJobStat = calculatestatshelpers::ComposeSubJobStat(
                grade::GetStatScale(grade, calculatestatshelpers::BaseValueColumn),
                grade::GetStatScale(grade, calculatestatshelpers::ScaleTo60Column),
                slvl);
        }
        else
        {
            sJobStat = 0;
        }

        // get each merit bonus stat, str,dex,vit and so on...
        MeritBonus = PChar->PMeritPoints->GetMeritValue(statMerit[StatIndex - 2], PChar);

        // Value output
        ref<uint16>(&PChar->stats, counter) = calculatestatshelpers::FinalBaseStat(raceStat, jobStat, sJobStat, MeritBonus);
        counter += 2;
    }
}

/************************************************************************
 *                                                                       *
 *  The preliminary version of the character loading. Function will be   *
 *  optimized after determining all the necessary data and tables        *
 *                                                                       *
 ************************************************************************/

auto LoadChar(Scheduler& scheduler, MapConfig config, const uint32 charId) -> std::unique_ptr<CCharEntity>
{
    TracyZoneScoped;

    std::unique_ptr<CCharEntity> charEntity = std::make_unique<CCharEntity>();
    auto*                        PChar      = charEntity.get();
    PChar->id                               = charId;

    uint8  meritPoints = 0;
    uint16 limitPoints = 0;
    int32  HP          = 0;
    int32  MP          = 0;

    // TODO: extract into LoadFromCharsSQL
    const char* fmtQuery = "SELECT "
                           "charname, "
                           "nation, "
                           "pos_zone, "
                           "pos_prevzone, "
                           "pos_prevzonelineid, "
                           "pos_rot, "
                           "pos_x, "
                           "pos_y, "
                           "pos_z, "
                           "moghouse, "
                           "boundary, "
                           "accid, "
                           "home_zone, "
                           "home_rot, "
                           "home_x, "
                           "home_y, "
                           "home_z, "
                           "missions, "
                           "assault, "
                           "campaign, "
                           "eminence, "
                           "quests, "
                           "keyitems, "
                           "abilities, "
                           "weaponskills, "
                           "titles, "
                           "zones, "
                           "playtime, "
                           "gmlevel, "
                           "languages, "
                           "job_master, "
                           "campaign_allegiance, "
                           "isstylelocked, "
                           "settings, "
                           "chatfilters_1, "
                           "chatfilters_2, "
                           "moghancement, "
                           "UNIX_TIMESTAMP(`lastupdate`) AS lastonline "
                           "FROM chars "
                           "WHERE charid = ?";

    auto rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->targid = 0x400;
        PChar->SetName(rset->get<std::string>("charname").c_str());

        PChar->loc.destination  = rset->get<uint16>("pos_zone");
        PChar->loc.prevzone     = rset->get<uint16>("pos_prevzone");
        PChar->m_PrevZonelineID = rset->get<uint32>("pos_prevzonelineid");

        PChar->loc.p.rotation = rset->get<uint8>("pos_rot");
        PChar->loc.p.x        = rset->get<float>("pos_x");
        PChar->loc.p.y        = rset->get<float>("pos_y");
        PChar->loc.p.z        = rset->get<float>("pos_z");
        PChar->m_moghouseID   = rset->get<uint32>("moghouse");
        PChar->loc.boundary   = rset->get<uint16>("boundary");
        PChar->accid          = rset->get<uint32>("accid");

        PChar->profile.home_point.destination = rset->get<uint16>("home_zone");
        PChar->profile.home_point.p.rotation  = rset->get<uint8>("home_rot");
        PChar->profile.home_point.p.x         = rset->get<float>("home_x");
        PChar->profile.home_point.p.y         = rset->get<float>("home_y");
        PChar->profile.home_point.p.z         = rset->get<float>("home_z");

        PChar->profile.nation = rset->get<uint8>("nation");

        db::extractFromBlob(rset, "quests", PChar->m_questLog);
        db::extractFromBlob(rset, "keyitems", PChar->keys);
        db::extractFromBlob(rset, "abilities", PChar->m_LearnedAbilities);
        db::extractFromBlob(rset, "weaponskills", PChar->m_LearnedWeaponskills);
        db::extractFromBlob(rset, "titles", PChar->m_TitleList);
        db::extractFromBlob(rset, "zones", PChar->m_ZonesVisitedList);
        db::extractFromBlob(rset, "missions", PChar->m_missionLog);
        db::extractFromBlob(rset, "assault", PChar->m_assaultLog);
        db::extractFromBlob(rset, "campaign", PChar->m_campaignLog);
        db::extractFromBlob(rset, "eminence", PChar->m_eminenceLog);

        PChar->SetPlayTime(std::chrono::seconds(rset->get<uint32>("playtime")));
        PChar->profile.campaign_allegiance = rset->get<uint8>("campaign_allegiance");
        PChar->setStyleLocked(rset->get<uint32>("isstylelocked") == 1);
        PChar->SetMoghancement(rset->get<uint16>("moghancement"));
        PChar->lastOnline      = earth_time::time_point(std::chrono::seconds(rset->get<uint32>("lastonline")));
        PChar->search.language = rset->get<uint8>("languages");

        PChar->m_GMlevel          = rset->get<uint8>("gmlevel");
        PChar->m_jobMasterDisplay = rset->get<uint32>("job_master") > 0;

        const auto playerSettings = rset->get<uint32>("settings");
        const auto MessageFilter  = rset->get<uint32>("chatfilters_1");
        const auto MessageFilter2 = rset->get<uint32>("chatfilters_2");

        std::memcpy(&PChar->playerConfig, &playerSettings, sizeof(uint32_t));
        std::memcpy(&PChar->playerConfig.MessageFilter, &MessageFilter, sizeof(uint32_t));
        std::memcpy(&PChar->playerConfig.MessageFilter2, &MessageFilter2, sizeof(uint32_t));
    }

    // TODO: Rename LoadFromCharSpellsSQL
    LoadSpells(PChar);

    // TODO: LoadFromCharProfileSQL
    fmtQuery = "SELECT "
               "rank_points,"
               "rank_sandoria,"
               "rank_bastok,"
               "rank_windurst,"
               "fame_sandoria,"
               "fame_bastok,"
               "fame_windurst,"
               "fame_norg, "
               "fame_jeuno, "
               "fame_aby_konschtat, "
               "fame_aby_tahrongi, "
               "fame_aby_latheine, "
               "fame_aby_misareaux, "
               "fame_aby_vunkerl, "
               "fame_aby_attohwa, "
               "fame_aby_altepa, "
               "fame_aby_grauberg, "
               "fame_aby_uleguerand, "
               "fame_adoulin,"
               "unity_leader "
               "FROM char_profile "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->profile.rankpoints = rset->get<uint16>("rank_points");

        PChar->profile.rank[0] = rset->get<uint8>("rank_sandoria");
        PChar->profile.rank[1] = rset->get<uint8>("rank_bastok");
        PChar->profile.rank[2] = rset->get<uint8>("rank_windurst");

        PChar->profile.fame[0]      = rset->get<uint16>("fame_sandoria");
        PChar->profile.fame[1]      = rset->get<uint16>("fame_bastok");
        PChar->profile.fame[2]      = rset->get<uint16>("fame_windurst");
        PChar->profile.fame[3]      = rset->get<uint16>("fame_norg");
        PChar->profile.fame[4]      = rset->get<uint16>("fame_jeuno");
        PChar->profile.fame[5]      = rset->get<uint16>("fame_aby_konschtat");
        PChar->profile.fame[6]      = rset->get<uint16>("fame_aby_tahrongi");
        PChar->profile.fame[7]      = rset->get<uint16>("fame_aby_latheine");
        PChar->profile.fame[8]      = rset->get<uint16>("fame_aby_misareaux");
        PChar->profile.fame[9]      = rset->get<uint16>("fame_aby_vunkerl");
        PChar->profile.fame[10]     = rset->get<uint16>("fame_aby_attohwa");
        PChar->profile.fame[11]     = rset->get<uint16>("fame_aby_altepa");
        PChar->profile.fame[12]     = rset->get<uint16>("fame_aby_grauberg");
        PChar->profile.fame[13]     = rset->get<uint16>("fame_aby_uleguerand");
        PChar->profile.fame[14]     = rset->get<uint16>("fame_adoulin");
        PChar->profile.unity_leader = rset->get<uint8>("unity_leader");
    }

    roeutils::onCharLoad(PChar);

    // TODO: LoadFromCharStorageSQL
    fmtQuery = "SELECT "
               "inventory,"
               "safe,"
               "locker,"
               "satchel,"
               "sack,"
               "`case`,"
               "wardrobe,"
               "wardrobe2,"
               "wardrobe3,"
               "wardrobe4,"
               "wardrobe5,"
               "wardrobe6,"
               "wardrobe7,"
               "wardrobe8 "
               "FROM char_storage "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->getStorage(LOC_INVENTORY)->AddBuff(rset->get<uint8>("inventory"));
        PChar->getStorage(LOC_MOGSAFE)->AddBuff(rset->get<uint8>("safe"));
        PChar->getStorage(LOC_MOGSAFE2)->AddBuff(rset->get<uint8>("safe"));
        PChar->getStorage(LOC_TEMPITEMS)->AddBuff(50);
        PChar->getStorage(LOC_MOGLOCKER)->AddBuff(rset->get<uint8>("locker"));
        PChar->getStorage(LOC_MOGSATCHEL)->AddBuff(rset->get<uint8>("satchel"));
        PChar->getStorage(LOC_MOGSACK)->AddBuff(rset->get<uint8>("sack"));
        PChar->getStorage(LOC_MOGCASE)->AddBuff(rset->get<uint8>("case"));

        PChar->getStorage(LOC_WARDROBE)->AddBuff(rset->get<uint8>("wardrobe"));
        PChar->getStorage(LOC_WARDROBE2)->AddBuff(rset->get<uint8>("wardrobe2"));
        PChar->getStorage(LOC_WARDROBE3)->AddBuff(rset->get<uint8>("wardrobe3"));
        PChar->getStorage(LOC_WARDROBE4)->AddBuff(rset->get<uint8>("wardrobe4"));

        PChar->getStorage(LOC_WARDROBE5)->AddBuff(rset->get<uint8>("wardrobe5"));
        PChar->getStorage(LOC_WARDROBE6)->AddBuff(rset->get<uint8>("wardrobe6"));
        PChar->getStorage(LOC_WARDROBE7)->AddBuff(rset->get<uint8>("wardrobe7"));
        PChar->getStorage(LOC_WARDROBE8)->AddBuff(rset->get<uint8>("wardrobe8"));

        // NOTE: Not from the db, hard-coded to 10!
        PChar->getStorage(LOC_RECYCLEBIN)->AddBuff(10);
    }

    // TODO: LoadFromCharLookSQL
    fmtQuery = "SELECT face, race, size, head, body, hands, legs, feet, main, sub, ranged "
               "FROM char_look "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->look.face = rset->get<uint8>("face");
        PChar->look.race = rset->get<uint8>("race");
        PChar->look.size = rset->get<uint8>("size");

        PChar->look.head   = rset->get<uint16>("head");
        PChar->look.body   = rset->get<uint16>("body");
        PChar->look.hands  = rset->get<uint16>("hands");
        PChar->look.legs   = rset->get<uint16>("legs");
        PChar->look.feet   = rset->get<uint16>("feet");
        PChar->look.main   = rset->get<uint16>("main");
        PChar->look.sub    = rset->get<uint16>("sub");
        PChar->look.ranged = rset->get<uint16>("ranged");

        std::memcpy(&PChar->mainlook, &PChar->look, sizeof(PChar->look));
    }

    // Model size doesn't matter here per caps, only race
    // From the packets, size is 4/3/8 as integer
    // For distance purposes, these are divided by 10
    switch (static_cast<CharRace>(PChar->look.race))
    {
        case CharRace::HumeMale:
        case CharRace::HumeFemale:
            PChar->modelHitboxSize = 4.0f / 10.0f;
            break;
        case CharRace::ElvaanMale:
        case CharRace::ElvaanFemale:
            PChar->modelHitboxSize = 4.0f / 10.0f;
            break;
        case CharRace::TarutaruMale:
        case CharRace::TarutaruFemale:
            PChar->modelHitboxSize = 3.0f / 10.0f;
            break;
        case CharRace::Mithra:
            PChar->modelHitboxSize = 4.0f / 10.0f;
            break;
        case CharRace::Galka:
            PChar->modelHitboxSize = 8.0f / 10.0f;
            break;
        default:
            PChar->modelHitboxSize = 4.0f / 10.0f;
            break;
    }

    // LoadFromCharStyleSQL
    fmtQuery = "SELECT head, body, hands, legs, feet, main, sub, ranged FROM char_style WHERE charid = ?";
    rset     = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->styleItems[SLOT_HEAD]   = rset->get<uint16>("head");
        PChar->styleItems[SLOT_BODY]   = rset->get<uint16>("body");
        PChar->styleItems[SLOT_HANDS]  = rset->get<uint16>("hands");
        PChar->styleItems[SLOT_LEGS]   = rset->get<uint16>("legs");
        PChar->styleItems[SLOT_FEET]   = rset->get<uint16>("feet");
        PChar->styleItems[SLOT_MAIN]   = rset->get<uint16>("main");
        PChar->styleItems[SLOT_SUB]    = rset->get<uint16>("sub");
        PChar->styleItems[SLOT_RANGED] = rset->get<uint16>("ranged");
    }

    // LoadFromCharJobsSQL
    fmtQuery = "SELECT unlocked, genkai, war, mnk, whm, blm, rdm, thf, pld, drk, bst, brd, rng, sam, nin, drg, smn, blu, cor, pup, dnc, sch, geo, run "
               "FROM char_jobs "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->jobs.unlocked = rset->get<uint32>("unlocked");
        PChar->jobs.genkai   = rset->get<uint8>("genkai");

        PChar->jobs.job[JOB_WAR] = rset->get<uint8>("war");
        PChar->jobs.job[JOB_MNK] = rset->get<uint8>("mnk");
        PChar->jobs.job[JOB_WHM] = rset->get<uint8>("whm");
        PChar->jobs.job[JOB_BLM] = rset->get<uint8>("blm");
        PChar->jobs.job[JOB_RDM] = rset->get<uint8>("rdm");
        PChar->jobs.job[JOB_THF] = rset->get<uint8>("thf");
        PChar->jobs.job[JOB_PLD] = rset->get<uint8>("pld");
        PChar->jobs.job[JOB_DRK] = rset->get<uint8>("drk");
        PChar->jobs.job[JOB_BST] = rset->get<uint8>("bst");
        PChar->jobs.job[JOB_BRD] = rset->get<uint8>("brd");
        PChar->jobs.job[JOB_RNG] = rset->get<uint8>("rng");
        PChar->jobs.job[JOB_SAM] = rset->get<uint8>("sam");
        PChar->jobs.job[JOB_NIN] = rset->get<uint8>("nin");
        PChar->jobs.job[JOB_DRG] = rset->get<uint8>("drg");
        PChar->jobs.job[JOB_SMN] = rset->get<uint8>("smn");
        PChar->jobs.job[JOB_BLU] = rset->get<uint8>("blu");
        PChar->jobs.job[JOB_COR] = rset->get<uint8>("cor");
        PChar->jobs.job[JOB_PUP] = rset->get<uint8>("pup");
        PChar->jobs.job[JOB_DNC] = rset->get<uint8>("dnc");
        PChar->jobs.job[JOB_SCH] = rset->get<uint8>("sch");
        PChar->jobs.job[JOB_GEO] = rset->get<uint8>("geo");
        PChar->jobs.job[JOB_RUN] = rset->get<uint8>("run");
    }

    // LoadFromCharExpSQL
    fmtQuery = "SELECT mode, war, mnk, whm, blm, rdm, thf, pld, drk, bst, brd, rng, sam, nin, drg, smn, blu, cor, pup, dnc, sch, geo, run, merits, limits "
               "FROM char_exp "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->MeritMode = rset->get<uint8>("mode");

        PChar->jobs.exp[JOB_WAR] = rset->get<uint16>("war");
        PChar->jobs.exp[JOB_MNK] = rset->get<uint16>("mnk");
        PChar->jobs.exp[JOB_WHM] = rset->get<uint16>("whm");
        PChar->jobs.exp[JOB_BLM] = rset->get<uint16>("blm");
        PChar->jobs.exp[JOB_RDM] = rset->get<uint16>("rdm");
        PChar->jobs.exp[JOB_THF] = rset->get<uint16>("thf");
        PChar->jobs.exp[JOB_PLD] = rset->get<uint16>("pld");
        PChar->jobs.exp[JOB_DRK] = rset->get<uint16>("drk");
        PChar->jobs.exp[JOB_BST] = rset->get<uint16>("bst");
        PChar->jobs.exp[JOB_BRD] = rset->get<uint16>("brd");
        PChar->jobs.exp[JOB_RNG] = rset->get<uint16>("rng");
        PChar->jobs.exp[JOB_SAM] = rset->get<uint16>("sam");
        PChar->jobs.exp[JOB_NIN] = rset->get<uint16>("nin");
        PChar->jobs.exp[JOB_DRG] = rset->get<uint16>("drg");
        PChar->jobs.exp[JOB_SMN] = rset->get<uint16>("smn");
        PChar->jobs.exp[JOB_BLU] = rset->get<uint16>("blu");
        PChar->jobs.exp[JOB_COR] = rset->get<uint16>("cor");
        PChar->jobs.exp[JOB_PUP] = rset->get<uint16>("pup");
        PChar->jobs.exp[JOB_DNC] = rset->get<uint16>("dnc");
        PChar->jobs.exp[JOB_SCH] = rset->get<uint16>("sch");
        PChar->jobs.exp[JOB_GEO] = rset->get<uint16>("geo");
        PChar->jobs.exp[JOB_RUN] = rset->get<uint16>("run");

        meritPoints = rset->get<uint8>("merits");
        limitPoints = rset->get<uint16>("limits");
    }

    // TODO: LoadFromCharStatsSQL
    fmtQuery = "SELECT mjob, sjob, hp, mp, mhflag, title, bazaar_message, zoning, "
               "pet_id, pet_type, pet_hp, pet_mp, pet_level "
               "FROM char_stats WHERE charid = ?";

    uint8 zoning = 0;
    rset         = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->SetMJob(rset->get<uint8>("mjob"));
        PChar->SetSJob(rset->get<uint8>("sjob"));

        HP = rset->get<int32>("hp");
        MP = rset->get<int32>("mp");

        PChar->profile.mhflag = rset->get<uint16>("mhflag");
        PChar->profile.title  = rset->get<uint16>("title");

        std::array<uint8, 512> bazaarMessageArray{};
        db::extractFromBlob(rset, "bazaar_message", bazaarMessageArray);
        const char* bazaarMessageStr = reinterpret_cast<const char*>(bazaarMessageArray.data());
        if (bazaarMessageStr != nullptr)
        {
            PChar->bazaar.message.insert(0, bazaarMessageStr);
        }
        else
        {
            PChar->bazaar.message = '\0';
        }

        zoning = rset->get<uint8>("zoning");

        // Determine if the pet should be respawned.
        int16 petHP = rset->get<int16>("pet_hp");
        if (petHP)
        {
            PChar->petZoningInfo.petHP        = petHP;
            PChar->petZoningInfo.petID        = rset->get<uint8>("pet_id");
            PChar->petZoningInfo.petMP        = rset->get<int16>("pet_mp");
            PChar->petZoningInfo.petType      = rset->get<PET_TYPE>("pet_type");
            PChar->petZoningInfo.petLevel     = rset->get<uint8>("pet_level");
            PChar->petZoningInfo.respawnPet   = true;
            auto jugTimestamp                 = static_cast<uint32>(PChar->getCharVar("jugpet-spawn-time"));
            PChar->petZoningInfo.jugSpawnTime = timer::from_utc(earth_time::time_point(std::chrono::seconds(jugTimestamp)));
            PChar->petZoningInfo.jugDuration  = std::chrono::seconds(PChar->getCharVar("jugpet-duration-seconds"));

            // clear the charvars used for jug state
            PChar->clearCharVarsWithPrefix("jugpet-");
        }
    }

    db::preparedStmt("UPDATE char_stats SET zoning = 0 WHERE charid = ? LIMIT 1", PChar->id);

    if (zoning == 2)
    {
        ShowDebug("Player <%s> logging in to zone <%u>", PChar->name.c_str(), PChar->getZone());

        // Set this value so we can not process some effects until the player is fully in-game.
        // This is cleared in the player global, onGameIn function.
        PChar->SetLocalVar("gameLogin", 1);
    }

    PChar->SetMLevel(PChar->jobs.job[PChar->GetMJob()]);
    PChar->SetSLevel(PChar->jobs.job[PChar->GetSJob()]);

    // TODO: LoadFromCharRecastSQL
    fmtQuery = "SELECT id, time, recast FROM char_recast WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            auto            now        = timer::now();
            auto            cast_time  = timer::from_utc(earth_time::time_point(std::chrono::seconds(rset->get<uint32>("time"))));
            auto            recast     = std::chrono::seconds(rset->get<uint32>("recast"));
            timer::duration chargeTime = 0s;
            uint8           maxCharges = 0;
            Charge_t*       charge     = ability::GetCharge(PChar, rset->get<uint32>("id"));
            if (charge != nullptr)
            {
                chargeTime = charge->chargeTime;
                maxCharges = charge->maxCharges;
            }
            if (now < cast_time + recast)
            {
                PChar->PRecastContainer->Load(RECAST_ABILITY, rset->get<Recast>("id"), (cast_time + recast - now), chargeTime, maxCharges);
            }
        }
    }

    // TODO: LoadFromCharSkillsSQL
    fmtQuery = "SELECT skillid, value, rank "
               "FROM char_skills "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            uint8 SkillID = rset->get<uint8>("skillid");
            if (SkillID < MAX_SKILLTYPE)
            {
                PChar->RealSkills.skill[SkillID] = rset->get<uint16>("value");
                if (SkillID >= SKILL_FISHING)
                {
                    PChar->RealSkills.rank[SkillID] = rset->get<uint8>("rank");
                }
            }
        }
    }

    // LoadFromCharUnlocksSQL
    fmtQuery = "SELECT outpost_sandy, outpost_bastok, outpost_windy, runic_portal, maw, "
               "campaign_sandy, campaign_bastok, campaign_windy, homepoints, survivals, "
               "abyssea_conflux, waypoints, eschan_portals, claimed_deeds, unique_event, "
               "maze_vouchers, maze_runes "
               "FROM char_unlocks "
               "WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->teleport.outpostSandy   = rset->get<uint32>("outpost_sandy");
        PChar->teleport.outpostBastok  = rset->get<uint32>("outpost_bastok");
        PChar->teleport.outpostWindy   = rset->get<uint32>("outpost_windy");
        PChar->teleport.runicPortal    = rset->get<uint32>("runic_portal");
        PChar->teleport.pastMaw        = rset->get<uint32>("maw");
        PChar->teleport.campaignSandy  = rset->get<uint32>("campaign_sandy");
        PChar->teleport.campaignBastok = rset->get<uint32>("campaign_bastok");
        PChar->teleport.campaignWindy  = rset->get<uint32>("campaign_windy");

        db::extractFromBlob(rset, "homepoints", PChar->teleport.homepoint);
        db::extractFromBlob(rset, "survivals", PChar->teleport.survival);
        db::extractFromBlob(rset, "abyssea_conflux", PChar->teleport.abysseaConflux);
        db::extractFromBlob(rset, "waypoints", PChar->teleport.waypoints);
        db::extractFromBlob(rset, "eschan_portals", PChar->teleport.eschanPortal);
        db::extractFromBlob(rset, "claimed_deeds", PChar->m_claimedDeeds);
        db::extractFromBlob(rset, "unique_event", PChar->m_uniqueEvents);
        db::extractFromBlob(rset, "maze_vouchers", PChar->maze().vouchers);
        db::extractFromBlob(rset, "maze_runes", PChar->maze().runes);
    }

    // TODO: Remove raw new's
    PChar->PMeritPoints = std::make_unique<CMeritPoints>(PChar);
    PChar->PMeritPoints->SetMeritPoints(meritPoints);
    PChar->PMeritPoints->SetLimitPoints(limitPoints);
    PChar->PJobPoints = std::make_unique<CJobPoints>(PChar);

    rset = db::preparedStmt("SELECT field_chocobo FROM char_pet WHERE charid = ?", PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        PChar->m_FieldChocobo = rset->get<uint32>("field_chocobo");
    }

    // TODO: LoadCharFlagsFromSQL
    fmtQuery = "SELECT gmModeEnabled, gmHiddenEnabled FROM char_flags WHERE charid = ?";

    rset = db::preparedStmt(fmtQuery, PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        bool gmEnabled = rset->get<uint32>("gmModeEnabled");
        bool gmHidden  = rset->get<uint32>("gmHiddenEnabled");

        if (gmEnabled)
        {
            // + 3 because visible GM level starts at 3 (0 is none, 1-2 are special icons)
            PChar->visibleGmLevel = std::min(PChar->m_GMlevel + 3, 7);
        }

        PChar->m_isGMHidden = gmHidden;
    }

    monstrosity::TryPopulateMonstrosityData(PChar);

    charutils::LoadInventory(PChar);

    CalculateStats(PChar);
    jobpointutils::RefreshGiftMods(PChar);

    // This must come after refreshing gift modifiers, but before loading job traits.
    blueutils::LoadSetSpells(PChar);

    BuildingCharSkillsTable(PChar);
    BuildingCharAbilityTable(PChar);
    BuildingCharTraitsTable(PChar);

    // Order matters as this uses merits and JP gifts.
    puppetutils::LoadAutomaton(PChar);

    PChar->animation = (HP == 0 ? ANIMATION_DEATH : ANIMATION_NONE);

    PChar->StatusEffectContainer->LoadStatusEffects();

    charutils::LoadEquip(PChar);
    charutils::EmptyRecycleBin(PChar);
    bool canRestore  = zoneutils::IsResidentialArea(PChar) && HP > 0;
    PChar->health.hp = canRestore ? PChar->GetMaxHP() : HP;
    PChar->health.mp = canRestore ? PChar->GetMaxMP() : MP;
    PChar->UpdateHealth();

    // Lazy loading: ensure initial zone is loaded synchronously before OnZoneIn
    // TODO: Hoist his block out of LoadChar() so we're guaranteeing that a char's zone exists
    //     : before we try to put them in it.
    if (zoneutils::IsLazyLoadingEnabled() && !zoneutils::GetZone(PChar->loc.destination))
    {
        // TODO: Remove this usage of blockOnMain, it's here to help with xi_test
        scheduler.blockOnMainThread(zoneutils::LoadZones(scheduler, config, { PChar->loc.destination }));
    }

    luautils::OnZoneIn(PChar);
    luautils::OnGameIn(PChar, zoning == 1);

    PChar->status = STATUS_TYPE::DISAPPEAR;

    return charEntity;
}

void LoadSpells(CCharEntity* PChar)
{
    TracyZoneScoped;

    // disable all spells
    PChar->m_SpellList.reset();

    // Compile a list of all enabled expansions
    std::vector<std::string> enabledExpansions;
    for (const auto& expansion : { "ROTZ", "COP", "TOAU", "WOTG", "ACP", "AMK", "ASA", "ABYSSEA", "SOA", "ROV", "TVR", "VOIDWATCH" })
    {
        if (luautils::IsContentEnabled(expansion))
        {
            enabledExpansions.push_back(fmt::format("\"{}\"", expansion));
        }
    }

    std::string condition = "spell_list.content_tag IS NULL";

    if (!enabledExpansions.empty())
    {
        condition = fmt::format("spell_list.content_tag IN ({}) OR spell_list.content_tag IS NULL", fmt::join(enabledExpansions, ","));
    }

    // Select all player spells from enabled expansions
    //
    // NOTE: We normally don't want to build a prepared statement with fmt::format,
    //     : but this query is entirely internal, so it's OK.
    auto query = fmt::format("SELECT char_spells.spellid "
                             "FROM char_spells "
                             "JOIN spell_list "
                             "ON spell_list.spellid = char_spells.spellid "
                             "WHERE charid = ? AND ({})",
                             condition);

    auto rset = db::preparedStmt(query, PChar->id);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            uint16 spellId = rset->get<uint16>("spellid");
            if (spell::GetSpell(static_cast<SpellID>(spellId)) != nullptr)
            {
                PChar->m_SpellList.set(spellId);
            }
        }
    }

    // Handle trust spells that are enabled via settings.
    bool hasTrustPermit =
        charutils::hasKeyItem(PChar, KeyItem::WINDURST_TRUST_PERMIT) ||
        charutils::hasKeyItem(PChar, KeyItem::BASTOK_TRUST_PERMIT) ||
        charutils::hasKeyItem(PChar, KeyItem::SAN_DORIA_TRUST_PERMIT);

    if (hasTrustPermit)
    {
        static const std::unordered_map<uint8, uint16> trustSpells = {
            { 1, 1002 }, // Cornelia
            { 2, 1003 }, // Matsui-P
        }; // This can be expanded if more trust spells are added as settings options.

        uint8 trustSetting = settings::get<uint8>("main.ENABLE_LIMITED_TIME_TRUST");

        auto it = trustSpells.find(trustSetting);
        if (it != trustSpells.end())
        {
            PChar->m_SpellList.set(it->second);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Download Character inventory                                         *
 *                                                                       *
 ************************************************************************/

void LoadInventory(CCharEntity* PChar)
{
    TracyZoneScoped;

    const char* query = "SELECT "
                        "itemid, "
                        "location, "
                        "slot, "
                        "quantity, "
                        "bazaar, "
                        "signature, "
                        "extra "
                        "FROM char_inventory "
                        "WHERE charid = ? "
                        "ORDER BY FIELD(location,0,1,9,2,3,4,5,6,7,8,10,11,12)";

    auto rset = db::preparedStmt(query, PChar->id);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            auto PItem = xi::items::spawn(rset->get<uint16>("itemid"));
            if (PItem != nullptr)
            {
                PItem->setLocationID(rset->get<uint8>("location"));
                PItem->setSlotID(rset->get<uint8>("slot"));
                PItem->setQuantity(rset->get<uint32>("quantity"));
                PItem->setCharPrice(rset->get<uint32>("bazaar"));

                db::extractFromBlob(rset, "extra", PItem->m_extra);

                if (PItem->getCharPrice() != 0)
                {
                    PItem->setSubType(ITEM_LOCKED);
                }

                if (PItem->isType(ITEM_LINKSHELL))
                {
                    auto* PLink = static_cast<CItemLinkshell*>(PItem.get());
                    if (PLink->GetLSType() == 0)
                    {
                        PLink->SetLSType((LSTYPE)(PItem->getID() - 0x200));
                    }
                    PItem->setSignature(rset->get<std::string>("signature"));
                }
                else if (PItem->hasFlag(ItemFlag::Inscribable))
                {
                    PItem->setSignature(rset->get<std::string>("signature"));
                }

                if (auto* PItemUsable = dynamic_cast<CItemUsable*>(PItem.get()))
                {
                    uint32 useTime = 0;
                    std::memcpy(&useTime, PItemUsable->m_extra + 0x04, sizeof(useTime));
                    if (useTime != 0)
                    {
                        PItemUsable->setLastUseTime(timer::now() - std::chrono::seconds(earth_time::vanadiel_timestamp() - useTime));
                    }
                }

                if (PItem->isType(ITEM_FURNISHING) && (PItem->getLocationID() == LOC_MOGSAFE || PItem->getLocationID() == LOC_MOGSAFE2))
                {
                    if (static_cast<CItemFurnishing*>(PItem.get())->isInstalled()) // Check if furniture (furnishing) item is actually installed
                    {
                        PChar->getStorage(LOC_STORAGE)->AddBuff(static_cast<CItemFurnishing*>(PItem.get())->getStorage());
                    }
                }
                const uint8 locID  = PItem->getLocationID();
                const uint8 slotID = PItem->getSlotID();
                PChar->getStorage(locID)->InsertItem(std::move(PItem), slotID);
            }
        }
    }

    // apply augments
    // loop over each container
    for (uint8 i = 0; i < CONTAINER_ID::MAX_CONTAINER_ID; ++i)
    {
        CItemContainer* PItemContainer = PChar->getStorage(i);

        if (PItemContainer != nullptr)
        {
            // now find each item in the container
            for (uint8 y = 0; y < MAX_CONTAINER_SIZE; ++y)
            {
                CItem* PItem = PItemContainer->GetItem(y);

                // check if the item is valid and can have an augment applied to it
                if (PItem != nullptr && ((PItem->isType(ITEM_EQUIPMENT) || PItem->isType(ITEM_WEAPON)) && !PItem->isSubType(ITEM_CHARGED)))
                {
                    // check if there are any valid augments to be applied to the item
                    for (uint8 j = 0; j < 4; ++j)
                    {
                        // found a match, apply the augment
                        if (((CItemEquipment*)PItem)->getAugment(j) != 0)
                        {
                            ((CItemEquipment*)PItem)->ApplyAugment(j);
                        }
                    }
                }
            }
        }
    }
}

void LoadEquip(CCharEntity* PChar)
{
    TracyZoneScoped;

    const char* Query = "SELECT "
                        "slotid,"
                        "equipslotid,"
                        "containerid "
                        "FROM char_equip "
                        "WHERE charid = ?";

    auto rset = db::preparedStmt(Query, PChar->id);
    if (rset)
    {
        // equipSlotData[equipSlotId] = { slotId, containerId }
        std::map<uint8, std::pair<uint8, uint8>> equipSlotData;

        // NOTE: This data is stored in the above map since if the item has an augment, another db
        // query will occur, which will destroy the current query results.
        while (rset->next())
        {
            uint8 equipSlotId          = rset->get<uint8>("equipslotid");
            uint8 slotId               = rset->get<uint8>("slotid");
            uint8 containerId          = rset->get<uint8>("containerid");
            equipSlotData[equipSlotId] = { slotId, containerId };
        }

        CItemLinkshell* PLinkshell1   = nullptr;
        CItemLinkshell* PLinkshell2   = nullptr;
        bool            hasMainWeapon = false;

        for (const auto& [equipSlotId, inventoryLoc] : equipSlotData)
        {
            if (equipSlotId < 16)
            {
                if (equipSlotId == SLOT_MAIN)
                {
                    hasMainWeapon = true;
                }

                EquipItem(PChar, inventoryLoc.first, equipSlotId, inventoryLoc.second);
            }
            else
            {
                CItem* PItem = PChar->getStorage(inventoryLoc.second)->GetItem(inventoryLoc.first);

                if ((PItem != nullptr) && PItem->isType(ITEM_LINKSHELL))
                {
                    PItem->setSubType(ITEM_LOCKED);
                    if (!PChar->bindEquip(equipSlotId, PItem))
                    {
                        continue;
                    }

                    if (equipSlotId == SLOT_LINK1)
                    {
                        PLinkshell1 = (CItemLinkshell*)PItem;
                    }
                    else if (equipSlotId == SLOT_LINK2)
                    {
                        PLinkshell2 = (CItemLinkshell*)PItem;
                    }
                }
            }
        }

        // If no weapon is equipped, equip the appropriate unarmed weapon item
        if (!hasMainWeapon)
        {
            CheckUnarmedWeapon(PChar);
        }

        if (PLinkshell1)
        {
            rset = db::preparedStmt("SELECT broken FROM linkshells WHERE linkshellid = ? LIMIT 1", PLinkshell1->GetLSID());
            if (rset && rset->rowsCount() && rset->next() && rset->get<uint32>("broken") == 1)
            { // if the linkshell has been broken, unequip
                uint8 SlotID     = PLinkshell1->getSlotID();
                uint8 LocationID = PLinkshell1->getLocationID();
                PLinkshell1->setSubType(ITEM_UNLOCKED);
                PChar->clearEquip(SLOT_LINK1);
                db::preparedStmt("DELETE char_equip FROM char_equip WHERE charid = ? AND slotid = ? AND containerid = ? LIMIT 1",
                                 PChar->id,
                                 SlotID,
                                 LocationID);
            }
            else
            {
                linkshell::AddOnlineMember(PChar, PLinkshell1, 1);
            }
        }

        if (PLinkshell2)
        {
            rset = db::preparedStmt("SELECT broken FROM linkshells WHERE linkshellid = ? LIMIT 1", PLinkshell2->GetLSID());
            if (rset && rset->rowsCount() && rset->next() && rset->get<uint32>("broken") == 1)
            { // if the linkshell has been broken, unequip
                uint8 SlotID     = PLinkshell2->getSlotID();
                uint8 LocationID = PLinkshell2->getLocationID();
                PLinkshell2->setSubType(ITEM_UNLOCKED);
                PChar->clearEquip(SLOT_LINK2);
                db::preparedStmt("DELETE char_equip FROM char_equip WHERE charid = ? AND slotid = ? AND containerid = ? LIMIT 1",
                                 PChar->id,
                                 SlotID,
                                 LocationID);
            }
            else
            {
                linkshell::AddOnlineMember(PChar, PLinkshell2, 2);
            }
        }
    }
    else
    {
        ShowError("Loading error from char_equip");
    }

    // Fill in the unarmed psuedo-weapons if no item was equipped
    if (PChar->m_Weapons[SLOT_MAIN] == nullptr)
    {
        CheckUnarmedWeapon(PChar);
    }
}

/************************************************************************
 *                                                                       *
 *  Send lists of current / completed quests and missions.               *
 *                                                                       *
 ************************************************************************/

void SendQuestMissionLog(CCharEntity* PChar)
{
    // Actual verified retail order.
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Sandoria);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Bastok);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Windurst);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Jeuno);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::OtherAreas);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Outlands);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::AhtUrghan);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::CrystalWar);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Sandoria);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Bastok);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Windurst);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Jeuno);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::OtherAreas);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Outlands);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::AhtUrghan);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::CrystalWar);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::Nations);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::ToAU_WoTG);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::Campaign1);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::Campaign2);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Abyssea);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Abyssea);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Adoulin);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Adoulin);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Coalition);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Coalition);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::MISSION>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISSION::TVR>(PChar);
}

void SendPartialMissionLog(CCharEntity* PChar, const MissionLog log, const bool completed)
{
    switch (log)
    {
        case MissionLog::Sandoria:
        case MissionLog::Bastok:
        case MissionLog::Windurst:
        case MissionLog::Zilart:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::Nations)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::MISSION>(PChar);
            break;
        }
        case MissionLog::ToAU:
        case MissionLog::WoTG:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::ToAU_WoTG)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::AhtUrghan);
            break;
        }
        case MissionLog::Assault:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::AhtUrghan)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::AhtUrghan);
            break;
        }
        case MissionLog::Campaign:
        {
            if (completed)
            {
                PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::Campaign1);
                PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, MissionComplete::Campaign2);
            }
            else
            {
                // Not a typo...
                PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::AhtUrghan);
            }
            break;
        }
        case MissionLog::CoP:
        case MissionLog::ACP:
        case MissionLog::AMK:
        case MissionLog::ASA:
        case MissionLog::SoA:
        case MissionLog::RoV:
        {
            // These expansions store both completed and in-progress in the same structure
            PChar->pushPacket<GP_SERV_COMMAND_MISSION::MISSION>(PChar);
            break;
        }
    }
}

void SendPartialQuestLog(CCharEntity* PChar, const QuestLog log, const bool completed)
{
    switch (log)
    {
        case QuestLog::Sandoria:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Sandoria)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Sandoria);
            break;
        }
        case QuestLog::Bastok:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Bastok)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Bastok);
            break;
        }
        case QuestLog::Windurst:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Windurst)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Windurst);
            break;
        }
        case QuestLog::Jeuno:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Jeuno)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Jeuno);
            break;
        }
        case QuestLog::OtherAreas:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::OtherAreas)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::OtherAreas);
            break;
        }
        case QuestLog::Outlands:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Outlands)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Outlands);
            break;
        }
        case QuestLog::AhtUrghan:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::AhtUrghan)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::AhtUrghan);
            break;
        }
        case QuestLog::CrystalWar:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::CrystalWar)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::CrystalWar);
            break;
        }
        case QuestLog::Abyssea:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Abyssea)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Abyssea);
            break;
        }
        case QuestLog::Adoulin:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Adoulin)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Adoulin);
            break;
        }
        case QuestLog::Coalition:
        {
            completed ? PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestComplete::Coalition)
                      : PChar->pushPacket<GP_SERV_COMMAND_MISSION::OTHER>(PChar, QuestOffer::Coalition);
            break;
        }
    }
}

void SendRecordsOfEminenceLog(CCharEntity* PChar)
{
    // Send spark updates
    PChar->pushPacket<GP_SERV_COMMAND_UNITY>(PChar);

    if (settings::get<bool>("main.ENABLE_ROE"))
    {
        // Current RoE quests
        PChar->pushPacket<GP_SERV_COMMAND_ROE_ACTIVELOG>(PChar);

        // Players logging in to a new timed record get one-time message
        if (PChar->m_eminenceCache.notifyTimedRecord)
        {
            PChar->m_eminenceCache.notifyTimedRecord = false;
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, roeutils::GetActiveTimedRecord(), 0, MsgBasic::ROETimed);
        }

        // 4-part Eminence Completion bitmap
        for (int i = 0; i < 4; i++)
        {
            PChar->pushPacket<GP_SERV_COMMAND_ROE_LOG>(PChar, i);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Send lists of character key items                                    *
 *                                                                       *
 ************************************************************************/

void SendKeyItems(CCharEntity* PChar)
{
    for (uint8 table = 0; table < PChar->keys.tables.size(); table++)
    {
        PChar->pushPacket<GP_SERV_COMMAND_SCENARIOITEM>(PChar, table);
    }
}

/************************************************************************
 *                                                                       *
 *  Send the character all its inventory                                 *
 *                                                                       *
 ************************************************************************/

void SendInventory(CCharEntity* PChar)
{
    auto pushContainer = [&](auto LocationID)
    {
        CItemContainer* container = PChar->getStorage(LocationID);
        if (container == nullptr)
        {
            return;
        }

        uint8 size = container->GetSize();
        for (uint8 slotID = 0; slotID <= size; ++slotID)
        {
            CItem* PItem = PChar->getStorage(LocationID)->GetItem(slotID);
            if (PItem != nullptr)
            {
                PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PItem, LocationID, slotID);
            }
        }

        // Mark this container as synced and send ITEM_SAME with updated flags
        PChar->inventorySyncState().markSynced(LocationID);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(LocationID, PChar);
    };

    // Send important items first
    // Note: it's possible that non-essential inventory items are sent in response to another packet

    // Container order based on retail capture
    for (auto&& containerID : { LOC_INVENTORY, LOC_MOGSAFE, LOC_MOGSAFE2, LOC_STORAGE, LOC_RECYCLEBIN, LOC_WARDROBE, LOC_WARDROBE2, LOC_WARDROBE3, LOC_WARDROBE4, LOC_WARDROBE5, LOC_WARDROBE6, LOC_WARDROBE7, LOC_WARDROBE8, LOC_TEMPITEMS, LOC_MOGLOCKER, LOC_MOGSATCHEL, LOC_MOGSACK, LOC_MOGCASE })
    {
        pushContainer(containerID);
    }

    for (int32 i = 0; i < 16; ++i)
    {
        CItem* PItem = PChar->getEquip((SLOTTYPE)i);
        if (PItem != nullptr)
        {
            PItem->setSubType(ITEM_LOCKED);
            PChar->pushPacket<GP_SERV_COMMAND_ITEM_LIST>(PItem, ItemLockFlg::NoDrop);
        }
    }

    CItem* PItem = PChar->getEquip(SLOT_LINK1);
    if (PItem != nullptr)
    {
        PItem->setSubType(ITEM_LOCKED);
        auto eloc1 = PChar->equipLocation(SLOT_LINK1);

        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PItem, *eloc1);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_LIST>(PItem, ItemLockFlg::Linkshell);
        PChar->pushPacket<GP_SERV_COMMAND_GROUP_COMLINK>(PChar, 1);
    }

    PItem = PChar->getEquip(SLOT_LINK2);
    if (PItem != nullptr)
    {
        PItem->setSubType(ITEM_LOCKED);
        auto eloc2 = PChar->equipLocation(SLOT_LINK2);

        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PItem, *eloc2);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_LIST>(PItem, ItemLockFlg::Linkshell);
        PChar->pushPacket<GP_SERV_COMMAND_GROUP_COMLINK>(PChar, 2);
    }

    PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
}

// Sends all 64 Unity ranking packets to the client (0x063 type 0x07)
// Packet sequence:
//   - PreviousWeek (resultSet 0x00): 32 packets (types 0x00-0x1F)
//   - CurrentWeek  (resultSet 0x01): 32 packets (types 0x00-0x1F)
// Client buffers all packets and marks data ready when complete.
// Sent on zone-in and when Unity menu is opened.
// TODO: Some of it needs further research to determine exact values.
void SendUnityPackets(CCharEntity* PChar)
{
    // Query database for unity system data
    const auto rset = db::preparedStmt("SELECT leader, members_current, points_current, members_prev, points_prev "
                                       "FROM unity_system");

    std::pair<int32, double> unity_current[11];
    std::pair<int32, double> unity_previous[11];

    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        auto unity_leader = rset->get<int>("leader") - 1;
        if (unity_leader >= 0 && unity_leader < 11)
        {
            unity_current[unity_leader].first   = rset->get<int32>("members_current");
            unity_current[unity_leader].second  = rset->get<double>("points_current");
            unity_previous[unity_leader].first  = rset->get<int32>("members_prev");
            unity_previous[unity_leader].second = rset->get<double>("points_prev");
        }
    }

    // Previous week (full results)
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::BASE>(UNITY_RESULTSET::PreviousWeek, UNITY_DATATYPE::Base);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::MEMBERS>(UNITY_RESULTSET::PreviousWeek, unity_previous);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::POINTS>(UNITY_RESULTSET::PreviousWeek, unity_previous);
    // Types 0x03-0x0F (empty/flag packets)
    for (int i = 3; i < 0x10; i++)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::BASE>(UNITY_RESULTSET::PreviousWeek, static_cast<UNITY_DATATYPE>(i));
    }
    // Types 0x10-0x1F for PreviousWeek (mostly 0x0008 flags from retail captures)
    for (int i = 0x10; i < 0x20; i++)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::PreviousWeek, i, 0x0008);
    }

    // Current week (partial results)
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::BASE>(UNITY_RESULTSET::CurrentWeek, UNITY_DATATYPE::Base);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::MEMBERS>(UNITY_RESULTSET::CurrentWeek, unity_current);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::POINTS>(UNITY_RESULTSET::CurrentWeek, unity_current);
    // Types 0x03-0x0F (empty/flag packets)
    for (int i = 3; i < 0x10; i++)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::BASE>(UNITY_RESULTSET::CurrentWeek, static_cast<UNITY_DATATYPE>(i));
    }
    // Types 0x10-0x1F for CurrentWeek with appropriate values
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x10, 0x2007);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x11, 0x2CC2);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x12, 0x6867); // ASCII 'gh'
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x13, 0x6E6F); // ASCII 'on'
    // Type 0x14: Personal ranking points (TODO: calculate from player's Unity contributions)
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::PERSONAL>(UNITY_RESULTSET::CurrentWeek, 0);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x15, 0x3605);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x16, 0x2007);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x17, 0x6C6C); // ASCII 'll'
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x18, 0x616E); // ASCII 'na'
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x19, 0x6767); // ASCII 'gg'
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x1A, 0x0000);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x1B, 0x2007);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x1C, 0x2007);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x1D, 0x0022);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x1E, 0x0004);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::UNITY::DATA>(UNITY_RESULTSET::CurrentWeek, 0x1F, 0x2007);
}

// Send relevant 0x044 packets for extended job information (BLU spells, Automaton, Monstrosity)
void SendExtendedJobPackets(CCharEntity* PChar)
{
    if (PChar->m_PMonstrosity)
    {
        PChar->pushPacket<GP_SERV_COMMAND_EXTENDED_JOB::MON>(PChar);
    }
    else
    {
        switch (PChar->GetMJob())
        {
            case JOB_PUP:
            {
                PChar->pushPacket<GP_SERV_COMMAND_EXTENDED_JOB::PUP>(PChar, true);
                break;
            }
            case JOB_BLU:
            {
                PChar->pushPacket<GP_SERV_COMMAND_EXTENDED_JOB::BLU>(PChar, true);
                break;
            }
            default:
                // TODO: Retail actually sends a packet in this case but content is unknown/unused
                break;
        }

        switch (PChar->GetSJob())
        {
            case JOB_PUP:
            {
                PChar->pushPacket<GP_SERV_COMMAND_EXTENDED_JOB::PUP>(PChar, false);
                break;
            }
            case JOB_BLU:
            {
                PChar->pushPacket<GP_SERV_COMMAND_EXTENDED_JOB::BLU>(PChar, false);
                break;
            }
            default:
                // TODO: Retail actually sends a packet in this case but content is unknown/unused
                break;
        }
    }
}

// Server sends a specific set of packets when certain player information change.
void SendLocalPlayerPackets(CCharEntity* PChar)
{
    PChar->pushPacket<GP_SERV_COMMAND_GROUP_ATTR>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS2>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MERITS>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY1>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::JOB_POINTS>(PChar);
}

/************************************************************************
 *                                                                       *
 *  Add a new item to the character in the selected container            *
 *                                                                       *
 ************************************************************************/

uint8 AddItem(CCharEntity* PChar, uint8 LocationID, uint16 ItemID, uint32 quantity, bool silence)
{
    if (tradeitemhelpers::ShouldRejectAddItemEmptyOrZero(PChar->getStorage(LocationID)->GetFreeSlotsCount(), quantity))
    {
        return ERROR_SLOTID;
    }

    auto PItem = xi::items::spawn(ItemID);
    if (tradeitemhelpers::ShouldRejectAddItemMissingDB(PItem != nullptr))
    {
        ShowWarning("AddItem: Item <%i> is not found in a database", ItemID);
        return ERROR_SLOTID;
    }

    PItem->setQuantity(quantity);
    return AddItem(PChar, LocationID, std::move(PItem), silence);
}

/************************************************************************
 *                                                                       *
 *  Add a new item to the character in the selected container            *
 *                                                                       *
 ************************************************************************/

auto AddItem(CCharEntity* PChar, uint8 LocationID, std::unique_ptr<CItem> PItem, bool silence) -> uint8
{
    if (tradeitemhelpers::ShouldUpdateCurrencyInstead(PItem->isType(ITEM_CURRENCY)))
    {
        UpdateItem(PChar, LocationID, tradeitemhelpers::CurrencyInventorySlot, PItem->getQuantity());
        return 0;
    }

    if (tradeitemhelpers::ShouldRejectRareAddItem(PItem->hasFlag(ItemFlag::Rare), HasItem(PChar, PItem->getID())))
    {
        if (!silence)
        {
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(PChar, PItem->getID(), 0, MsgStd::ItemEx);
        }
        return ERROR_SLOTID;
    }

    auto* PStorage = PChar->getStorage(LocationID);
    uint8 SlotID   = PStorage->InsertItem(std::move(PItem));
    if (SlotID == ERROR_SLOTID)
    {
        ShowDebug("AddItem: Location %i is full", LocationID);
        return SlotID;
    }

    auto* PInserted = PStorage->GetItem(SlotID);

    const char* Query = "INSERT INTO char_inventory("
                        "charid, "
                        "location, "
                        "slot, "
                        "itemId, "
                        "quantity, "
                        "signature, "
                        "extra) "
                        "VALUES(?, ?, ?, ?, ?, ?, ?) "
                        "LIMIT 1";

    if (!db::preparedStmt(Query, PChar->id, LocationID, SlotID, PInserted->getID(), PInserted->getQuantity(), PInserted->getSignature(), PInserted->m_extra))
    {
        ShowError("AddItem: Cannot insert item to database");
        PStorage->RemoveItem(SlotID);
        return ERROR_SLOTID;
    }

    PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PInserted, static_cast<CONTAINER_ID>(LocationID), SlotID);
    PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);

    return SlotID;
}

/************************************************************************
 *                                                                       *
 *  Check the availability of the item from the character                *
 *                                                                       *
 ************************************************************************/

bool HasItem(CCharEntity* PChar, uint16 ItemID)
{
    if (inventorymovehelpers::ShouldRejectZeroItemID(ItemID))
    {
        return false;
    }
    for (uint8 LocID = 0; inventorymovehelpers::IsContainerLoopID(LocID); ++LocID)
    {
        if (inventorymovehelpers::FoundInStorage(PChar->getStorage(LocID)->SearchItem(ItemID)))
        {
            return true;
        }
    }
    return false;
}

uint32 getItemCount(CCharEntity* PChar, uint16 ItemID)
{
    if (inventorymovehelpers::ShouldRejectZeroItemID(ItemID))
    {
        return 0;
    }

    uint32 itemCount = 0;
    for (uint8 LocID = 0; inventorymovehelpers::IsContainerLoopID(LocID); ++LocID)
    {
        CItemContainer* PItemContainer = PChar->getStorage(LocID);
        // clang-format off
            PItemContainer->ForEachItem([&ItemID, &itemCount](CItem* PItem)
            {
                itemCount = inventorymovehelpers::AccumulateItemCount(itemCount, ItemID, PItem->getID(), PItem->getQuantity());
            });
        // clang-format on
    }

    return itemCount;
}

void UpdateSubJob(CCharEntity* PChar)
{
    jobpointutils::RefreshGiftMods(PChar);
    charutils::BuildingCharSkillsTable(PChar);
    charutils::CalculateStats(PChar);
    charutils::CheckValidEquipment(PChar);
    PChar->PRecastContainer->ChangeJob();
    charutils::BuildingCharAbilityTable(PChar);
    charutils::BuildingCharTraitsTable(PChar);

    PChar->UpdateHealth();
    PChar->health.hp = PChar->GetMaxHP();
    PChar->health.mp = PChar->GetMaxMP();

    charutils::SaveCharStats(PChar);
    charutils::SaveCharJob(PChar, PChar->GetMJob());
    charutils::SaveCharExp(PChar, PChar->GetMJob());
    PChar->updatemask |= UPDATE_HP;

    PChar->pushPacket<GP_SERV_COMMAND_JOB_INFO>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS2>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
    PChar->pushPacket<CCharStatusPacket>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MERITS>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY1>(PChar);
    PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY2>(PChar);
    PChar->pushPacket<CCharSyncPacket>(PChar);
}

/************************************************************************
 *                                                                       *
 *  Move the object to the specified cells or the first empty            *
 *                                                                       *
 ************************************************************************/

uint8 MoveItem(CCharEntity* PChar, uint8 LocationID, uint8 SlotID, uint8 NewSlotID)
{
    CItemContainer* PItemContainer = PChar->getStorage(LocationID);

    if (inventorymovehelpers::ShouldRejectMoveNoFreeSlots(PItemContainer->GetFreeSlotsCount()))
    {
        ShowError("charutils::MoveItem: item can't be moved");
        return ERROR_SLOTID;
    }

    if (inventorymovehelpers::ShouldRejectMoveTargetOccupied(
            NewSlotID,
            NewSlotID != ERROR_SLOTID && PItemContainer->GetItem(NewSlotID) != nullptr))
    {
        ShowError("charutils::MoveItem: item can't be moved");
        return ERROR_SLOTID;
    }

    auto PMoving = PItemContainer->RemoveItem(SlotID);
    if (inventorymovehelpers::ShouldRejectMoveRemoveFailed(PMoving != nullptr))
    {
        ShowError("charutils::MoveItem: item can't be moved");
        return ERROR_SLOTID;
    }

    NewSlotID = inventorymovehelpers::ShouldAutoAssignMoveSlot(NewSlotID)
                    ? PItemContainer->InsertItem(std::move(PMoving))
                    : PItemContainer->InsertItem(std::move(PMoving), NewSlotID);

    if (inventorymovehelpers::ShouldRejectMoveInsertFailed(NewSlotID))
    {
        ShowError("charutils::MoveItem: item can't be moved");
        return ERROR_SLOTID;
    }

    const auto rset = db::preparedStmt("UPDATE char_inventory "
                                       "SET slot = ? "
                                       "WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                                       NewSlotID,
                                       PChar->id,
                                       LocationID,
                                       SlotID);

    if (inventorymovehelpers::ShouldRollbackMoveDBFailure(rset != nullptr, rset && rset->rowsAffected()))
    {
        PItemContainer->MoveItemTo(NewSlotID, *PItemContainer, SlotID);
        ShowError("charutils::MoveItem: item can't be moved");
        return ERROR_SLOTID;
    }

    PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(nullptr, static_cast<CONTAINER_ID>(LocationID), SlotID, PItemContainer->GetItem(NewSlotID));
    PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PItemContainer->GetItem(NewSlotID), static_cast<CONTAINER_ID>(LocationID), NewSlotID);
    return NewSlotID;
}

/************************************************************************
 *                                                                       *
 *  Update the number of items in the specified container and slot       *
 *                                                                       *
 ************************************************************************/

uint32 UpdateItem(CCharEntity* PChar, uint8 LocationID, uint8 slotID, int32 quantity, bool force)
{
    CItem* PItem = PChar->getStorage(LocationID)->GetItem(slotID);

    if (styleupdatehelpers::ShouldRejectNullUpdateItem(PItem != nullptr))
    {
        ShowDebug("UpdateItem: No item in slot %u", slotID);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(nullptr, static_cast<CONTAINER_ID>(LocationID), slotID);
        return 0;
    }

    uint16 ItemID = PItem->getID();

    if (styleupdatehelpers::ShouldRejectInvalidQuantity(PItem->getQuantity(), PItem->getReserve(), quantity))
    {
        ShowDebug("UpdateItem: %s trying to move invalid quantity %u of itemID %u", PChar->getName(), quantity, ItemID);
        return 0;
    }

    auto* PState = dynamic_cast<CItemState*>(PChar->PAI->GetCurrentState());
    if (PState)
    {
        CItem* item = PState->GetItem();

        if (styleupdatehelpers::ShouldRejectBusyItemInUse(
                item != nullptr,
                item != nullptr && item->getSlotID() == PItem->getSlotID() && item->getLocationID() == PItem->getLocationID(),
                force))
        {
            return 0;
        }
    }

    // Equipped ammo decrements its stack on consumption without leaving the slot.
    const bool isEquippedAmmo = styleupdatehelpers::IsEquippedAmmoConsumption(
        PItem->state() == ItemState::Equipped,
        PChar->getEquip(SLOT_AMMO) == PItem);
    if (styleupdatehelpers::ShouldRejectBusyNonAmmo(PItem->isBusy(), isEquippedAmmo, force))
    {
        ShowWarningFmt("UpdateItem: refusing to mutate busy item {} in state {} (loc={}, slot={}, char={})",
                       ItemID,
                       magic_enum::enum_name(PItem->state()),
                       LocationID,
                       slotID,
                       PChar->getName());
        return 0;
    }

    uint32 newQuantity = styleupdatehelpers::CapQuantityToStack(PItem->getQuantity(), quantity, PItem->getStackSize());

    if (styleupdatehelpers::ShouldKeepItemOnUpdate(newQuantity, PItem->isType(ITEM_CURRENCY)))
    {
        db::preparedStmt("UPDATE char_inventory "
                         "SET quantity = ? "
                         "WHERE charid = ? AND location = ? AND slot = ?",
                         newQuantity,
                         PChar->id,
                         LocationID,
                         slotID);
        PItem->setQuantity(newQuantity);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_NUM>(static_cast<CONTAINER_ID>(LocationID), slotID, newQuantity);
    }
    else if (styleupdatehelpers::ShouldDeleteItemOnUpdate(newQuantity))
    {
        db::preparedStmt("DELETE FROM char_inventory "
                         "WHERE charid = ? AND location = ? AND slot = ?",
                         PChar->id,
                         LocationID,
                         slotID);
        // Hold the extracted item alive until end of scope
        auto PRemoved = PChar->getStorage(LocationID)->RemoveItem(slotID);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(nullptr, static_cast<CONTAINER_ID>(LocationID), slotID);

        if (styleupdatehelpers::ShouldRefreshStyleOnDrop(PChar->getStyleLocked(), HasItem(PChar, ItemID)))
        {
            if (PItem->isType(ITEM_WEAPON))
            {
                if (PChar->styleItems[SLOT_MAIN] == ItemID)
                {
                    charutils::UpdateWeaponStyle(PChar, SLOT_MAIN, (CItemWeapon*)PChar->getEquip(SLOT_MAIN));
                }
                else if (PChar->styleItems[SLOT_SUB] == ItemID)
                {
                    charutils::UpdateWeaponStyle(PChar, SLOT_SUB, (CItemWeapon*)PChar->getEquip(SLOT_SUB));
                }
            }
            else if (PItem->isType(ITEM_EQUIPMENT))
            {
                auto equipSlotID = ((CItemEquipment*)PItem)->getSlotType();
                if (PChar->styleItems[equipSlotID] == ItemID)
                {
                    switch (equipSlotID)
                    {
                        case SLOT_HEAD:
                        case SLOT_BODY:
                        case SLOT_HANDS:
                        case SLOT_LEGS:
                        case SLOT_FEET:
                            charutils::UpdateArmorStyle(PChar, equipSlotID);
                            break;
                    }
                }
            }
        }
        luautils::OnItemDrop(PChar, PItem);

        // Remove soon to be stale PItem pointer from sync state
        PChar->inventorySyncState().removeEquipChange(PItem);
    }
    return ItemID;
}

// A wrapper around UpdateItem, with some packets
void DropItem(CCharEntity* PChar, uint8 container, uint8 slotID, int32 quantity, uint16 ItemID)
{
    if (inventorymovehelpers::ShouldEmitDropMessages(
            charutils::UpdateItem(PChar, container, slotID, inventorymovehelpers::DropQuantityDelta(quantity))))
    {
        ShowInfo("Player %s DROPPING itemID: %s (%u) quantity: %u", PChar->getName(), xi::items::lookup(ItemID)->getName(), ItemID, quantity);
        PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(nullptr, ItemID, quantity, MsgStd::ThrowAway);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
    }
}

/************************************************************************
 *                                                                       *
 *  Check the possibility of trade between characters                    *
 *                                                                       *
 ************************************************************************/

bool CanTrade(CCharEntity* PChar, CCharEntity* PTarget)
{
    if (tradeitemhelpers::ShouldRejectMonstrosityTrade(PChar->m_PMonstrosity != nullptr, PTarget->m_PMonstrosity != nullptr))
    {
        return false;
    }

    if (tradeitemhelpers::ShouldRejectTradeForSpace(PTarget->getStorage(LOC_INVENTORY)->GetFreeSlotsCount(), PChar->UContainer->GetItemsCount()))
    {
        ShowDebug("Unable to trade, %s doesn't have enough inventory space", PTarget->getName());
        return false;
    }

    for (uint8 slotid = 0; slotid <= tradeitemhelpers::TradeSlotMax; ++slotid)
    {
        CItem* PItem = PChar->UContainer->GetItem(slotid);

        if (PItem != nullptr && tradeitemhelpers::ShouldRejectRareDuplicate(PItem->hasFlag(ItemFlag::Rare), HasItem(PTarget, PItem->getID())))
        {
            ShowDebug("Unable to trade, %s has the rare item already (%s)", PTarget->getName(), PItem->getName());
            return false;
        }
    }

    return true;
}

/************************************************************************
 *                                                                       *
 *  Do the exchange between characters                                   *
 *                                                                       *
 ************************************************************************/

void DoTrade(CCharEntity* PChar, CCharEntity* PTarget)
{
    ShowDebug("%s->%s trade item movement started", PChar->getName(), PTarget->getName());
    for (uint8 slotid = 0; slotid <= tradeitemhelpers::TradeSlotMax; ++slotid)
    {
        CItem* PItem = PChar->UContainer->GetItem(slotid);

        if (PItem != nullptr)
        {
            if (tradeitemhelpers::ShouldCloneSingleStackTrade(PItem->getStackSize(), PItem->getReserve()))
            {
                auto PNewItem = xi::items::clone(*PItem);
                ShowDebug("Adding %s to %s inventory stacksize 1", PNewItem->getName(), PTarget->getName());
                PNewItem->setReserve(0);
                AddItem(PTarget, LOC_INVENTORY, std::move(PNewItem));
            }
            else
            {
                ShowDebug("Adding %s to %s inventory", PItem->getName(), PTarget->getName());
                AddItem(PTarget, LOC_INVENTORY, PItem->getID(), PItem->getReserve());
            }
            ShowDebug("Removing %s from %s's inventory", PItem->getName(), PChar->getName());
            auto amount = PItem->getReserve();
            PItem->setReserve(0);
            UpdateItem(PChar, LOC_INVENTORY, PItem->getSlotID(), tradeitemhelpers::TradeRemoveQuantity(amount));
            PChar->UContainer->ClearSlot(slotid);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Remove equipped item from character without updating the external    *
 *  species (used as an auxiliary function in a bundle with others)      *
 *                                                                       *
 ************************************************************************/

void UnequipItem(CCharEntity* PChar, uint8 equipSlotID, Recalculate recalculate)
{
    if (equippolicyhelpers::ShouldRejectNullChar(PChar == nullptr))
    {
        ShowWarning("PChar was null.");
        return;
    }

    if (!equippolicyhelpers::IsEquipSlotIDValid(equipSlotID))
    {
        ShowWarning("Invalid slot ID. Must be between 0 and 15.");
        return;
    }

    CItem* PItem = PChar->getEquip((SLOTTYPE)equipSlotID);

    if (equippolicyhelpers::HasSlotEquipped(PItem != nullptr, PItem != nullptr && PItem->isType(ITEM_EQUIPMENT)))
    {
        // if removeSlotLookID is available it should be prioritized as it will encompass a larger set of slots
        const auto removeSlotLookID = ((CItemEquipment*)PItem)->getRemoveSlotLookId();
        const auto removeSlotID     = ((CItemEquipment*)PItem)->getRemoveSlotId();
        const auto removeSlotMask   = equippolicyhelpers::PreferRemoveSlotLookID(removeSlotLookID, removeSlotID);

        std::array<unequipremovedarmorlookhelpers::EquippedModel, 16> equippedModels{};
        for (uint8 i = SLOT_HEAD; i <= SLOT_FEET; ++i)
        {
            if ((removeSlotMask & (1u << i)) == 0)
            {
                continue;
            }

            CItemEquipment* equippedItem = PChar->getEquip(static_cast<SLOTTYPE>(i));
            if (equippedItem != nullptr)
            {
                equippedModels[i] = {
                    .present = true,
                    .modelID = static_cast<uint16>(equippedItem->getModelId()),
                };
            }
        }
        for (const auto& plan : unequipremovedarmorlookhelpers::PlansFor(removeSlotLookID, removeSlotID, equippedModels))
        {
            switch (plan.slot)
            {
                case SLOT_HEAD: PChar->look.head = plan.modelID; break;
                case SLOT_BODY: PChar->look.body = plan.modelID; break;
                case SLOT_HANDS: PChar->look.hands = plan.modelID; break;
                case SLOT_LEGS: PChar->look.legs = plan.modelID; break;
                case SLOT_FEET: PChar->look.feet = plan.modelID; break;
            }
        }

        // todo: issues as item 0 reference is being handled as a real equipment piece
        //      thought to be source of nin bug
        PChar->clearEquip(equipSlotID);

        const uint16_t removedScriptType = static_cast<CItemEquipment*>(PItem)->getScriptType();
        if ((removedScriptType & SCRIPT_EQUIP) != 0)
        {
            std::array<uint16_t, 16> remainingScriptTypes{};
            for (uint8 i = 0; i < 16; ++i)
            {
                CItem* PSlotItem = PChar->getEquip(static_cast<SLOTTYPE>(i));
                if ((PSlotItem != nullptr) && PSlotItem->isType(ITEM_EQUIPMENT))
                {
                    remainingScriptTypes[i] = static_cast<CItemEquipment*>(PSlotItem)->getScriptType();
                }
            }
            const auto scriptFlagsPlan = unequipscriptflagshelpers::PlanFor(removedScriptType, remainingScriptTypes);
            if (scriptFlagsPlan.recomputeEquipFlag)
            {
                PChar->m_EquipFlag = scriptFlagsPlan.equipFlag;
            }
        }

        const auto itemRecastPlan = unequipitemrecasthelpers::PlanFor({
            .itemIsCharged  = PItem->isSubType(ITEM_CHARGED),
            .itemSlotID     = PItem->getSlotID(),
            .itemLocationID = PItem->getLocationID(),
        });
        if (itemRecastPlan.removeItemRecast)
        {
            PChar->PRecastContainer->Del(RECAST_ITEM, static_cast<Recast>(itemRecastPlan.recastKey)); // Also remove item from the Recast List no matter what bag its in
        }
        const auto itemUnlockPlan = unequipitemunlockhelpers::PlanFor();
        if (itemUnlockPlan.setItemSubtype)
        {
            PItem->setSubType(itemUnlockPlan.subtype);
        }

        CItemEquipment* const mainAfterClear = PChar->getEquip(SLOT_MAIN);
        const auto subStatePlan = unequipsubstatehelpers::PlanFor({
            .equipSlotID                = equipSlotID,
            .hasMainAfterClear          = mainAfterClear != nullptr,
            .mainAfterClearIsEquipment = mainAfterClear != nullptr && mainAfterClear->isType(ITEM_EQUIPMENT),
        });
        if (subStatePlan.checkUnarmedWeapon)
        {
            CheckUnarmedWeapon(PChar);
        }
        if (subStatePlan.clearDualWield)
        {
            PChar->m_dualWield = false;
        }
        PChar->delEquipModifiers(&((CItemEquipment*)PItem)->modList, ((CItemEquipment*)PItem)->getReqLvl(), equipSlotID);
        PChar->PLatentEffectContainer->DelLatentEffects(((CItemEquipment*)PItem)->getReqLvl(), equipSlotID);
        PChar->delPetModifiers(&((CItemEquipment*)PItem)->petModList);

        const auto armorLookPlan = unequiparmorlookhelpers::PlanFor(equipSlotID);
        const auto rangedLookPlan = unequiprangedlookhelpers::PlanFor({
            .equipSlotID          = equipSlotID,
            .hasRangedAfterClear = PChar->getEquip(SLOT_RANGED) != nullptr,
        });
        const auto subLookPlan = unequipsublookhelpers::PlanFor(equipSlotID);
        const auto mainSubLookPlan = unequipmainsublookhelpers::PlanFor({
            .equipSlotID             = equipSlotID,
            .removedMainIsWeapon     = PItem->isType(ITEM_WEAPON),
            .removedMainIsHandToHand = PItem->isType(ITEM_WEAPON) && static_cast<CItemWeapon*>(PItem)->getSkillType() == SKILL_HAND_TO_HAND,
            .hasSubAfterClear        = PChar->getEquip(SLOT_SUB) != nullptr,
        });
        const auto weaponFinalizePlan = unequipweaponfinalizehelpers::PlanFor({
            .equipSlotID                     = equipSlotID,
            .removedRangedIsStringInstrument = equipSlotID == SLOT_RANGED && static_cast<CItemWeapon*>(PItem)->getSkillType() == SKILL_STRING_INSTRUMENT,
            .removedRangedIsWindInstrument   = equipSlotID == SLOT_RANGED && static_cast<CItemWeapon*>(PItem)->getSkillType() == SKILL_WIND_INSTRUMENT,
        });
        const auto applyWeaponFinalizePlan = [&]()
        {
            if (weaponFinalizePlan.clearTP)
            {
                PChar->health.tp = 0;
            }
            if (weaponFinalizePlan.clearAftermath)
            {
                PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Aftermath);
            }
            if (weaponFinalizePlan.buildWeaponSkills)
            {
                BuildingCharWeaponSkills(PChar);
            }
        };
        switch (equipSlotID)
        {
            case SLOT_HEAD:
                if (armorLookPlan.setArmorLook)
                {
                    PChar->look.head = armorLookPlan.modelID;
                }
                break;
            case SLOT_BODY:
                if (armorLookPlan.setArmorLook)
                {
                    PChar->look.body = armorLookPlan.modelID;
                }
                break;
            case SLOT_HANDS:
                if (armorLookPlan.setArmorLook)
                {
                    PChar->look.hands = armorLookPlan.modelID;
                }
                break;
            case SLOT_LEGS:
                if (armorLookPlan.setArmorLook)
                {
                    PChar->look.legs = armorLookPlan.modelID;
                }
                break;
            case SLOT_FEET:
                if (armorLookPlan.setArmorLook)
                {
                    PChar->look.feet = armorLookPlan.modelID;
                }
                break;
            case SLOT_SUB:
            {
                if (subLookPlan.setSubLook)
                {
                    PChar->look.sub = subLookPlan.modelID;
                }
                PChar->m_Weapons[SLOT_SUB] = xi::items::unarmed(); // << equips "nothing" in the sub slot to prevent multi attack exploit
                applyWeaponFinalizePlan();
                UpdateWeaponStyle(PChar, equipSlotID, nullptr);
            }
            break;
            case SLOT_AMMO:
            {
                if (rangedLookPlan.setRangedLook)
                {
                    PChar->look.ranged = rangedLookPlan.modelID;
                }
                PChar->m_Weapons[SLOT_AMMO] = nullptr;
                UpdateWeaponStyle(PChar, equipSlotID, nullptr);
            }
            break;
            case SLOT_RANGED:
            {
                if (rangedLookPlan.setRangedLook)
                {
                    PChar->look.ranged = rangedLookPlan.modelID;
                }
                PChar->m_Weapons[SLOT_RANGED] = nullptr;
                applyWeaponFinalizePlan();
                UpdateWeaponStyle(PChar, equipSlotID, nullptr);
            }
            break;
            case SLOT_MAIN:
            {
                if (mainSubLookPlan.setSubLook)
                {
                    PChar->look.sub = mainSubLookPlan.modelID;
                }

                const bool isEngaged = PChar->PAI->IsEngaged();
                auto* const state = isEngaged ? dynamic_cast<CAttackState*>(PChar->PAI->GetCurrentState()) : nullptr;
                const auto attackTimerPlan = unequipmainattacktimerhelpers::PlanFor({
                    .equipSlotID         = equipSlotID,
                    .isEngaged           = isEngaged,
                    .currentStateIsAttack = state != nullptr,
                });
                if (attackTimerPlan.resetAttackTimer)
                {
                    state->ResetAttackTimer();
                }

                // If main hand is empty, figure out which UnarmedItem to give the player.
                if (!PChar->getEquip(SLOT_MAIN) || !PChar->getEquip(SLOT_MAIN)->isType(ITEM_EQUIPMENT))
                {
                    CheckUnarmedWeapon(PChar);
                }

                applyWeaponFinalizePlan();
                UpdateWeaponStyle(PChar, equipSlotID, nullptr);
            }
            break;
        }

        luautils::OnItemUnequip(PChar, PItem);

        PChar->inventorySyncState().queueEquipChange(LOC_INVENTORY, 0, static_cast<SLOTTYPE>(equipSlotID), PItem, Equipping::No);

        const auto recalculatePlan = unequiprecalculatehelpers::PlanFor(recalculate);
        if (recalculatePlan.buildSkills)
        {
            charutils::BuildingCharSkillsTable(PChar);
        }
        if (recalculatePlan.updateHealth)
        {
            PChar->UpdateHealth();
        }
        if (recalculatePlan.markUpdateHP)
        {
            PChar->updatemask |= UPDATE_HP;
        }
        if (recalculatePlan.markUpdateLook)
        {
            PChar->updatemask |= UPDATE_LOOK;
        }
    }
}

bool hasSlotEquipped(CCharEntity* PChar, uint8 equipSlotID)
{
    CItem* PItem = PChar->getEquip((SLOTTYPE)equipSlotID);
    return equippolicyhelpers::HasSlotEquipped(PItem != nullptr, PItem != nullptr && PItem->isType(ITEM_EQUIPMENT));
}

void RemoveSub(CCharEntity* PChar)
{
    CItemEquipment* PItem = PChar->getEquip(SLOT_SUB);

    if (equippolicyhelpers::RemoveSubShouldUnequip(PItem != nullptr, PItem != nullptr && PItem->isType(ITEM_EQUIPMENT)))
    {
        UnequipItem(PChar, SLOT_SUB);
    }
}

/************************************************************************
 *                                                                       *
 *  Try to equip the subject in compliance with all conditions           *
 *                                                                       *
 ************************************************************************/

bool EquipArmor(CCharEntity* PChar, uint8 slotID, uint8 equipSlotID, uint8 containerID)
{
    CItemEquipment* PItem   = dynamic_cast<CItemEquipment*>(PChar->getStorage(containerID)->GetItem(slotID));
    CItemEquipment* oldItem = PChar->getEquip((SLOTTYPE)equipSlotID);

    if (PItem == nullptr)
    {
        ShowDebug("No item in inventory slot %u", slotID);
        return false;
    }

    {
        const auto effectiveLevel = checkequipmenthelpers::EffectiveLevelForGearReq(
            settings::get<bool>("map.DISABLE_GEAR_SCALING"),
            PChar->GetMLevel(),
            PChar->jobs.job[PChar->GetMJob()]);
        if (!equippolicyhelpers::IsEquipArmorEligible(
                equippolicyhelpers::IsEquipSlotBlocked(PChar->m_EquipBlock, equipSlotID),
                equippolicyhelpers::IsJobAllowedForItem(PItem->getJobs(), static_cast<uint8>(PChar->GetMJob())),
                equippolicyhelpers::IsSuperiorLevelOK(PItem->getSuperiorLevel(), PChar->getMod(Mod::SUPERIOR_LEVEL)),
                equippolicyhelpers::IsReqLevelOK(PItem->getReqLvl(), effectiveLevel),
                PItem->isEquippableByRace(PChar->look.race)))
        {
            return false;
        }
    }

    if (equipSlotID == SLOT_MAIN)
    {
        CItemEquipment* PSubItem = PChar->getEquip(SLOT_SUB);
        if (equippolicyhelpers::ShouldRemoveSubOnMainEquip(
                slotID == PItem->getSlotID(),
                oldItem != nullptr && oldItem->isType(ITEM_WEAPON),
                PItem->isType(ITEM_WEAPON),
                oldItem != nullptr && oldItem->isType(ITEM_WEAPON) && static_cast<CItemWeapon*>(oldItem)->isTwoHanded(),
                PItem->isType(ITEM_WEAPON) && static_cast<CItemWeapon*>(PItem)->isTwoHanded(),
                PSubItem != nullptr && PSubItem->isType(ITEM_EQUIPMENT),
                PSubItem != nullptr && PSubItem->IsShield()))
        {
            RemoveSub(PChar);
        }
    }

    UnequipItem(PChar, equipSlotID, Recalculate::No);

    const auto directRestrictionPlan = equiparmordirecthelpers::PlanFor({
        .equipSlotID    = equipSlotID,
        .itemEquipSlots = PItem->getEquipSlotId(),
        .removeSlots    = PItem->getRemoveSlotId(),
        .modelID        = static_cast<uint16>(PItem->getModelId()),
    });

    // When equipping PItem, remove all slots restricted by PItem. For example,
    // equipping a Black Cloak removes head equipment.
    if (directRestrictionPlan.applies)
    {
        for (std::size_t actionIndex = 0; actionIndex < directRestrictionPlan.actionCount; ++actionIndex)
        {
            const auto action = directRestrictionPlan.actions[actionIndex];
            if (action.kind == equiparmordirecthelpers::ActionKind::Unequip)
            {
                UnequipItem(PChar, action.slot, Recalculate::No);
            }
            else if (action.kind == equiparmordirecthelpers::ActionKind::SetArmorLook)
            {
                switch (action.slot)
                {
                    case SLOT_HEAD:
                        PChar->look.head = action.modelID;
                        break;
                    case SLOT_BODY:
                        PChar->look.body = action.modelID;
                        break;
                    case SLOT_HANDS:
                        PChar->look.hands = action.modelID;
                        break;
                    case SLOT_LEGS:
                        PChar->look.legs = action.modelID;
                        break;
                    case SLOT_FEET:
                        PChar->look.feet = action.modelID;
                        break;
                }
            }
        }

        // When equipping PItem into a slot - Remove equip in other slots which may have restricted equip in this slot
        // e.g. Equipping head equipment should result in the removal of an equipped Black Cloak
        for (uint8 i = 0; i < SLOT_BACK; ++i)
        {
            CItemEquipment* armor = PChar->getEquip((SLOTTYPE)i);
            if (equiparmorreversehelpers::ShouldUnequip({
                    .isEquipment       = armor && armor->isType(ITEM_EQUIPMENT),
                    .removeSlots       = static_cast<std::uint16_t>(armor != nullptr ? armor->getRemoveSlotId() : 0),
                    .incomingEquipSlots = PItem->getEquipSlotId(),
                }))
            {
                UnequipItem(PChar, i, Recalculate::No);
            }
        }

        const auto targetLookPlan = equiparmortargetlookhelpers::PlanFor(equipSlotID, static_cast<uint16>(PItem->getModelId()));
        switch (equipSlotID)
        {
            case SLOT_MAIN:
            {
                const auto mainLookPlan = equiparmormainlookhelpers::PlanFor(static_cast<uint16>(PItem->getModelId()));
                if (PItem->isType(ITEM_WEAPON))
                {
                    switch (static_cast<CItemWeapon*>(PItem)->getSkillType())
                    {
                        case SKILL_HAND_TO_HAND:
                        case SKILL_GREAT_SWORD:
                        case SKILL_GREAT_AXE:
                        case SKILL_SCYTHE:
                        case SKILL_POLEARM:
                        case SKILL_GREAT_KATANA:
                        case SKILL_STAFF:
                        {
                            CItemEquipment* sub = PChar->getEquip(SLOT_SUB);
                            const auto mainSubPlan = equiparmormainsubhelpers::PlanFor({
                                .incomingIsH2H    = static_cast<CItemWeapon*>(PItem)->getSkillType() == SKILL_HAND_TO_HAND,
                                .hasSubEquipment  = sub != nullptr && sub->isType(ITEM_EQUIPMENT),
                                .subIsWeapon       = sub != nullptr && sub->isType(ITEM_WEAPON),
                                .subSkillNone      = sub != nullptr && sub->isType(ITEM_WEAPON) && static_cast<CItemWeapon*>(sub)->getSkillType() == SKILL_NONE,
                                .modelID           = static_cast<uint16>(PItem->getModelId()),
                            });
                            if (mainSubPlan.unequipSub)
                            {
                                UnequipItem(PChar, SLOT_SUB, Recalculate::No);
                            }
                            if (mainSubPlan.setH2HSubLook)
                            {
                                PChar->look.sub = mainSubPlan.h2hSubModel;
                            }
                        }
                        break;
                    }
                    if (PChar->PAI->IsEngaged())
                    {
                        auto* state = dynamic_cast<CAttackState*>(PChar->PAI->GetCurrentState());
                        if (state)
                        {
                            state->ResetAttackTimer();
                        }
                    }
                    PChar->m_Weapons[SLOT_MAIN] = PItem;
                }
                if (mainLookPlan.setMainLook)
                {
                    PChar->look.main = mainLookPlan.modelID;
                }
                UpdateWeaponStyle(PChar, equipSlotID, (CItemWeapon*)PItem);
            }
            break;
            case SLOT_SUB:
            {
                const auto subLookPlan = equiparmorsublookhelpers::PlanFor(static_cast<uint16>(PItem->getModelId()));
                CItemWeapon* weapon = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_MAIN));
                // NULL weapon can be unarmed weapon that just got unequipped
                if (!weapon)
                {
                    if (PItem->IsShield())
                    {
                        if (subLookPlan.setSubLook)
                        {
                            PChar->look.sub = subLookPlan.modelID;
                        }
                        UpdateWeaponStyle(PChar, equipSlotID, PItem);
                        break;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    switch (weapon->getSkillType())
                    {
                        case SKILL_HAND_TO_HAND:
                        {
                            const auto subPlan = equiparmorsubhelpers::PlanFor({
                                .mainKind         = equiparmorsubhelpers::MainKind::HandToHand,
                                .incomingIsWeapon = PItem->isType(ITEM_WEAPON),
                            });
                            if (subPlan.unequipMain)
                            {
                                UnequipItem(PChar, SLOT_MAIN, Recalculate::No);
                            }
                            break;
                        }
                        case SKILL_DAGGER:
                        case SKILL_SWORD:
                        case SKILL_AXE:
                        case SKILL_KATANA:
                        case SKILL_CLUB:
                        {
                            CItemWeapon* PNewItemWeapon = dynamic_cast<CItemWeapon*>(PItem);
                            bool         isWeapon       = PItem->isType(ITEM_WEAPON);

                            if (isWeapon && (!charutils::hasTrait(PChar, TRAIT_DUAL_WIELD) || (PNewItemWeapon && PNewItemWeapon->getSkillType() == SKILL_NONE)))
                            {
                                return false;
                            }
                            const auto subPlan = equiparmorsubhelpers::PlanFor({
                                .mainKind         = equiparmorsubhelpers::MainKind::OneHanded,
                                .incomingIsWeapon = isWeapon,
                            });
                            if (subPlan.setSubWeapon)
                            {
                                PChar->m_Weapons[SLOT_SUB] = PItem;
                            }
                            // only set m_dualWield if equipping a weapon (not for example a shield)
                            if (subPlan.setDualWield)
                            {
                                PChar->m_dualWield = true;
                            }
                        }
                        break;
                        default:
                        {
                            const auto subPlan = equiparmorsubhelpers::PlanFor({
                                .mainKind         = equiparmorsubhelpers::MainKind::Other,
                                .incomingIsWeapon = PItem->isType(ITEM_WEAPON),
                            });
                            if (subPlan.unequipMain)
                            {
                                UnequipItem(PChar, SLOT_MAIN, Recalculate::No);
                            }
                            else if (static_cast<CItemWeapon*>(PItem)->getSkillType() != SKILL_NONE)
                            {
                                // allow Grips to be equipped
                                return false;
                            }
                        }
                    }
                }
                if (subLookPlan.setSubLook)
                {
                    PChar->look.sub = subLookPlan.modelID;
                }
                UpdateWeaponStyle(PChar, equipSlotID, PItem);
            }
            break;
            case SLOT_RANGED:
            {
                const auto rangedLookPlan = equiparmorrangedlookhelpers::PlanFor(static_cast<uint16>(PItem->getModelId()));
                if (PItem->isType(ITEM_WEAPON))
                {
                    CItemWeapon* weapon = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_AMMO));
                    if (equiparmorrangedhelpers::ShouldUnequipOther({
                            .incomingIsWeapon = true,
                            .otherIsWeapon    = weapon != nullptr,
                            .incomingSkill    = static_cast<CItemWeapon*>(PItem)->getSkillType(),
                            .incomingSubSkill = static_cast<CItemWeapon*>(PItem)->getSubSkillType(),
                            .otherSkill       = static_cast<std::uint8_t>(weapon ? weapon->getSkillType() : 0),
                            .otherSubSkill    = static_cast<std::uint8_t>(weapon ? weapon->getSubSkillType() : 0),
                        }))
                    {
                        // If the subtype of the ranged weapon is not compatible with the ammo, unequip it, except for Archery where Longbow and Shortbow both use arrows
                        UnequipItem(PChar, SLOT_AMMO, Recalculate::No);
                    }
                    PChar->m_Weapons[SLOT_RANGED] = PItem;
                }
                if (rangedLookPlan.setRangedLook)
                {
                    PChar->look.ranged = rangedLookPlan.modelID;
                }
                UpdateWeaponStyle(PChar, equipSlotID, PItem);
            }
            break;
            case SLOT_AMMO:
            {
                if (PItem->isType(ITEM_WEAPON))
                {
                    CItemWeapon* weapon = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_RANGED));
                    if (equiparmorrangedhelpers::ShouldUnequipOther({
                            .incomingIsWeapon = true,
                            .otherIsWeapon    = weapon != nullptr,
                            .incomingSkill    = static_cast<CItemWeapon*>(PItem)->getSkillType(),
                            .incomingSubSkill = static_cast<CItemWeapon*>(PItem)->getSubSkillType(),
                            .otherSkill       = static_cast<std::uint8_t>(weapon ? weapon->getSkillType() : 0),
                            .otherSubSkill    = static_cast<std::uint8_t>(weapon ? weapon->getSubSkillType() : 0),
                        }))
                    {
                        // If the subtype of the ammo is not compatible with the ranged weapon, unequip it, except for Archery where Longbow and Shortbow both use arrows
                        UnequipItem(PChar, SLOT_RANGED, Recalculate::No);
                    }
                    const auto ammoLookPlan = equiparmorammolookhelpers::PlanFor({
                            .incomingIsWeapon          = true,
                            .hasRangedAfterCompatibility = PChar->getEquip(SLOT_RANGED) != nullptr,
                            .modelID                   = static_cast<uint16>(PItem->getModelId()),
                        });
                    if (ammoLookPlan.setRangedLook)
                    {
                        PChar->look.ranged = ammoLookPlan.modelID;
                    }
                    PChar->m_Weapons[SLOT_AMMO] = PItem;
                    UpdateWeaponStyle(PChar, equipSlotID, PItem);
                }
            }
            break;
            case SLOT_HEAD:
            {
                if (targetLookPlan.setArmorLook)
                {
                    PChar->look.head = targetLookPlan.modelID;
                }
            }
            break;
            case SLOT_BODY:
            {
                if (targetLookPlan.setArmorLook)
                {
                    PChar->look.body = targetLookPlan.modelID;
                }
            }
            break;
            case SLOT_HANDS:
            {
                if (targetLookPlan.setArmorLook)
                {
                    PChar->look.hands = targetLookPlan.modelID;
                }
            }
            break;
            case SLOT_LEGS:
            {
                if (targetLookPlan.setArmorLook)
                {
                    PChar->look.legs = targetLookPlan.modelID;
                }
            }
            break;
            case SLOT_FEET:
            {
                if (targetLookPlan.setArmorLook)
                {
                    PChar->look.feet = targetLookPlan.modelID;
                }
            }
            break;
        }

        if (!PChar->bindEquip(equipSlotID, PItem))
        {
            return false;
        }

        // Changed visible equipment
        if (equipSlotID >= SLOT_HEAD && equipSlotID <= SLOT_FEET)
        {
            UpdateRemovedSlotsLook(PChar);
        }
    }
    else
    {
        ShowWarning("Item %i is not equipable in equip slot %i", PItem->getID(), equipSlotID);
        return false;
    }
    return true;
}

auto canEquipItemOnAnyJob(CCharEntity* PChar, const CItemEquipment* PItem) -> bool
{
    // TODO: Check for Su level for the player's job, and apply to the condition.
    return styleupdatehelpers::CanEquipItemOnAnyJob(
        PItem == nullptr,
        PItem ? PItem->getJobs() : 0u,
        PItem ? PItem->getReqLvl() : 0,
        std::to_array(PChar->jobs.job));
}

auto hasValidStyle(CCharEntity* PChar, const CItemEquipment* PItem, const CItemEquipment* AItem) -> bool
{
    const auto* PWeapon = dynamic_cast<const CItemWeapon*>(PItem);
    const auto* AWeapon = dynamic_cast<const CItemWeapon*>(AItem);
    return styleupdatehelpers::HasValidStyle(
        AItem != nullptr && PItem != nullptr,
        AItem != nullptr && PItem != nullptr && AItem->IsShield() && PItem->IsShield(),
        AItem != nullptr && HasItem(PChar, AItem->getID()),
        AItem != nullptr && canEquipItemOnAnyJob(PChar, AItem),
        AItem != nullptr && AItem->getID() == styleupdatehelpers::MarvelousCheer,
        PWeapon != nullptr,
        PWeapon ? PWeapon->getSkillType() : 0,
        PWeapon != nullptr && AWeapon != nullptr,
        AWeapon ? AWeapon->getSkillType() : 0);
}

void SetStyleLock(CCharEntity* PChar, bool isStyleLocked)
{
    auto input = stylelocktransitionhelpers::Input{
        .currentlyLocked = PChar->getStyleLocked(),
        .requestedLocked = isStyleLocked,
    };
    if (isStyleLocked)
    {
        for (uint8 i = 0; i < styleupdatehelpers::SlotLink1; i++)
        {
            auto* PItem             = PChar->getEquip((SLOTTYPE)i);
            input.equipment[i] = {
                .hasItem = PItem != nullptr,
                .itemID  = static_cast<std::uint16_t>(PItem ? PItem->getID() : 0),
            };
        }
    }
    const auto plan = stylelocktransitionhelpers::PlanFor(input);

    if (plan.snapshotStyleItems)
    {
        for (uint8 i = 0; i < styleupdatehelpers::SlotLink1; i++)
        {
            PChar->styleItems[i] = plan.styleItems[i];
        }
        std::memcpy(&PChar->mainlook, &PChar->look, sizeof(PChar->look));
    }
    else if (plan.clearStyleItems)
    {
        for (unsigned short& styleItem : PChar->styleItems)
        {
            styleItem = 0;
        }
    }

    if (plan.notifyChange)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(isStyleLocked ? MsgStd::StyleLockOn : MsgStd::StyleLockOff);
    }
    PChar->setStyleLocked(plan.locked);
}

void UpdateWeaponStyle(CCharEntity* PChar, uint8 equipSlotID, CItemEquipment* PItem)
{
    const bool styleLocked = PChar->getStyleLocked();
    if (styleupdatehelpers::ShouldSkipStyleUpdateWhenUnlocked(styleLocked))
    {
        return;
    }

    const CItemEquipment* appearance      = xi::items::lookup<CItemEquipment>(PChar->styleItems[equipSlotID]);
    uint16                appearanceModel = styleupdatehelpers::AppearanceModelOrZero(appearance != nullptr, appearance ? appearance->getModelId() : 0);
    const bool            styleSlot       = equipSlotID == SLOT_MAIN || equipSlotID == SLOT_SUB || equipSlotID == SLOT_RANGED;
    const auto*           PWeapon         = equipSlotID == SLOT_MAIN ? dynamic_cast<CItemWeapon*>(PItem) : nullptr;
    const auto            plan            = weaponstyleupdatehelpers::PlanFor({
        .styleLocked          = styleLocked,
        .styleValid           = styleSlot && hasValidStyle(PChar, PItem, appearance),
        .incomingItemNil      = PItem == nullptr,
        .incomingIsWeapon     = PWeapon != nullptr,
        .incomingIsHandToHand = PWeapon != nullptr && PWeapon->getSkillType() == SKILL_HAND_TO_HAND,
        .incomingIsTwoHanded  = PWeapon != nullptr && styleupdatehelpers::IsTwoHandedStyleSkill(PWeapon->getSkillType()),
        .equipSlotID          = equipSlotID,
        .appearanceModel      = appearanceModel,
        .currentMainModel     = PChar->look.main,
        .currentSubModel      = PChar->look.sub,
        .currentRangedModel   = PChar->look.ranged,
    });
    if (plan.setMainLook)
    {
        PChar->mainlook.main = plan.mainModel;
    }
    if (plan.setSubLook)
    {
        PChar->mainlook.sub = plan.subModel;
    }
    if (plan.setRangedLook)
    {
        PChar->mainlook.ranged = plan.rangedModel;
    }
}

void UpdateArmorStyle(CCharEntity* PChar, uint8 equipSlotID)
{
    const bool styleLocked = PChar->getStyleLocked();
    if (styleupdatehelpers::ShouldSkipStyleUpdateWhenUnlocked(styleLocked))
    {
        return;
    }

    uint16                itemID          = PChar->styleItems[equipSlotID];
    const CItemEquipment* appearance      = xi::items::lookup<CItemEquipment>(itemID);
    const auto            plan            = armorstyleupdatehelpers::PlanFor({
        .styleLocked        = styleLocked,
        .hasAppearance      = appearance != nullptr,
        .stillHasAppearance = HasItem(PChar, itemID),
        .canEquipAppearance = canEquipItemOnAnyJob(PChar, appearance),
        .modelID            = static_cast<std::uint16_t>(appearance ? appearance->getModelId() : 0),
        .equipSlotID        = equipSlotID,
    });
    if (!plan.setMainLook)
    {
        return;
    }

    switch (plan.slot)
    {
        case SLOT_HEAD:
            PChar->mainlook.head = plan.modelID;
            break;
        case SLOT_BODY:
            PChar->mainlook.body = plan.modelID;
            break;
        case SLOT_HANDS:
            PChar->mainlook.hands = plan.modelID;
            break;
        case SLOT_LEGS:
            PChar->mainlook.legs = plan.modelID;
            break;
        case SLOT_FEET:
            PChar->mainlook.feet = plan.modelID;
            break;
    }
}

/// <summary>
/// Updates the Char's lockstyle look to account for gear that occupies multiple slots
/// This includes items like Black Cloak which restricts the equip AND look of headgear.
/// This also incluses items like Onca Suit which restricts equip and look of legs, but only look of hands and feet.
/// </summary>
/// <param name="PChar">Character to have Lockstyle look updated</param>
void UpdateRemovedSlotsLookForLockStyle(CCharEntity* PChar)
{
    if (!PChar || !PChar->getStyleLocked())
    {
        return;
    }

    auto items = PChar->styleItems;
    for (auto i = 0; i < 16; i++)
    {
        if (items[i] == 0)
        {
            continue;
        }

        const auto* PItem = xi::items::lookup<CItemEquipment>(items[i]);
        const auto  plan  = lockstyleremovedlookhelpers::PlanFor({
            .styleItemID      = items[i],
            .itemFound        = PItem != nullptr,
            .modelID          = PItem ? static_cast<uint16>(PItem->getModelId()) : static_cast<uint16>(0),
            .removeSlotLookID = PItem ? static_cast<std::uint32_t>(PItem->getRemoveSlotLookId()) : 0u,
            .removeSlotID     = PItem ? static_cast<std::uint32_t>(PItem->getRemoveSlotId()) : 0u,
        });
        if (!plan.applies)
        {
            continue;
        }

        for (auto i = 4u; i <= 8u; i++)
        {
            if (equiparmorremovedlookhelpers::ShouldSetTargetLook(plan.effectiveRemoveID, static_cast<uint8>(i)))
            {
                switch (i)
                {
                    case SLOT_HEAD:
                        PChar->mainlook.head = plan.modelID;
                        break;
                    case SLOT_BODY:
                        PChar->mainlook.body = plan.modelID;
                        break;
                    case SLOT_HANDS:
                        PChar->mainlook.hands = plan.modelID;
                        break;
                    case SLOT_LEGS:
                        PChar->mainlook.legs = plan.modelID;
                        break;
                    case SLOT_FEET:
                        PChar->mainlook.feet = plan.modelID;
                        break;
                }
            }
        }
    }
}

/// <summary>
/// Updates the Char's look to account for gear that occupies multiple slots
/// This includes items like Black Cloak which restricts the equip AND look of headgear.
/// This also incluses items like Onca Suit which restricts equip and look of legs, but only look of hands and feet.
/// </summary>
/// <param name="PChar">Character to have look updated</param>
void UpdateRemovedSlotsLook(CCharEntity* PChar)
{
    if (!PChar)
    {
        return;
    }

    std::array<equiparmorremovedlookhelpers::Input, equiparmorremovedlookhelpers::SourceSlotCount> removedLookInputs{};
    for (int i = SLOT_HEAD; equiparmorremovedlookhelpers::IsSourceSlot(static_cast<uint8>(i)); i++)
    {
        CItemEquipment* armor = PChar->getEquip((SLOTTYPE)i);
        removedLookInputs[i - SLOT_HEAD] = {
            .itemPresent      = armor != nullptr,
            .itemIsEquipment  = armor && armor->isType(ITEM_EQUIPMENT),
            .modelID          = armor ? static_cast<uint16>(armor->getModelId()) : static_cast<uint16>(0),
            .removeSlotLookID = armor ? static_cast<std::uint32_t>(armor->getRemoveSlotLookId()) : 0u,
        };
    }

    const auto plan = equiparmorremovedlookhelpers::PlanFor(removedLookInputs);
    for (std::uint8_t index = 0; index < plan.actionCount; ++index)
    {
        const auto& action = plan.actions[index];
        switch (action.targetSlot)
        {
            case SLOT_HEAD:
                PChar->look.head = action.modelID;
                break;
            case SLOT_BODY:
                PChar->look.body = action.modelID;
                break;
            case SLOT_HANDS:
                PChar->look.hands = action.modelID;
                break;
            case SLOT_LEGS:
                PChar->look.legs = action.modelID;
                break;
            case SLOT_FEET:
                PChar->look.feet = action.modelID;
                break;
        }
    }
}

void AddItemToRecycleBin(CCharEntity* PChar, uint32 container, uint8 slotID, uint8 quantity)
{
    auto* RecycleBin     = PChar->getStorage(LOC_RECYCLEBIN);
    auto* OtherContainer = PChar->getStorage(container);

    auto* PSrcItem = OtherContainer->GetItem(slotID);
    if (inventorymovehelpers::ShouldRejectRecycleNullSource(PSrcItem != nullptr))
    {
        return;
    }

    if (inventorymovehelpers::ShouldRejectRecycleBusy(PSrcItem->isBusy()))
    {
        ShowWarningFmt("AddItemToRecycleBin: refusing to move busy item {} (state={}, char={})",
                       PSrcItem->getID(),
                       magic_enum::enum_name(PSrcItem->state()),
                       PChar->getName());
        return;
    }

    const uint16 itemID   = PSrcItem->getID();
    const auto   itemName = PSrcItem->getName();

    if (inventorymovehelpers::ShouldUseSimpleRecycleMove(RecycleBin->GetFreeSlotsCount()))
    {
        const uint8 NewSlotID = OtherContainer->MoveItemTo(slotID, *RecycleBin);
        if (inventorymovehelpers::ShouldRejectRecycleMoveFailed(NewSlotID))
        {
            return;
        }

        const auto rset = db::preparedStmt("UPDATE char_inventory SET location = ?, slot = ? WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                                           LOC_RECYCLEBIN,
                                           NewSlotID,
                                           PChar->id,
                                           container,
                                           slotID);
        if (inventorymovehelpers::ShouldRollbackRecycleDBFailure(rset != nullptr, rset && rset->rowsAffected()))
        {
            RecycleBin->MoveItemTo(NewSlotID, *OtherContainer, slotID);
            return;
        }

        auto* PInserted = RecycleBin->GetItem(NewSlotID);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(nullptr, static_cast<CONTAINER_ID>(container), slotID);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PInserted, LOC_RECYCLEBIN, NewSlotID);
        PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(nullptr, itemID, quantity, MsgStd::ThrowAway);
        luautils::OnItemDrop(PChar, PInserted, IsRecycleBin::Yes);
    }
    else // Bin is full
    {
        // Evict recycle bin slot 1
        auto PEvictedItem = RecycleBin->RemoveItem(inventorymovehelpers::RecycleBinEvictSlot);
        db::preparedStmt("DELETE FROM char_inventory WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                         PChar->id,
                         LOC_RECYCLEBIN,
                         inventorymovehelpers::RecycleBinEvictSlot);

        if (PEvictedItem)
        {
            luautils::OnItemDrop(PChar, PEvictedItem.get());
        }

        // Slide slots 2..10 down to 1..9
        for (int i = 2; i <= inventorymovehelpers::RecycleBinLast; ++i)
        {
            if (inventorymovehelpers::ShouldSkipEmptySlideSlot(RecycleBin->GetItem(i) != nullptr))
            {
                continue;
            }
            RecycleBin->MoveItemTo(i, *RecycleBin, inventorymovehelpers::RecycleBinSlideTarget(i));

            const auto rset = db::preparedStmt("UPDATE char_inventory SET location = ?, slot = ? WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                                               LOC_RECYCLEBIN,
                                               inventorymovehelpers::RecycleBinSlideTarget(i),
                                               PChar->id,
                                               LOC_RECYCLEBIN,
                                               i);
            if (inventorymovehelpers::ShouldRollbackRecycleDBFailure(rset != nullptr, rset && rset->rowsAffected()))
            {
                ShowError("Problem moving Recycle Bin items! (%s - %s)", PChar->getName(), itemName);
            }
        }

        // Move new item from source container into freed slot 10
        OtherContainer->MoveItemTo(slotID, *RecycleBin, inventorymovehelpers::RecycleBinFullInsertSlot);
        auto* PInserted = RecycleBin->GetItem(inventorymovehelpers::RecycleBinFullInsertSlot);

        const auto rset = db::preparedStmt("UPDATE char_inventory SET location = ?, slot = ? WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                                           LOC_RECYCLEBIN,
                                           inventorymovehelpers::RecycleBinFullInsertSlot,
                                           PChar->id,
                                           container,
                                           slotID);
        if (inventorymovehelpers::ShouldRollbackRecycleDBFailure(rset != nullptr, rset && rset->rowsAffected()))
        {
            ShowError("Problem moving Recycle Bin items! (%s - %s)", PChar->getName(), itemName);
        }

        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(nullptr, static_cast<CONTAINER_ID>(container), slotID);
        for (int i = inventorymovehelpers::RecycleBinFirst; i <= inventorymovehelpers::RecycleBinLast; ++i)
        {
            CItem* PUpdatedItem = RecycleBin->GetItem(i);
            PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PUpdatedItem, LOC_RECYCLEBIN, i);
        }
        PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(nullptr, itemID, quantity, MsgStd::ThrowAway);
        luautils::OnItemDrop(PChar, PInserted, IsRecycleBin::Yes);
    }
    PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
}

void EmptyRecycleBin(CCharEntity* PChar)
{
    TracyZoneScoped;

    CItemContainer* recycleBin = PChar->getStorage(LOC_RECYCLEBIN);

    for (uint8 slotID = 1; inventorymovehelpers::IsEmptyRecycleBinSlot(slotID, recycleBin->GetSize()); ++slotID)
    {
        if (CItem* PItem = recycleBin->GetItem(slotID))
        {
            luautils::OnItemDrop(PChar, PItem);
        }
    }

    db::preparedStmt("DELETE FROM char_inventory WHERE charid = ? AND location = ?", PChar->id, inventorymovehelpers::RecycleBinLocationForDelete);
    recycleBin->Clear();
}

void SaveJobChangeGear(CCharEntity* PChar)
{
    if (PChar == nullptr)
    {
        return;
    }

    auto getEquipIdFromSlot = [](CCharEntity* PChar, SLOTTYPE slot) -> uint16
    {
        return (PChar->getEquip(slot) != nullptr) ? PChar->getEquip(slot)->getID() : 0;
    };

    uint16 main   = getEquipIdFromSlot(PChar, SLOT_MAIN);
    uint16 sub    = getEquipIdFromSlot(PChar, SLOT_SUB);
    uint16 ranged = getEquipIdFromSlot(PChar, SLOT_RANGED);
    uint16 ammo   = getEquipIdFromSlot(PChar, SLOT_AMMO);
    uint16 head   = getEquipIdFromSlot(PChar, SLOT_HEAD);
    uint16 body   = getEquipIdFromSlot(PChar, SLOT_BODY);
    uint16 hands  = getEquipIdFromSlot(PChar, SLOT_HANDS);
    uint16 legs   = getEquipIdFromSlot(PChar, SLOT_LEGS);
    uint16 feet   = getEquipIdFromSlot(PChar, SLOT_FEET);
    uint16 neck   = getEquipIdFromSlot(PChar, SLOT_NECK);
    uint16 waist  = getEquipIdFromSlot(PChar, SLOT_WAIST);
    uint16 ear1   = getEquipIdFromSlot(PChar, SLOT_EAR1);
    uint16 ear2   = getEquipIdFromSlot(PChar, SLOT_EAR2);
    uint16 ring1  = getEquipIdFromSlot(PChar, SLOT_RING1);
    uint16 ring2  = getEquipIdFromSlot(PChar, SLOT_RING2);
    uint16 back   = getEquipIdFromSlot(PChar, SLOT_BACK);

    db::preparedStmt(
        "INSERT INTO char_equip_saved SET "
        "charid = ?, "
        "jobid = ?, "
        "main = ?, "
        "sub = ?, "
        "ranged = ?, "
        "ammo = ?, "
        "head = ?, "
        "body = ?, "
        "hands = ?, "
        "legs = ?, "
        "feet = ?, "
        "neck = ?, "
        "waist = ?, "
        "ear1 = ?, "
        "ear2 = ?, "
        "ring1 = ?, "
        "ring2 = ?, "
        "back = ? "
        "ON DUPLICATE KEY UPDATE "
        "main = VALUES(main), "
        "sub = VALUES(sub), "
        "ranged = VALUES(ranged), "
        "ammo = VALUES(ammo), "
        "head = VALUES(head), "
        "body = VALUES(body), "
        "hands = VALUES(hands), "
        "legs = VALUES(legs), "
        "feet = VALUES(feet), "
        "neck = VALUES(neck), "
        "waist = VALUES(waist), "
        "ear1 = VALUES(ear1), "
        "ear2 = VALUES(ear2), "
        "ring1 = VALUES(ring1), "
        "ring2 = VALUES(ring2), "
        "back = VALUES(back)",
        PChar->id,
        PChar->GetMJob(),
        main,
        sub,
        ranged,
        ammo,
        head,
        body,
        hands,
        legs,
        feet,
        neck,
        waist,
        ear1,
        ear2,
        ring1,
        ring2,
        back);
}

void LoadJobChangeGear(CCharEntity* PChar)
{
    if (PChar == nullptr)
    {
        return;
    }

    const auto rset = db::preparedStmt("SELECT main, sub, ranged, ammo, head, body, hands, legs, feet, neck, waist, ear1, ear2, ring1, ring2, back "
                                       "FROM char_equip_saved AS equip "
                                       "WHERE charid = ? AND jobid = ? LIMIT 1",
                                       PChar->id,
                                       PChar->GetMJob());
    FOR_DB_SINGLE_RESULT(rset)
    {
        const std::vector<uint8> validContainers = { LOC_INVENTORY, LOC_WARDROBE, LOC_WARDROBE2, LOC_WARDROBE3, LOC_WARDROBE4, LOC_WARDROBE5, LOC_WARDROBE6, LOC_WARDROBE7, LOC_WARDROBE8 };

        for (uint8 equipSlot = SLOT_MAIN; equipSlot <= SLOT_BACK; equipSlot++)
        {
            const auto itemId = rset->get<uint16>(equipSlot);

            if (itemId > 0)
            {
                for (const auto container : validContainers)
                {
                    bool found = false;

                    for (uint8 slot = 0; slot < PChar->getStorage(container)->GetSize(); slot++)
                    {
                        auto* PEquip = dynamic_cast<CItemEquipment*>(PChar->getStorage(container)->GetItem(slot));

                        // ensure this is the item we actually want from the db
                        if (PEquip && PEquip->getID() == itemId)
                        {
                            // Validate that we're not trying to equip the same item to two different slots
                            CItemEquipment* compareItem = nullptr;

                            // Get item that theoretically could be equipped an adjacent slot
                            if (equipSlot == SLOT_MAIN || equipSlot == SLOT_EAR1 || equipSlot == SLOT_RING1)
                            {
                                // Check one item to the "right"
                                compareItem = PChar->getEquip(static_cast<SLOTTYPE>(equipSlot + 1));
                            }
                            else if (equipSlot == SLOT_SUB || equipSlot == SLOT_EAR2 || equipSlot == SLOT_RING2)
                            {
                                // Check one item to the "left"
                                compareItem = PChar->getEquip(static_cast<SLOTTYPE>(equipSlot - 1));
                            }

                            // If there's no item to compare then this item is valid
                            // If there is, check they aren't the same via pointer comparison (2 unique copies)
                            if (!compareItem || (compareItem && compareItem != PEquip))
                            {
                                found = true;
                                charutils::EquipItem(PChar, PEquip->getSlotID(), equipSlot, static_cast<CONTAINER_ID>(container));
                                break;
                            }
                        }
                    }

                    if (found)
                    {
                        break;
                    }
                }
            }
        }
    }
}

void EquipItem(CCharEntity* PChar, uint8 slotID, uint8 equipSlotID, uint8 containerID)
{
    if (equippolicyhelpers::ShouldRejectNullCharOrStorage(PChar == nullptr, PChar == nullptr || PChar->getStorage(containerID) == nullptr))
    {
        return;
    }

    CItemEquipment* PItem = dynamic_cast<CItemEquipment*>(PChar->getStorage(containerID)->GetItem(slotID));

    if (equippolicyhelpers::IsAlreadyEquippedInSlot(PItem != nullptr, PItem != nullptr && PItem == PChar->getEquip(static_cast<SLOTTYPE>(equipSlotID))))
    {
        return;
    }

    // slotID of zero = unequip; skip if equipping same item already in paired dual slot
    if (!equippolicyhelpers::IsUnequipRequest(slotID))
    {
        const auto paired = equippolicyhelpers::PairedSlotForDuplicateCheck(equipSlotID);
        if (paired != 0xFF)
        {
            auto* PPaired = PChar->getEquip(static_cast<SLOTTYPE>(paired));
            if (equippolicyhelpers::ShouldSkipCrossSlotSameItem(slotID, equipSlotID, PItem != nullptr && PItem == PPaired))
            {
                return;
            }
        }
    }

    // if player attempts to change their ranged weapon during a ranged state then prevent equip
    // this prevents players from starting a RA with short delay x-bow and ending with high dmg longbow
    if (equippolicyhelpers::ShouldBlockRangedEquipDuringRA(
            equipSlotID,
            PChar->getEquip(SLOT_RANGED) != nullptr,
            PChar->PAI && PChar->PAI->IsCurrentState<CRangeState>()))
    {
        return;
    }

    if (equipSlotID == SLOT_SUB && PItem && !PItem->IsShield())
    {
        auto PItemWeapon = dynamic_cast<CItemWeapon*>(PItem);
        auto PMainItem   = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_MAIN));

        if (equippolicyhelpers::ShouldRequire2HForGrip(
                true,
                false,
                PItemWeapon != nullptr,
                PItemWeapon ? PItemWeapon->getSkillType() : 0,
                PMainItem != nullptr && PMainItem->isTwoHanded()))
        {
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::Requires2HForGrip);
            return;
        }

        if (PItemWeapon && PItemWeapon->getSkillType() != SKILL_NONE)
        {
            // Don't attempt to equip item in equip menu if you don't have dual wield trait (client sees BLU, THF, DNC, NIN, /DNC or /NIN etc as able to equip sub weapons even if sub is too low or no trait on BLU)
            if (equippolicyhelpers::ShouldRequireDualWield(true, false, true, PItemWeapon->getSkillType(), PChar->hasTrait(TRAIT_DUAL_WIELD)))
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, PItemWeapon->getID(), 0, MsgBasic::NeedDualWield);
                return;
            }

            // Don't allow Dual Wield injections to offhand when you dont have a mainahdn (this was visual only)
            // Don't allow non-shields in offhand with no weapon
            if (equippolicyhelpers::ShouldBlockOffhandDualWield(
                    true,
                    false,
                    true,
                    PItemWeapon->getSkillType(),
                    PMainItem != nullptr,
                    PMainItem != nullptr && PMainItem->isTwoHanded()))
            {
                return;
            }
        }

        // Disallow everything but shields if you're using H2H
        // Equipping a shield will unequip the H2H weapon and you will go barefisted with a shield
        if (equippolicyhelpers::ShouldBlockSubWithH2HMain(true, false, PMainItem != nullptr && PMainItem->getSkillType() == SKILL_HAND_TO_HAND))
        {
            return;
        }
    }

    if (equippolicyhelpers::IsUnequipRequest(slotID))
    {
        CItemEquipment* PSubItem = PChar->getEquip(SLOT_SUB);

        UnequipItem(PChar, equipSlotID);

        if (equippolicyhelpers::ShouldRemoveSubOnMainUnequip(equipSlotID, PSubItem != nullptr, PSubItem != nullptr && PSubItem->IsShield()))
        {
            RemoveSub(PChar);
        }
    }
    else
    {
        if ((PItem != nullptr) && PItem->isType(ITEM_EQUIPMENT))
        {
            if (!PItem->isSubType(ITEM_LOCKED) && EquipArmor(PChar, slotID, equipSlotID, containerID))
            {
                const auto equipSuccessPlan = equipitemsuccesshelpers::PlanFor({
                    .hasEquipScript   = (PItem->getScriptType() & SCRIPT_EQUIP) != 0,
                    .hasUsableCharges = PItem->isType(ITEM_USABLE) && static_cast<CItemUsable*>(PItem)->getCurrentCharges() != 0,
                    .isSubSlot        = equipSlotID == SLOT_SUB,
                    .mainNeedsUnarmed = equipSlotID == SLOT_SUB &&
                                        (!PChar->getEquip(SLOT_MAIN) || !PChar->getEquip(SLOT_MAIN)->isType(ITEM_EQUIPMENT)),
                });

                if (equipSuccessPlan.setScriptEquipFlag)
                {
                    PChar->m_EquipFlag |= PItem->getScriptType();
                }

                if (equipSuccessPlan.assignChargeTime)
                {
                    PItem->setAssignTime(timer::now());
                }
                if (equipSuccessPlan.addItemRecast)
                {
                    // Add recast timer to the Recast List from any bag.
                    PChar->PRecastContainer->Add(RECAST_ITEM, static_cast<Recast>(slotID << 8 | containerID), PItem->getReuseTime());
                }
                if (equipSuccessPlan.pushItemAttr)
                {
                    // Do not forget to update the timer when equipping the subject.
                    PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PItem, static_cast<CONTAINER_ID>(containerID), slotID);
                }

                if (equipSuccessPlan.lockItem)
                {
                    PItem->setSubType(ITEM_LOCKED);
                }

                if (equipSuccessPlan.checkUnarmedWeapon)
                {
                    // If main hand is empty, check which UnarmedItem to use.
                    CheckUnarmedWeapon(PChar);
                }

                if (equipSuccessPlan.addEquipModifiers)
                {
                    PChar->addEquipModifiers(&PItem->modList, PItem->getReqLvl(), equipSlotID);
                }
                if (equipSuccessPlan.addLatentEffects)
                {
                    PChar->PLatentEffectContainer->AddLatentEffects(PItem->latentList, PItem->getReqLvl(), equipSlotID);
                }
                if (equipSuccessPlan.checkLatentsEquip)
                {
                    PChar->PLatentEffectContainer->CheckLatentsEquip(equipSlotID);
                }
                if (equipSuccessPlan.addPetModifiers)
                {
                    PChar->addPetModifiers(&PItem->petModList);
                }

                // Only call the lua onEquip if it's a valid equip - e.g. has passed EquipArmor and other checks above
                if (equipSuccessPlan.onItemEquip)
                {
                    luautils::OnItemEquip(PChar, PItem);
                }

                // queue look update on valid equip
                if (equipSuccessPlan.queueEquipChange)
                {
                    PChar->inventorySyncState().queueEquipChange(static_cast<CONTAINER_ID>(containerID), slotID, static_cast<SLOTTYPE>(equipSlotID), PItem, Equipping::Yes);
                }
            }
        }
    }

    const auto finalizePlan = equipitemfinalizehelpers::PlanFor({
        .clearTP = equippolicyhelpers::ShouldClearTPOnWeaponEquip(
            equipSlotID,
            PItem != nullptr,
            PItem != nullptr && PItem->isType(ITEM_EQUIPMENT),
            (PItem != nullptr && PItem->isType(ITEM_EQUIPMENT)) ? static_cast<CItemWeapon*>(PItem)->getSkillType() : 0,
            SKILL_STRING_INSTRUMENT,
            SKILL_WIND_INSTRUMENT),
        .mainNeedsUnarmed = !PChar->getEquip(SLOT_MAIN) || !PChar->getEquip(SLOT_MAIN)->isType(ITEM_EQUIPMENT) ||
                            PChar->m_Weapons[SLOT_MAIN] == xi::items::unarmedH2H(),
    });

    if (finalizePlan.clearTP)
    {
        // If the weapon ISN'T a wind based instrument or a string based instrument.
        PChar->health.tp = 0;
        if (finalizePlan.clearAftermath)
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Aftermath);
        }

        if (finalizePlan.checkUnarmedWeapon)
        {
            CheckUnarmedWeapon(PChar);
        }

        if (finalizePlan.buildWeaponSkills)
        {
            BuildingCharWeaponSkills(PChar);
        }
    }

    if (finalizePlan.buildSkills)
    {
        charutils::BuildingCharSkillsTable(PChar);
    }
    if (finalizePlan.updateHealth)
    {
        PChar->UpdateHealth();
    }

    if (finalizePlan.markUpdateHP)
    {
        PChar->updatemask |= UPDATE_HP;
    }
    if (finalizePlan.markUpdateLook)
    {
        PChar->updatemask |= UPDATE_LOOK;
    }
}

/************************************************************************
 *                                                                       *
 *  Check the feature of the character wearing the items equipped on it  *
 *                                                                       *
 ************************************************************************/

void CheckValidEquipment(CCharEntity* PChar)
{
    CItemEquipment* PItem = nullptr;

    for (uint8 slotID = 0; slotID < checkequipmenthelpers::EquipSlotCount; ++slotID)
    {
        PItem = PChar->getEquip((SLOTTYPE)slotID);
        if (checkequipmenthelpers::ShouldSkipNonEquipment(PItem != nullptr, PItem != nullptr && PItem->isType(ITEM_EQUIPMENT)))
        {
            continue;
        }

        const auto effectiveLevel = checkequipmenthelpers::EffectiveLevelForGearReq(
            settings::get<bool>("map.DISABLE_GEAR_SCALING"),
            PChar->GetMLevel(),
            PChar->jobs.job[PChar->GetMJob()]);
        if (checkequipmenthelpers::ShouldUnequipByLevel(PItem->getReqLvl(), effectiveLevel))
        {
            UnequipItem(PChar, slotID);
            continue;
        }

        if (checkequipmenthelpers::IsSubSlot(slotID) && !PItem->IsShield())
        {
            // Unequip if no main weapon or a non-grip subslot without DW
            const bool hasMain  = PChar->getEquip(SLOT_MAIN) != nullptr;
            const bool hasDW    = charutils::hasTrait(PChar, TRAIT_DUAL_WIELD);
            const bool isGrip   = ((CItemWeapon*)PItem)->getSkillType() == SKILL_NONE;
            if (checkequipmenthelpers::ShouldUnequipInvalidSub(false, hasMain, hasDW, isGrip))
            {
                UnequipItem(PChar, SLOT_SUB);
                continue;
            }
        }

        const auto jobBit  = checkequipmenthelpers::JobBitForMainJob(static_cast<uint8>(PChar->GetMJob()));
        const auto slotBit = checkequipmenthelpers::SlotBitForEquipSlot(slotID);
        if (checkequipmenthelpers::ShouldKeepEquipment(PItem->getJobs(), jobBit, PItem->getEquipSlotId(), slotBit))
        {
            continue;
        }

        UnequipItem(PChar, slotID);
    }
    // Unarmed H2H weapon check
    {
        auto* mainEquip = PChar->getEquip(SLOT_MAIN);
        if (checkequipmenthelpers::ShouldCheckUnarmedWeapon(
                mainEquip != nullptr,
                mainEquip != nullptr && mainEquip->isType(ITEM_EQUIPMENT),
                PChar->m_Weapons[SLOT_MAIN] == xi::items::unarmedH2H()))
        {
            CheckUnarmedWeapon(PChar);
        }
    }

    BuildingCharWeaponSkills(PChar);
    PChar->RequestPersist(CHAR_PERSIST::EQUIP);
}

void RemoveAllEquipment(CCharEntity* PChar)
{
    CItemEquipment* PItem = nullptr;

    for (uint8 slotID = 0; slotID < 16; ++slotID)
    {
        PItem = PChar->getEquip((SLOTTYPE)slotID);

        if ((PItem != nullptr) && PItem->isType(ITEM_EQUIPMENT))
        {
            UnequipItem(PChar, slotID);
        }
    }
    // Determines the UnarmedItem to use, since all slots are empty now.
    CheckUnarmedWeapon(PChar);

    BuildingCharWeaponSkills(PChar);
    PChar->RequestPersist(CHAR_PERSIST::EQUIP);
}

/************************************************************************
 *                                                                       *
 *  Load the Characters weapon skill list                                *
 *                                                                       *
 ************************************************************************/

void BuildingCharWeaponSkills(CCharEntity* PChar)
{
    std::memset(&PChar->m_WeaponSkills, 0, sizeof(PChar->m_WeaponSkills));

    CItemWeapon* PItem    = nullptr;
    int          main_ws  = 0;
    int          range_ws = 0;

    for (auto&& slot : { std::make_tuple(SLOT_MAIN, std::ref(main_ws)), std::make_tuple(SLOT_RANGED, std::ref(range_ws)) })
    {
        if (PChar->m_Weapons[std::get<0>(slot)])
        {
            PItem = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[std::get<0>(slot)]);

            // As of writing, the only unlockable weapons are: wsnm, ksnm, nyzul vigil weapons
            if (PItem && weaponskillrosterhelpers::ShouldUseUnlockableWeaponMod(true, PItem->isUnlockable(), PItem->isUnlocked()))
            {
                std::get<1>(slot) = battleutils::GetScaledItemModifier(PChar, PItem, Mod::ADDS_WEAPONSKILL);
            }
        }
    }

    // add in melee ws
    PItem       = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_MAIN));
    uint8 skill = weaponskillrosterhelpers::MeleeSkillTypeFromMain(PItem != nullptr, PItem ? PItem->getSkillType() : 0);

    const auto& MeleeWeaponSkillList = battleutils::GetWeaponSkills(skill);
    for (auto&& PSkill : MeleeWeaponSkillList)
    {
        if (weaponskillrosterhelpers::ShouldAddMeleeWeaponSkill(battleutils::CanUseWeaponskill(PChar, PSkill), PSkill->getID(), main_ws))
        {
            addWeaponSkill(PChar, PSkill->getID());
        }
    }

    // add in ranged ws
    PItem = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_RANGED));
    if (PItem != nullptr && weaponskillrosterhelpers::ShouldConsiderRangedWeaponSkills(true, PItem->isType(ITEM_WEAPON), PItem->getSkillType()))
    {
        skill                             = weaponskillrosterhelpers::RangedSkillTypeFromItem(true, PItem->getSkillType());
        const auto& RangedWeaponSkillList = battleutils::GetWeaponSkills(skill);
        for (auto&& PSkill : RangedWeaponSkillList)
        {
            if (weaponskillrosterhelpers::ShouldAddRangedWeaponSkill(battleutils::CanUseWeaponskill(PChar, PSkill), PSkill->getID(), range_ws))
            {
                addWeaponSkill(PChar, PSkill->getID());
            }
        }
    }
}

void BuildingCharPetAbilityTable(CCharEntity* PChar, CPetEntity* PPet, uint32 PetID)
{
    if (petabilitytablehelpers::ShouldRejectNullPetOrChar(PPet == nullptr, PChar == nullptr))
    {
        ShowWarning("PPet or PChar was null.");
        return;
    }

    std::memset(&PChar->m_PetCommands, 0, sizeof(PChar->m_PetCommands));

    if (petabilitytablehelpers::ShouldClearPetCommandsOnly(PetID))
    { // technically Fire Spirit but we're using this to null the abilities shown
        PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
        return;
    }

    if (petabilitytablehelpers::IsSummonerJob(static_cast<uint8>(PChar->GetMJob()), static_cast<uint8>(PChar->GetSJob())))
    {
        std::vector<CAbility*> AbilitiesList = ability::GetAbilities(JOB_SMN);

        for (auto PAbility : AbilitiesList)
        {
            if (petabilitytablehelpers::ShouldConsiderSMNPetAbility(PPet->GetMLevel(), PAbility->getLevel(), PetID, CheckAbilityAddtype(PChar, PAbility)))
            {
                if (PetID == PETID_CARBUNCLE)
                {
                    if (petabilitytablehelpers::IsCarbuncleAbility(PAbility->getID()))
                    {
                        addPetAbility(PChar, petabilitytablehelpers::CarbunclePetAbilityBit(PAbility->getID()));
                    }
                }
                else if (petabilitytablehelpers::IsElementalAvatarPet(PetID))
                {
                    if (petabilitytablehelpers::ElementalAvatarAbilityInBand(PAbility->getID(), PetID))
                    {
                        addPetAbility(PChar, petabilitytablehelpers::AvatarPetAbilityBit(PAbility->getID()));
                    }
                }
                else if (PetID == PETID_DIABOLOS)
                {
                    if (petabilitytablehelpers::IsDiabolosAbility(PAbility->getID()))
                    {
                        addPetAbility(PChar, petabilitytablehelpers::AvatarPetAbilityBit(PAbility->getID()));
                    }
                }
                else if (PetID == PETID_CAIT_SITH)
                {
                    if (petabilitytablehelpers::IsCaitSithAbility(PAbility->getID()))
                    {
                        addPetAbility(PChar, petabilitytablehelpers::AvatarPetAbilityBit(PAbility->getID()));
                    }
                }
                else if (PetID == PETID_SIREN)
                {
                    if (petabilitytablehelpers::IsSirenAbility(PAbility->getID()))
                    {
                        addPetAbility(PChar, petabilitytablehelpers::SirenPetAbilityBit(PAbility->getID()));
                    }
                }
            }
        }
    }
    if (PPet->getPetType() == PET_TYPE::JUG_PET)
    {
        auto skillList{ battleutils::GetMobSkillList(PPet->m_MobSkillList) };
        for (auto&& abilityid : skillList)
        {
            addPetAbility(PChar, petabilitytablehelpers::JugPetAbilityBit(static_cast<uint16>(abilityid)));
        }
    }
    PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
}

void BuildingCharAbilityTable(CCharEntity* PChar)
{
    if (abilitytablehelpers::ShouldRejectNullChar(PChar == nullptr))
    {
        ShowWarning("charutils::BuildingCharAbilityTable() - PChar was null.");
        return;
    }

    std::memset(&PChar->m_Abilities, 0, sizeof(PChar->m_Abilities));

    for (auto PAbility : ability::GetAbilities(PChar->GetMJob()))
    {
        if (PAbility == nullptr)
        {
            continue;
        }

        if (abilitytablehelpers::ShouldStopMainAbilityLoop(PChar->GetMLevel(), PAbility->getLevel()))
        {
            break;
        }

        if (abilitytablehelpers::ShouldAddMainAbility(PAbility->getID(), CheckAbilityAddtype(PChar, PAbility)))
        {
            addAbility(PChar, PAbility->getID());
            Charge_t*       charge     = ability::GetCharge(PChar, static_cast<uint16>(PAbility->getRecastId()));
            timer::duration chargeTime = 0s;
            auto            maxCharges = 0;
            if (charge)
            {
                const auto meritSecs = PChar->PMeritPoints->GetMeritValue((MERIT_TYPE)charge->merit, PChar);
                chargeTime           = charge->chargeTime - std::chrono::seconds(meritSecs);
                maxCharges           = charge->maxCharges;
            }
            if (abilitytablehelpers::ShouldInitAbilityRecast(PChar->PRecastContainer->Has(RECAST_ABILITY, PAbility->getRecastId())))
            {
                PChar->PRecastContainer->Add(RECAST_ABILITY, PAbility->getRecastId(), 0s, chargeTime, maxCharges);
            }
        }
    }

    // To stop a character with no SJob to receive the traits with job = 0 in the DB.
    if (abilitytablehelpers::ShouldSkipSubJobAbilities(static_cast<uint8>(PChar->GetSJob())))
    {
        return;
    }

    for (auto PAbility : ability::GetAbilities(PChar->GetSJob()))
    {
        if (!PAbility)
        {
            continue;
        }

        if (abilitytablehelpers::ShouldStopSubAbilityLoop(PChar->GetSLevel(), PAbility->getLevel()))
        {
            break;
        }

        if (abilitytablehelpers::ShouldAddSubAbility(PAbility->getLevel(), PAbility->getID(), PAbility->getAddType(), CheckAbilityAddtype(PChar, PAbility)))
        {
            addAbility(PChar, PAbility->getID());
            Charge_t*       charge     = ability::GetCharge(PChar, static_cast<uint16>(PAbility->getRecastId()));
            timer::duration chargeTime = 0s;
            auto            maxCharges = 0;
            if (charge)
            {
                const auto meritSecs = PChar->PMeritPoints->GetMeritValue((MERIT_TYPE)charge->merit, PChar);
                chargeTime           = charge->chargeTime - std::chrono::seconds(meritSecs);
                maxCharges           = charge->maxCharges;
            }
            if (abilitytablehelpers::ShouldInitAbilityRecast(PChar->PRecastContainer->Has(RECAST_ABILITY, PAbility->getRecastId())))
            {
                PChar->PRecastContainer->Add(RECAST_ABILITY, PAbility->getRecastId(), 0s, chargeTime, maxCharges);
            }
        }
    }
}

bool isArtsBonusActive(CCharEntity* PChar, SKILLTYPE SkillID)
{
    return (SkillID >= SKILL_DIVINE_MAGIC && SkillID <= SKILL_ENFEEBLING_MAGIC &&
            PChar->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::LightArts, xi::StatusEffect::AddendumWhite })) ||
           (SkillID >= SKILL_ENFEEBLING_MAGIC && SkillID <= SKILL_DARK_MAGIC &&
            PChar->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::DarkArts, xi::StatusEffect::AddendumBlack }));
}

// calculates the bonus skill based on active sch arts
int16 ArtsBonusSkill(CCharEntity* PChar, SKILLTYPE SkillID)
{
    int16 skillBonus = 0;

    uint16 maxMainSkill = battleutils::GetMaxSkill(SkillID, PChar->GetMJob(), PChar->GetMLevel());
    uint16 maxSubSkill  = battleutils::GetMaxSkill(SkillID, PChar->GetSJob(), PChar->GetSLevel());

    uint16 artsSkill    = battleutils::GetMaxSkill(SKILL_ENHANCING_MAGIC, JOB_RDM, PChar->GetMLevel());                               // B+ skill
    uint16 skillCapD    = battleutils::GetMaxSkill(SkillID, JOB_SCH, PChar->GetMLevel());                                             // D skill cap
    uint16 skillCapE    = battleutils::GetMaxSkill(SKILL_DARK_MAGIC, JOB_RDM, PChar->GetMLevel());                                    // E skill cap
    auto   currentSkill = std::clamp<uint16>((PChar->RealSkills.skill[(int32)SkillID] / 10), 0, std::max(maxMainSkill, maxSubSkill)); // working skill before bonuses
    uint16 artsBaseline = 0;                                                                                                          // Level based baseline to which to raise skills
    uint8  mLevel       = PChar->GetMLevel();
    if (mLevel < 51)
    {
        artsBaseline = (uint16)(5 + 2.7 * (mLevel - 1));
    }
    else if (mLevel < 61)
    {
        artsBaseline = (uint16)(137 + 4.7 * (mLevel - 50));
    }
    else if (mLevel < 71)
    {
        artsBaseline = (uint16)(184 + 3.7 * (mLevel - 60));
    }
    else if (mLevel < 75)
    {
        artsBaseline = (uint16)(221 + 5.0 * (mLevel - 70));
    }
    else // >= 75
    {
        artsBaseline = skillCapD + 36;
    }

    if (currentSkill < skillCapE)
    {
        // If the player's skill is below the E cap
        // give enough bonus points to raise it to the arts baseline
        skillBonus += std::max(artsBaseline - currentSkill, 0);
    }
    else if (currentSkill < skillCapD)
    {
        // if the skill is at or above the E cap but below the D cap
        // raise it up to the B+ skill cap minus the difference between the current skill rank and the scholar base skill cap (D)
        // i.e. give a bonus of the difference between the B+ skill cap and the D skill cap
        skillBonus += std::max((artsSkill - skillCapD), 0);
    }
    else if (currentSkill < artsSkill)
    {
        // If the player's skill is at or above the D cap but below the B+ cap
        // give enough bonus points to raise it to the B+ cap
        skillBonus += std::max(artsSkill - currentSkill, 0);
    }

    if (PChar->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::LightArts, xi::StatusEffect::AddendumWhite }))
    {
        skillBonus += PChar->getMod(Mod::LIGHT_ARTS_SKILL);
    }
    else
    {
        skillBonus += PChar->getMod(Mod::DARK_ARTS_SKILL);
    }

    return skillBonus;
}

/************************************************************************
 *                                                                       *
 *  Collect the work table of the character skills based on real.        *
 *  Add restrictions, note the skills of the main job (rank! = 0)        *
 *                                                                       *
 ************************************************************************/

// TODO: This whole thing should eventually get a refactored to be less dependent on arbitrary ordering of modifier IDs and conditionals on skill ranges.
void BuildingCharSkillsTable(CCharEntity* PChar)
{
    MERIT_TYPE skillMerit[] = { MERIT_H2H,
                                MERIT_DAGGER,
                                MERIT_SWORD,
                                MERIT_GSWORD,
                                MERIT_AXE,
                                MERIT_GAXE,
                                MERIT_SCYTHE,
                                MERIT_POLEARM,
                                MERIT_KATANA,
                                MERIT_GKATANA,
                                MERIT_CLUB,
                                MERIT_STAFF,
                                MERIT_AUTOMATON_SKILLS,
                                MERIT_AUTOMATON_SKILLS,
                                MERIT_AUTOMATON_SKILLS,
                                MERIT_ARCHERY,
                                MERIT_MARKSMANSHIP,
                                MERIT_THROWING,
                                MERIT_GUARDING,
                                MERIT_EVASION,
                                MERIT_SHIELD,
                                MERIT_PARRYING,
                                MERIT_DIVINE,
                                MERIT_HEALING,
                                MERIT_ENHANCING,
                                MERIT_ENFEEBLING,
                                MERIT_ELEMENTAL,
                                MERIT_DARK,
                                MERIT_SUMMONING,
                                MERIT_NINJITSU,
                                MERIT_SINGING,
                                MERIT_STRING,
                                MERIT_WIND,
                                MERIT_BLUE,
                                MERIT_GEO,
                                MERIT_HANDBELL };

    uint8 meritIndex = 0;

    bool automatonSkillUpdated = false;

    // Iterate over skill IDs (offsetting by 79 to get modifier ID)
    for (int32 i = 1; i < 48; ++i)
    {
        // ignore unused skills
        if (buildingskillshelpers::IsUnusedCombatSkillSlot(i))
        {
            PChar->WorkingSkills.skill[i] = buildingskillshelpers::SkillCappedBlueFlag;
            continue;
        }
        uint16 maxMainSkill = battleutils::GetMaxSkill((SKILLTYPE)i, PChar->GetMJob(), PChar->GetMLevel());
        uint16 maxSubSkill  = battleutils::GetMaxSkill((SKILLTYPE)i, PChar->GetSJob(), PChar->GetSLevel());
        int16  skillBonus   = 0;

        // apply arts bonuses
        if (isArtsBonusActive(PChar, static_cast<SKILLTYPE>(i)))
        {
            skillBonus += ArtsBonusSkill(PChar, static_cast<SKILLTYPE>(i));
        }
        else if (buildingskillshelpers::IsAutomatonSkill(i))
        {
            // TODO: does this need to change if you are /PUP?
            maxMainSkill = battleutils::GetMaxSkill(1, PChar->GetMLevel()); // A+ capped down to the Automaton's rating
        }

        skillBonus += PChar->PMeritPoints->GetMeritValue(skillMerit[meritIndex], PChar);
        meritIndex++;

        // Add 79 to get the modifier ID
        skillBonus += PChar->getMod(static_cast<Mod>(buildingskillshelpers::SkillModID(i))); // This can be a negative value. Example: Shiva's Shotel.

        uint8 mainSkillRank = battleutils::GetSkillRank((SKILLTYPE)i, PChar->GetMJob());
        uint8 subSkillRank  = battleutils::GetSkillRank((SKILLTYPE)i, PChar->GetSJob());

        PChar->WorkingSkills.rank[i] = mainSkillRank;

        PChar->RealSkills.rank[i] = buildingskillshelpers::ResolveRealSkillRank(mainSkillRank, subSkillRank);

        uint16 currentSkill = buildingskillshelpers::RealSkillLevels(PChar->RealSkills.skill[i]);

        // Main Job Skills.
        if (buildingskillshelpers::IsMainJobSkillPath(maxMainSkill))
        {
            currentSkill = buildingskillshelpers::CapCurrentSkill(currentSkill, maxMainSkill);

            PChar->WorkingSkills.skill[i] = buildingskillshelpers::WorkingSkillFromCurrentAndBonus(currentSkill, skillBonus);

            if (buildingskillshelpers::ShouldSetBlueCapFlag(currentSkill, maxMainSkill))
            {
                PChar->WorkingSkills.skill[i] = buildingskillshelpers::WithBlueFlag(PChar->WorkingSkills.skill[i]);
            }
        }

        // Sub Job Skills.
        else if (buildingskillshelpers::IsSubJobSkillPath(maxMainSkill, maxSubSkill))
        {
            currentSkill = buildingskillshelpers::CapCurrentSkill(currentSkill, maxSubSkill);

            PChar->WorkingSkills.skill[i] = buildingskillshelpers::WorkingSkillFromCurrentAndBonus(currentSkill, skillBonus);

            if (buildingskillshelpers::ShouldSetBlueCapFlag(currentSkill, maxSubSkill))
            {
                PChar->WorkingSkills.skill[i] = buildingskillshelpers::WithBlueFlag(PChar->WorkingSkills.skill[i]);
            }
        }

        // Job setup doesn't have this skill.
        else
        {
            PChar->WorkingSkills.skill[i] = buildingskillshelpers::NonJobSkillWorkingValue(skillBonus);
        }

        // Automaton skills are special (especially with magic...)
        if (buildingskillshelpers::IsAutomatonSkill(i))
        {
            if (auto PAutomaton = dynamic_cast<CAutomatonEntity*>(PChar->PPet))
            {
                switch (i)
                {
                    case SKILL_AUTOMATON_MAGIC:
                        PAutomaton->WorkingSkills.skill[i] = PChar->WorkingSkills.skill[i];

                        PAutomaton->WorkingSkills.skill[SKILL_HEALING_MAGIC]    = PChar->WorkingSkills.skill[i];
                        PAutomaton->WorkingSkills.skill[SKILL_ENHANCING_MAGIC]  = PChar->WorkingSkills.skill[i];
                        PAutomaton->WorkingSkills.skill[SKILL_ENFEEBLING_MAGIC] = PChar->WorkingSkills.skill[i];
                        PAutomaton->WorkingSkills.skill[SKILL_ELEMENTAL_MAGIC]  = PChar->WorkingSkills.skill[i];
                        PAutomaton->WorkingSkills.skill[SKILL_DARK_MAGIC]       = PChar->WorkingSkills.skill[i];
                        break;

                    default:
                        PAutomaton->WorkingSkills.skill[i] = PChar->WorkingSkills.skill[i];
                        break;
                }

                automatonSkillUpdated = true;
            }
        }
    }

    for (int32 i = 48; i < 58; ++i)
    {
        PChar->WorkingSkills.skill[i] = buildingskillshelpers::CraftWorkingSkill(PChar->RealSkills.skill[i], PChar->RealSkills.rank[i]);

        if (buildingskillshelpers::ShouldSetCraftBlueFlag(PChar->RealSkills.rank[i], PChar->RealSkills.skill[i]))
        {
            PChar->WorkingSkills.skill[i] = buildingskillshelpers::WithBlueFlag(PChar->WorkingSkills.skill[i]);
        }
    }

    for (int32 i = 58; i < 64; ++i)
    {
        PChar->WorkingSkills.skill[i] = buildingskillshelpers::SkillUnusedFFFF;
    }

    // Update skills menu
    if (automatonSkillUpdated)
    {
        charutils::SendExtendedJobPackets(PChar);
    }
}

void BuildingCharTraitsTable(CCharEntity* PChar)
{
    for (std::size_t i = 0; i < PChar->TraitList.size(); ++i)
    {
        CTrait* PTrait = PChar->TraitList.at(i);
        PChar->delModifier(PTrait->getMod(), PTrait->getValue());
    }
    PChar->TraitList.clear();
    std::memset(&PChar->m_TraitList, 0, sizeof(PChar->m_TraitList));

    auto mjob = PChar->GetMJob();
    auto sjob = PChar->GetSJob();
    auto mlvl = PChar->GetMLevel();
    auto slvl = PChar->GetSLevel();

    // NOTE: Monstrosity (MON) is treated as its own job, but each species is it's own
    //     : combination of main/sub job for stats, traits and abilities.
    if (petabilitytablehelpers::ShouldApplyMonstrosityJobs(PChar->m_PMonstrosity != nullptr))
    {
        mjob = PChar->m_PMonstrosity->MainJob;
        sjob = PChar->m_PMonstrosity->SubJob;
        mlvl = PChar->m_PMonstrosity->levels[PChar->m_PMonstrosity->MonstrosityId];
        slvl = mlvl;
    }

    battleutils::AddTraits(PChar, traits::GetTraits(mjob), mlvl);
    battleutils::AddTraits(PChar, traits::GetTraits(sjob), slvl);

    if (petabilitytablehelpers::ShouldCalculateBlueTraits(static_cast<uint8>(mjob), static_cast<uint8>(sjob)))
    {
        blueutils::CalculateTraits(PChar);
    }

    PChar->delModifier(Mod::MEVA, PChar->m_magicEvasion);

    PChar->m_magicEvasion = battleutils::GetMaxSkill(petabilitytablehelpers::PlayerMEVASkillRank, mlvl); // Player MEVA is Rank G
    PChar->addModifier(Mod::MEVA, PChar->m_magicEvasion);
}

/************************************************************************
 *                                                                       *
 *  Try to increase the value of the skill                               *
 *                                                                       *
 ************************************************************************/

void TrySkillUP(CCharEntity* PChar, SKILLTYPE SkillID, uint8 lvl, bool forceSkillUp, bool useSubSkill)
{
    TracyZoneScoped;

    // This usually happens after a crash
    uint8 rawSkillID = static_cast<uint8>(SkillID);
    if (!skilluphelpers::IsSkillIDValid(rawSkillID))
    {
        ShowWarning("SkillID (%d) exceeds MAX_SKILLTYPE.", rawSkillID);
        return;
    }

    if (skilluphelpers::ShouldConsiderSkillUp(PChar->WorkingSkills.rank[rawSkillID], PChar->WorkingSkills.skill[rawSkillID], useSubSkill))
    {
        uint16 CurSkill     = PChar->RealSkills.skill[rawSkillID];
        uint16 MainCapSkill = battleutils::GetMaxSkill(SkillID, PChar->GetMJob(), PChar->GetMLevel());
        uint16 SubCapSkill  = battleutils::GetMaxSkill(SkillID, PChar->GetSJob(), PChar->GetSLevel());
        uint16 MainMaxSkill = battleutils::GetMaxSkill(SkillID, PChar->GetMJob(), std::min(PChar->GetMLevel(), lvl));
        uint16 SubMaxSkill  = battleutils::GetMaxSkill(SkillID, PChar->GetSJob(), std::min(PChar->GetSLevel(), lvl));
        uint16 CapSkill     = skilluphelpers::ResolveCapSkill(MainCapSkill, SubCapSkill, useSubSkill);
        uint16 MaxSkill     = skilluphelpers::ResolveMaxSkill(MainCapSkill, SubCapSkill, MainMaxSkill, SubMaxSkill, useSubSkill);
        // Max skill this victim level will allow.
        // Note this is no longer retail accurate, since now 'decent challenge' mobs allow to cap any skill.

        int16  Diff          = skilluphelpers::SkillDiff(MaxSkill, CurSkill);
        double SkillUpChance = skilluphelpers::BaseSkillUpChance(Diff, CurSkill, settings::get<double>("map.SKILLUP_CHANCE_MULTIPLIER"));

        double random = xirand::GetRandomNumber(1.);

        SkillUpChance = skilluphelpers::ClampSkillUpChance(SkillUpChance);

        // Check for skillup% bonus. https://www.bg-wiki.com/bg/Category:Skill_Up_Food
        // Assuming multiplicative even though rate is already a % because 0.5 + 0.8 would be > 1.
        if (skilluphelpers::IsCombatSkillUpSkill(static_cast<uint8>(SkillID)))
        // if should effect automaton replace the above with: (SkillID >= 1 && SkillID <= 31)
        {
            SkillUpChance = skilluphelpers::ApplySkillUpRateMod(SkillUpChance, PChar->getMod(Mod::COMBAT_SKILLUP_RATE));
        }
        else if (skilluphelpers::IsMagicSkillUpSkill(static_cast<uint8>(SkillID)))
        {
            SkillUpChance = skilluphelpers::ApplySkillUpRateMod(SkillUpChance, PChar->getMod(Mod::MAGIC_SKILLUP_RATE));
        }

        if (skilluphelpers::ShouldGainSkillUp(Diff, random, SkillUpChance, forceSkillUp))
        {
            double chance      = 0;
            uint8  SkillAmount = 1;
            uint8  tier        = skilluphelpers::SkillUpTier(Diff);

            for (uint8 i = 0; i < 4; ++i) // 1 + 4 possible additional ones (maximum 5)
            {
                random = xirand::GetRandomNumber(1.);
                chance = skilluphelpers::ExtraSkillUpTierChance(tier);

                if (skilluphelpers::ShouldStopExtraSkillUp(chance, random, SkillAmount))
                {
                    break;
                }

                tier -= 1;
                SkillAmount += 1;
            }
            // convert to 10th units
            CapSkill = skilluphelpers::CapSkillTenths(CapSkill);

            uint8 rovKeyItemCount = 0;
            for (const auto skillupIncreaseKeyItem : skillupIncreaseKeyItems)
            {
                if (hasKeyItem(PChar, skillupIncreaseKeyItem))
                {
                    rovKeyItemCount += 1;
                }
            }

            SkillAmount = skilluphelpers::ApplyRovSkillAmount(SkillAmount, rovKeyItemCount);
            SkillAmount = skilluphelpers::ApplySkillAmountMultiplier(SkillAmount, settings::get<uint8>("map.SKILLUP_AMOUNT_MULTIPLIER"));

            if (skilluphelpers::HitsSkillCap(SkillAmount, CurSkill, CapSkill))
            {
                // skill is capped. set blue flag
                SkillAmount = skilluphelpers::CapSkillAmountToCeiling(SkillAmount, CurSkill, CapSkill);
                PChar->WorkingSkills.skill[SkillID] |= skilluphelpers::SkillCappedBlueFlag;
            }

            // check if skillup changed the bonus from sch arts
            int16 skillBonus = 0;
            if (isArtsBonusActive(PChar, SkillID))
            {
                skillBonus = ArtsBonusSkill(PChar, SkillID);
            }

            PChar->RealSkills.skill[SkillID] += SkillAmount;
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, SkillID, SkillAmount, MsgBasic::SkillGain);

            if (skilluphelpers::CrossedSkillLevel(CurSkill, SkillAmount)) // if gone up a level
            {
                // Light/Dark Arts artificially boost certain skills
                // if skillup happens when real skill is below the base for active arts, don't increment the shown skill
                if (skilluphelpers::ShouldIncrementWorkingSkill(
                        isArtsBonusActive(PChar, SkillID),
                        skillBonus,
                        isArtsBonusActive(PChar, SkillID) ? ArtsBonusSkill(PChar, SkillID) : skillBonus))
                {
                    PChar->WorkingSkills.skill[SkillID] += 1;
                }
                PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS2>(PChar);
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, SkillID, (CurSkill + SkillAmount) / 10, MsgBasic::SkillLevelUp);

                CheckWeaponSkill(PChar, SkillID);
                /* ignoring this for now
                if (SkillID >= 1 && SkillID <= 12)
                {
                PChar->addModifier(Mod::ATT, 1);
                PChar->addModifier(Mod::ACC, 1);
                }
                */
            }
            SaveCharSkills(PChar, SkillID);
        }
    }
}

void CheckWeaponSkill(CCharEntity* PChar, uint8 skill)
{
    auto* weapon       = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[SLOT_MAIN]);
    auto* rangedWeapon = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[SLOT_RANGED]);

    const bool mainMatches   = weaponskillrosterhelpers::IsMatchingWeaponSkill(weapon != nullptr, weapon ? weapon->getSkillType() : 0, skill);
    const bool rangedMatches = weaponskillrosterhelpers::IsMatchingWeaponSkill(rangedWeapon != nullptr, rangedWeapon ? rangedWeapon->getSkillType() : 0, skill);

    if (weaponskillrosterhelpers::ShouldSkipCheckWeaponSkill(mainMatches, rangedMatches))
    {
        return;
    }

    const auto& WeaponSkillList = battleutils::GetWeaponSkills(skill);
    uint16      curSkill        = weaponskillrosterhelpers::RealSkillLevels(PChar->RealSkills.skill[skill]);

    for (auto&& PSkill : WeaponSkillList)
    {
        if (weaponskillrosterhelpers::ShouldUnlockWeaponSkillOnSkillUp(curSkill, PSkill->getSkillLevel(), battleutils::CanUseWeaponskill(PChar, PSkill)))
        {
            addWeaponSkill(PChar, PSkill->getID());
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, PSkill->getID(), PSkill->getID(), MsgBasic::LearnsAbility);
            PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Methods for working with key items                                   *
 *                                                                       *
 ************************************************************************/

auto hasKeyItem(const CCharEntity* PChar, const KeyItem keyItemId) -> bool
{
    const auto keyItemTable = keyitemspellhelpers::KeyItemTableIndex(static_cast<uint16_t>(keyItemId));
    const auto keyItemIndex = keyitemspellhelpers::KeyItemBitIndex(static_cast<uint16_t>(keyItemId));

    if (!keyitemspellhelpers::KeyItemTableInRange(keyItemTable, PChar->keys.tables.size()))
    {
        ShowErrorFmt("charutils::hasKeyItem() - Index {} exceeds key items table capacity.", keyItemTable);
        return false;
    }

    return PChar->keys.tables[keyItemTable].keyList[keyItemIndex];
}

auto seenKeyItem(CCharEntity* PChar, KeyItem keyItemId) -> bool
{
    const auto keyItemTable = keyitemspellhelpers::KeyItemTableIndex(static_cast<uint16_t>(keyItemId));
    const auto keyItemIndex = keyitemspellhelpers::KeyItemBitIndex(static_cast<uint16_t>(keyItemId));

    if (!keyitemspellhelpers::KeyItemTableInRange(keyItemTable, PChar->keys.tables.size()))
    {
        ShowErrorFmt("charutils::seenKeyItem() - Index {} exceeds key items table capacity.", keyItemTable);
        return false;
    }

    return PChar->keys.tables[keyItemTable].seenList[keyItemIndex];
}

void markSeenKeyItem(CCharEntity* PChar, KeyItem keyItemId)
{
    const auto keyItemTable = keyitemspellhelpers::KeyItemTableIndex(static_cast<uint16_t>(keyItemId));
    const auto keyItemIndex = keyitemspellhelpers::KeyItemBitIndex(static_cast<uint16_t>(keyItemId));

    if (!keyitemspellhelpers::KeyItemTableInRange(keyItemTable, PChar->keys.tables.size()))
    {
        ShowErrorFmt("charutils::markSeenKeyItem() - Index {} exceeds key items table capacity.", keyItemTable);
        return;
    }

    PChar->keys.tables[keyItemTable].seenList[keyItemIndex] = true;
}

void unseenKeyItem(CCharEntity* PChar, KeyItem keyItemId)
{
    const auto keyItemTable = keyitemspellhelpers::KeyItemTableIndex(static_cast<uint16_t>(keyItemId));
    const auto keyItemIndex = keyitemspellhelpers::KeyItemBitIndex(static_cast<uint16_t>(keyItemId));

    if (!keyitemspellhelpers::KeyItemTableInRange(keyItemTable, PChar->keys.tables.size()))
    {
        ShowErrorFmt("charutils::unseenKeyItem() - Index {} exceeds key items table capacity.", keyItemTable);
        return;
    }

    PChar->keys.tables[keyItemTable].seenList[keyItemIndex] = false;
}

void addKeyItem(CCharEntity* PChar, KeyItem keyItemId)
{
    const auto keyItemTable = keyitemspellhelpers::KeyItemTableIndex(static_cast<uint16_t>(keyItemId));
    const auto keyItemIndex = keyitemspellhelpers::KeyItemBitIndex(static_cast<uint16_t>(keyItemId));

    if (!keyitemspellhelpers::KeyItemTableInRange(keyItemTable, PChar->keys.tables.size()))
    {
        ShowErrorFmt("charutils::addKeyItem() - Index {} exceeds key items table capacity.", keyItemTable);
        return;
    }

    PChar->keys.tables[keyItemTable].keyList[keyItemIndex] = true;
}

void delKeyItem(CCharEntity* PChar, KeyItem keyItemId)
{
    const auto keyItemTable = keyitemspellhelpers::KeyItemTableIndex(static_cast<uint16_t>(keyItemId));
    const auto keyItemIndex = keyitemspellhelpers::KeyItemBitIndex(static_cast<uint16_t>(keyItemId));

    if (!keyitemspellhelpers::KeyItemTableInRange(keyItemTable, PChar->keys.tables.size()))
    {
        ShowErrorFmt("charutils::delKeyItem() - Index {} exceeds key items table capacity.", keyItemTable);
        return;
    }

    PChar->keys.tables[keyItemTable].keyList[keyItemIndex] = false;
}

/************************************************************************
 *                                                                       *
 *  Methods for working with spells                                      *
 *                                                                       *
 ************************************************************************/

int32 hasSpell(CCharEntity* PChar, uint16 SpellID)
{
    return PChar->m_SpellList[SpellID];
}

int32 addSpell(CCharEntity* PChar, uint16 spellID)
{
    auto* PSpell = spell::GetSpell(static_cast<SpellID>(spellID));
    if (keyitemspellhelpers::ShouldAddSpell(PSpell != nullptr, hasSpell(PChar, spellID) != 0))
    {
        PChar->m_SpellList[spellID] = true;
        return keyitemspellhelpers::SpellMutationSuccess();
    }
    return keyitemspellhelpers::SpellMutationNoOp();
}

int32 delSpell(CCharEntity* PChar, uint16 spellID)
{
    auto* PSpell = spell::GetSpell(static_cast<SpellID>(spellID));
    if (keyitemspellhelpers::ShouldDelSpell(PSpell != nullptr, hasSpell(PChar, spellID) != 0))
    {
        PChar->m_SpellList[spellID] = false;
        return keyitemspellhelpers::SpellMutationSuccess();
    }
    return keyitemspellhelpers::SpellMutationNoOp();
}

/************************************************************************
 *                                                                       *
 *  Learned abilities (corsair rolls)                                    *
 *                                                                       *
 ************************************************************************/

int32 hasLearnedAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return hasBit(AbilityID, PChar->m_LearnedAbilities, sizeof(PChar->m_LearnedAbilities));
}

int32 addLearnedAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return addBit(AbilityID, PChar->m_LearnedAbilities, sizeof(PChar->m_LearnedAbilities));
}

int32 delLearnedAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return delBit(AbilityID, PChar->m_LearnedAbilities, sizeof(PChar->m_LearnedAbilities));
}

/************************************************************************
 *                                                                       *
 *  Learned weaponskills                                                 *
 *                                                                       *
 ************************************************************************/

bool hasLearnedWeaponskill(CCharEntity* PChar, uint8 wsUnlockId)
{
    if (keyitemspellhelpers::ShouldRejectNullChar(PChar == nullptr))
    {
        ShowError("PChar is null.");
        return false;
    }

    if (!keyitemspellhelpers::LearnedWeaponskillInRange(wsUnlockId, PChar->m_LearnedWeaponskills.size()))
    {
        ShowError("wsUnlockId is greater than learned weaponskill bitset.");
        return false;
    }

    return PChar->m_LearnedWeaponskills[wsUnlockId];
}

void addLearnedWeaponskill(CCharEntity* PChar, uint8 wsUnlockId)
{
    if (keyitemspellhelpers::ShouldRejectNullChar(PChar == nullptr))
    {
        ShowError("PChar is null.");
        return;
    }

    if (!keyitemspellhelpers::LearnedWeaponskillInRange(wsUnlockId, PChar->m_LearnedWeaponskills.size()))
    {
        ShowError("wsUnlockId is greater than learned weaponskill bitset.");
        return;
    }

    PChar->m_LearnedWeaponskills[wsUnlockId] = true;
}

void delLearnedWeaponskill(CCharEntity* PChar, uint8 wsUnlockId)
{
    if (keyitemspellhelpers::ShouldRejectNullChar(PChar == nullptr))
    {
        ShowError("PChar is null.");
        return;
    }

    if (!keyitemspellhelpers::LearnedWeaponskillInRange(wsUnlockId, PChar->m_LearnedWeaponskills.size()))
    {
        ShowError("wsUnlockId is greater than learned weaponskill bitset.");
        return;
    }

    PChar->m_LearnedWeaponskills[wsUnlockId] = false;
}


int32 hasTitle(CCharEntity* PChar, uint16 Title)
{
    return hasBit(Title, PChar->m_TitleList, sizeof(PChar->m_TitleList));
}

int32 addTitle(CCharEntity* PChar, uint16 Title)
{
    return addBit(Title, PChar->m_TitleList, sizeof(PChar->m_TitleList));
}

int32 delTitle(CCharEntity* PChar, uint16 Title)
{
    return delBit(Title, PChar->m_TitleList, sizeof(PChar->m_TitleList));
}

void setTitle(CCharEntity* PChar, uint16 Title)
{
    PChar->profile.title = Title;
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);

    addTitle(PChar, Title);
    SaveTitles(PChar);
}

/************************************************************************
 *                                                                       *
 *  Methods for working with basic abilities                             *
 *                                                                       *
 ************************************************************************/

int32 hasAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return hasBit(AbilityID, PChar->m_Abilities, sizeof(PChar->m_Abilities));
}

int32 addAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return addBit(AbilityID, PChar->m_Abilities, sizeof(PChar->m_Abilities));
}

int32 delAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return delBit(AbilityID, PChar->m_Abilities, sizeof(PChar->m_Abilities));
}

/************************************************************************
 *                                                                       *
 *  Weapon Skill functions                                               *
 *                                                                       *
 ************************************************************************/

int32 hasWeaponSkill(CCharEntity* PChar, uint16 WeaponSkillID)
{
    return hasBit(WeaponSkillID, PChar->m_WeaponSkills, sizeof(PChar->m_WeaponSkills));
}

int32 addWeaponSkill(CCharEntity* PChar, uint16 WeaponSkillID)
{
    return addBit(WeaponSkillID, PChar->m_WeaponSkills, sizeof(PChar->m_WeaponSkills));
}

int32 delWeaponSkill(CCharEntity* PChar, uint16 WeaponSkillID)
{
    return delBit(WeaponSkillID, PChar->m_WeaponSkills, sizeof(PChar->m_WeaponSkills));
}

bool canUseWeaponSkill(CCharEntity* PChar, uint16 wsid)
{
    CWeaponSkill* PWeaponSkill = battleutils::GetWeaponSkill(wsid);

    if (PWeaponSkill == nullptr)
    {
        ShowError("Invalid Weaponskill ID passed to function.");
        return false;
    }

    return equippolicyhelpers::CanUseWeaponSkillByLevel(PChar->GetSkill(PWeaponSkill->getType()), PWeaponSkill->getSkillLevel());
}

/************************************************************************
 *                                                                       *
 *  Trait Functions                                                      *
 *                                                                       *
 ************************************************************************/

int32 hasTrait(CCharEntity* PChar, uint16 TraitID)
{
    if (equippolicyhelpers::ShouldRejectNonPCTrait(PChar->objtype == TYPE_PC))
    {
        ShowError("charutils::hasTrait Attempt to reference a trait from a non-character entity: %s %i", PChar->name.c_str(), PChar->id);
        return 0;
    }
    return hasBit(TraitID, PChar->m_TraitList, sizeof(PChar->m_TraitList));
}

int32 addTrait(CCharEntity* PChar, uint16 TraitID)
{
    if (equippolicyhelpers::ShouldRejectNonPCTrait(PChar->objtype == TYPE_PC))
    {
        ShowError("charutils::addTrait Attempt to reference a trait from a non-character entity: %s %i", PChar->name.c_str(), PChar->id);
        return 0;
    }
    return addBit(TraitID, PChar->m_TraitList, sizeof(PChar->m_TraitList));
}

int32 delTrait(CCharEntity* PChar, uint16 TraitID)
{
    if (equippolicyhelpers::ShouldRejectNonPCTrait(PChar->objtype == TYPE_PC))
    {
        ShowError("charutils::delTrait Attempt to reference a trait from a non-character entity: %s %i", PChar->name.c_str(), PChar->id);
        return 0;
    }
    return delBit(TraitID, PChar->m_TraitList, sizeof(PChar->m_TraitList));
}

/************************************************************************
 *                                                                       *
 *  Pet Command Functions                                                *
 *                                                                       *
 ************************************************************************/

int32 hasPetAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return hasBit(AbilityID, PChar->m_PetCommands, sizeof(PChar->m_PetCommands));
}

int32 addPetAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return addBit(AbilityID, PChar->m_PetCommands, sizeof(PChar->m_PetCommands));
}

int32 delPetAbility(CCharEntity* PChar, uint16 AbilityID)
{
    return delBit(AbilityID, PChar->m_PetCommands, sizeof(PChar->m_PetCommands));
}

/************************************************************************
 *                                                                       *
 *  Initialize the experience (exp) table                                *
 *                                                                       *
 ************************************************************************/

void LoadExpTable()
{
    TracyZoneScoped;

    auto rset = db::preparedStmt("SELECT r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20 "
                                 "FROM exp_table "
                                 "ORDER BY level ASC "
                                 "LIMIT ?",
                                 ExpTableRowCount);

    uint32 x = 0;
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        for (uint32 y = 0; y < 20; ++y)
        {
            g_ExpTable[x][y] = rset->get<uint16>(y);
        }

        ++x;
    }

    rset = db::preparedStmt("SELECT level, exp FROM exp_base LIMIT 100");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        if (const auto level = rset->get<uint8>("level") - 1; level < 100)
        {
            g_ExpPerLevel[level] = rset->get<uint16>("exp");
        }
    }

    // Retail /check EXP difficulty curve (internal/expdifficulty; pure defaults).
    // Highest EXP first — matches LoadExpDifficultyCurves descending sort.
    std::vector<std::pair<uint16, EMobDifficulty>> retailCurve = {
        { 400, EMobDifficulty::IncrediblyTough },
        { 350, EMobDifficulty::VeryTough },
        { 220, EMobDifficulty::Tough },
        { 200, EMobDifficulty::EvenMatch },
        { 160, EMobDifficulty::DecentChallenge },
        { 60, EMobDifficulty::EasyPrey },
    };
    // pair: (IEP min mob level, IEP min base EXP) — matches LoadExpDifficultyCurves args.
    std::pair<uint16, uint8> iep = { 56, 1 };
    SetExpDifficultyCurve(retailCurve, iep);
}

void SetExpDifficultyCurve(std::vector<std::pair<uint16, EMobDifficulty>>& curve, std::pair<uint16, uint8>& incrediblyEasyPreyData)
{
    ExpToDifficultyTable    = curve;
    IncrediblyEasyPreyCheck = incrediblyEasyPreyData;
}

/************************************************************************
 *                                                                       *
 *  Return mob difficulty according to level difference                  *
 *                                                                       *
 ************************************************************************/

EMobDifficulty CheckMob(uint8 charlvl, CBattleEntity* PMob)
{
    auto moblvl = PMob ? PMob->GetMLevel() + PMob->getMod(Mod::EXP_LVL_MOD) : -1;

    uint32 baseExp = GetBaseExp(charlvl, moblvl);

    if (baseExp == 0)
    {
        return EMobDifficulty::TooWeak;
    }

    // Iterate over exp  difficulty table, populated similarly to
    // { 400, EMobDifficulty::IncrediblyTough }
    // { 350, EMobDifficulty::EMobDifficulty::VeryTough }
    for (auto& entry : ExpToDifficultyTable)
    {
        auto exp = entry.first;

        if (baseExp >= exp)
        {
            auto difficulty = entry.second;
            return difficulty;
        }
    }

    auto IEPLevel = IncrediblyEasyPreyCheck.first;
    auto IEPExp   = IncrediblyEasyPreyCheck.second;

    if (baseExp >= IEPExp && moblvl >= IEPLevel)
    {
        return EMobDifficulty::IncrediblyEasyPrey;
    }

    return EMobDifficulty::TooWeak;
}

/************************************************************************
 *                                                                       *
 *  Unmodified EXP that the character will receive from the target       *
 *                                                                       *
 ************************************************************************/

uint32 GetBaseExp(uint8 charlvl, int16 moblvl)
{
    const int32 levelDif = moblvl - charlvl + 44;

    if (charlvl > 0 && charlvl < 100)
    {
        return g_ExpTable[std::clamp(levelDif, 0, ExpTableRowCount - 1)][(charlvl - 1) / 5];
    }

    return 0;
}

/************************************************************************
 *                                                                       *
 *  Calculate the amount of experience required to get the next level    *
 *                                                                       *
 ************************************************************************/

uint32 GetExpNEXTLevel(uint8 charlvl)
{
    if (charlvl > 0 && charlvl < 100)
    {
        return g_ExpPerLevel[charlvl];
    }
    return 0;
}

/************************************************************************
 *                                                                       *
 *  Distributes gil to party members.                                    *
 *                                                                       *
 ************************************************************************/

// TODO: REALISATION MUST BE IN TREASUREPOOL

void DistributeGil(CCharEntity* PChar, CMobEntity* PMob)
{
    TracyZoneScoped;

    // work out the amount of gil to give (guessed; replace with testing)
    uint32 gil    = PMob->GetRandomGil();
    uint32 gBonus = 0;

    const auto mobGilMultiplier = settings::get<float>("map.MOB_GIL_MULTIPLIER");
    if (distributegilhelpers::ShouldApplyMobGilMultiplier(gil, mobGilMultiplier))
    {
        gil = distributegilhelpers::ApplyMobGilMultiplier(gil, mobGilMultiplier);
    }

    const auto allMobsGilBonus = settings::get<uint8>("map.ALL_MOBS_GIL_BONUS");
    if (distributegilhelpers::ShouldApplyAllMobsGilBonus(allMobsGilBonus))
    {
        gBonus = distributegilhelpers::AllMobsGilBonusAmount(allMobsGilBonus, PMob->GetMLevel());
        gil    = distributegilhelpers::ApplyAllMobsGilBonus(
            gil, distributegilhelpers::ClampAllMobsGilBonus(gBonus, settings::get<uint32>("map.MAX_GIL_BONUS")));
    }

    // TODO: pin down moghancement money which seems to be a % bonus applied individually?
    // Gilfinder bonus is 1 + (128 + 0..GF level * 16)/256
    // https://docs.google.com/spreadsheets/d/134YjiVWoqn9UKOFrJFXZPHZChNa6heWzY0xXOGIteC8/edit
    if (distributegilhelpers::ShouldApplyGilfinder(PMob->m_GilfinderLevel))
    {
        const auto roll       = xirand::GetRandomNumber<uint16_t>(0, distributegilhelpers::GilfinderRollMax(PMob->m_GilfinderLevel));
        const auto multiplier = distributegilhelpers::GilfinderMultiplier(roll);
        gil                   = distributegilhelpers::ApplyGilfinder(gil, multiplier);
    }

    const int16 killshotBonus = PChar->getMod(Mod::MOGHANCEMENT_GIL_BONUS_P);
    if (distributegilhelpers::ShouldApplyKillshotGilBonus(killshotBonus))
    {
        gil = distributegilhelpers::ApplyKillshotGil(gil, distributegilhelpers::KillshotGilMultiplier(killshotBonus));
    }

    // Distribute gil to player/party/alliance
    if (PChar->PParty != nullptr)
    {
        std::vector<CCharEntity*> members;

        // First gather all valid party members
        // clang-format off
            PChar->ForAlliance([PMob, &members](CBattleEntity* PPartyMember)
            {
                if (distributegilhelpers::IsGilShareMemberEligible(
                        PPartyMember->getZone() == PMob->getZone(),
                        isWithinDistance(PPartyMember->loc.p, PMob->loc.p, distributegilhelpers::GilShareDistance))) // TODO: verify range
                {
                    members.emplace_back((CCharEntity*)PPartyMember);
                }
            });
        // clang-format on

        // all members might not be in range
        if (!members.empty())
        {
            // Calculate gil for each party member.
            uint32 gilPerPerson = distributegilhelpers::GilPerPerson(gil, members.size());

            for (auto PMember : members)
            {
                UpdateItem(PMember, LOC_INVENTORY, 0, gilPerPerson);
                PMember->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PMember, PMember, gilPerPerson, 0, MsgBasic::Obtains);
            }
        }
    }
    else if (distributegilhelpers::ShouldAwardSoloGil(false, isWithinDistance(PChar->loc.p, PMob->loc.p, distributegilhelpers::GilShareDistance)))
    {
        UpdateItem(PChar, LOC_INVENTORY, 0, static_cast<int32>(gil));
        PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, static_cast<int32>(gil), 0, MsgBasic::Obtains);
    }
}

void DistributeItem(CCharEntity* PChar, CBaseEntity* PEntity, uint16 itemid, uint16 dropRate)
{
    TracyZoneScoped;

    uint16 thDropRate = dropRate * 10;

    if (auto* PMob = dynamic_cast<CMobEntity*>(PEntity))
    {
        thDropRate = treasurehunterhelpers::GetDropRate(static_cast<int>(PMob->m_THLvl), static_cast<int>(thDropRate));
    }

    if (thDropRate > 0 && (1 + xirand::GetRandomNumber(10000)) <= thDropRate * settings::get<float>("map.DROP_RATE_MULTIPLIER"))
    {
        PChar->PTreasurePool->addItem(itemid, PEntity);
    }
}

double GetPlayerShareMultiplier(uint16 membersInZone, bool regionBuff)
{
    if (settings::get<bool>("main.DISABLE_PARTY_EXP_PENALTY"))
    {
        return 1.00;
    }

    // Alliance share
    if (membersInZone > 6)
    {
        return 1.8f / membersInZone;
    }

    // Party share
    if (regionBuff)
    {
        switch (membersInZone)
        {
            case 1:
                return 1.00;
            case 2:
                return 0.75;
            case 3:
                return 0.55;
            case 4:
                return 0.45;
            case 5:
                return 0.39;
            case 6:
                return 0.35;
            default:
                return 1.8 / membersInZone;
        }
    }
    else
    {
        switch (membersInZone)
        {
            case 1:
                return 1.00;
            case 2:
                return 0.60;
            case 3:
                return 0.45;
            case 4:
                return 0.40;
            case 5:
                return 0.37;
            case 6:
                return 0.35;
            default:
                return 1.8 / membersInZone;
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Allocate experience points                                           *
 *                                                                       *
 ************************************************************************/

void DistributeExperiencePoints(CCharEntity* PChar, CMobEntity* PMob)
{
    TracyZoneScoped;

    uint8       pcinzone = 0;
    uint8       minlevel = 0;
    uint8       maxlevel = PChar->GetMLevel();
    REGION_TYPE region   = PChar->loc.zone->GetRegionID();

    if (PChar->PParty)
    {
        if (PChar->PParty->GetSyncTarget())
        {
            if (expdistributehelpers::IsSyncTargetBlocking(
                    distance(PMob->loc.p, PChar->PParty->GetSyncTarget()->loc.p),
                    PChar->PParty->GetSyncTarget()->health.hp == 0))
            {
                // clang-format off
                    PChar->ForParty([&PMob](CBattleEntity* PMember)
                    {
                        if (PMember->getZone() == PMob->getZone() && distance(PMember->loc.p, PMob->loc.p) < 100)
                        {
                            if (CCharEntity* PChar = dynamic_cast<CCharEntity*>(PMember))
                            {
                                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::LevelSyncNoExp);
                            }
                        }
                    });
                // clang-format on

                return;
            }
        }
    }

    // clang-format off
        PChar->ForAlliance([&pcinzone, &PMob, &minlevel, &maxlevel](CBattleEntity* PMember)
        {
            if (PMember->getZone() == PMob->getZone() && distance(PMember->loc.p, PMob->loc.p) < 100)
            {
                if (PMember->PPet != nullptr && PMember->PPet->GetMLevel() > maxlevel && PMember->PPet->objtype != TYPE_PET)
                {
                    maxlevel = PMember->PPet->GetMLevel();
                }
                if (PMember->GetMLevel() > maxlevel)
                {
                    maxlevel = PMember->GetMLevel();
                }
                else if (PMember->GetMLevel() < minlevel)
                {
                    minlevel = PMember->GetMLevel();
                }
                pcinzone++;
            }
        });
    // clang-format on

    pcinzone            = expdistributehelpers::MaxTrackedPartySize(pcinzone, PMob->m_HiPartySize);
    maxlevel            = expdistributehelpers::MaxTrackedPCLevel(maxlevel, PMob->m_HiPCLvl);
    PMob->m_HiPartySize = pcinzone;
    PMob->m_HiPCLvl     = maxlevel;

    // clang-format off
        PChar->ForAlliance([&PMob, &region, &maxlevel, &pcinzone](CBattleEntity* PPartyMember)
        {
            CCharEntity* PMember = dynamic_cast<CCharEntity*>(PPartyMember);
            if (!expdistributehelpers::ShouldProcessMember(PMember != nullptr, PMember != nullptr && PMember->isDead()))
            {
                return;
            }

            bool chainactive = false;

            const int16 moblevel    = PMob->GetMLevel() + PMob->getMod(Mod::EXP_LVL_MOD);
            const uint8 memberlevel = PMember->GetMLevel();

            EMobDifficulty mobCheck = CheckMob(maxlevel, PMob);
            float          exp      = static_cast<float>(GetBaseExp(maxlevel, moblevel));

            if (mobCheck > EMobDifficulty::TooWeak)
            {
                if (PMember->getZone() == PMob->getZone())
                {
                    if (settings::get<bool>("map.EXP_PARTY_GAP_PENALTIES"))
                    {
                        uint8 partyGapNoExp = settings::get<uint8>("map.EXP_PARTY_GAP_NO_EXP");
                        exp = expdistributehelpers::ApplyPartyGapPenalty(
                            exp,
                            maxlevel,
                            memberlevel,
                            partyGapNoExp,
                            GetExpNEXTLevel(memberlevel),
                            GetExpNEXTLevel(maxlevel));
                    }

                    bool isInSignetZone =
                        PMember->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Signet) &&
                        region >= REGION_TYPE::RONFAURE &&
                        region <= REGION_TYPE::JEUNO;

                    bool isInSanctionZone =
                        PMember->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Sanction) &&
                        region >= REGION_TYPE::WEST_AHT_URHGAN &&
                        region <= REGION_TYPE::ALZADAAL;

                    exp *= GetPlayerShareMultiplier(pcinzone, isInSignetZone || isInSanctionZone);

                    if (PMob->getMobMod(MOBMOD_EXP_BONUS))
                    {
                        exp = expdistributehelpers::ApplyMonsterBonus(exp, PMob->getMobMod(MOBMOD_EXP_BONUS));
                    }

                    // Per monster caps pulled from: https://ffxiclopedia.fandom.com/wiki/Experience_Points
                    exp = expdistributehelpers::CapExpByLevel(exp, PMember->GetMLevel());

                    if (mobCheck > EMobDifficulty::DecentChallenge)
                    {
                        if (PMember->expChain.chainTime > timer::now() || PMember->expChain.chainTime == timer::time_point::min())
                        {
                            chainactive = true;
                            exp = expdistributehelpers::ApplyChainMultiplier(exp, PMember->expChain.chainNumber);
                        }
                        else
                        {
                            PMember->expChain.chainTime = timer::now() + std::chrono::seconds(
                                expdistributehelpers::ChainTimerSeconds(PMember->GetMLevel()));
                            PMember->expChain.chainNumber = 1;
                        }

                        if (chainactive)
                        {
                            PMember->expChain.chainTime = timer::now() + std::chrono::seconds(
                                expdistributehelpers::ActiveChainTimerSeconds(
                                    PMember->GetMLevel(), PMember->expChain.chainNumber));
                        }
                    }
                    // pet or companion exp penalty needs to be added here
                    if (distance(PMember->loc.p, PMob->loc.p) > 100)
                    {
                        PMember->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PMember, PMember, 0, 0, MsgBasic::TooFarForExp);
                        return;
                    }

                    exp = charutils::AddExpBonus(PMember, exp);

                    charutils::AddExperiencePoints(false, true, false, PMember, PMob, (uint32)exp, mobCheck, chainactive);
                }
            }
        });
    // clang-format on
}

/************************************************************************
 *                                                                       *
 *  Allocate capacity points                                             *
 *                                                                       *
 ************************************************************************/

void DistributeCapacityPoints(CCharEntity* PChar, CMobEntity* PMob)
{
    TracyZoneScoped;

    // TODO: Capacity Points cannot be gained in Abyssea or Reives.  In addition, Gates areas,
    //       Ra'Kaznar, Escha, and Reisenjima reduce party penalty for capacity points earned.
    ZONEID zone     = PChar->loc.zone->GetID();
    uint8  mobLevel = PMob->GetMLevel();

    PChar->ForAlliance(
        [&PMob, &zone, &mobLevel](CBattleEntity* PPartyMember)
        {
            CCharEntity* PMember = dynamic_cast<CCharEntity*>(PPartyMember);

            if (!capacitydistributehelpers::ShouldAwardMember(
                    PMember != nullptr,
                    PMember != nullptr && PMember->isDead(),
                    PMember != nullptr && PMember->loc.zone->GetID() == zone,
                    PMember != nullptr && hasKeyItem(PMember, KeyItem::JOB_BREAKER),
                    PMember != nullptr ? PMember->GetMLevel() : 0))
            {
                // Do not grant Capacity points if null, Dead, different area, no Job Breaker, or below 99
                return;
            }

            bool  chainActive = false;
            int16 levelDiff   = capacitydistributehelpers::LevelDiff(mobLevel);

            // Capacity Chains are only granted for Mobs level 100+
            // Ref: https://www.bg-wiki.com/ffxi/Job_Points
            float capacityPoints = 0;

            if (capacitydistributehelpers::ShouldComputeCapacity(mobLevel))
            {
                // Base Capacity Point formula derived from the table located at:
                // https://ffxiclopedia.fandom.com/wiki/Job_Points#Capacity_Points
                capacityPoints = capacitydistributehelpers::BaseCapacityPoints(levelDiff);

                if (capacitydistributehelpers::IsChainActive(
                        PMember->capacityChain.chainTime, timer::now(), timer::time_point::min()))
                {
                    chainActive = true;

                    // TODO: Needs verification, pulled from: https://www.bluegartr.com/threads/120445-Job-Points-discussion?p=6138288&viewfull=1#post6138288
                    // Assumption: Chain0 is no bonus, Chains 10+ capped at 1.5 value, f(chain) = 1 + 0.05 * chain
                    capacityPoints = capacitydistributehelpers::ApplyChainModifier(
                        capacityPoints, PMember->capacityChain.chainNumber);
                }
                else
                {
                    // TODO: Capacity Chain Timer is reduced after Chain 30
                    PMember->capacityChain.chainTime   = timer::now() + std::chrono::seconds(capacitydistributehelpers::ChainTimerSeconds);
                    PMember->capacityChain.chainNumber = 1;
                }

                if (chainActive)
                {
                    PMember->capacityChain.chainTime = timer::now() + std::chrono::seconds(capacitydistributehelpers::ChainTimerSeconds);
                }

                capacityPoints = AddCapacityBonus(PMember, capacityPoints);
                AddCapacityPoints(PMember, PMob, capacityPoints, levelDiff, chainActive);
            }
        });
}

/************************************************************************
 *                                                                       *
 *  Return adjusted Capacity point value based on bonuses                *
 *  Note: rawBonus uses whole number percentage values until returning   *
 *                                                                       *
 ************************************************************************/

uint16 AddCapacityBonus(CCharEntity* PChar, uint16 capacityPoints)
{
    TracyZoneScoped;

    float rawBonus = 0;

    // COMMITMENT from Capacity Bands

    if (PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Commitment) && PChar->loc.zone->GetRegionID() != REGION_TYPE::ABYSSEA)
    {
        CStatusEffect* commitment = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Commitment);
        int16          percentage = commitment->GetPower();
        int16          cap        = commitment->GetSubPower();
        rawBonus += std::clamp<int32>(((capacityPoints * percentage) / 100), 0, cap);
        commitment->SetSubPower(cap -= rawBonus);

        if (cap <= 0)
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Commitment);
        }
    }

    // Mod::CAPACITY_BONUS is currently used for JP Gifts, and can easily be used elsewhere
    // This value is stored as uint, as a whole number percentage value
    rawBonus += PChar->getMod(Mod::CAPACITY_BONUS);

    // Unity Concord Ranking: 2 * (Unity Ranking - 1)
    uint8 unity = PChar->profile.unity_leader;
    if (unity >= 1 && unity <= 11)
    {
        rawBonus += 2 * (roeutils::RoeSystem.unityLeaderRank[unity - 1] - 1);
    }

    // RoE Objectives
    for (const auto& recordValue : roeCapacityBonusRecords)
    {
        if (roeutils::GetEminenceRecordCompletion(PChar, recordValue.first))
        {
            rawBonus += recordValue.second;
        }
    }

    // RoV Key Items - Fuchsia, Puce, Ochre (30%)
    for (auto capacityBonusKeyItem : capacityBonusKeyItems)
    {
        if (hasKeyItem(PChar, capacityBonusKeyItem))
        {
            rawBonus += 30;
        }
    }

    capacityPoints *= 1.0f + rawBonus / 100;
    return capacityPoints;
}

/************************************************************************
 *                                                                       *
 *  Add Capacity Points to an individual player                          *
 *                                                                       *
 ************************************************************************/

void AddCapacityPoints(CCharEntity* PChar, CBaseEntity* PMob, uint32 capacityPoints, int16 levelDiff, bool isCapacityChain)
{
    TracyZoneScoped;

    if (capacityawardhelpers::ShouldRejectDead(PChar->isDead()))
    {
        return;
    }

    capacityPoints = capacityawardhelpers::ApplyExpRate(capacityPoints, settings::get<float>("map.EXP_RATE"));

    if (capacityawardhelpers::ShouldAwardCapacityPoints(capacityPoints))
    {
        // Capacity Chains start at lv100 mobs
        const bool sendChainMessage = capacitydistributehelpers::ShouldSendChainMessage(levelDiff, isCapacityChain);
        if (sendChainMessage)
        {
            if (capacitydistributehelpers::HasNonZeroChainNumber(PChar->capacityChain.chainNumber))
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, capacityPoints, PChar->capacityChain.chainNumber, MsgBasic::CapacityChain);
            }
            else
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, capacityPoints, 0, MsgBasic::CapacityPointsGained);
            }
            if (capacityawardhelpers::ShouldAdvanceChainNumber(sendChainMessage))
            {
                PChar->capacityChain.chainNumber = capacitydistributehelpers::NextChainNumberAfterAward(PChar->capacityChain.chainNumber);
            }
        }
        else
        {
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, capacityPoints, 0, MsgBasic::CapacityPointsGained);
        }

        // Add capacity points
        if (PChar->PJobPoints->AddCapacityPoints(capacityPoints))
        {
            PChar->loc.zone->PushPacket(PChar, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PMob, PChar->PJobPoints->GetJobPoints(), 0, MsgBasic::JobPointGained));
        }
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::JOB_POINTS>(PChar);

        if (capacityawardhelpers::ShouldFireRoeCapacity(PMob == PChar)) // Only mob kills count for gain EXP records
        {
            roeutils::event(ROE_EXPGAIN, PChar, RoeDatagram("capacity", capacityPoints));
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Losing exp on death. retainPercent is the amount of exp to be        *
 *  saved on death, e.g. 0.05 = retain 5% of lost exp. A value of        *
 *  1 means no exp loss. A value of 0 means full exp loss.               *
 *                                                                       *
 ************************************************************************/

void DelExperiencePoints(CCharEntity* PChar, float retainPercent, uint16 forcedXpLoss)
{
    TracyZoneScoped;

    if (!explosshelpers::IsRetainPercentValid(retainPercent))
    {
        ShowWarning("Invalid retainPercent value (%f) received.", retainPercent);
        return;
    }

    const auto expLossLevel = settings::get<uint8>("map.EXP_LOSS_LEVEL");
    if (!explosshelpers::IsExpLossLevelSettingValid(expLossLevel))
    {
        ShowWarning("Invalid EXP_LOSS_LEVEL setting value was obtained (%d).", expLossLevel);
        return;
    }

    if (explosshelpers::ShouldSkipByLevel(PChar->GetMLevel(), expLossLevel, forcedXpLoss))
    {
        return;
    }

    // MONs don't lose exp on death
    if (PChar->m_PMonstrosity != nullptr)
    {
        return;
    }

    const uint8 mLevel = explosshelpers::EffectiveLossLevel(PChar->GetMLevel(), PChar->m_LevelRestriction);
    uint16      exploss = explosshelpers::ResolveLossAmount(
        explosshelpers::BaseLossAmount(mLevel, GetExpNEXTLevel(mLevel)),
        forcedXpLoss,
        retainPercent,
        settings::get<float>("map.EXP_LOSS_RATE"));

    // Save exp lost.
    PChar->setCharVar("expLost", exploss);

    // Handle deleveling
    const uint16 currentExp = PChar->jobs.exp[PChar->GetMJob()];
    const uint8  jobLevel   = PChar->jobs.job[PChar->GetMJob()];
    if (explosshelpers::ShouldDelevel(currentExp, exploss, jobLevel))
    {
        // de-level!
        int32 lowerLevelMaxExp = GetExpNEXTLevel(jobLevel - 1);
        PChar->jobs.exp[PChar->GetMJob()] = static_cast<uint16>(explosshelpers::DelevelResidualExp(
            lowerLevelMaxExp, exploss, currentExp));
        PChar->jobs.job[PChar->GetMJob()] -= 1;

        if (explosshelpers::ShouldApplyDelevelToEntity(PChar->m_LevelRestriction, PChar->jobs.job[PChar->GetMJob()]))
        {
            PChar->SetMLevel(PChar->jobs.job[PChar->GetMJob()]);
            PChar->SetSLevel(PChar->jobs.job[PChar->GetSJob()]);
        }

        jobpointutils::RefreshGiftMods(PChar);
        BuildingCharSkillsTable(PChar);
        CalculateStats(PChar);
        CheckValidEquipment(PChar);

        BuildingCharAbilityTable(PChar);
        BuildingCharTraitsTable(PChar);
        BuildingCharWeaponSkills(PChar);

        PChar->pushPacket<GP_SERV_COMMAND_JOB_INFO>(PChar);
        PChar->pushPacket<CCharStatusPacket>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS2>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MERITS>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY1>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY2>(PChar);
        charutils::SendExtendedJobPackets(PChar);
        PChar->pushPacket<CCharSyncPacket>(PChar);

        PChar->UpdateHealth();

        SaveCharStats(PChar);
        SaveCharJob(PChar, PChar->GetMJob());

        if (PChar->PParty != nullptr)
        {
            if (PChar->PParty->GetSyncTarget() == PChar)
            {
                PChar->PParty->RefreshSync();
            }
            PChar->PParty->ReloadParty();
        }

        PChar->loc.zone->PushPacket(PChar, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, PChar->jobs.job[PChar->GetMJob()], 0, MsgBasic::LevelDown));
        luautils::OnPlayerLevelDown(PChar);
        PChar->updatemask |= UPDATE_HP;
    }
    else if (explosshelpers::ShouldZeroExpAtLevel1(currentExp, exploss, jobLevel))
    {
        PChar->jobs.exp[PChar->GetMJob()] = 0;
    }
    else
    {
        PChar->jobs.exp[PChar->GetMJob()] = explosshelpers::RemainingExpAfterLoss(currentExp, exploss);
    }

    SaveCharExp(PChar, PChar->GetMJob());
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);
}

/************************************************************************
 *                                                                       *
 *  Add experience points to the specified character                     *
 *                                                                       *
 ************************************************************************/

void AddExperiencePoints(bool expFromRaise, bool awardRegionPoints, bool fromScripts, CCharEntity* PChar, CBaseEntity* PMob, uint32 exp, EMobDifficulty mobCheck, bool isexpchain)
{
    TracyZoneScoped;

    if (expawardhelpers::ShouldRejectDead(PChar->isDead(), expFromRaise))
    {
        return;
    }

    // Scripts have their own settings in main.lua settings. This is for exp from combat.
    if (expawardhelpers::ShouldApplyExpRate(expFromRaise, fromScripts))
    {
        exp = (uint32)(exp * settings::get<float>("map.EXP_RATE"));
    }
    uint16 currentExp  = PChar->jobs.exp[PChar->GetMJob()];
    bool   onLimitMode = false;

    // Incase player de-levels to 74 on the field
    if (expawardhelpers::IsLimitModeFromMerit(PChar->MeritMode, PChar->jobs.job[PChar->GetMJob()], expFromRaise))
    {
        onLimitMode = true;
    }

    // we check if the player is level capped and max exp..
    if (expawardhelpers::IsLimitModeFromCap(
            PChar->jobs.job[PChar->GetMJob()],
            PChar->jobs.genkai,
            PChar->jobs.exp[PChar->GetMJob()],
            GetExpNEXTLevel(PChar->jobs.job[PChar->GetMJob()])))
    {
        onLimitMode = true;
    }

    // exp added from raise shouldn't display a message. Don't need a message for zero exp either
    if (expawardhelpers::ShouldShowExpMessage(expFromRaise, exp))
    {
        const bool useChain = expawardhelpers::ShouldUseChainMessage(
            mobCheck >= EMobDifficulty::EvenMatch, isexpchain);
        if (useChain)
        {
            const auto msgClass = expawardhelpers::SelectMessage(
                onLimitMode, true, PChar->expChain.chainNumber != 0);
            switch (msgClass)
            {
                case expawardhelpers::ExpMessage::ExpChain:
                    PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, exp, PChar->expChain.chainNumber, MsgBasic::ExpChain);
                    break;
                case expawardhelpers::ExpMessage::LimitChain:
                    PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, exp, PChar->expChain.chainNumber, MsgBasic::LimitChain);
                    break;
                case expawardhelpers::ExpMessage::LimitGained:
                    PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, exp, 0, MsgBasic::LimitPointsGained);
                    break;
                case expawardhelpers::ExpMessage::ExpGained:
                default:
                    PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, exp, 0, MsgBasic::ExperiencePointsGained);
                    break;
            }
            if (expawardhelpers::ShouldIncrementChainNumber(true))
            {
                PChar->expChain.chainNumber++;
            }
        }
        else
        {
            if (onLimitMode)
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, exp, 0, MsgBasic::LimitPointsGained);
            }
            else
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PChar, exp, 0, MsgBasic::ExperiencePointsGained);
            }
        }
    }

    if (onLimitMode)
    {
        // add limit points
        if (PChar->PMeritPoints->AddLimitPoints(exp))
        {
            PChar->loc.zone->PushPacket(PChar, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PMob, PChar->PMeritPoints->GetMeritPoints(), 0, MsgBasic::MeritPointGained));
        }
    }
    else
    {
        // add normal exp
        PChar->jobs.exp[PChar->GetMJob()] += exp;
    }

    if (!expFromRaise && !fromScripts && awardRegionPoints)
    {
        REGION_TYPE region = PChar->loc.zone->GetRegionID();

        // Should this user be awarded conquest points..
        if (PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Signet) && (region >= REGION_TYPE::RONFAURE && region <= REGION_TYPE::JEUNO))
        {
            // Add influence for the players region..
            conquest::AddConquestPoints(PChar, exp);
        }

        // Should this user be awarded imperial standing..
        if (PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Sanction) && (region >= REGION_TYPE::WEST_AHT_URHGAN && region <= REGION_TYPE::ALZADAAL))
        {
            charutils::AddPoints(PChar, "imperial_standing", (int32)(exp * 0.1f));
            PChar->pushPacket<GP_SERV_COMMAND_CONQUEST>(PChar);
        }

        // TODO: WOTG Expansion Sigil

        // Cruor Drops in Abyssea zones.
        uint16 Pzone = PChar->getZone();
        if (zoneutils::GetCurrentRegion(Pzone) == REGION_TYPE::ABYSSEA)
        {
            uint16 TextID = luautils::GetTextIDVariable(Pzone, "CRUOR_OBTAINED");
            // uint32 Total  = charutils::GetPoints(PChar, "cruor");
            // uint32 Cruor  = 0; // Need to work out how to do cruor chains, until then no cruor will drop unless this line is customized for non retail play.

            if (TextID == 0)
            {
                ShowWarning("Failed to fetch Cruor Message ID for zone: %i", Pzone);
            }

            // TODO: Implement this once formula for Cruor attainment is implemented
            // if (Cruor >= 1)
            // {
            //     PChar->pushPacket<CMessageSpecialPacket>(PChar, TextID, Cruor, Total + Cruor, 0, 0);
            //     charutils::AddPoints(PChar, "cruor", Cruor);
            // }
        }
    }

    PChar->PAI->EventHandler.triggerListener("EXPERIENCE_POINTS", PChar, PMob, exp);

    // Player levels up
    if (expawardhelpers::ShouldLevelUp(
            currentExp, exp, GetExpNEXTLevel(PChar->jobs.job[PChar->GetMJob()]), onLimitMode))
    {
        if (expawardhelpers::IsAtGenkaiCap(PChar->jobs.job[PChar->GetMJob()], PChar->jobs.genkai))
        {
            PChar->jobs.exp[PChar->GetMJob()] = static_cast<uint16>(expawardhelpers::CapExpAtNextMinusOne(
                GetExpNEXTLevel(PChar->jobs.job[PChar->GetMJob()])));
            if (PChar->PParty && PChar->PParty->GetSyncTarget() == PChar)
            {
                PChar->PParty->SetSyncTarget("", MsgStd::LevelSyncRemoveIneligibleExp);
            }
        }
        else
        {
            PChar->jobs.exp[PChar->GetMJob()] -= GetExpNEXTLevel(PChar->jobs.job[PChar->GetMJob()]);
            PChar->jobs.exp[PChar->GetMJob()] = static_cast<uint16>(expawardhelpers::PostLevelResidualExp(
                PChar->jobs.exp[PChar->GetMJob()],
                GetExpNEXTLevel(PChar->jobs.job[PChar->GetMJob()] + 1)));
            PChar->jobs.job[PChar->GetMJob()] += 1;

            if (expawardhelpers::ShouldApplyLevelToEntity(PChar->m_LevelRestriction, PChar->GetMLevel()))
            {
                PChar->SetMLevel(PChar->jobs.job[PChar->GetMJob()]);
                PChar->SetSLevel(PChar->jobs.job[PChar->GetSJob()]);

                jobpointutils::RefreshGiftMods(PChar);
                BuildingCharSkillsTable(PChar);
                CalculateStats(PChar);
                BuildingCharAbilityTable(PChar);
                BuildingCharTraitsTable(PChar);
                BuildingCharWeaponSkills(PChar);
                puppetutils::LoadAutomaton(PChar);
            }
            PChar->PLatentEffectContainer->CheckLatentsJobLevel();

            if (PChar->PParty != nullptr)
            {
                if (PChar->PParty->GetSyncTarget() == PChar)
                {
                    PChar->PParty->RefreshSync();
                }
                PChar->PParty->ReloadParty();
            }

            PChar->UpdateHealth();

            if (expawardhelpers::ShouldShowLevelUpAnimation(expFromRaise))
            {
                // Level up animation and message
                PChar->loc.zone->PushPacket(PChar, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE2>(PChar, PMob, PChar->jobs.job[PChar->GetMJob()], 0, MsgBasic::LevelUp));
                // Set HP and MP to max range
                PChar->health.hp = PChar->GetMaxHP();
                PChar->health.mp = PChar->GetMaxMP();
            }

            SaveCharStats(PChar);
            SaveCharJob(PChar, PChar->GetMJob());
            SaveCharExp(PChar, PChar->GetMJob());

            PChar->pushPacket<GP_SERV_COMMAND_JOB_INFO>(PChar);
            PChar->pushPacket<CCharStatusPacket>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS2>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MERITS>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY1>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY2>(PChar);
            charutils::SendExtendedJobPackets(PChar);
            PChar->pushPacket<CCharSyncPacket>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);

            luautils::OnPlayerLevelUp(PChar);
            roeutils::event(ROE_EVENT::ROE_LEVELUP, PChar, RoeDatagramList{});
            PChar->updatemask |= UPDATE_HP;
            return;
        }
    }

    SaveCharStats(PChar);
    SaveCharJob(PChar, PChar->GetMJob());
    SaveCharExp(PChar, PChar->GetMJob());
    PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);

    if (onLimitMode)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MERITS>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY1>(PChar);
        PChar->pushPacket<GP_SERV_COMMAND_MISCDATA::MONSTROSITY2>(PChar);
    }

    if (PMob != PChar) // Only mob kills count for gain EXP records
    {
        roeutils::event(ROE_EXPGAIN, PChar, RoeDatagram("exp", exp));
    }
}

/************************************************************************
 *                                                                       *
 *  Establish a restriction of character level                           *
 *                                                                       *
 ************************************************************************/

void SetLevelRestriction(CCharEntity* PChar, uint8 lvl)
{
}

void SaveCharPosition(CCharEntity* PChar)
{
    TracyZoneScoped;

    if (PChar->status == STATUS_TYPE::DISAPPEAR)
    {
        return;
    }

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "pos_rot = ?,"
                     "pos_x = ?,"
                     "pos_y = ?,"
                     "pos_z = ?,"
                     "boundary = ? "
                     "WHERE charid = ?",
                     PChar->loc.p.rotation,
                     PChar->loc.p.x,
                     PChar->loc.p.y,
                     PChar->loc.p.z,
                     PChar->loc.boundary,
                     PChar->id);
}

/* TODO: Move linkshell persistence here
void SaveCharLinkshells(CCharEntity* PChar)
{
    for (uint8 lsSlot = 16; lsSlot < 18; ++lsSlot)
    {
        auto eloc = PChar->equipLocation(lsSlot);
        if (!eloc)
        {
            sql->Query("DELETE FROM char_linkshells WHERE charid = %u AND lsslot = %u LIMIT 1", PChar->id, lsSlot);
        }
        else
        {
            const char* fmtQuery = "INSERT INTO char_linkshells SET charid = %u, lsslot = %u, location = %u, slot = %u ON DUPLICATE KEY UPDATE location = %u, slot = %u";
            sql->Query(fmtQuery, PChar->id, lsSlot, static_cast<uint8>(eloc->Container), eloc->Slot, static_cast<uint8>(eloc->Container), eloc->Slot);
        }
    }
}
*/

void SaveQuestsList(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "quests = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_questLog,
                     PChar->id);
}

void SaveFame(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_profile "
                     "SET "
                     "fame_sandoria = ?,"
                     "fame_bastok = ?,"
                     "fame_windurst = ?,"
                     "fame_norg = ?,"
                     "fame_jeuno = ?,"
                     "fame_aby_konschtat = ?,"
                     "fame_aby_tahrongi = ?,"
                     "fame_aby_latheine = ?,"
                     "fame_aby_misareaux = ?,"
                     "fame_aby_vunkerl = ?,"
                     "fame_aby_attohwa = ?,"
                     "fame_aby_altepa = ?,"
                     "fame_aby_grauberg = ?,"
                     "fame_aby_uleguerand = ?,"
                     "fame_adoulin = ? "
                     "WHERE charid = ?",
                     PChar->profile.fame[0],
                     PChar->profile.fame[1],
                     PChar->profile.fame[2],
                     PChar->profile.fame[3],
                     PChar->profile.fame[4],
                     PChar->profile.fame[5],
                     PChar->profile.fame[6],
                     PChar->profile.fame[7],
                     PChar->profile.fame[8],
                     PChar->profile.fame[9],
                     PChar->profile.fame[10],
                     PChar->profile.fame[11],
                     PChar->profile.fame[12],
                     PChar->profile.fame[13],
                     PChar->profile.fame[14],
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save Character Missions                                              *
 *                                                                       *
 ************************************************************************/

void SaveMissionsList(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "missions = ?, "
                     "assault = ?, "
                     "campaign = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_missionLog,
                     PChar->m_assaultLog,
                     PChar->m_campaignLog,
                     PChar->id);

    db::preparedStmt("UPDATE char_profile "
                     "SET "
                     "rank_points = ?, "
                     "rank_sandoria = ?, "
                     "rank_bastok = ?, "
                     "rank_windurst = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->profile.rankpoints,
                     PChar->profile.rank[0],
                     PChar->profile.rank[1],
                     PChar->profile.rank[2],
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save Eminence Records                                                *
 *                                                                       *
 ************************************************************************/

void SaveEminenceData(CCharEntity* PChar)
{
    TracyZoneScoped;

    if (!settings::get<bool>("main.ENABLE_ROE"))
    {
        return;
    }

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "eminence = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_eminenceLog,
                     PChar->id);

    PChar->m_eminenceCache.lastWriteout = timer::now();
}

void SaveCharInventoryCapacity(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_storage "
                     "SET "
                     "inventory = ?,"
                     "safe = ?,"
                     "locker = ?,"
                     "satchel = ?,"
                     "sack = ?, "
                     "`case` = ?, "
                     "wardrobe = ?, "
                     "wardrobe2 = ?, "
                     "wardrobe3 = ?, "
                     "wardrobe4 = ?, "
                     "wardrobe5 = ?, "
                     "wardrobe6 = ?, "
                     "wardrobe7 = ?, "
                     "wardrobe8 = ? "
                     "WHERE charid = ?",
                     PChar->getStorage(LOC_INVENTORY)->GetSize(),
                     PChar->getStorage(LOC_MOGSAFE)->GetSize(),
                     PChar->getStorage(LOC_MOGLOCKER)->GetSize(),
                     PChar->getStorage(LOC_MOGSATCHEL)->GetSize(),
                     PChar->getStorage(LOC_MOGSACK)->GetSize(),
                     PChar->getStorage(LOC_MOGCASE)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE2)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE3)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE4)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE5)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE6)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE7)->GetSize(),
                     PChar->getStorage(LOC_WARDROBE8)->GetSize(),
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save list of key items                                               *
 *                                                                       *
 ************************************************************************/

void SaveKeyItems(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars SET keyitems = ? WHERE charid = ? LIMIT 1",
                     PChar->keys,
                     PChar->id);
}

void SaveSpell(CCharEntity* PChar, uint16 spellID)
{
    TracyZoneScoped;

    db::preparedStmt("INSERT IGNORE INTO char_spells "
                     "VALUES (?, ?) LIMIT 1",
                     PChar->id,
                     spellID);
}

void DeleteSpell(CCharEntity* PChar, uint16 spellID)
{
    TracyZoneScoped;

    db::preparedStmt("DELETE FROM char_spells "
                     "WHERE charid = ? AND spellid = ? LIMIT 1",
                     PChar->id,
                     spellID);
}

void SaveLearnedAbilities(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars SET "
                     "abilities = ?, "
                     "weaponskills = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_LearnedAbilities,
                     PChar->m_LearnedWeaponskills,
                     PChar->id);
}

void SaveTitles(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET titles = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_TitleList,
                     PChar->id);

    db::preparedStmt("UPDATE char_stats "
                     "SET "
                     "title = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->profile.title,
                     PChar->id);
}

void SaveZonesVisited(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET zones = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_ZonesVisitedList,
                     PChar->id);
}

void SavePrevZoneLineID(CCharEntity* PChar, uint32 ZoneLineID)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET pos_prevzonelineid = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     ZoneLineID,
                     PChar->id);
}

void SaveCharEquip(CCharEntity* PChar)
{
    TracyZoneScoped;

    for (uint8 i = 0; i < 18; ++i)
    {
        auto eloc = PChar->equipLocation(i);
        if (!eloc)
        {
            db::preparedStmt("DELETE FROM char_equip WHERE charid = ? AND equipslotid = ? LIMIT 1", PChar->id, i);
        }
        else
        {
            db::preparedStmt("INSERT INTO char_equip "
                             "SET charid = ?, equipslotid = ?, slotid = ?, containerid = ? "
                             "ON DUPLICATE KEY UPDATE slotid  = ?, containerid = ?",
                             PChar->id,
                             i,
                             eloc->Slot,
                             static_cast<uint8>(eloc->Container),
                             eloc->Slot,
                             static_cast<uint8>(eloc->Container));
        }
    }
}

void SaveCharLook(CCharEntity* PChar)
{
    TracyZoneScoped;

    look_t* look = (PChar->getStyleLocked() ? &PChar->mainlook : &PChar->look);
    db::preparedStmt("UPDATE char_look "
                     "SET head = ?, body = ?, hands = ?, legs = ?, feet = ?, main = ?, sub = ?, ranged = ? "
                     "WHERE charid = ?",
                     look->head,
                     look->body,
                     look->hands,
                     look->legs,
                     look->feet,
                     look->main,
                     look->sub,
                     look->ranged,
                     PChar->id);

    db::preparedStmt("UPDATE chars SET isstylelocked = ? WHERE charid = ?", PChar->getStyleLocked() ? 1 : 0, PChar->id);

    db::preparedStmt("INSERT INTO char_style (charid, head, body, hands, legs, feet, main, sub, ranged) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE "
                     "charid = VALUES(charid), head = VALUES(head), body = VALUES(body), "
                     "hands = VALUES(hands), legs = VALUES(legs), feet = VALUES(feet), "
                     "main = VALUES(main), sub = VALUES(sub), ranged = VALUES(ranged)",
                     PChar->id,
                     PChar->styleItems[SLOT_HEAD],
                     PChar->styleItems[SLOT_BODY],
                     PChar->styleItems[SLOT_HANDS],
                     PChar->styleItems[SLOT_LEGS],
                     PChar->styleItems[SLOT_FEET],
                     PChar->styleItems[SLOT_MAIN],
                     PChar->styleItems[SLOT_SUB],
                     PChar->styleItems[SLOT_RANGED]);
}

/************************************************************************
 *                                                                       *
 *  Save some of the current characteristics of the character            *
 *                                                                       *
 ************************************************************************/

void SaveCharStats(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_stats "
                     "SET hp = ?, mp = ?, mhflag = ?, mjob = ?, sjob = ?, "
                     "pet_id = ?, pet_type = ?, pet_hp = ?, pet_mp = ?, pet_level = ? "
                     "WHERE charid = ?",
                     PChar->health.hp,
                     PChar->health.mp,
                     PChar->profile.mhflag,
                     PChar->GetMJob(),
                     PChar->GetSJob(true),
                     PChar->petZoningInfo.petID,
                     static_cast<uint8>(PChar->petZoningInfo.petType),
                     PChar->petZoningInfo.petHP,
                     PChar->petZoningInfo.petMP,
                     PChar->petZoningInfo.petLevel,
                     PChar->id);

    // These two are jug only variables. We should probably move pet char stats into its own table, but in the meantime
    // we use charvars for jug specific things
    if (PChar->petZoningInfo.jugSpawnTime > timer::time_point{})
    {
        const auto jugTimestamp = earth_time::timestamp(timer::to_utc(PChar->petZoningInfo.jugSpawnTime));
        PChar->setCharVar("jugpet-spawn-time", jugTimestamp);
        PChar->setCharVar("jugpet-duration-seconds", static_cast<int32>(timer::count_seconds(PChar->petZoningInfo.jugDuration)));
    }
    else
    {
        PChar->setCharVar("jugpet-spawn-time", 0);
        PChar->setCharVar("jugpet-duration-seconds", 0);
    }
}

/************************************************************************
 *                                                                       *
 *  Save the char's GM level                                             *
 *                                                                       *
 ************************************************************************/

void SaveCharGMLevel(CCharEntity* PChar)
{
    // Go host pure half: mapapp.PlanSaveCharGMLevelSQL (slice 6395).
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET gmlevel = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_GMlevel,
                     PChar->id);

    db::preparedStmt("UPDATE char_flags "
                     "SET gmModeEnabled = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->visibleGmLevel >= 3 ? 1 : 0,
                     PChar->id);
}

void SavePlayerSettings(CCharEntity* PChar)
{
    TracyZoneScoped;

    uint32_t playerSettings = {};
    std::memcpy(&playerSettings, &PChar->playerConfig, sizeof(uint32_t));

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "settings = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     playerSettings,
                     PChar->id);
}

void SaveJobMasterDisplay(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "job_master = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     PChar->m_jobMasterDisplay,
                     PChar->id);
}

void SaveChatFilterFlags(CCharEntity* PChar)
{
    TracyZoneScoped;

    uint32_t filters1 = {};
    uint32_t filters2 = {};

    std::memcpy(&filters1, &PChar->playerConfig.MessageFilter, sizeof(uint32_t));
    std::memcpy(&filters2, &PChar->playerConfig.MessageFilter2, sizeof(uint32_t));

    db::preparedStmt("UPDATE chars "
                     "SET "
                     "chatfilters_1 = ?, "
                     "chatfilters_2 = ? "
                     "WHERE charid = ? "
                     "LIMIT 1",
                     filters1,
                     filters2,
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save the char's language preference                                  *
 *                                                                       *
 ************************************************************************/

void SaveLanguages(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars SET languages = ? WHERE charid = ?", PChar->search.language, PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save character's nation changes                                      *
 *                                                                       *
 ************************************************************************/

void SaveCharNation(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET nation = ? "
                     "WHERE charid = ?",
                     PChar->profile.nation,
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save character's current campaign allegiance                         *
 *                                                                       *
 ************************************************************************/

void SaveCampaignAllegiance(const CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET campaign_allegiance = ? "
                     "WHERE charid = ?",
                     PChar->profile.campaign_allegiance,
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Saves character's current moghancement                               *
 *                                                                       *
 ************************************************************************/

void SaveCharMoghancement(const CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET moghancement = ? "
                     "WHERE charid = ?",
                     PChar->m_moghancementID,
                     PChar->id);
}

/************************************************************************
 *                                                                       *
 *  Save the current levels of the character's jobs                      *
 *                                                                       *
 ************************************************************************/

void SaveCharJob(const CCharEntity* PChar, const JOBTYPE job)
{
    TracyZoneScoped;

    if (job == JOB_NON || job >= MAX_JOBTYPE)
    {
        ShowWarningFmt("Attempt to save Invalid Job with JOBTYPE {}.", job);
        return;
    }

    // Monstrosity job and level data is handled elsewhere, bail out now
    if (job == JOB_MON)
    {
        return;
    }

    std::string fmtQuery = "";

    switch (job)
    {
        case JOB_WAR:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, war = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_MNK:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, mnk = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_WHM:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, whm = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_BLM:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, blm = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_RDM:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, rdm = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_THF:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, thf = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_PLD:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, pld = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_DRK:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, drk = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_BST:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, bst = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_BRD:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, brd = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_RNG:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, rng = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_SAM:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, sam = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_NIN:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, nin = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_DRG:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, drg = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_SMN:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, smn = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_BLU:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, blu = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_COR:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, cor = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_PUP:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, pup = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_DNC:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, dnc = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_SCH:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, sch = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_GEO:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, geo = ? WHERE charid = ? LIMIT 1";
            break;
        case JOB_RUN:
            fmtQuery = "UPDATE char_jobs SET unlocked = ?, run = ? WHERE charid = ? LIMIT 1";
            break;
        default:
            fmtQuery = "";
            break;
    }

    db::preparedStmt(fmtQuery, PChar->jobs.unlocked, PChar->jobs.job[job], PChar->id);
}

void SaveCharExp(const CCharEntity* PChar, const JOBTYPE job)
{
    TracyZoneScoped;

    if (job == JOB_NON || job >= MAX_JOBTYPE)
    {
        ShowWarningFmt("Attempt to save Char XP with invalid JOBTYPE {}.", job);
        return;
    }

    // Monstrosity exp data is handled elsewhere, bail out now
    if (job == JOB_MON)
    {
        return;
    }

    std::string query = "";

    switch (job)
    {
        case JOB_WAR:
            query = "UPDATE char_exp SET war = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_MNK:
            query = "UPDATE char_exp SET mnk = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_WHM:
            query = "UPDATE char_exp SET whm = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_BLM:
            query = "UPDATE char_exp SET blm = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_RDM:
            query = "UPDATE char_exp SET rdm = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_THF:
            query = "UPDATE char_exp SET thf = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_PLD:
            query = "UPDATE char_exp SET pld = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_DRK:
            query = "UPDATE char_exp SET drk = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_BST:
            query = "UPDATE char_exp SET bst = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_BRD:
            query = "UPDATE char_exp SET brd = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_RNG:
            query = "UPDATE char_exp SET rng = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_SAM:
            query = "UPDATE char_exp SET sam = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_NIN:
            query = "UPDATE char_exp SET nin = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_DRG:
            query = "UPDATE char_exp SET drg = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_SMN:
            query = "UPDATE char_exp SET smn = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_BLU:
            query = "UPDATE char_exp SET blu = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_COR:
            query = "UPDATE char_exp SET cor = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_PUP:
            query = "UPDATE char_exp SET pup = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_DNC:
            query = "UPDATE char_exp SET dnc = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_SCH:
            query = "UPDATE char_exp SET sch = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_GEO:
            query = "UPDATE char_exp SET geo = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        case JOB_RUN:
            query = "UPDATE char_exp SET run = ?, merits = ?, limits = ? WHERE charid = ?";
            break;
        default:
            query = "";
            break;
    }

    db::preparedStmt(query, PChar->jobs.exp[job], PChar->PMeritPoints->GetMeritPoints(), PChar->PMeritPoints->GetLimitPoints(), PChar->id);
}

void SaveCharSkills(const CCharEntity* PChar, const uint8 skillID)
{
    TracyZoneScoped;

    if (skillID >= MAX_SKILLTYPE)
    {
        ShowWarningFmt("charutils::SaveCharSkills() - skillID >= MAX_SKILLTYPE.");
        return;
    }

    db::preparedStmt("INSERT INTO char_skills "
                     "SET charid = ?, skillid = ?, value = ?, rank = ? "
                     "ON DUPLICATE KEY UPDATE value = ?, rank = ?",
                     PChar->id,
                     skillID,
                     PChar->RealSkills.skill[skillID],
                     PChar->RealSkills.rank[skillID],
                     PChar->RealSkills.skill[skillID],
                     PChar->RealSkills.rank[skillID]);
}

/************************************************************************
 *                                                                       *
 *  Save Teleports - (homepoints, outposts, maws, etc)                   *
 *                                                                       *
 ************************************************************************/

void SaveTeleport(CCharEntity* PChar, TELEPORT_TYPE type)
{
    TracyZoneScoped;

    switch (type)
    {
        case TELEPORT_TYPE::OUTPOST_SANDY:
        {
            db::preparedStmt("UPDATE char_unlocks SET outpost_sandy = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.outpostSandy,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::OUTPOST_BASTOK:
        {
            db::preparedStmt("UPDATE char_unlocks SET outpost_bastok = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.outpostBastok,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::OUTPOST_WINDY:
        {
            db::preparedStmt("UPDATE char_unlocks SET outpost_windy = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.outpostWindy,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::RUNIC_PORTAL:
        {
            db::preparedStmt("UPDATE char_unlocks SET runic_portal = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.runicPortal,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::PAST_MAW:
        {
            db::preparedStmt("UPDATE char_unlocks SET maw = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.pastMaw,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::CAMPAIGN_SANDY:
        {
            db::preparedStmt("UPDATE char_unlocks SET campaign_sandy = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.campaignSandy,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::CAMPAIGN_BASTOK:
        {
            db::preparedStmt("UPDATE char_unlocks SET campaign_bastok = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.campaignBastok,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::CAMPAIGN_WINDY:
        {
            db::preparedStmt("UPDATE char_unlocks SET campaign_windy = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.campaignWindy,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::HOMEPOINT:
        {
            db::preparedStmt("UPDATE char_unlocks SET homepoints = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.homepoint,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::SURVIVAL:
        {
            db::preparedStmt("UPDATE char_unlocks SET survivals = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.survival,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::ABYSSEA_CONFLUX:
        {
            db::preparedStmt("UPDATE char_unlocks SET abyssea_conflux = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.abysseaConflux,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::WAYPOINT:
        {
            db::preparedStmt("UPDATE char_unlocks SET waypoints = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.waypoints,
                             PChar->id);
        }
        break;
        case TELEPORT_TYPE::ESCHAN_PORTAL:
        {
            db::preparedStmt("UPDATE char_unlocks SET eschan_portals = ? WHERE charid = ? LIMIT 1",
                             PChar->teleport.eschanPortal,
                             PChar->id);
        }
        break;
        default:
        {
            ShowError("charutils:SaveTeleport: Unknown type parameter.");
        }
        break;
    }
}

void SaveMazeUnlocks(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_unlocks SET maze_vouchers = ?, maze_runes = ? WHERE charid = ? LIMIT 1",
                     PChar->maze().vouchers,
                     PChar->maze().runes,
                     PChar->id);

    PChar->pushPacket<GP_SERV_COMMAND_DUNGEON>(PChar);
}

void SaveLastLogout(const CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE chars "
                     "SET last_logout = CURRENT_TIMESTAMP "
                     "WHERE charid = ?",
                     PChar->id);
}

float AddExpBonus(CCharEntity* PChar, float exp)
{
    TracyZoneScoped;

    int32 bonus = 0;
    if (PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Dedication) && PChar->loc.zone->GetRegionID() != REGION_TYPE::ABYSSEA)
    {
        CStatusEffect* dedication = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Dedication);
        int16          percentage = dedication->GetPower();
        int16          cap        = dedication->GetSubPower();
        bonus += std::clamp<int32>((int32)((exp * percentage) / 100), 0, cap);
        dedication->SetSubPower(cap -= bonus);

        if (cap <= 0)
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Dedication);
        }
    }

    int16 rovBonus = 0;
    for (const auto experienceBonusKeyItem : experienceBonusKeyItems)
    {
        if (hasKeyItem(PChar, experienceBonusKeyItem))
        {
            rovBonus += 30;
        }
    }

    bonus += (int32)(exp * ((PChar->getMod(Mod::EXP_BONUS) + rovBonus) / 100.0f));

    if (bonus + (int32)exp < 0)
    {
        exp = 0;
    }
    else
    {
        exp = exp + bonus;
    }

    return exp;
}

auto hasMogLockerAccess(const CCharEntity* PChar) -> bool
{
    TracyZoneScoped;

    const auto tstamp     = static_cast<uint32>(PChar->getCharVar("mog-locker-expiry-timestamp"));
    const auto accessType = static_cast<uint32>(PChar->getCharVar("mog-locker-access-type"));
    if (earth_time::vanadiel_timestamp() < tstamp)
    {
        const auto curZone = PChar->loc.zone;
        switch (accessType)
        {
            case 1: // All areas
                // Allowed if in a zone with a Nomad Moogle or in your own Mog House
                return curZone->CanUseMisc(MISC_MOGMENU) || PChar->m_moghouseID == PChar->id;
            case 0: // Al Zahbi only
            default:
                const auto zoneId = curZone->GetID();

                // Either in your own MH in Al Zahbi or Whitegate
                if (PChar->m_moghouseID == PChar->id &&
                    (zoneId == ZONE_AL_ZAHBI || zoneId == ZONE_AHT_URHGAN_WHITEGATE))
                {
                    return true;
                }

                // Or in Nashmau where a Nomad Moogle is present.
                if (zoneId == ZONE_NASHMAU)
                {
                    return true;
                }
        };
    }

    return false;
}

uint8 getQuestStatus(CCharEntity* PChar, uint8 log, uint8 quest)
{
    uint8 current  = PChar->m_questLog[log].current[quest / 8] & (1 << (quest % 8));
    uint8 complete = PChar->m_questLog[log].complete[quest / 8] & (1 << (quest % 8));

    return (complete != 0 ? 2 : (current != 0 ? 1 : 0));
}

/************************************************************************
 *                                                                       *
 *  Record now as when the character has died and save it to the db.     *
 *                                                                       *
 ************************************************************************/

void SaveDeathTime(CCharEntity* PChar)
{
    TracyZoneScoped;

    uint32 secondsSinceDeath = static_cast<uint32>(timer::count_seconds(PChar->GetTimeSinceDeath()));
    db::preparedStmt("UPDATE char_stats SET death = ? WHERE charid = ? LIMIT 1", secondsSinceDeath, PChar->id);
}

void SavePlayTime(CCharEntity* PChar)
{
    TracyZoneScoped;

    const timer::duration playDuration = PChar->GetPlayTime();
    const uint32          playtime     = static_cast<uint32>(timer::count_seconds(playDuration));

    db::preparedStmt("UPDATE chars SET playtime = ? WHERE charid = ? LIMIT 1", playtime, PChar->id);

    // Removes new player icon if played for more than 240 hours
    if (PChar->isNewPlayer() && playDuration >= 240h)
    {
        PChar->playerConfig.NewAdventurerOffFlg = true;
        PChar->updatemask |= UPDATE_HP;

        SavePlayerSettings(PChar);
    }
}

/************************************************************************
 *                                                                       *
 *  Checks which UnarmedItem to grant when SLOT_MAIN is empty.           *
 *                                                                       *
 ************************************************************************/

void CheckUnarmedWeapon(CCharEntity* PChar)
{
    TracyZoneScoped;

    CItem* PSubslot = PChar->getEquip(SLOT_SUB);

    // Main or sub job provides H2H skill, and sub slot is empty.
    const bool mainH2H = battleutils::GetSkillRank(SKILL_HAND_TO_HAND, PChar->GetMJob()) > 0;
    const bool subH2H  = battleutils::GetSkillRank(SKILL_HAND_TO_HAND, PChar->GetSJob()) > 0;
    const bool subOK   = weaponskillrosterhelpers::IsSubEmptyOrNonEquipment(
        PSubslot != nullptr, PSubslot != nullptr && PSubslot->isType(ITEM_EQUIPMENT));
    if (weaponskillrosterhelpers::ShouldUseUnarmedH2H(mainH2H, subH2H, subOK))
    {
        PChar->m_Weapons[SLOT_MAIN] = xi::items::unarmedH2H();
        PChar->look.main            = weaponskillrosterhelpers::UnarmedLookMain(true); // The secret to H2H animations.  setModelId for UnarmedH2H didn't work.
    }
    else
    {
        PChar->m_Weapons[SLOT_MAIN] = xi::items::unarmed();
        PChar->look.main            = weaponskillrosterhelpers::UnarmedLookMain(false);
    }
    BuildingCharWeaponSkills(PChar);
}

auto CheckAbilityAddtype(CCharEntity* PChar, const CAbility* PAbility) -> bool
{
    const auto addType = PAbility->getAddType();

    bool meritExists         = false;
    bool meritCountPositive  = false;
    if (addType & ADDTYPE_MERIT)
    {
        auto* merit = PChar->PMeritPoints->GetMerit(static_cast<MERIT_TYPE>(PAbility->getMeritModID()));
        if (!merit)
        {
            ShowWarning("charutils::CheckAbilityAddtype: Attempt to add invalid Merit Ability (%d).", PAbility->getMeritModID());
            return false;
        }
        meritExists        = true;
        meritCountPositive = merit->count > 0;
    }

    const bool hasAstralFlow = PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AstralFlow);
    const bool hasLearned    = hasLearnedAbility(PChar, PAbility->getID());
    const bool hasLightArts  = PChar->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::LightArts, xi::StatusEffect::AddendumWhite });
    const bool hasDarkArts   = PChar->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::DarkArts, xi::StatusEffect::AddendumBlack });

    const bool hasPet = PChar->PPet != nullptr;
    bool petIsMob     = false;
    bool petIsJugPet  = false;
    bool petIsAvatar  = false;
    bool petIsAutomaton = false;
    bool petIsMobOrJugPet = false;
    uint32 petID = 0;
    if (hasPet)
    {
        petIsMob = PChar->PPet->objtype == TYPE_MOB;
        if (PChar->PPet->objtype == TYPE_PET)
        {
            auto* petEntity = static_cast<CPetEntity*>(PChar->PPet);
            petIsJugPet     = petEntity->getPetType() == PET_TYPE::JUG_PET;
            petIsAvatar     = petEntity->getPetType() == PET_TYPE::AVATAR;
            petIsAutomaton  = petEntity->getPetType() == PET_TYPE::AUTOMATON;
            petID           = petEntity->petID();
        }
        petIsMobOrJugPet = petIsMob || (PChar->PPet->objtype == TYPE_PET && petIsJugPet);
    }

    return abilitytablehelpers::CheckAbilityAddtype(
        addType,
        meritExists,
        meritCountPositive,
        hasAstralFlow,
        hasLearned,
        hasLightArts,
        hasDarkArts,
        hasPet,
        petIsMobOrJugPet,
        petIsJugPet,
        petIsMob,
        petIsAvatar,
        petID,
        petIsAutomaton);
}


void RemoveInvisible(const CCharEntity* PChar)
{
    if (PChar && PChar->StatusEffectContainer)
    {
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Invisible);
    }
}

void RemoveStratagems(CCharEntity* PChar, CSpell* PSpell)
{
    if (PSpell->getSpellGroup() == SPELLGROUP_WHITE)
    {
        // rapture to be deleted in applicable scripts
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Penury);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Celerity);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Enlightenment);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Altruism);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Tranquility);
        if (PSpell->getAOE() == SPELLAOE_RADIAL_ACCE)
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Accession);
        }
        if (PSpell->getSkillType() == SKILL_ENHANCING_MAGIC)
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Perpetuance);
        }
    }
    else if (PSpell->getSpellGroup() == SPELLGROUP_BLACK)
    {
        // ebullience to be deleted in applicable scripts
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Parsimony);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Alacrity);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Enlightenment);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Focalization);
        PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Equanimity);
        if (PSpell->getAOE() == SPELLAOE_RADIAL_MANI)
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Manifestation);
        }
    }
}

void RemoveAllEquipMods(CCharEntity* PChar)
{
    for (uint8 slotID = 0; slotID < 16; ++slotID)
    {
        CItemEquipment* PItem = PChar->getEquip((SLOTTYPE)slotID);
        if (PItem)
        {
            PChar->delEquipModifiers(&PItem->modList, PItem->getReqLvl(), slotID);
            if (PItem->getReqLvl() <= PChar->GetMLevel())
            {
                PChar->PLatentEffectContainer->DelLatentEffects(PItem->getReqLvl(), slotID);
                PChar->PLatentEffectContainer->CheckLatentsEquip(slotID);
            }
        }
    }
}

void ApplyAllEquipMods(CCharEntity* PChar)
{
    for (uint8 slotID = 0; slotID < 16; ++slotID)
    {
        CItemEquipment* PItem = PChar->getEquip((SLOTTYPE)slotID);
        if (PItem)
        {
            PChar->addEquipModifiers(&PItem->modList, PItem->getReqLvl(), slotID);
            if (PItem->getReqLvl() <= PChar->GetMLevel())
            {
                PChar->PLatentEffectContainer->AddLatentEffects(PItem->latentList, PItem->getReqLvl(), slotID);
                PChar->PLatentEffectContainer->CheckLatentsEquip(slotID);
            }
        }
    }
}

void ClearTempItems(CCharEntity* PChar)
{
    TracyZoneScoped;

    CItemContainer* Temp = PChar->getStorage(LOC_TEMPITEMS);

    db::preparedStmt("DELETE FROM char_inventory WHERE charid = ? AND location = 3", PChar->id);
    Temp->Clear();
}

void ReloadParty(CCharEntity* PChar)
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT partyid, allianceid, partyflag & ? AS partyflag "
                                       "FROM accounts_sessions s JOIN accounts_parties p ON "
                                       "s.charid = p.charid "
                                       "WHERE p.charid = ? LIMIT 1",
                                       (PARTY_SECOND | PARTY_THIRD),
                                       PChar->id);
    FOR_DB_SINGLE_RESULT(rset)
    {
        auto       partyid     = rset->get<uint32>("partyid");
        auto       allianceid  = rset->get<uint32>("allianceid");
        const auto partynumber = rset->get<uint32>("partyflag");

        // first, parties and alliances must be created or linked if the character's current party has changed
        // for example, joining a party from another server
        if (PChar->PParty)
        {
            if (PChar->PParty->GetPartyID() != partyid)
            {
                PChar->PParty->SetPartyID(partyid);
            }
        }
        else
        {
            // find if party exists on this server already
            CParty* PParty = nullptr;
            // clang-format off
                zoneutils::ForEachZone([partyid, &PParty](CZone* PZone)
                {
                    PZone->ForEachChar([partyid, &PParty](const CCharEntity* PChar)
                    {
                        if (PChar->PParty && PChar->PParty->GetPartyID() == partyid)
                        {
                            PParty = PChar->PParty;
                        }
                    });
                });
            // clang-format on

            // create new party if it doesn't exist already
            if (!PParty)
            {
                PParty = new CParty(partyid);
            }

            PParty->PushMember(PChar);
        }

        CBattleEntity* PSyncTarget = PChar->PParty->GetSyncTarget();
        if (PSyncTarget && PChar->getZone() == PSyncTarget->getZone() && !(PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::LevelSync)) &&
            PSyncTarget->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::LevelSync) &&
            PSyncTarget->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync)->GetDuration() == 0s)
        {
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, PSyncTarget->GetMLevel(), MsgBasic::LevelSyncActivated);
            PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Dispelable);
            PChar->StatusEffectContainer->AddStatusEffectSilent(xi::StatusEffect::LevelSync, static_cast<uint16>(xi::StatusEffect::LevelSync), PSyncTarget->GetMLevel(), 0s, 0s);
        }

        if (allianceid != 0)
        {
            if (PChar->PParty->m_PAlliance)
            {
                if (PChar->PParty->m_PAlliance->m_AllianceID != allianceid)
                {
                    PChar->PParty->m_PAlliance->m_AllianceID = allianceid;
                }
            }
            else
            {
                // find if the alliance exists on this server already
                // clang-format off
                    CAlliance* PAlliance = nullptr;
                    zoneutils::ForEachZone([allianceid, &PAlliance](CZone* PZone)
                    {
                        PZone->ForEachChar([allianceid, &PAlliance](CCharEntity* PChar)
                        {
                            if (PChar->PParty && PChar->PParty->m_PAlliance && PChar->PParty->m_PAlliance->m_AllianceID == allianceid)
                            {
                                PAlliance = PChar->PParty->m_PAlliance;
                            }
                        });
                    });
                // clang-format on

                // create new alliance if it doesn't exist on this server already
                if (!PAlliance)
                {
                    PAlliance = new CAlliance(allianceid);
                }

                PAlliance->pushParty(PChar->PParty, partynumber);
            }
        }
        else if (PChar->PParty->m_PAlliance)
        {
            PChar->PParty->m_PAlliance->delParty(PChar->PParty);
        }

        // once parties and alliances have been reassembled, reload the party/parties
        PChar->PParty->ReloadParty();
    }
    else
    {
        if (PChar->PParty)
        {
            PChar->PParty->DelMember(PChar);
        }
        PChar->ReloadPartyDec();
    }

    // Attempt to disband party if the last trust was just released
    // NOTE: Trusts are not counted as party members, so the current member count will be 1
    if (PChar->PParty && PChar->PParty->HasOnlyOneMember() && PChar->PTrusts.empty())
    {
        // Looks good so far, check OTHER processes to see if we should disband
        if (PChar->PParty->GetMemberCountAcrossAllProcesses() == 1)
        {
            PChar->PParty->DisbandParty();
            destroy(PChar->PParty);
        }
    }
}

bool IsAidBlocked(CCharEntity* PInitiator, CCharEntity* PTarget)
{
    if (PTarget->getBlockingAid())
    {
        // clang-format off
            bool inAlliance = false;
            PTarget->ForAlliance([&PInitiator, &inAlliance](CBattleEntity* PEntity)
            {
                if (PEntity->id == PInitiator->id)
                {
                    inAlliance = true;
                }
            });
        // clang-format on

        if (!inAlliance)
        {
            return true;
        }
    }
    return false;
}

void AddPoints(CCharEntity* PChar, const char* type, int32 amount, int32 max)
{
    TracyZoneScoped;

    const auto currentPointsValue = GetPoints(PChar, type);
    const auto newPointsValue     = std::clamp(currentPointsValue + amount, 0, max);
    SetPoints(PChar, type, newPointsValue);

    if (strcmp(type, "unity_accolades") == 0 && amount > 0)
    {
        float evalPoints = static_cast<float>(amount) / 1000;

        AddPoints(PChar, "current_accolades", amount, std::numeric_limits<int32>::max()); // Do not cap current_accolades

        db::preparedStmt("UPDATE unity_system SET points_current = points_current + ? WHERE leader = ?", evalPoints, PChar->profile.unity_leader);

        roeutils::UpdateUnityTrust(PChar, true);

        PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);
    }
}

void SetPoints(CCharEntity* PChar, const char* type, int32 amount)
{
    TracyZoneScoped;

    // TODO: Extract this into some sort of database metadata system
    //     : that's populated on startup.
    static std::unordered_set<std::string> charPointsColumnNames;
    if (charPointsColumnNames.empty())
    {
        const auto names = db::getTableColumnNames("char_points");
        for (const auto& name : names)
        {
            charPointsColumnNames.insert(name);
        }
    }

    if (charPointsColumnNames.find(type) == charPointsColumnNames.end())
    {
        ShowErrorFmt("charutils::SetPoints: Invalid type {} for {}", type, PChar->getName());
        return;
    }

    // NOTE: We normally don't want to build a prepared statement with fmt::format,
    //     : but this query is entirely internal and we've just validated the incoming
    //     : column name, so it's OK.
    const auto query = fmt::format("UPDATE char_points SET {} = ? WHERE charid = ?", type);
    db::preparedStmt(query, amount, PChar->id);

    if (strcmp(type, "spark_of_eminence") == 0)
    {
        PChar->pushPacket<GP_SERV_COMMAND_UNITY>(PChar);
    }
}

int32 GetPoints(CCharEntity* PChar, const char* type)
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT * FROM char_points WHERE charid = ? LIMIT 1", PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        return rset->get<int32>(type);
    }

    return 0;
}

void SetUnityLeader(CCharEntity* PChar, uint8 leaderID)
{
    TracyZoneScoped;

    if (leaderID < 1 || leaderID > 11)
    {
        return;
    }

    PChar->profile.unity_leader = leaderID;
    if (PChar->PUnityChat)
    {
        unitychat::DelOnlineMember(PChar, PChar->PUnityChat->getLeader());
    }
    unitychat::AddOnlineMember(PChar, PChar->profile.unity_leader);

    db::preparedStmt("UPDATE char_profile SET unity_leader = ? WHERE charid = ?", PChar->profile.unity_leader, PChar->id);
}

std::string GetConquestPointsName(CCharEntity* PChar)
{
    switch (PChar->profile.nation)
    {
        case 0:
            return "sandoria_cp";
        case 1:
            return "bastok_cp";
        case 2:
            return "windurst_cp";
        default:
            ShowError("Invalid nation received, returning nothing.");
            return "";
    }
}

auto SendToZone(CCharEntity* PChar, uint16 zoneId) -> bool
{
    TracyZoneScoped;

    if (PChar->PSession->blowfish.status == BLOWFISH_PENDING_ZONE)
    {
        return false;
    }

    auto ipp = IPP(zoneutils::GetZoneIPP(zoneId));
    if (ipp.getIP() == 0)
    {
        ShowErrorFmt("charutils::SendToZone : Invalid zoneId {}", zoneId);
        return false;
    }

    // Go host pure half: zoneutils.ShouldDenySendToZoneAtCap / FormatSendToZoneCapDenied (6442).
    if (zoneutils::IsZoneAtPlayerCap(zoneId, PChar->m_GMlevel > 0))
    {
        ShowInfoFmt("charutils::SendToZone : zone {} at player cap, denying {} (gm={})", zoneId, PChar->name, PChar->m_GMlevel);
        return false;
    }

    db::preparedStmt("UPDATE chars "
                     "SET pos_zone = ?, pos_prevzone = ?, pos_rot = ?,"
                     "pos_x = ?, pos_y = ?, pos_z = ?,"
                     "moghouse = ?, boundary = ? "
                     "WHERE charid = ?",
                     PChar->loc.destination,
                     (PChar->inMogHouse() || PChar->loc.destination == PChar->getZone()) ? PChar->loc.prevzone : PChar->getZone(),
                     PChar->loc.p.rotation,
                     PChar->loc.p.x,
                     PChar->loc.p.y,
                     PChar->loc.p.z,
                     PChar->m_moghouseID,
                     PChar->loc.boundary,
                     PChar->id);

    if (PChar->shouldPetPersistThroughZoning())
    {
        PChar->setPetZoningInfo();
    }
    else
    {
        PChar->resetPetZoningInfo();
    }

    // If player somehow gets zoned, force crit fail their synth
    if (PChar->activeTransaction<SynthTransaction>())
    {
        charutils::forceSynthCritFail("SendToZone", PChar);
    }

    PChar->requestedZoneChange = true;
    PChar->requestedWarp       = false; // a previous warp can get us here, which could infinitely loop. So un-request warp.

    PChar->PSession->zone_ipp = {};
    PChar->pushPacket<GP_SERV_COMMAND_LOGOUT>(GP_GAME_LOGOUT_STATE::ZONECHANGE, IPP(ipp));

    PChar->status = STATUS_TYPE::DISAPPEAR;

    // Save pet if any
    if (PChar->shouldPetPersistThroughZoning())
    {
        PChar->setPetZoningInfo();
    }

    return true;
}

void SendDisconnect(CCharEntity* PChar)
{
    TracyZoneScoped;

    SaveCharPosition(PChar);
    PChar->clearPacketList();

    PChar->loc.destination     = 0xFFFF;
    PChar->status              = STATUS_TYPE::SHUTDOWN;
    PChar->requestedZoneChange = true;

    // Save pet if any
    if (PChar->shouldPetPersistThroughZoning())
    {
        PChar->setPetZoningInfo();
    }

    PChar->pushPacket<GP_SERV_COMMAND_LOGOUT>(GP_GAME_LOGOUT_STATE::LOGOUT, IPP());
}

// This is just an alias for SendDisconnect?
void ForceLogout(CCharEntity* PChar)
{
    charutils::SendDisconnect(PChar);
}

void ForceRezone(CCharEntity* PChar)
{
    PChar->loc.destination = PChar->getZone();
    PChar->status          = STATUS_TYPE::DISAPPEAR;
    PChar->loc.boundary    = 0;

    PChar->clearPacketList();

    PChar->requestedZoneChange = true;

    // Save pet if any
    if (PChar->shouldPetPersistThroughZoning())
    {
        PChar->setPetZoningInfo();
    }
}

auto HomePoint(CCharEntity* PChar, bool resetHPMP) -> bool
{
    TracyZoneScoped;

    if (entityspawnhelpers::ShouldRejectHomePointAtCap(
            zoneutils::IsZoneAtPlayerCap(PChar->profile.home_point.destination, PChar->m_GMlevel > 0)))
    {
        PChar->pushPacket<GP_SERV_COMMAND_SYSTEMMES>(0, 0, MsgStd::CouldNotEnter);
        PChar->requestedWarp = false;
        return false;
    }

    // player initiated warp/warp 2 or otherwise
    if (entityspawnhelpers::ShouldResetHPMPOnHomePoint(resetHPMP))
    {
        // remove weakness on homepoint
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Weakness);
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelSync);

        PChar->SetDeathTime(timer::time_point::min());

        PChar->health.hp = PChar->GetMaxHP();
        PChar->health.mp = PChar->GetMaxMP();
    }

    PChar->loc.boundary    = 0;
    PChar->loc.p           = PChar->profile.home_point.p;
    PChar->loc.destination = PChar->profile.home_point.destination;

    PChar->status    = STATUS_TYPE::DISAPPEAR;
    PChar->animation = ANIMATION_NONE;
    PChar->updatemask |= UPDATE_HP;

    PChar->clearPacketList();
    return SendToZone(PChar, PChar->loc.destination);
}

bool AddWeaponSkillPoints(CCharEntity* PChar, SLOTTYPE slotid, int wspoints)
{
    TracyZoneScoped;

    CItemWeapon* PWeapon = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[slotid]);

    if (miscprogresshelpers::ShouldAddWeaponSkillPoints(
            PWeapon != nullptr,
            PWeapon != nullptr && PWeapon->isUnlockable(),
            PWeapon != nullptr && PWeapon->isUnlocked()))
    {
        if (miscprogresshelpers::ShouldRebuildAfterWSPoints(PWeapon->addWsPoints(wspoints)))
        {
            // weapon is now broken
            charutils::BuildingCharWeaponSkills(PChar);
            PChar->PLatentEffectContainer->CheckLatentsWeaponBreak(slotid);
            PChar->pushPacket<GP_SERV_COMMAND_CLISTATUS>(PChar);
            PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
        }

        return true;
    }
    return false;
}

int32 GetCharVar(CCharEntity* PChar, const std::string& var)
{
    if (PChar == nullptr)
    {
        return 0;
    }

    return PChar->getCharVar(var);
}

void SetCharVar(uint32 charId, const std::string& var, int32 value, uint32 expiry /* = 0 */)
{
    if (auto player = zoneutils::GetChar(charId))
    {
        player->setCharVar(var, value, expiry);
        return;
    }

    PersistCharVar(charId, var, value, expiry);

    message::send(ipc::CharVarUpdate{
        .charId  = charId,
        .value   = value,
        .expiry  = expiry,
        .varName = var,
    });
}

void SetCharVar(CCharEntity* PChar, const std::string& var, int32 value, uint32 expiry /* = 0 */)
{
    if (PChar == nullptr)
    {
        return;
    }

    return PChar->setCharVar(var, value, expiry);
}

int32 ClearCharVarsWithPrefix(CCharEntity* PChar, const std::string& prefix)
{
    if (PChar == nullptr)
    {
        return 0;
    }

    PChar->clearCharVarsWithPrefix(prefix);
    return 0;
}

void ClearCharVarFromAll(const std::string& varName, bool localOnly)
{
    if (!localOnly)
    {
        db::preparedStmt("DELETE FROM char_vars WHERE varname = ?", varName);
    }

    // clang-format off
        zoneutils::ForEachZone([varName](CZone* PZone)
        {
            PZone->ForEachChar([varName](CCharEntity* PChar)
            {
                PChar->updateCharVarCache(varName, 0);
            });
        });
    // clang-format on
}

void IncrementCharVar(uint32 charId, const std::string& var, int32 value)
{
    db::preparedStmt("INSERT INTO char_vars SET charid = ?, varname = ?, value = ? ON DUPLICATE KEY UPDATE value = value + ?", charId, var, value, value);
}

void IncrementCharVar(CCharEntity* PChar, const std::string& var, int32 value)
{
    if (PChar == nullptr)
    {
        return;
    }

    IncrementCharVar(PChar->id, var, value);

    PChar->removeFromCharVarCache(var);
}

auto FetchCharVar(uint32 charId, const std::string& varName) -> std::pair<int32, uint32>
{
    const auto rset = db::preparedStmt("SELECT value, expiry FROM char_vars WHERE charid = ? AND varname = ? LIMIT 1", charId, varName);

    int32  value  = 0;
    uint32 expiry = 0;

    if (rset && rset->rowsCount() && rset->next())
    {
        value  = rset->get<int32>(0);
        expiry = rset->get<uint32>(1);

        if (expiry > 0 && expiry <= earth_time::timestamp())
        {
            value = 0;
            db::preparedStmt("DELETE FROM char_vars WHERE charid = ? AND varname = ?", charId, varName);
        }
    }

    return { value, expiry };
}

void PersistCharVar(uint32 charId, const std::string& var, int32 value, uint32 expiry /* = 0 */)
{
    if (value == 0)
    {
        db::preparedStmt("DELETE FROM char_vars WHERE charid = ? AND varname = ? LIMIT 1", charId, var);
    }
    else
    {
        db::preparedStmt("INSERT INTO char_vars SET charid = ?, varname = ?, value = ?, expiry = ? ON DUPLICATE KEY UPDATE value = ?, expiry = ?", charId, var, value, expiry, value, expiry);
    }
}

uint16 getWideScanRange(JOBTYPE job, uint8 level)
{
    // Set Widescan range
    // Distances need verified, based current values off what we had in traits.sql and data at http://wiki.ffxiclopedia.org/wiki/Wide_Scan
    // NOTE: Widescan was formerly piggy backed onto traits (resist slow) but is not a real trait, any attempt to give it a trait will place a dot on
    // characters trait menu.

    // Limit to BST and RNG, and try to use old distance values for tiers
    if (job == JOB_RNG)
    {
        // Range for RNG >=80 needs verification.
        if (level >= 80)
        {
            return 350;
        }
        else if (level >= 60)
        {
            return 300;
        }
        else if (level >= 40)
        {
            return 250;
        }
        else if (level >= 20)
        {
            return 200;
        }
        else
        {
            return 150;
        }
    }
    else if (job == JOB_BST)
    {
        if (level >= 80)
        {
            return 300;
        }
        else if (level >= 60)
        {
            return 250;
        }
        else if (level >= 40)
        {
            return 200;
        }
        else if (level >= 20 || settings::get<bool>("map.ALL_JOBS_WIDESCAN"))
        {
            return 150;
        }
        else
        {
            return 50;
        }
    }

    // Default to base widescan if not RNG or BST
    if (settings::get<bool>("map.ALL_JOBS_WIDESCAN"))
    {
        return 150;
    }
    else
    {
        return 0;
    }
}

uint16 getWideScanRange(CCharEntity* PChar)
{
    // Get maximum widescan range from main job or sub job
    return std::max(getWideScanRange(PChar->GetMJob(), PChar->GetMLevel()), getWideScanRange(PChar->GetSJob(), PChar->GetSLevel()));
}

void SendTimerPacket(CCharEntity* PChar, uint32 seconds)
{
    PChar->pushPacket<GP_SERV_COMMAND_BATTLEFIELD>(seconds);
}

void SendTimerPacket(CCharEntity* PChar, timer::duration dur)
{
    auto timeLimitSeconds = static_cast<uint32>(timer::count_seconds(dur));
    SendTimerPacket(PChar, timeLimitSeconds);
}

void SendClearTimerPacket(CCharEntity* PChar)
{
    PChar->pushPacket<GP_SERV_COMMAND_BATTLEFIELD>();
}

earth_time::time_point getTraverserEpoch(CCharEntity* PChar)
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT UNIX_TIMESTAMP(traverser_start) AS start FROM char_unlocks WHERE charid = ? LIMIT 1", PChar->id);
    FOR_DB_SINGLE_RESULT(rset)
    {
        return earth_time::time_point(std::chrono::seconds(rset->getOrDefault<uint32>("start", 0)));
    }

    return earth_time::time_point(std::chrono::seconds(0));
}

// TODO: Perhaps allow for optional argument to support GM Commands
void setTraverserEpoch(CCharEntity* PChar)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_unlocks SET traverser_start = CURRENT_TIMESTAMP() WHERE charid = ?", PChar->id);
}

uint32 getClaimedTraverserStones(CCharEntity* PChar)
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT traverser_claimed FROM char_unlocks WHERE charid = ? LIMIT 1", PChar->id);
    FOR_DB_SINGLE_RESULT(rset)
    {
        return rset->get<uint32>("traverser_claimed");
    }

    return 0;
}

void addClaimedTraverserStones(CCharEntity* PChar, uint16 numStones)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_unlocks SET traverser_claimed = traverser_claimed + ? WHERE charid = ?", numStones, PChar->id);
}

void setClaimedTraverserStones(CCharEntity* PChar, uint16 stoneTotal)
{
    TracyZoneScoped;

    db::preparedStmt("UPDATE char_unlocks SET traverser_claimed = ? WHERE charid = ?", stoneTotal, PChar->id);
}

uint32 getAvailableTraverserStones(CCharEntity* PChar)
{
    TracyZoneScoped;

    earth_time::time_point traverserEpoch   = earth_time::time_point::min();
    uint32                 traverserClaimed = 0;

    const auto rset = db::preparedStmt("SELECT UNIX_TIMESTAMP(traverser_start) AS start, traverser_claimed FROM char_unlocks WHERE charid = ? LIMIT 1", PChar->id);
    FOR_DB_SINGLE_RESULT(rset)
    {
        traverserEpoch   = earth_time::time_point(std::chrono::seconds(rset->getOrDefault<uint32>("start", 0)));
        traverserClaimed = rset->get<uint32>("traverser_claimed");
    }

    if (miscprogresshelpers::IsTraverserEpochUnset(static_cast<uint32>(std::chrono::duration_cast<std::chrono::seconds>(traverserEpoch.time_since_epoch()).count())))
    {
        // Players cannot accrue Traverser Stones until the epoch has been set.  This is not possible
        // in quests, but is always displayed in player currencies.
        return 0;
    }

    // Handle reduction for Celerity Key Items
    uint8 celerityCount = 0;
    for (const auto traverserStoneReductionKeyItem : traverserStoneReductionKeyItems)
    {
        if (hasKeyItem(PChar, traverserStoneReductionKeyItem))
        {
            celerityCount += 1;
        }
    }

    const auto waitHours = miscprogresshelpers::TraverserWaitHours(celerityCount);
    earth_time::duration elapsedSinceEpoch = earth_time::now() - traverserEpoch;
    const auto elapsedHours = static_cast<uint32>(std::chrono::floor<std::chrono::hours>(elapsedSinceEpoch).count());
    const auto stonesGenerated = miscprogresshelpers::TraverserStonesGenerated(elapsedHours, waitHours);

    return miscprogresshelpers::AvailableTraverserStones(stonesGenerated, traverserClaimed);
}

void ReadHistory(CCharEntity* PChar)
{
    TracyZoneScoped;

    if (PChar == nullptr)
    {
        return;
    }

    const auto rset = db::preparedStmt("SELECT enemies_defeated, times_knocked_out, mh_entrances, " // 2
                                       "joined_parties, joined_alliances, spells_cast, "
                                       "abilities_used, ws_used, items_used, "
                                       "chats_sent, npc_interactions, battles_fought, "
                                       "gm_calls, distance_travelled "
                                       "FROM char_history "
                                       "WHERE charid = ? LIMIT 1",
                                       PChar->id);
    FOR_DB_SINGLE_RESULT(rset)
    {
        PChar->m_charHistory.enemiesDefeated   = rset->get<uint32>("enemies_defeated");
        PChar->m_charHistory.timesKnockedOut   = rset->get<uint32>("times_knocked_out");
        PChar->m_charHistory.mhEntrances       = rset->get<uint32>("mh_entrances");
        PChar->m_charHistory.joinedParties     = rset->get<uint32>("joined_parties");
        PChar->m_charHistory.joinedAlliances   = rset->get<uint32>("joined_alliances");
        PChar->m_charHistory.spellsCast        = rset->get<uint32>("spells_cast");
        PChar->m_charHistory.abilitiesUsed     = rset->get<uint32>("abilities_used");
        PChar->m_charHistory.wsUsed            = rset->get<uint32>("ws_used");
        PChar->m_charHistory.itemsUsed         = rset->get<uint32>("items_used");
        PChar->m_charHistory.chatsSent         = rset->get<uint32>("chats_sent");
        PChar->m_charHistory.npcInteractions   = rset->get<uint32>("npc_interactions");
        PChar->m_charHistory.battlesFought     = rset->get<uint32>("battles_fought");
        PChar->m_charHistory.gmCalls           = rset->get<uint32>("gm_calls");
        PChar->m_charHistory.distanceTravelled = rset->get<uint32>("distance_travelled");
    }
}

void WriteHistory(const CCharEntity* PChar)
{
    TracyZoneScoped;

    if (PChar == nullptr)
    {
        return;
    }

    db::preparedStmt(
        "INSERT INTO char_history SET "
        "charid = ?, "
        "enemies_defeated = ?, "
        "times_knocked_out = ?, "
        "mh_entrances = ?, "
        "joined_parties = ?, "
        "joined_alliances = ?, "
        "spells_cast = ?, "
        "abilities_used = ?, "
        "ws_used = ?, "
        "items_used = ?, "
        "chats_sent = ?, "
        "npc_interactions = ?, "
        "battles_fought = ?, "
        "gm_calls = ?, "
        "distance_travelled = ? "
        "ON DUPLICATE KEY UPDATE "
        "enemies_defeated = VALUES(enemies_defeated), "
        "times_knocked_out = VALUES(times_knocked_out), "
        "mh_entrances = VALUES(mh_entrances), "
        "joined_parties = VALUES(joined_parties), "
        "joined_alliances = VALUES(joined_alliances), "
        "spells_cast = VALUES(spells_cast), "
        "abilities_used = VALUES(abilities_used), "
        "ws_used = VALUES(ws_used), "
        "items_used = VALUES(items_used), "
        "chats_sent = VALUES(chats_sent), "
        "npc_interactions = VALUES(npc_interactions), "
        "battles_fought = VALUES(battles_fought), "
        "gm_calls = VALUES(gm_calls), "
        "distance_travelled = VALUES(distance_travelled)",
        PChar->id,
        PChar->m_charHistory.enemiesDefeated,
        PChar->m_charHistory.timesKnockedOut,
        PChar->m_charHistory.mhEntrances,
        PChar->m_charHistory.joinedParties,
        PChar->m_charHistory.joinedAlliances,
        PChar->m_charHistory.spellsCast,
        PChar->m_charHistory.abilitiesUsed,
        PChar->m_charHistory.wsUsed,
        PChar->m_charHistory.itemsUsed,
        PChar->m_charHistory.chatsSent,
        PChar->m_charHistory.npcInteractions,
        PChar->m_charHistory.battlesFought,
        PChar->m_charHistory.gmCalls,
        PChar->m_charHistory.distanceTravelled);
}

uint8 getMaxItemLevel(CCharEntity* PChar)
{
    uint8 maxItemLevel = 0;

    for (uint8 slotID = 0; slotID < 16; ++slotID)
    {
        CItemEquipment* PItem = PChar->getEquip((SLOTTYPE)slotID);

        if (PItem && PItem->getILvl() > maxItemLevel)
        {
            maxItemLevel = PItem->getILvl();
        }
    }

    return maxItemLevel;
}

uint8 getItemLevelDifference(CCharEntity* PChar)
{
    float itemLevelDiff = 0;
    uint8 highestItem   = 0;

    // Find the highest iLevel in weapons, this is 50% of the iLvl diff value
    for (uint8 slotID = 0; slotID < 4; ++slotID)
    {
        CItemEquipment* PItem = PChar->getEquip((SLOTTYPE)slotID);

        if (PItem && PItem->getILvl() > highestItem)
        {
            highestItem = PItem->getILvl();
        }
    }

    if (highestItem > 99)
    {
        itemLevelDiff += (highestItem - 99) / 2.0f;
    }

    for (uint8 slotID = 4; slotID < 9; ++slotID)
    {
        CItemEquipment* PItem = PChar->getEquip((SLOTTYPE)slotID);

        if (PItem && PItem->getILvl() > 99)
        {
            itemLevelDiff += (PItem->getILvl() - 99) / 10.0f;
        }
    }

    return floor(itemLevelDiff);
}

uint8 getMainhandItemLevel(CCharEntity* PChar)
{
    CItemEquipment* PItem = PChar->getEquip(SLOTTYPE::SLOT_MAIN);

    if (PItem)
    {
        return PItem->getILvl();
    }

    return 0;
}

// Return Ranged Weapon Item Level; If ranged slot exists use that, else use Ammo
uint8 getRangedItemLevel(CCharEntity* PChar)
{
    CItemEquipment* PItem = PChar->getEquip(SLOTTYPE::SLOT_RANGED);
    if (PItem)
    {
        return PItem->getILvl();
    }

    PItem = PChar->getEquip(SLOTTYPE::SLOT_AMMO);
    if (PItem)
    {
        return PItem->getILvl();
    }

    return 0;
}

bool hasEntitySpawned(CCharEntity* PChar, CBaseEntity* entity)
{
    SpawnIDList_t* spawnlist = nullptr;

    if (entityspawnhelpers::ShouldRejectNullEntity(entity != nullptr))
    {
        return false;
    }

    switch (entityspawnhelpers::SpawnListKindFromObjType(
        static_cast<uint8>(entity->objtype),
        static_cast<uint8>(TYPE_MOB),
        static_cast<uint8>(TYPE_NPC),
        static_cast<uint8>(TYPE_PC),
        static_cast<uint8>(TYPE_PET),
        static_cast<uint8>(TYPE_TRUST)))
    {
        case entityspawnhelpers::SpawnListKind::Mob:
            spawnlist = &PChar->SpawnMOBList;
            break;
        case entityspawnhelpers::SpawnListKind::NPC:
            spawnlist = &PChar->SpawnNPCList;
            break;
        case entityspawnhelpers::SpawnListKind::PC:
            spawnlist = &PChar->SpawnPCList;
            break;
        case entityspawnhelpers::SpawnListKind::Pet:
            spawnlist = &PChar->SpawnPETList;
            break;
        case entityspawnhelpers::SpawnListKind::Trust:
            spawnlist = &PChar->SpawnTRUSTList;
            break;
        default:
            return false;
    }

    return entityspawnhelpers::IsSpawnedInList(spawnlist->find(entity->id) != spawnlist->end());
}

uint32 getCharIdFromName(const std::string& name)
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT charid FROM chars WHERE charname = ? LIMIT 1", name);
    FOR_DB_SINGLE_RESULT(rset)
    {
        return rset->get<uint32>("charid");
    }

    return 0;
}

uint32 getAccountIdFromName(const std::string& name)
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT accid FROM chars WHERE charname = ? LIMIT 1", name);
    FOR_DB_SINGLE_RESULT(rset)
    {
        return rset->get<uint32>("accid");
    }

    return 0;
}

auto getCharIdAndAccountIdFromName(const std::string& name) -> std::pair<uint32, uint32>
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT charid, accid FROM chars WHERE charname = ? LIMIT 1", name);
    FOR_DB_SINGLE_RESULT(rset)
    {
        return { rset->get<uint32>("charid"), rset->get<uint32>("accid") };
    }

    return { 0, 0 };
}

void forceSynthCritFail(const std::string& sourceFunction, CCharEntity* PChar)
{
    // NOTE:
    // Supposed non-losable items are reportedly lost if this condition is met:
    // https://ffxiclopedia.fandom.com/wiki/Lu_Shang%27s_Fishing_Rod
    // The broken rod can never be lost in a normal failed synth. It will only be lost if the synth is
    // interrupted in some way, such as by being attacked or moving to another area (e.g. ship docking).

    ShowWarning("%s: Force crit-failing %s synthesis!", sourceFunction, PChar->getName());
    synthutils::doSynthCriticalFail(PChar);
}

void removeCharFromZone(CCharEntity* PChar)
{
    // Store old blowfish, recalculate expected new blowfish
    if (zoneouthelpers::ShouldMarkBlowfishPendingZone(PChar->PSession != nullptr))
    {
        PChar->PSession->blowfish.status = BLOWFISH_PENDING_ZONE;
    }

    PChar->TradePending.clean();
    PChar->InvitePending.clean();

    if (zoneouthelpers::ShouldNotifyNominateOnLeave(PChar->loc.zone != nullptr))
    {
        PChar->loc.zone->nominateManager().onCharLeavingZone(PChar);
    }

    PChar->WideScanTarget = std::nullopt;

    if (zoneouthelpers::ShouldClearAttackAnimation(static_cast<uint8>(PChar->animation)))
    {
        PChar->animation = ANIMATION_NONE;
        PChar->updatemask |= UPDATE_HP;
    }

    if (zoneouthelpers::ShouldClearTrusts(!PChar->PTrusts.empty()))
    {
        PChar->ClearTrusts();
    }

    const bool isShutdownLogout = zoneouthelpers::IsShutdownLogout(static_cast<uint8>(PChar->status));
    if (isShutdownLogout)
    {
        if (PChar->PParty != nullptr)
        {
            const bool hasAlliance = PChar->PParty->m_PAlliance != nullptr;
            const bool isLeader    = PChar->PParty->GetLeader() == PChar;
            const bool onlyMember  = PChar->PParty->HasOnlyOneMember();
            const bool onlyParty   = hasAlliance && PChar->PParty->m_PAlliance->hasOnlyOneParty();
            switch (zoneouthelpers::ClassifyZoneOutPartyLeave(true, hasAlliance, isLeader, onlyMember, onlyParty))
            {
                case zoneouthelpers::ZoneOutPartyAction::DissolveAlliance:
                    PChar->PParty->m_PAlliance->dissolveAlliance();
                    break;
                case zoneouthelpers::ZoneOutPartyAction::RemovePartyFromAlliance:
                    PChar->PParty->m_PAlliance->removeParty(PChar->PParty);
                    break;
                case zoneouthelpers::ZoneOutPartyAction::RemoveMember:
                    PChar->PParty->RemoveMember(PChar);
                    break;
                default:
                    break;
            }
        }

        if (zoneouthelpers::ShouldSetPetZoningInfo(PChar->shouldPetPersistThroughZoning()))
        {
            PChar->setPetZoningInfo();
        }
        else
        {
            PChar->resetPetZoningInfo();
        }

        PChar->PSession->shuttingDown = zoneouthelpers::SessionShuttingDownValue(true);
        db::preparedStmt("UPDATE char_stats SET zoning = ? WHERE charid = ?", zoneouthelpers::CharStatsZoningValue(true), PChar->id);
    }
    else
    {
        PChar->PSession->shuttingDown = zoneouthelpers::SessionShuttingDownValue(false);
        db::preparedStmt("UPDATE char_stats SET zoning = ? WHERE charid = ?", zoneouthelpers::CharStatsZoningValue(false), PChar->id);
    }

    if (zoneouthelpers::ShouldDecreaseZoneCounter(PChar->loc.zone != nullptr))
    {
        PChar->loc.zone->DecreaseZoneCounter(PChar);
    }

    PChar->StatusEffectContainer->SaveStatusEffects(zoneouthelpers::SaveStatusEffectsLogoutFlag(PChar->PSession->shuttingDown));
    PChar->PersistData();
    charutils::SavePlayTime(PChar);
    charutils::SaveCharStats(PChar);
    charutils::SaveCharExp(PChar, PChar->GetMJob());
    charutils::SaveEminenceData(PChar);
    charutils::SaveLastLogout(PChar);

    PChar->status = STATUS_TYPE::DISAPPEAR;
}

void updateSession(MapSession* PSession, CCharEntity* PChar, CZone* currentZone)
{
    db::preparedStmt("UPDATE accounts_sessions SET targid = ?, server_addr = ?, client_port = ?, last_zoneout_time = 0 WHERE charid = ? LIMIT 1",
                     PChar->targid,
                     currentZone->GetIP(),
                     PSession->client_ipp.getPort(),
                     PChar->id);
}

void loadDeathTimestamp(CCharEntity* PChar)
{
    const auto rset = db::preparedStmt("SELECT death FROM char_stats WHERE charid = ? LIMIT 1", PChar->id);
    if (rset && rset->rowsCount() && rset->next())
    {
        // Update the character's death timestamp based off of how long they were previously dead
        const auto secondsSinceDeathRaw = rset->get<uint32>("death");
        const auto secondsSinceDeath    = std::chrono::seconds(secondsSinceDeathRaw);
        if (zoneouthelpers::ShouldApplyDeathTimestamp(PChar->health.hp))
        {
            PChar->SetDeathTime(timer::time_point(timer::now() - secondsSinceDeath));
            const auto remaining = zoneouthelpers::RemainingDeathDurationSeconds(
                std::chrono::duration_cast<std::chrono::seconds>(CCharEntity::death_duration).count(),
                secondsSinceDeathRaw);
            PChar->Die(std::chrono::seconds(remaining));
        }
    }
}

bool isOrchestrionPlaced(CCharEntity* PChar)
{
    for (auto safeContainerId : { LOC_MOGSAFE, LOC_MOGSAFE2 })
    {
        CItemContainer* PContainer = PChar->getStorage(safeContainerId);
        for (int slotIndex = 1; zoneouthelpers::IsValidMogsafeSlotIndex(slotIndex, PContainer->GetSize()); ++slotIndex)
        {
            CItem* PContainerItem = PContainer->GetItem(slotIndex);
            if (PContainerItem != nullptr &&
                zoneouthelpers::IsOrchestrionFurniture(
                    PContainerItem->isType(ITEM_FURNISHING),
                    PContainerItem->isType(ITEM_FURNISHING) && static_cast<CItemFurnishing*>(PContainerItem)->isInstalled(),
                    PContainerItem->getID()))
            {
                return true;
            }
        }
    }

    return false;
}

void updateMannequins(CCharEntity* PChar)
{
    for (auto safeContainerId : { LOC_MOGSAFE, LOC_MOGSAFE2 })
    {
        CItemContainer* PContainer = PChar->getStorage(safeContainerId);
        for (int slotIndex = 1; zoneouthelpers::IsValidMogsafeSlotIndex(slotIndex, PContainer->GetSize()); ++slotIndex)
        {
            CItem* PContainerItem = PContainer->GetItem(slotIndex);
            if (PContainerItem != nullptr && PContainerItem->isType(ITEM_FURNISHING))
            {
                auto* PFurnishing = static_cast<CItemFurnishing*>(PContainerItem);
                if (zoneouthelpers::IsInstalledMannequin(true, PFurnishing->isInstalled(), PFurnishing->isMannequin()))
                {
                    auto& mannequin = PFurnishing->exdata<Exdata::Mannequin>();

                    if (zoneouthelpers::ShouldWarnInvalidMannequinRace(mannequin.Race))
                    {
                        ShowWarning("Invalid Mannequin placed (race of 0 in exdata, when races start at 1). It will be unusable.");
                    }

                    PChar->pushPacket<GP_SERV_COMMAND_ITEM_SUBCONTAINER>(PChar, safeContainerId, slotIndex, mannequin);
                }
            }
        }
    }
}

bool raceChange(CCharEntity* PChar, CharRace newRace, CharFace newFace, CharSize newSize)
{
    if (entityspawnhelpers::ShouldRejectNullCharRaceChange(PChar != nullptr))
    {
        return false;
    }

    if (!entityspawnhelpers::IsRaceChangeArgsInBounds(
            static_cast<uint8>(newRace),
            static_cast<uint8>(newFace),
            static_cast<uint8>(newSize),
            static_cast<uint8>(CharRace::HumeMale),
            static_cast<uint8>(CharRace::Galka),
            static_cast<uint8>(CharFace::Face8B),
            static_cast<uint8>(CharSize::Large)))
    {
        ShowError("charutils::raceChange: Arguments out of bounds for charid: %u", PChar->id);
        return false;
    }

    if (!db::preparedStmt("UPDATE char_look SET "
                          "face = ?, race = ?, size = ? "
                          "WHERE charid = ?",
                          newFace,
                          newRace,
                          newSize,
                          PChar->id))
    {
        ShowError("charutils::raceChange: Failed to update char_look for charid: %u", PChar->id);
        return false;
    }

    for (uint8 slotId = SLOT_MAIN; entityspawnhelpers::IsRaceChangeEquipSlotInRange(slotId, SLOT_MAIN, SLOT_BACK); ++slotId)
    {
        if (auto* PItem = PChar->getEquip(static_cast<SLOTTYPE>(slotId)))
        {
            if (entityspawnhelpers::ShouldUnequipOnRaceChange(PItem->isEquippableByRace(static_cast<uint8>(newRace))))
            {
                charutils::UnequipItem(PChar, slotId);
            }
        }
    }

    ForceRezone(PChar);
    return true;
}

void ApplyAbilityRecast(CCharEntity* PChar, const CAbility* PAbility, const Charge_t* charge, const timer::duration baseChargeTime, const timer::duration recastTime)
{
    if (miscprogresshelpers::HasChargeAdd(charge != nullptr))
    {
        PChar->PRecastContainer->Add(RECAST_ABILITY, PAbility->getRecastId(), recastTime, baseChargeTime, charge->maxCharges);
    }
    else
    {
        PChar->PRecastContainer->Add(RECAST_ABILITY, PAbility->getRecastId(), recastTime);
    }

    const auto recastId = static_cast<uint16>(PAbility->getRecastId());
    if (miscprogresshelpers::ShouldShareBloodPactTimer(settings::get<bool>("map.BLOOD_PACT_SHARED_TIMER"), recastId))
    {
        PChar->PRecastContainer->Add(RECAST_ABILITY, static_cast<Recast>(miscprogresshelpers::PairedBloodPactRecast(recastId)), recastTime);
    }

    // Yonin (recastId 146) and Innin share a server-side timer via the SQL recastId update.
    // Also add Innin's original client-facing recast ID (147) so the client greys out Innin.
    if (miscprogresshelpers::ShouldMirrorYoninToInnin(recastId))
    {
        PChar->PRecastContainer->Add(RECAST_ABILITY, static_cast<Recast>(miscprogresshelpers::RecastInnin), recastTime);
    }

    PChar->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PChar);
}

void TrackArrowUsageForScavenge(CCharEntity* PChar, CItemWeapon* PAmmo)
{
    TracyZoneScoped;

    const auto arrowsUsed = static_cast<uint32>(PChar->GetLocalVar("ArrowsUsed"));
    const auto ammoID     = PAmmo->getID();

    // Check if local has been set yet
    if (miscprogresshelpers::ShouldInitArrowsUsed(arrowsUsed))
    {
        // Local not set yet so set
        PChar->SetLocalVar("ArrowsUsed", miscprogresshelpers::EncodeArrowsUsed(ammoID));
    }
    else
    {
        // Local exists now check if arrow used is same as last time
        if (miscprogresshelpers::IsSameArrowAsLast(arrowsUsed, ammoID))
        {
            // Same arrow used as last time now check that arrows used do not go above 1980
            if (miscprogresshelpers::ShouldIncrementArrowsUsed(arrowsUsed))
            {
                // Safe to increment arrows used
                PChar->SetLocalVar("ArrowsUsed", miscprogresshelpers::IncrementArrowsUsed(arrowsUsed));
            }
        }
        else
        {
            // Different arrow is being used so remake local
            PChar->SetLocalVar("ArrowsUsed", miscprogresshelpers::EncodeArrowsUsed(ammoID));
        }
    }
}

}; // namespace charutils
