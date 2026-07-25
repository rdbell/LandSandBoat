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

#include "battleutils.h"

#include "common/database.h"
#include "common/logging.h"
#include "common/settings.h"
#include "common/timer.h"
#include "common/utils.h"
#include "ranged_ammo_capacity.h"
#include "paralyze_shadow_capacity.h"
#include "combat_status_tails_capacity.h"
#include "claim_capacity.h"
#include "enmity_combat_capacity.h"
#include "spikes_capacity.h"
#include "entity_action_capacity.h"
#include "traits_enmity_capacity.h"
#include "wildcard_randomdeal_capacity.h"
#include "can_afford_spell_capacity.h"
#include "spikes_status_capacity.h"
#include "take_damage_capacity.h"
#include "take_physical_capacity.h"
#include "treasure_hunter_proc_capacity.h"
#include "enspell_handle_capacity.h"
#include "skillchain_effect_capacity.h"
#include "trick_attack_capacity.h"
#include "draw_in_capacity.h"
#include "enspell_damage_tails_capacity.h"
#include "enspell_damage_tier_capacity.h"
#include "weather_get_capacity.h"
#include "entity_equip_capacity.h"
#include "weather_matches_capacity.h"
#include "barrage_capacity.h"
#include "scaled_item_modifier_capacity.h"
#include "spell_cost_capacity.h"
#include "dmg_taken_capacity.h"
#include "ws_tp_capacity.h"
#include "spell_recast_capacity.h"
#include "spell_cast_capacity.h"
#include "multi_hits_capacity.h"
#include "crit_hit_rate_capacity.h"
#include "intimidate_capacity.h"
#include "hit_count_capacity.h"
#include "skillchain_tables_capacity.h"
#include "combat_bonus_tails_capacity.h"
#include "damage_affinity_capacity.h"
#include "fstr_capacity.h"
#include "enmity_mod_capacity.h"
#include "weaponskill_use_capacity.h"
#include "skill_cap_capacity.h"
#include "tp_return_capacity.h"
#include "tp_from_damage_capacity.h"
#include "ninja_tool_capacity.h"
#include "base_delay_capacity.h"
#include "skillchain_damage_capacity.h"
#include "skillchain_inject_capacity.h"
#include "physical_hit_rate_capacity.h"
#include "pdif_capacity.h"
#include "level_correction_capacity.h"
#include "attuner_capacity.h"
#include "spell_interrupt_capacity.h"
#include "combat_status_mitigation_capacity.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <unordered_map>

#include "packets/char_status.h"
#include "packets/s2c/0x01d_item_same.h"

#include "lua/luautils.h"

#include "action/action.h"
#include "ai/ai_container.h"
#include "ai/controllers/pet_controller.h"
#include "ai/controllers/player_charm_controller.h"
#include "ai/states/magic_state.h"
#include "attack.h"
#include "attackutils.h"
#include "charutils.h"
#include "enmity_container.h"
#include "entities/battle_entity.h"
#include "entities/mob_entity.h"
#include "entities/automaton_entity.h"
#include "entities/pet_entity.h"
#include "entities/trust_entity.h"
#include "enums/action/hit_distortion.h"
#include "enums/action/info.h"
#include "enums/msg_std.h"
#include "enums/weather.h"
#include "item_container.h"
#include "items.h"
#include "items/item_weapon.h"
#include "job_points.h"
#include "map/navmesh/navmesh.h"
#include "map_engine.h"
#include "mob_modifier.h"
#include "mobskill.h"
#include "modifier.h"
#include "notoriety_container.h"
#include "packets/pet_sync.h"
#include "packets/s2c/0x029_battle_message.h"
#include "packets/s2c/0x058_assist.h"
#include "packets/s2c/0x05b_wpos.h"
#include "packets/s2c/0x0ac_command_data.h"
#include "packets/s2c/0x119_abil_recast.h"
#include "party.h"
#include "petskill.h"
#include "recast_container.h"
#include "spell.h"
#include "status_effect_container.h"
#include "trait.h"
#include "utils/petutils.h"
#include "weapon_skill.h"
#include "zoneutils.h"

#include <map/ximesh/ximesh.h>

/************************************************************************
 *                                                                       *
 *  Lists used in battleutils                                            *
 *                                                                       *
 ************************************************************************/

std::array<std::array<uint16, 14>, 100>                                            g_SkillTable;
std::array<std::array<uint8, MAX_JOBTYPE>, MAX_SKILLTYPE>                          g_SkillRanks;
std::array<std::array<uint16, MAX_SKILLCHAIN_COUNT + 1>, MAX_SKILLCHAIN_LEVEL + 1> g_SkillChainDamageModifiers;

std::array<CWeaponSkill*, MAX_WEAPONSKILL_ID> g_PWeaponSkillList; // Holds all Weapon skills
std::array<CMobSkill*, MAX_MOBSKILL_ID>       g_PMobSkillList;    // List of mob skills
std::unordered_map<uint32, CPetSkill*>        g_PPetSkillList;    // List of pet skills

std::array<std::list<CWeaponSkill*>, MAX_SKILLTYPE> g_PWeaponSkillsList;
std::unordered_map<uint16, std::vector<uint16>>     g_PMobSkillLists; // List of mob skills defined from mob_skill_lists.sql

namespace battleutils
{

const float worldAngleMinDistance = 0.5f;
[[maybe_unused]] const uint8 worldAngleMaxDeviance = 8;

void LoadSkillTable()
{
    uint32 x    = 0;
    auto   rset = db::preparedStmt("SELECT r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13 "
                                   "FROM skill_caps "
                                   "ORDER BY level "
                                   "LIMIT 100");

    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        for (uint32 y = 0; y < 14; ++y)
        {
            g_SkillTable[x][y] = rset->get<uint16>(std::format("r{}", y));
        }

        ++x;
    }

    rset = db::preparedStmt("SELECT skillid,war,mnk,whm,blm,rdm,thf,pld,drk,bst,brd,rng,sam,nin,drg,smn,blu,cor,pup,dnc,sch,geo,run "
                            "FROM skill_ranks LIMIT 64");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        const auto SkillID = std::clamp<uint8>(rset->get<uint8>("skillid"), 0, MAX_SKILLTYPE - 1);

        // NOTE: Skip over Monstrosity, they re-use other jobs ranks
        g_SkillRanks[SkillID][JOB_WAR] = std::clamp<uint8>(rset->get<uint8>("war"), 0, 11);
        g_SkillRanks[SkillID][JOB_MNK] = std::clamp<uint8>(rset->get<uint8>("mnk"), 0, 11);
        g_SkillRanks[SkillID][JOB_WHM] = std::clamp<uint8>(rset->get<uint8>("whm"), 0, 11);
        g_SkillRanks[SkillID][JOB_BLM] = std::clamp<uint8>(rset->get<uint8>("blm"), 0, 11);
        g_SkillRanks[SkillID][JOB_RDM] = std::clamp<uint8>(rset->get<uint8>("rdm"), 0, 11);
        g_SkillRanks[SkillID][JOB_THF] = std::clamp<uint8>(rset->get<uint8>("thf"), 0, 11);
        g_SkillRanks[SkillID][JOB_PLD] = std::clamp<uint8>(rset->get<uint8>("pld"), 0, 11);
        g_SkillRanks[SkillID][JOB_DRK] = std::clamp<uint8>(rset->get<uint8>("drk"), 0, 11);
        g_SkillRanks[SkillID][JOB_BST] = std::clamp<uint8>(rset->get<uint8>("bst"), 0, 11);
        g_SkillRanks[SkillID][JOB_BRD] = std::clamp<uint8>(rset->get<uint8>("brd"), 0, 11);
        g_SkillRanks[SkillID][JOB_RNG] = std::clamp<uint8>(rset->get<uint8>("rng"), 0, 11);
        g_SkillRanks[SkillID][JOB_SAM] = std::clamp<uint8>(rset->get<uint8>("sam"), 0, 11);
        g_SkillRanks[SkillID][JOB_NIN] = std::clamp<uint8>(rset->get<uint8>("nin"), 0, 11);
        g_SkillRanks[SkillID][JOB_DRG] = std::clamp<uint8>(rset->get<uint8>("drg"), 0, 11);
        g_SkillRanks[SkillID][JOB_SMN] = std::clamp<uint8>(rset->get<uint8>("smn"), 0, 11);
        g_SkillRanks[SkillID][JOB_BLU] = std::clamp<uint8>(rset->get<uint8>("blu"), 0, 11);
        g_SkillRanks[SkillID][JOB_COR] = std::clamp<uint8>(rset->get<uint8>("cor"), 0, 11);
        g_SkillRanks[SkillID][JOB_PUP] = std::clamp<uint8>(rset->get<uint8>("pup"), 0, 11);
        g_SkillRanks[SkillID][JOB_DNC] = std::clamp<uint8>(rset->get<uint8>("dnc"), 0, 11);
        g_SkillRanks[SkillID][JOB_SCH] = std::clamp<uint8>(rset->get<uint8>("sch"), 0, 11);
        g_SkillRanks[SkillID][JOB_GEO] = std::clamp<uint8>(rset->get<uint8>("geo"), 0, 11);
        g_SkillRanks[SkillID][JOB_RUN] = std::clamp<uint8>(rset->get<uint8>("run"), 0, 11);
    }
}

/************************************************************************
 *                                                                       *
 *  Load Skills List                                                     *
 *                                                                       *
 ************************************************************************/

void LoadWeaponSkillsList()
{
    const auto rset = db::preparedStmt("SELECT weaponskillid, name, jobs, type, skilllevel, element, animation, "
                                       "animationTime, `range`, aoe, radius, primary_sc, secondary_sc, tertiary_sc, main_only, unlock_id "
                                       "FROM weapon_skills "
                                       "WHERE weaponskillid < ? "
                                       "ORDER BY type, skilllevel ASC",
                                       MAX_WEAPONSKILL_ID);
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        auto* PWeaponSkill = new CWeaponSkill(rset->get<uint16>("weaponskillid"));

        PWeaponSkill->setName(rset->get<std::string>("name"));

        // Jobs are stored in DB as 22 entries.
        // Index 0 is reserved for NON, index 23 for MON (both left as 0).
        std::array<uint8, MAX_JOBTYPE> jobs{};
        std::array<uint8, 22>          tempJobs{};
        db::extractFromBlob(rset, "jobs", tempJobs);
        std::memcpy(&jobs[1], tempJobs.data(), 22);
        PWeaponSkill->setJob(jobs);

        PWeaponSkill->setType(rset->get<uint8>("type"));
        PWeaponSkill->setSkillLevel(rset->get<uint16>("skilllevel"));
        PWeaponSkill->setElement(rset->get<uint8>("element"));
        PWeaponSkill->setAnimationId(rset->get<uint8>("animation"));
        PWeaponSkill->setAnimationTime(std::chrono::milliseconds(rset->get<uint32>("animationTime")));
        PWeaponSkill->setRange(rset->get<uint8>("range"));
        PWeaponSkill->setAoe(rset->get<uint8>("aoe"));
        PWeaponSkill->setRadius(rset->get<uint8>("radius"));
        PWeaponSkill->setPrimarySkillchain(rset->get<uint8>("primary_sc"));
        PWeaponSkill->setSecondarySkillchain(rset->get<uint8>("secondary_sc"));
        PWeaponSkill->setTertiarySkillchain(rset->get<uint8>("tertiary_sc"));
        PWeaponSkill->setMainOnly(rset->get<uint8>("main_only"));
        PWeaponSkill->setUnlockId(rset->get<uint8>("unlock_id"));

        g_PWeaponSkillList[PWeaponSkill->getID()] = PWeaponSkill;
        g_PWeaponSkillsList[PWeaponSkill->getType()].emplace_back(PWeaponSkill);

        auto filename = fmt::format("./scripts/actions/weaponskills/{}.lua", PWeaponSkill->getName());
        luautils::LoadLuaObjectFromFile(filename);
    }
}

void LoadMobSkillsList()
{
    // Load all mob skills
    auto rset = db::preparedStmt("SELECT mob_skill_id, mob_anim_id, mob_skill_name, "
                                 "mob_skill_aoe, mob_skill_aoe_radius, mob_skill_distance, mob_anim_time, mob_prepare_time, "
                                 "mob_valid_targets, mob_skill_flag, mob_skill_param, knockback, primary_sc, secondary_sc, tertiary_sc "
                                 "FROM mob_skills");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        auto* PMobSkill = new CMobSkill(rset->get<uint16>("mob_skill_id"));

        PMobSkill->setAnimationID(rset->get<uint16>("mob_anim_id"));
        PMobSkill->setName(rset->get<std::string>("mob_skill_name"));
        PMobSkill->setAoe(rset->get<uint8>("mob_skill_aoe"));
        PMobSkill->setAoeRadius(rset->get<float>("mob_skill_aoe_radius"));
        PMobSkill->setDistance(rset->get<float>("mob_skill_distance"));
        PMobSkill->setAnimationTime(std::chrono::milliseconds(rset->get<uint32>("mob_anim_time")));
        PMobSkill->setActivationTime(std::chrono::milliseconds(rset->get<uint32>("mob_prepare_time")));
        auto validTargets = rset->get<uint16>("mob_valid_targets");
        if ((validTargets & TARGET_SELF) && (validTargets & TARGET_ENEMY))
        {
            ShowWarningFmt("Mob skill {} ({}) has both TARGET_SELF and TARGET_ENEMY set", PMobSkill->getName(), PMobSkill->getID());
        }
        PMobSkill->setValidTargets(validTargets);
        PMobSkill->setFlag(rset->get<uint16>("mob_skill_flag"));
        PMobSkill->setParam(rset->get<int16>("mob_skill_param"));
        PMobSkill->setKnockback(rset->get<Knockback>("knockback"));
        PMobSkill->setPrimarySkillchain(rset->get<uint8>("primary_sc"));
        PMobSkill->setSecondarySkillchain(rset->get<uint8>("secondary_sc"));
        PMobSkill->setTertiarySkillchain(rset->get<uint8>("tertiary_sc"));
        PMobSkill->setMsg(MsgBasic::UsesSkillTakesDamage); // standard damage message. Scripters will change this.
        g_PMobSkillList[PMobSkill->getID()] = PMobSkill;

        auto filename = fmt::format("./scripts/actions/mobskills/{}.lua", PMobSkill->getName());
        luautils::LoadLuaObjectFromFile(filename);
    }

    rset = db::preparedStmt("SELECT skill_list_id, mob_skill_id FROM mob_skill_lists");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        const auto skillListId = rset->get<uint16>("skill_list_id");
        auto       skillId     = rset->get<uint16>("mob_skill_id");

        g_PMobSkillLists[skillListId].emplace_back(skillId);
    }
}

void LoadPetSkillsList()
{
    // Load all pet skills
    const auto rset = db::preparedStmt("SELECT pet_skill_id, pet_anim_id, pet_skill_name, "
                                       "pet_skill_aoe, pet_skill_radius, pet_skill_distance, pet_anim_time, pet_prepare_time, "
                                       "pet_valid_targets, pet_message, pet_skill_flag, pet_skill_param, pet_skill_finish_category, knockback, primary_sc, secondary_sc, tertiary_sc, mob_skill_id "
                                       "FROM pet_skills");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        auto* PPetSkill = new CPetSkill(rset->get<uint16>("pet_skill_id"));

        PPetSkill->setAnimationID(rset->get<uint16>("pet_anim_id"));
        PPetSkill->setName(rset->get<std::string>("pet_skill_name"));
        PPetSkill->setAoe(rset->get<uint8>("pet_skill_aoe"));
        PPetSkill->setRadius(rset->get<uint8>("pet_skill_radius"));
        PPetSkill->setDistance(rset->get<float>("pet_skill_distance"));
        PPetSkill->setAnimationTime(std::chrono::milliseconds(rset->get<uint32>("pet_anim_time")));
        PPetSkill->setActivationTime(std::chrono::milliseconds(rset->get<uint32>("pet_prepare_time")));
        PPetSkill->setValidTargets(rset->get<uint16>("pet_valid_targets"));
        PPetSkill->setMsg(rset->get<MsgBasic>("pet_message"));
        PPetSkill->setFlag(rset->get<uint8>("pet_skill_flag"));
        PPetSkill->setParam(rset->get<int16>("pet_skill_param"));
        PPetSkill->setSkillFinishCategory(rset->get<uint8>("pet_skill_finish_category"));
        PPetSkill->setKnockback(rset->get<uint8>("knockback"));
        PPetSkill->setPrimarySkillchain(rset->get<uint8>("primary_sc"));
        PPetSkill->setSecondarySkillchain(rset->get<uint8>("secondary_sc"));
        PPetSkill->setTertiarySkillchain(rset->get<uint8>("tertiary_sc"));
        PPetSkill->setMobSkillID(rset->get<uint16>("mob_skill_id"));
        g_PPetSkillList[PPetSkill->getID()] = PPetSkill;

        auto filename = fmt::format("./scripts/actions/abilities/pets/{}.lua", PPetSkill->getName());
        luautils::LoadLuaObjectFromFile(filename);
    }
}

void LoadSkillChainDamageModifiers()
{
    const auto rset = db::preparedStmt("SELECT chain_level, chain_count, initial_modifier, magic_burst_modifier "
                                       "FROM skillchain_damage_modifiers "
                                       "ORDER BY chain_level, chain_count");
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        const auto level = rset->get<uint16>("chain_level");
        const auto count = rset->get<uint16>("chain_count");
        const auto value = rset->get<uint16>("initial_modifier");

        g_SkillChainDamageModifiers[level][count] = value;
    }
}

/************************************************************************
 *                                                                       *
 *  Clear Up (Free) Skills List                                          *
 *                                                                       *
 ************************************************************************/

void FreeWeaponSkillsList()
{
    for (int32 SkillId = 0; SkillId < MAX_WEAPONSKILL_ID; ++SkillId)
    {
        destroy(g_PWeaponSkillList[SkillId]);
    }
}

void FreeMobSkillList()
{
    for (auto& mobskill : g_PMobSkillList)
    {
        destroy(mobskill);
    }
}

void FreePetSkillList()
{
    for (auto& petskill : g_PPetSkillList)
    {
        destroy(petskill.second);
    }
    g_PPetSkillList.clear();
}

/************************************************************************
 *                                                                       *
 *  Get Skill Rank By SkillId and JobId                                  *
 *                                                                       *
 ************************************************************************/

uint8 GetSkillRank(SKILLTYPE SkillID, JOBTYPE JobID)
{
    return g_SkillRanks[SkillID][JobID];
}

/************************************************************************
 *                                                                       *
 *  Return Max Skill by SkillType, JobType, and Level                    *
 *                                                                       *
 ************************************************************************/

uint16 GetMaxSkill(SKILLTYPE SkillID, JOBTYPE JobID, uint8 level)
{
    // The skill_caps table is 0-indexed, so our maximum level should one lower
    // than the size of the array.
    const auto maxLevel = static_cast<uint8>(g_SkillTable.size() - 1);

    // TODO: Research on mobs level 99+ is still on-going. This line can be removed once the correct formula/skilltype have been established.
    // max indexed value and level is capped at 99 as stated above for skill_caps table
    const auto levelAfterSoftCap = skillcaphelpers::CapLevelForSkillTable(level);

    if (skillcaphelpers::LevelExceedsSkillTable(levelAfterSoftCap, maxLevel))
    {
        ShowDebug("battleutils::GetMaxSkill() received level value greater than array size! (Received: %d, Clamped to: %d)", levelAfterSoftCap, maxLevel);
    }

    const auto idx  = skillcaphelpers::ResolveSkillTableLevel(level, maxLevel, true);
    const auto rank = g_SkillRanks[SkillID][JobID];
    return g_SkillTable[idx][rank];
}

uint16 GetMaxSkill(uint8 rank, uint8 level)
{
    const auto maxLevel = static_cast<uint8>(g_SkillTable.size() - 1);

    if (skillcaphelpers::LevelExceedsSkillTable(level, maxLevel))
    {
        ShowDebug("battleutils::GetMaxSkill() received level value greater than array size! (Received: %d, Clamped to: %d)", level, maxLevel);
    }

    const auto idx = skillcaphelpers::ResolveSkillTableLevel(level, maxLevel, false);
    return g_SkillTable[idx][rank];
}

bool isValidSelfTargetWeaponskill(int wsid)
{
    return weaponskillusehelpers::IsValidSelfTargetWeaponskill(wsid);
}

bool CanUseWeaponskill(CCharEntity* PChar, CWeaponSkill* PSkill)
{
    const auto unlockID = PSkill->getUnlockId();
    return weaponskillusehelpers::CanUseWeaponskill({
        .skillLevel = PSkill->getSkillLevel(),
        .unlockID   = unlockID,
        .mainOnly   = PSkill->mainOnly(),
        .mainJobReq = PSkill->getJob(PChar->GetMJob()),
        .subJobReq  = PSkill->getJob(PChar->GetSJob()),
        .charSkill  = PChar->GetSkill(PSkill->getType()),
        .hasLearned = unlockID != 0 && charutils::hasLearnedWeaponskill(PChar, unlockID),
        .mLevel     = PChar->GetMLevel(),
    });
}

/************************************************************************
 *                                                                       *
 *  Get Enmity Modifier                                                  *
 *                                                                       *
 ************************************************************************/

int32 GetEnmityModDamage(int16 level)
{
    return enmitymodhelpers::GetEnmityModDamage(level);
}

int32 GetEnmityModCure(int16 level)
{
    return enmitymodhelpers::GetEnmityModCure(level);
}

/************************************************************************
 *                                                                       *
 *  Get Weapon Skill by ID                                               *
 *                                                                       *
 ************************************************************************/

CWeaponSkill* GetWeaponSkill(uint16 WSkillID)
{
    if (WSkillID >= MAX_WEAPONSKILL_ID)
    {
        ShowError("WSkillID (%d) exceeds MAX_WEAPONSKILL_ID.", WSkillID);
        return nullptr;
    }

    // False positive: this is CWeaponSkill*, so it's OK
    // cppcheck-suppress CastIntegerToAddressAtReturn
    return g_PWeaponSkillList[WSkillID];
}

/************************************************************************
 *                                                                       *
 * Get List of Weapon Skills from skill type                             *
 *                                                                       *
 ************************************************************************/

const std::list<CWeaponSkill*>& GetWeaponSkills(uint8 skill)
{
    if (skill >= MAX_SKILLTYPE)
    {
        ShowWarning("Skill (%d) exceeds MAX_SKILLTYPE", skill);
        return g_PWeaponSkillsList[SKILL_NONE];
    }

    return g_PWeaponSkillsList[skill];
}

/************************************************************************
 *                                                                       *
 *  Get Mob Skill by Id                                                  *
 *                                                                       *
 ************************************************************************/

CMobSkill* GetMobSkill(uint16 SkillID)
{
    if (SkillID < g_PMobSkillList.size())
    {
        // False positive: this is CMobSkill*, so it's OK
        // cppcheck-suppress CastIntegerToAddressAtReturn
        return g_PMobSkillList[SkillID];
    }
    else
    {
        return nullptr;
    }
}

/************************************************************************
 *                                                                       *
 *  Get Pet Skill by Id                                                  *
 *                                                                       *
 ************************************************************************/

CPetSkill* GetPetSkill(uint16 SkillID)
{
    if (g_PPetSkillList.find(SkillID) != g_PPetSkillList.end())
    {
        return g_PPetSkillList[SkillID];
    }
    else
    {
        return nullptr;
    }
}

/************************************************************************
 *                                                                       *
 *  Get Mob Skills by List Id                                            *
 *                                                                       *
 ************************************************************************/

const std::vector<uint16>& GetMobSkillList(uint16 ListID)
{
    return g_PMobSkillLists[ListID];
}

// TODO: Apply fire in generous quantities. Replace with existing lua functions.
int32 CalculateEnspellDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, uint8 Tier, uint8 element, CItemWeapon* pWeaponHit)
{
    int32 damage = 0;

    auto* PChar    = dynamic_cast<CCharEntity*>(PAttacker);
    int32 totalMod = PAttacker->getMod(Mod::ENSPELL_DMG_BONUS);
    int32 exclude  = 0;
    if (PChar)
    {
        constexpr SLOTTYPE slots[] = { SLOT_MAIN, SLOT_SUB };
        for (SLOTTYPE slot : slots)
        {
            if (auto* eq = PChar->getEquip(slot); eq && eq != pWeaponHit)
            {
                exclude += eq->getModifier(Mod::ENSPELL_DMG_BONUS);
            }
        }
    }
    int32 bonus = enspelldamagetailshelpers::EnspellBonusFromExclude(totalMod, exclude);

    // Tier 1/2 base product dual-wired to enspelldamagetierhelpers (slice 6767 / 0804).
    // Host residual: equip-exclude bonus, skill/merit reads, ENSPELL_DMG writeback.
    if (Tier == 1)
    {
        // Cast-time base stored in Mod::ENSPELL_DMG; no mod mutation.
        int32 merit = 0;
        if (PChar)
        {
            merit = PChar->PMeritPoints->GetMeritValue(MERIT_ENSPELL_DAMAGE, PChar);
        }
        damage = enspelldamagetierhelpers::CalculateEnspellTier1Damage(
            PAttacker->getMod(Mod::ENSPELL_DMG), bonus, merit);
    }
    else if (Tier == 2)
    {
        // Per-hit ramp of ENSPELL_DMG potency; pure product returns new mod value.
        const uint16 skill       = PAttacker->GetSkill(SKILL_ENHANCING_MAGIC);
        const int32  enspellDMG  = PAttacker->getMod(Mod::ENSPELL_DMG);
        int32        merit       = 0;
        if (PChar)
        {
            merit = PChar->PMeritPoints->GetMeritValue(MERIT_ENSPELL_DAMAGE, PChar);
        }

        const auto res = enspelldamagetierhelpers::CalculateEnspellTier2Damage(
            enspellDMG, skill, bonus, merit);
        damage = res.damage;
        if (res.newEnspellDMG != enspellDMG)
        {
            PAttacker->setModifier(Mod::ENSPELL_DMG, res.newEnspellDMG);
        }
    }
    else if (Tier == 3) // enlight or endark
    {
        bool decayMod     = false;
        bool removeStatus = false;
        damage            = enspelldamagetailshelpers::CalculateEnspellTier3Damage(
            PAttacker->getMod(Mod::ENSPELL_DMG), decayMod, removeStatus);

        if (decayMod)
        {
            PAttacker->delModifier(Mod::ENSPELL_DMG, 1);
        }
        else if (removeStatus)
        {
            if (element == ELEMENT_DARK)
            {
                PAttacker->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Endark);
            }
            else
            {
                PAttacker->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Enlight);
            }
        }

        damage += bonus;
    }
    else if (Tier == 4) // Rune Enhancement
    {
        // see https://www.ffxiah.com/forum/topic/56613/rune-enhancement-damage-formula-testing/ for data and comments
        CItemWeapon* PWeapon = nullptr;

        // Prefer player equip if attacker is a player
        if (auto* PCharAtk = dynamic_cast<CCharEntity*>(PAttacker))
        {
            if (auto* equip = PCharAtk->getEquip(SLOT_MAIN))
            {
                PWeapon = dynamic_cast<CItemWeapon*>(equip);
            }
        }

        // If no player equip, try the entity's internal weapon slot (used by mobs/trusts)
        if (PWeapon == nullptr)
        {
            PWeapon = dynamic_cast<CItemWeapon*>(PAttacker->m_Weapons[SLOT_MAIN]);
        }

        const double weaponDPS = PWeapon ? PWeapon->getDPS() : 0.0;
        const double runeDPS   = enspelldamagetailshelpers::EnspellRuneDPS(
            weaponDPS, PWeapon != nullptr, PAttacker->IsDualWielding());

        xi::StatusEffect highestRuneEffect = PAttacker->StatusEffectContainer->GetHighestRuneEffect();
        int              runeBonus         = PAttacker->StatusEffectContainer->GetEffectsCount(highestRuneEffect);

        double min = 0.0;
        double max = 0.0;
        enspelldamagetailshelpers::EnspellRuneMinMax(runeDPS, runeBonus, min, max);

        if (max == 0.0)
        {
            damage = 0;
        }
        else
        {
            // see https://bugs.llvm.org/show_bug.cgi?id=18767#c1 ; essentially, [min, max] range on this RNG call excludes the max
            // so we must add +1 to our max to achieve the range we want
            // TODO: verify gaussian vs linear distribution for RNG from retail
            damage = (int32)xirand::GetRandomNumber<double>(min, max + 1);
        }
    }

    // --------------------------
    // Enspell % multiplier bucket
    // --------------------------

    // Total % mod on the attacker (armor + both weapons)
    int32 totalPctMod = PAttacker->getMod(Mod::ENSPELL_DMG_PCT);

    // Exclude the other weapon's % contribution, same pattern as flat +n above
    int32 excludePct = 0;
    int32 weaponPct  = 0;

    if (PChar)
    {
        // pWeaponHit is the weapon that procced this add-effect (hand-specific)
        if (pWeaponHit)
        {
            weaponPct = pWeaponHit->getModifier(Mod::ENSPELL_DMG_PCT);
        }

        constexpr SLOTTYPE slots[] = { SLOT_MAIN, SLOT_SUB };
        for (SLOTTYPE slot : slots)
        {
            if (auto* eq = PChar->getEquip(slot); eq && eq != pWeaponHit)
            {
                excludePct += eq->getModifier(Mod::ENSPELL_DMG_PCT);
            }
        }
    }

    const int32 pctApplicable = enspelldamagetailshelpers::EnspellPctApplicable(totalPctMod, excludePct);
    const int32 nonWeaponPct  = enspelldamagetailshelpers::EnspellNonWeaponPct(pctApplicable, weaponPct);
    const bool  composure     = enspelldamagetailshelpers::ShouldApplyComposureEnspellBonus(
        PChar != nullptr,
        PChar && PChar->GetMJob() == JOB_RDM,
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Composure),
        Tier,
        element);
    const float mult = enspelldamagetailshelpers::EnspellDamageMultiplier(nonWeaponPct, weaponPct, composure);
    damage           = enspelldamagetailshelpers::ApplyEnspellDamageMultiplier(damage, mult);

    // matching day 10% bonus, matching weather 10% or 25% for double weather
    uint32 WeekDay = static_cast<uint8>(vanadiel_time::get_weekday());
    auto   weather = GetWeather(PAttacker, false);

    DAYTYPE strongDay[8]           = { FIRESDAY, ICEDAY, WINDSDAY, EARTHSDAY, LIGHTNINGDAY, WATERSDAY, LIGHTSDAY, DARKSDAY };
    DAYTYPE weakDay[8]             = { WATERSDAY, FIRESDAY, ICEDAY, WINDSDAY, EARTHSDAY, LIGHTNINGDAY, DARKSDAY, LIGHTSDAY };
    Weather strongWeatherSingle[8] = { Weather::HotSpell, Weather::Snow, Weather::Wind, Weather::DustStorm, Weather::Thunder, Weather::Rain, Weather::Auroras, Weather::Gloom };
    Weather strongWeatherDouble[8] = { Weather::HeatWave, Weather::Blizzards, Weather::Gales, Weather::SandStorm, Weather::Thunderstorms, Weather::Squall, Weather::StellarGlare, Weather::Darkness };
    Weather weakWeatherSingle[8]   = { Weather::Rain, Weather::HotSpell, Weather::Snow, Weather::Wind, Weather::DustStorm, Weather::Thunder, Weather::Gloom, Weather::Auroras };
    Weather weakWeatherDouble[8]   = { Weather::Squall, Weather::HeatWave, Weather::Blizzards, Weather::Gales, Weather::SandStorm, Weather::Thunderstorms, Weather::Darkness, Weather::StellarGlare };
    uint32  obi[8]                 = { 15435, 15436, 15437, 15438, 15439, 15440, 15441, 15442 };
    Mod     resistarray[8]         = { Mod::FIRE_MEVA, Mod::ICE_MEVA, Mod::WIND_MEVA, Mod::EARTH_MEVA, Mod::THUNDER_MEVA, Mod::WATER_MEVA, Mod::LIGHT_MEVA, Mod::DARK_MEVA };
    bool    obiBonus               = false;

    const float resist = enspelldamagetailshelpers::EnspellResistLadder(
        PDefender->getMod(resistarray[element - 1]), xirand::GetRandomNumber(1.));

    if (PAttacker->objtype == TYPE_PC)
    {
        CItemEquipment* waist = ((CCharEntity*)PAttacker)->getEquip(SLOT_WAIST);
        if (waist && waist->getID() == obi[element - 1])
        {
            obiBonus = true;
        }
    }

    float       mobExtra     = 0.0f;
    std::int8_t dayArm       = 0;
    std::int8_t weatherArm   = 0;
    bool        dayChanceOK  = false;
    bool        weatherChanceOK = false;

    if (PAttacker->objtype != TYPE_PC)
    {
        // mobs random multiplier — roll preserved before day/weather chance rolls
        mobExtra = xirand::GetRandomNumber(100) / 1000.0f;
    }

    if (WeekDay == strongDay[element - 1])
    {
        dayArm      = 1;
        dayChanceOK = obiBonus || xirand::GetRandomNumber(100) < 33;
    }
    else if (WeekDay == weakDay[element - 1])
    {
        dayArm      = -1;
        dayChanceOK = obiBonus || xirand::GetRandomNumber(100) < 33;
    }

    if (weather == strongWeatherSingle[element - 1])
    {
        weatherArm      = 1;
        weatherChanceOK = obiBonus || xirand::GetRandomNumber(100) < 33;
    }
    else if (weather == strongWeatherDouble[element - 1])
    {
        weatherArm      = 2;
        weatherChanceOK = obiBonus || xirand::GetRandomNumber(100) < 33;
    }
    else if (weather == weakWeatherSingle[element - 1])
    {
        weatherArm      = -1;
        weatherChanceOK = obiBonus || xirand::GetRandomNumber(100) < 33;
    }
    else if (weather == weakWeatherDouble[element - 1])
    {
        weatherArm      = -2;
        weatherChanceOK = obiBonus || xirand::GetRandomNumber(100) < 33;
    }

    const float dBonus = enspelldamagetailshelpers::EnspellDayWeatherBonus(
        PAttacker->objtype == TYPE_PC, mobExtra, dayArm, weatherArm, dayChanceOK, weatherChanceOK);

    damage = enspelldamagetailshelpers::ApplyEnspellResistAndDayBonus(damage, resist, dBonus);
    damage = MagicDmgTaken(PDefender, damage, (ELEMENT)(element));

    if (damage > 0)
    {
        damage = std::max(damage - PDefender->getMod(Mod::PHALANX), 0);
        damage = HandleOneForAll(PDefender, damage);
        damage = HandleStoneskin(PDefender, damage);
    }

    damage = enspelldamagetailshelpers::ClampEnspellFinalDamage(damage);

    return damage;
}

/************************************************************************
 *                                                                       *
 *  Calculate Spike Damage                                               *
 *                                                                       *
 ************************************************************************/

auto CalculateSpikeDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, action_result_t* Action, uint16 damageTaken) -> int32
{
    auto       spikeElement = static_cast<ELEMENT>(static_cast<uint8>(GetSpikesDamageType(Action->spikesEffect)) - (uint8)xi::DamageType::Elemental);
    const bool elementOOR   = static_cast<uint8>(Action->spikesEffect) > static_cast<uint8>(ELEMENT::ELEMENT_DARK);
    const auto preMDT       = combatstatustailshelpers::CalculateSpikeDamagePreMDT(
        Action->spikesParam,
        PDefender->getMod(Mod::SPIKES_DMG_BONUS),
        static_cast<SPIKES>(Action->spikesEffect) == SPIKES::SPIKE_DREAD,
        damageTaken,
        elementOOR);

    int32 damage = preMDT.damage;
    if (preMDT.clampElementToFire)
    {
        ShowWarningFmt("CalculateSpikeDamage: Spike Element from PDefender ({}, id {}) out of range, got {}. Setting to Fire.", PDefender->getName(), PDefender->id, static_cast<int32>(spikeElement));
        spikeElement = ELEMENT::ELEMENT_FIRE;
    }

    damage = MagicDmgTaken(PAttacker, damage, spikeElement); // apply MDT/MDT2/DT, liement to whoever is taking damage

    if (combatstatustailshelpers::ShouldApplySpikeHealMessage(damage)) // apply heal message
    {
        Action->spikesMessage = MsgBasic::SpikesEffectHeal;
    }

    return damage;
}

auto HandleSpikesDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, action_result_t* Action, const int32 damage) -> bool
{
    Action->spikesEffect  = static_cast<ActionReactKind>(PDefender->getMod(Mod::SPIKES));
    Action->spikesMessage = MsgBasic::SpikesEffectDmg;
    Action->spikesParam   = std::max<int16>(PDefender->getMod(Mod::SPIKES_DMG), 0);

    // Preserve original && short-circuit: only roll hit-rate RNG when Retaliation is up and engaged.
    bool retaliationProc = false;
    if (PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Retaliation) && PDefender->PAI->IsEngaged())
    {
        retaliationProc = spikeshelpers::RetaliationProc(
            true,
            true,
            battleutils::GetHitRate(PDefender, PAttacker),
            xirand::GetRandomNumber(100),
            facing(PDefender->loc.p, PAttacker->loc.p, 64));
    }
    const auto spikesPath = spikeshelpers::ClassifySpikesPath(
        retaliationProc,
        Action->spikesEffect != ActionReactKind::None,
        PDefender->getMod(Mod::ITEM_SUBEFFECT) > 0);

    // Handle Retaliation
    if (spikesPath == spikeshelpers::SpikesPath::Retaliation)
    {
        // Retaliation rate is based on player acc vs mob evasion. Missed retaliations do not even display in log.
        // Other theories exist but were not proven or reliably tested (I have to assume too many things to even consider JP translations about weapon
        // delay), this at least has data to back it up.
        // https://web.archive.org/web/20141228105335/http://www.bluegartr.com/threads/120193-Retaliation-Testing?s=7a6221e10ffdfaa6a7f5e8f0387f787d&p=4620727&viewfull=1#post4620727
        Action->resolution   = ActionResolution::Hit;
        Action->spikesEffect = ActionReactKind::Counter;

        if (battleutils::IsAbsorbByShadow(PAttacker, PDefender)) // Struck a shadow
        {
            Action->spikesMessage = MsgBasic::RetaliateShadowAbsorbs;
        }
        else // Struck the target
        {
            SKILLTYPE skilltype = SKILLTYPE::SKILL_NONE;

            if (PDefender->objtype == TYPE_PC)
            {
                if (auto* weapon = dynamic_cast<CItemWeapon*>(PDefender->m_Weapons[SLOT_MAIN]))
                {
                    skilltype = static_cast<SKILLTYPE>(weapon->getSkillType());
                }
                else
                {
                    skilltype = SKILLTYPE::SKILL_HAND_TO_HAND;
                }

                // Check for skillup
                charutils::TrySkillUP(static_cast<CCharEntity*>(PDefender), skilltype, PAttacker->GetMLevel());
            }

            // Check if crit
            const bool crit = battleutils::GetCritHitRate(PDefender, PAttacker, true) > xirand::GetRandomNumber(100);

            // Dmg math.
            const float DamageRatio = GetDamageRatio(PDefender, PAttacker, crit, 1.0f, skilltype, SLOT_MAIN, false);
            uint16      dmg         = static_cast<uint32>((PDefender->GetMainWeaponDmg() + battleutils::GetFSTR(PDefender, PAttacker, SLOT_MAIN)) * DamageRatio);
            dmg                     = attackutils::CheckForDamageMultiplier(static_cast<CCharEntity*>(PDefender), dynamic_cast<CItemWeapon*>(PDefender->m_Weapons[SLOT_MAIN]), dmg, PHYSICAL_ATTACK_TYPE::NORMAL, SLOT_MAIN);
            dmg                     = spikeshelpers::RetaliationDamage(dmg, PDefender->getMod(Mod::RETALIATION));

            // TP and stoneskin are handled inside TakePhysicalDamage
            Action->spikesMessage = MsgBasic::RetaliateDamage;
            Action->spikesParam =
                battleutils::TakePhysicalDamage(PDefender, PAttacker, PHYSICAL_ATTACK_TYPE::NORMAL, dmg, false, SLOT_MAIN, 1, nullptr, true, true, true);
        }
    }

    // Handle spikes from spells or auto-spikes (scripted) effects
    else if (spikesPath == spikeshelpers::SpikesPath::SpellOrAuto)
    {
        const int16 autoSpikes = PDefender->objtype == TYPE_MOB ? static_cast<CMobEntity*>(PDefender)->getMobMod(MOBMOD_AUTO_SPIKES) : 0;
        // check if spikes are handled in mobs script
        if (spikeshelpers::ShouldCallOnSpikesDamage(PDefender->objtype == TYPE_MOB, autoSpikes))
        {
            luautils::OnSpikesDamage(PDefender, PAttacker, Action, Action->spikesParam);
        }

        // calculate damage
        int32 spikesDamage = CalculateSpikeDamage(PAttacker, PDefender, Action, static_cast<uint16>(abs(damage)));
        if (spikesDamage > 0)
        {
            spikesDamage = std::max(spikesDamage - PAttacker->getMod(Mod::PHALANX), 0);
            spikesDamage = HandleOneForAll(PAttacker, spikesDamage);
            spikesDamage = HandleStoneskin(PAttacker, spikesDamage);
        }

        Action->spikesParam = spikeshelpers::SpikesPacketParam(
            spikesDamage, PAttacker->GetMaxHP(), PAttacker->health.hp);

        if (spikeshelpers::ShouldRunSpellSpikeSwitch(PDefender->objtype == TYPE_MOB, autoSpikes))
        {
            switch (static_cast<SPIKES>(Action->spikesEffect))
            {
                case SPIKE_BLAZE:
                case SPIKE_ICE:
                case SPIKE_SHOCK:
                    PAttacker->takeDamage(spikesDamage, PDefender, ATTACK_TYPE::MAGICAL, GetSpikesDamageType(Action->spikesEffect));
                    break;

                case SPIKE_DREAD:
                    if (spikeshelpers::DreadSpikesOnUndeadNull(true, PAttacker->m_EcoSystem == xi::Ecosystem::Undead))
                    {
                        // is undead no effect
                        Action->spikesEffect = ActionReactKind::None;
                        return false;
                    }
                    else
                    {
                        if (PDefender->isAlive())
                        {
                            if (auto* PEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::DreadSpikes))
                            {
                                // see https://www.bg-wiki.com/ffxi/Dread_Spikes

                                // Subpower is the remaining damage that can be drained. When it reaches 0 the effect ends
                                const int remainingDrain = PEffect->GetSubPower();
                                if (remainingDrain - abs(damage) <= 0) // power absorbed from Dread Spikes takes pre-MDT etc values
                                {
                                    spikesDamage        = std::min(spikesDamage, remainingDrain);
                                    Action->spikesParam = static_cast<uint16>(spikesDamage);
                                    PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::DreadSpikes);
                                }
                                else
                                {
                                    PEffect->SetSubPower(remainingDrain - abs(damage));
                                }
                            }

                            if (spikesDamage > 0) // do not add HP if spikes damage was absorbed.
                            {
                                Action->spikesMessage = MsgBasic::SpikesEffectHPDrain;
                                PDefender->addHP(spikesDamage);
                            }
                        }
                        PAttacker->takeDamage(spikesDamage, PDefender, ATTACK_TYPE::MAGICAL, xi::DamageType::Dark);
                    }
                    break;

                case SPIKE_REPRISAL:
                    if (spikeshelpers::ReprisalApplies(true, Action->resolution == ActionResolution::Block))
                    {
                        PAttacker->takeDamage(spikesDamage, PDefender, ATTACK_TYPE::MAGICAL, xi::DamageType::Light);
                    }
                    else
                    {
                        // only works on shield blocks
                        Action->spikesEffect = ActionReactKind::None;
                        return false;
                    }
                    break;

                default:
                    break;
            }
        }

        // Check for status effect proc. Todo: move to scripts soon™ after item additionalEffect refactor Teo is working on
        HandleSpikesStatusEffect(PAttacker, PDefender, Action);

        battleutils::DirtyExp(PAttacker, PDefender);
        if (PAttacker->isDead())
        {
            battleutils::ClaimMob(PAttacker, PDefender);
        }
        return true;
    }

    // Deal with spikesEffect effect gear
    else if (spikesPath == spikeshelpers::SpikesPath::ItemGear)
    {
        if (CCharEntity* PCharDef = dynamic_cast<CCharEntity*>(PDefender))
        {
            for (auto&& slot : { SLOT_SUB, SLOT_BODY, SLOT_LEGS, SLOT_HEAD, SLOT_HANDS, SLOT_FEET })
            {
                CItemEquipment* PItem = PCharDef->getEquip(slot);
                if (PItem && !PItem->isType(ITEM_WEAPON))
                {
                    uint8 chance = 0;

                    Action->spikesEffect = ActionReactKind::None;
                    auto spikes_type     = battleutils::GetScaledItemModifier(PDefender, PItem, Mod::ITEM_SUBEFFECT);
                    if (spikeshelpers::ItemSubEffectIsSpikeType(spikes_type))
                    {
                        Action->spikesEffect = static_cast<ActionReactKind>(spikes_type);
                    }

                    Action->spikesParam = battleutils::GetScaledItemModifier(PDefender, PItem, Mod::ITEM_ADDEFFECT_DMG);
                    chance              = battleutils::GetScaledItemModifier(PDefender, PItem, Mod::ITEM_ADDEFFECT_CHANCE);

                    if (CMobEntity* PMobAtt = dynamic_cast<CMobEntity*>(PDefender))
                    {
                        PMobAtt->m_HiPCLvl = std::max(PMobAtt->m_HiPCLvl, PDefender->GetMLevel());
                    }

                    if (Action->spikesEffect != ActionReactKind::None && HandleSpikesEquip(PAttacker, PDefender, Action, static_cast<uint8>(Action->spikesParam), Action->spikesEffect, chance))
                    {
                        return true;
                    }
                }
            }
        }
    }
    else if (spikesPath == spikeshelpers::SpikesPath::ClearNone)
    {
        Action->spikesParam   = 0;
        Action->spikesMessage = MsgBasic::None;
    }
    return false;
}

auto HandleParrySpikesDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, action_result_t* Action, const int32 damage) -> bool
{
    Action->spikesEffect  = static_cast<ActionReactKind>(PDefender->getMod(Mod::PARRY_SPIKES));
    Action->spikesMessage = MsgBasic::SpikesEffectDmg;
    Action->spikesParam   = std::max<int16>(PDefender->getMod(Mod::PARRY_SPIKES_DMG), 0);

    if (spikeshelpers::ParrySpikesActive(Action->spikesEffect == ActionReactKind::None))
    {
        // calculate damage
        int32 spikesDamage = CalculateSpikeDamage(PAttacker, PDefender, Action, static_cast<uint16>(abs(damage)));
        if (spikesDamage > 0)
        {
            spikesDamage = std::max(spikesDamage - PAttacker->getMod(Mod::PHALANX), 0);
            spikesDamage = HandleOneForAll(PAttacker, spikesDamage);
            spikesDamage = HandleStoneskin(PAttacker, spikesDamage);
        }

        Action->spikesParam = spikeshelpers::SpikesPacketParam(
            spikesDamage, PAttacker->GetMaxHP(), PAttacker->health.hp);

        PAttacker->takeDamage(spikesDamage, PDefender, ATTACK_TYPE::MAGICAL, GetSpikesDamageType(Action->spikesEffect));

        battleutils::DirtyExp(PAttacker, PDefender);
        if (spikeshelpers::ShouldClaimOnSpikeKill(PAttacker->isDead()))
        {
            battleutils::ClaimMob(PAttacker, PDefender);
        }
        return true;
    }

    return false;
}

auto HandleSpikesEquip(CBattleEntity* PAttacker, CBattleEntity* PDefender, action_result_t* Action, const uint8 damage, const ActionReactKind spikesType, const uint8 chance) -> bool
{
    const int lvlDiff = spikeshelpers::SpikesEquipLevelDiff(PDefender->GetMLevel(), PAttacker->GetMLevel());

    if (spikeshelpers::SpikesEquipProcs(chance, lvlDiff, xirand::GetRandomNumber(100)))
    {
        if (spikesType == ActionReactKind::CurseSpikes)
        {
            Action->spikesMessage = MsgBasic::StatusSpikes;
            Action->spikesParam   = static_cast<uint16>(xi::StatusEffect::CurseI);
        }
        /* Todo: wire this up fully.
        else if (spikesType == SUBEFFECT_DEATH_SPIKES)
        {
            Action->spikesMessage = MsgBasic::StatusSpikes;
            Action->spikesParam   = xi::StatusEffect::Ko;
            PDefender->setHP(0);
        }
        */
        else
        {
            auto ratio = spikeshelpers::SpikesEquipRatio(damage);

            // calculate damage
            int32 spikesDamage = CalculateSpikeDamage(PAttacker, PDefender, Action, damage - xirand::GetRandomNumber<uint16>(ratio) + xirand::GetRandomNumber<uint16>(ratio));
            if (spikesDamage > 0)
            {
                spikesDamage = std::max(spikesDamage - PAttacker->getMod(Mod::PHALANX), 0);
                spikesDamage = HandleOneForAll(PAttacker, spikesDamage);
                spikesDamage = HandleStoneskin(PAttacker, spikesDamage);
            }
            // Note: original only wrote Action->spikesParam on <=0 path here; positive path left prior param.
            // Preserve that quirk for non-positive damage only.
            if (spikesDamage <= 0)
            {
                Action->spikesParam = spikeshelpers::SpikesPacketParam(
                    spikesDamage, PAttacker->GetMaxHP(), PAttacker->health.hp);
            }

            PAttacker->takeDamage(spikesDamage, PDefender, ATTACK_TYPE::MAGICAL, GetSpikesDamageType(spikesType));
        }

        // Temp till moved to script.
        HandleSpikesStatusEffect(PAttacker, PDefender, Action);

        return true;
    }
    else
    {
        // Technically, these should be the default values and then conditional branches change them
        // However, it wasn't worth the effort when the whole thing is going to be eventually burned down to make way for fully scripted spikes
        Action->spikesEffect  = ActionReactKind::None;
        Action->spikesParam   = 0;
        Action->spikesMessage = MsgBasic::None;
    }

    return false;
}

void HandleSpikesStatusEffect(const CBattleEntity* PAttacker, const CBattleEntity* PDefender, const action_result_t* Action)
{
    // Preserve original RNG: only roll for ice/shock arms.
    const bool hasCurseI    = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::CurseI);
    const bool hasParalysis = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Paralysis);
    const bool hasStun      = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Stun);

    int roll = 0;
    if (Action->spikesEffect == ActionReactKind::IceSpikes || Action->spikesEffect == ActionReactKind::ShockSpikes)
    {
        roll = xirand::GetRandomNumber(100);
    }

    const auto action = spikesstatushelpers::ResolveSpikesStatusEffect(
        static_cast<uint8>(Action->spikesEffect),
        PDefender != nullptr,
        PDefender ? PDefender->GetMLevel() : static_cast<uint8>(0),
        PAttacker->GetMLevel(),
        hasCurseI,
        hasParalysis,
        hasStun,
        roll);

    switch (action)
    {
        case spikesstatushelpers::SpikesStatusAction::ApplyCurse:
            PAttacker->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::CurseI, static_cast<uint16>(xi::StatusEffect::CurseI), 15, 0s, 3min);
            break;
        case spikesstatushelpers::SpikesStatusAction::ApplyParalysis:
            PAttacker->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Paralysis, static_cast<uint16>(xi::StatusEffect::Paralysis), 20, 0s, 30s);
            break;
        case spikesstatushelpers::SpikesStatusAction::ApplyStun:
            PAttacker->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Stun, static_cast<uint16>(xi::StatusEffect::Stun), 1, 0s, 3s);
            break;
        default:
            break;
    }
}

/************************************************************************
 *                                                                       *
 *  Handle Enspell effect and damage                                     *
 *                                                                       *
 ************************************************************************/

void HandleEnspell(CBattleEntity* PAttacker, CBattleEntity* PDefender, action_result_t* Action, bool isFirstSwing, CItemWeapon* weapon, int32 finaldamage, CAttack& attack)
{
    CCharEntity* PChar = nullptr;

    if (PAttacker->objtype == TYPE_PC)
    {
        PChar = dynamic_cast<CCharEntity*>(PAttacker);

        // TODO: cleanup lua further so we can handle all this add effect stuff somewhere else
        // Treasure hunter takes priority over enspells
        if (treasurehunterprochelpers::ShouldEvaluateTreasureHunterProc(
                PChar != nullptr,
                settings::get<bool>("map.DISABLE_TREASURE_HUNTER_PROCS"),
                finaldamage,
                isFirstSwing,
                PDefender->objtype == TYPE_MOB,
                PChar && PChar->GetMJob() == JOB_THF,
                PChar && PChar->hasTrait(TRAITTYPE::TRAIT_TREASURE_HUNTER)))
        {
            auto* PMob = dynamic_cast<CMobEntity*>(PDefender);
            if (PMob && treasurehunterprochelpers::CanUpgradeTreasureHunter(
                            static_cast<int16>(PMob->m_THLvl), PChar->getMod(Mod::TREASURE_HUNTER_CAP)))
            {
                // Roll only when upgrade path is open (matches original control flow).
                const auto thResult = treasurehunterprochelpers::ResolveTreasureHunterProc(
                    static_cast<int16>(PMob->m_THLvl),
                    PChar->getMod(Mod::TREASURE_HUNTER),
                    PChar->getMod(Mod::TREASURE_HUNTER_CAP),
                    PChar->getMod(Mod::TREASURE_HUNTER_PROC),
                    PMob->getMod(Mod::TREASURE_HUNTER_PROC),
                    attack.IsSneakAttack(),
                    attack.IsTrickAttack(),
                    xirand::GetRandomNumber<float>(0.0f, 1.0f));

                if (thResult.eligible)
                {
                    // Apply auto-upgrade before proc increment (mirrors production order).
                    if (thResult.autoUpgraded)
                    {
                        PMob->m_THLvl = thResult.newMobTH;
                    }
                    if (thResult.procced)
                    {
                        PMob->m_THLvl++;

                        Action->additionalEffect = ActionProcAddEffect::LightDamage; // Looks like enlight, and is reflected in the capture
                        Action->addEffectMessage = static_cast<MsgBasic>(MsgStd::TreasureHunterProc);
                        Action->addEffectParam   = PMob->m_THLvl;
                        return;
                    }
                }
            }
        }
    }

    Action->additionalEffect = ActionProcAddEffect::None;
    Action->addEffectMessage = MsgBasic::None;
    Action->addEffectParam   = 0;

    xi::StatusEffect previous_daze       = xi::StatusEffect::None;
    uint16           previous_daze_power = 0;

    const auto sambaDaze = enspellhandlehelpers::SelectPreviousDaze(
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::DrainSamba),
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AspirSamba),
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::HasteSamba),
        PDefender->m_EcoSystem == xi::Ecosystem::Undead);

    switch (sambaDaze)
    {
        case enspellhandlehelpers::SambaDaze::Drain:
            previous_daze       = xi::StatusEffect::DrainDaze;
            previous_daze_power = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::DrainSamba)->GetPower();
            break;
        case enspellhandlehelpers::SambaDaze::Aspir:
            previous_daze       = xi::StatusEffect::AspirDaze;
            previous_daze_power = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::AspirSamba)->GetPower();
            break;
        case enspellhandlehelpers::SambaDaze::Haste:
            previous_daze       = xi::StatusEffect::HasteDaze;
            previous_daze_power = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::HasteSamba)->GetPower();
            break;
        default:
            break;
    }

    if (previous_daze != xi::StatusEffect::None)
    {
        const auto clearPath = enspellhandlehelpers::ClassifyDazeClearPath(
            PAttacker->objtype == TYPE_PC,
            PAttacker->PParty != nullptr,
            PAttacker->objtype == TYPE_TRUST,
            PAttacker->PMaster != nullptr);

        if (clearPath == enspellhandlehelpers::DazeClearPath::Party)
        {
            for (auto* PMember : PAttacker->PParty->members)
            {
                PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::DrainDaze, PMember->id);
                PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::HasteDaze, PMember->id);
                PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::AspirDaze, PMember->id);
            }
        }
        else if (clearPath == enspellhandlehelpers::DazeClearPath::TrustMasterParty)
        {
            // clang-format off
                static_cast<CCharEntity*>(PAttacker->PMaster)->ForPartyWithTrusts(
                [&](CBattleEntity* PMember)
                {
                    PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::DrainDaze, PMember->id);
                    PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::HasteDaze, PMember->id);
                    PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::AspirDaze, PMember->id);
                });
            // clang-format on
        }
        else
        {
            PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::DrainDaze, PAttacker->id);
            PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::HasteDaze, PAttacker->id);
            PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::AspirDaze, PAttacker->id);
        }
        if (PDefender->objtype == TYPE_PC)
        {
            PDefender->StatusEffectContainer->AddStatusEffectSilent(previous_daze, 0, previous_daze_power, 0s, 10s, PAttacker->id);
        }
        else
        {
            if (previous_daze == xi::StatusEffect::DrainDaze && PDefender->m_EcoSystem != xi::Ecosystem::Undead)
            {
                PDefender->StatusEffectContainer->AddStatusEffectSilent(xi::StatusEffect::DrainDaze, 0, previous_daze_power, 0s, 10s, PAttacker->id);
            }
            else
            {
                PDefender->StatusEffectContainer->AddStatusEffectSilent(previous_daze, 0, previous_daze_power, 0s, 10s, PAttacker->id);
            }
        }
    }

    auto checkWeaponAdditionalEffect = [&]() -> bool
    {
        if (PAttacker->objtype == TYPE_PC)
        {
            bool hasGlobalAdditionalEffect     = battleutils::GetScaledItemModifier(PAttacker, weapon, Mod::ITEM_ADDEFFECT_TYPE) > 0;     // additional_effect.lua
            bool hasItemScriptAdditionalEffect = battleutils::GetScaledItemModifier(PAttacker, weapon, Mod::ITEM_ADDEFFECT_SCRIPTED) > 0; // scripts/items/{}.lua

            if (hasGlobalAdditionalEffect && hasItemScriptAdditionalEffect)
            {
                ShowErrorFmt("Item '{}' has misconfigured additional effect data with both item script and add effect global configured", weapon->getName());
            }

            if (hasGlobalAdditionalEffect && luautils::additionalEffectAttack(PAttacker, PDefender, weapon, Action, finaldamage) == 0 && Action->hasAdditionalEffect())
            {
                if (enspellhandlehelpers::ShouldRewriteAddEffectToRecoverHP(
                        static_cast<std::uint16_t>(Action->addEffectMessage), Action->addEffectParam))
                {
                    Action->addEffectMessage = MsgBasic::AddEffectRecoversHP;
                }
                return true;
            }

            if (hasItemScriptAdditionalEffect && luautils::OnItemAdditionalEffect(PAttacker, PDefender, weapon, Action, finaldamage) == 0 && Action->hasAdditionalEffect())
            {
                if (enspellhandlehelpers::ShouldRewriteAddEffectToRecoverHP(
                        static_cast<std::uint16_t>(Action->addEffectMessage), Action->addEffectParam))
                {
                    Action->addEffectMessage = MsgBasic::AddEffectRecoversHP;
                }
                return true;
            }
        }

        return false;
    };

    bool checkedPriorityWeaponAddEffect = false;

    // TODO: grip priority too?
    if (enspellhandlehelpers::WeaponAddEffectPriority(
            PAttacker->objtype == TYPE_PC,
            battleutils::GetScaledItemModifier(PAttacker, weapon, Mod::ITEM_ADDEFFECT_PRIORITY)))
    {
        if (checkWeaponAdditionalEffect())
        {
            return; // Lambda handled the function
        }

        checkedPriorityWeaponAddEffect = true;
    }

    {
        const auto enspell     = static_cast<uint8>(PAttacker->getMod(Mod::ENSPELL));
        const auto enspellChance = static_cast<uint8>(PAttacker->getMod(Mod::ENSPELL_CHANCE));
        const auto activeRunes = static_cast<int>(PAttacker->StatusEffectContainer->GetActiveRuneCount());
        // RNG order: roll only when enspell>0 and chance!=0 (matches short-circuit; advances RNG before runes short-circuit).
        const int  enspellRoll = (enspell > 0 && enspellChance != 0) ? xirand::GetRandomNumber(100) : 0;

        if (enspellhandlehelpers::EnspellShouldProc(enspell, enspellChance, enspellRoll, activeRunes))
        {
            static ActionProcAddEffect enspell_subeffects[8] = {
                ActionProcAddEffect::FireDamage,
                ActionProcAddEffect::IceDamage,
                ActionProcAddEffect::WindDamage,
                ActionProcAddEffect::EarthDamage,
                ActionProcAddEffect::LightningDamage,
                ActionProcAddEffect::WaterDamage,
                ActionProcAddEffect::LightDamage,
                ActionProcAddEffect::DarkDamage,
            };

            const auto path = enspellhandlehelpers::ClassifyEnspellPath(
                enspell,
                PDefender->m_EcoSystem == xi::Ecosystem::Undead,
                activeRunes > 0,
                isFirstSwing);

            if (path == enspellhandlehelpers::EnspellPath::BloodWeapon)
            {
                Action->additionalEffect = ActionProcAddEffect::HPDrain;
                Action->addEffectMessage = MsgBasic::AddEffectHPDrained;

                const auto jp = (PAttacker->objtype == TYPE_PC)
                                    ? static_cast<CCharEntity*>(PAttacker)->PJobPoints->GetJobPointValue(JP_BLOOD_WEAPON_EFFECT)
                                    : static_cast<std::uint8_t>(0);
                const int32 absorbed = enspellhandlehelpers::BloodWeaponAbsorbed(
                    Action->param, PAttacker->objtype == TYPE_PC, jp);

                Action->addEffectParam = PAttacker->addHP(absorbed);

                if (PChar != nullptr)
                {
                    PChar->updatemask |= UPDATE_HP;
                }
            }
            else if (path == enspellhandlehelpers::EnspellPath::Rune)
            {
                xi::StatusEffect highestRuneEffect = PAttacker->StatusEffectContainer->GetHighestRuneEffect();
                xi::StatusEffect newestRuneEffect  = PAttacker->StatusEffectContainer->GetNewestRuneEffect();
                int              highestRuneCount  = PAttacker->StatusEffectContainer->GetEffectsCount(highestRuneEffect);

                xi::DamageType damageType = xi::DamageType::None;
                int            element    = 0;

                if (enspellhandlehelpers::RuneUsesNewest(highestRuneCount))
                {
                    element                  = GetRuneEnhancementElement(newestRuneEffect);
                    Action->additionalEffect = enspell_subeffects[static_cast<uint16>(newestRuneEffect) - static_cast<uint16>(xi::StatusEffect::Ignis)];
                    damageType               = GetRuneEnhancementDamageType(newestRuneEffect);
                }
                else
                {
                    element                  = GetRuneEnhancementElement(highestRuneEffect);
                    Action->additionalEffect = enspell_subeffects[static_cast<uint16>(highestRuneEffect) - static_cast<uint16>(xi::StatusEffect::Ignis)];
                    damageType               = GetRuneEnhancementDamageType(highestRuneEffect);
                }

                Action->addEffectParam = CalculateEnspellDamage(PAttacker, PDefender, 4, element, weapon);

                {
                    std::int32_t  normParam = 0;
                    std::uint16_t normMsg   = 0;
                    enspellhandlehelpers::NormalizeEnspellDamageParam(Action->addEffectParam, normParam, normMsg);
                    Action->addEffectParam   = normParam;
                    Action->addEffectMessage = static_cast<MsgBasic>(normMsg);
                }

                PDefender->takeDamage(Action->addEffectParam, PAttacker, ATTACK_TYPE::MAGICAL, damageType);
            }
            else if (path == enspellhandlehelpers::EnspellPath::Auspice)
            {
                std::uint8_t tier = 0;
                std::uint8_t el   = 0;
                enspellhandlehelpers::ElementalEnspellTierAndElement(enspell, path, tier, el);

                Action->additionalEffect = ActionProcAddEffect::LightDamage;
                Action->addEffectMessage = MsgBasic::AddEffectAdditionalDamage;
                Action->addEffectParam   = CalculateEnspellDamage(PAttacker, PDefender, tier, el, weapon);

                if (Action->addEffectParam < 0)
                {
                    std::int32_t  normParam = 0;
                    std::uint16_t normMsg   = 0;
                    enspellhandlehelpers::NormalizeEnspellDamageParam(Action->addEffectParam, normParam, normMsg);
                    Action->addEffectParam   = normParam;
                    Action->addEffectMessage = static_cast<MsgBasic>(normMsg);
                }

                PDefender->takeDamage(Action->addEffectParam, PAttacker, ATTACK_TYPE::MAGICAL, GetEnspellDamageType((ENSPELL)enspell));
            }
            else if (path == enspellhandlehelpers::EnspellPath::ElementalII || path == enspellhandlehelpers::EnspellPath::ElementalI)
            {
                std::uint8_t tier = 0;
                std::uint8_t el   = 0;
                enspellhandlehelpers::ElementalEnspellTierAndElement(enspell, path, tier, el);

                if (path == enspellhandlehelpers::EnspellPath::ElementalII)
                {
                    Action->additionalEffect = static_cast<ActionProcAddEffect>(
                        enspellhandlehelpers::ElementalEnspellSubEffect(enspell, true));
                }
                else
                {
                    Action->additionalEffect = static_cast<ActionProcAddEffect>(
                        enspellhandlehelpers::ElementalEnspellSubEffect(enspell, false));
                }
                Action->addEffectParam = CalculateEnspellDamage(PAttacker, PDefender, tier, el, weapon);

                if (Action->hasAdditionalEffect())
                {
                    {
                        std::int32_t  normParam = 0;
                        std::uint16_t normMsg   = 0;
                        enspellhandlehelpers::NormalizeEnspellDamageParam(Action->addEffectParam, normParam, normMsg);
                        Action->addEffectParam   = normParam;
                        Action->addEffectMessage = static_cast<MsgBasic>(normMsg);
                    }

                    PDefender->takeDamage(Action->addEffectParam, PAttacker, ATTACK_TYPE::MAGICAL, GetEnspellDamageType((ENSPELL)enspell));
                }
            }
            // ElementalIISkip / None: no effect this hit (matches original fall-through).
        }
        // check weapon for additional effects only if priority hasn't been checked already
        else if (!checkedPriorityWeaponAddEffect && checkWeaponAdditionalEffect())
        {
            return; // Lambda handled the function
        }
        // check script for grip if main failed
        else if ([&]() -> bool {
                     if (PAttacker->objtype != TYPE_PC)
                     {
                         return false;
                     }
                     auto* PSub = static_cast<CCharEntity*>(PAttacker)->getEquip(SLOT_SUB);
                     if (PSub == nullptr || weapon != PAttacker->m_Weapons[SLOT_MAIN])
                     {
                         return false;
                     }
                     auto* PSubWeapon = static_cast<CItemWeapon*>(PSub);
                     return enspellhandlehelpers::GripAddEffectGate(
                                true,
                                true,
                                PSubWeapon->getSkillType() == SKILL_NONE,
                                battleutils::GetScaledItemModifier(PAttacker, PSub, Mod::ITEM_ADDEFFECT_TYPE) > 0) &&
                            luautils::additionalEffectAttack(PAttacker, PDefender, PSubWeapon, Action, finaldamage) == 0 &&
                            Action->hasAdditionalEffect();
                 }())
        {
            if (enspellhandlehelpers::ShouldRewriteAddEffectToRecoverHP(
                    static_cast<std::uint16_t>(Action->addEffectMessage), Action->addEffectParam))
            {
                Action->addEffectMessage = MsgBasic::AddEffectRecoversHP;
            }
        }
        else if (enspellhandlehelpers::MobPetAddEffectGate(
                     PAttacker->objtype == TYPE_MOB || PAttacker->objtype == TYPE_PET,
                     (PAttacker->objtype == TYPE_MOB || PAttacker->objtype == TYPE_PET)
                         ? static_cast<CMobEntity*>(PAttacker)->getMobMod(MOBMOD_ADD_EFFECT)
                         : static_cast<std::int16_t>(0)))
        {
            luautils::OnAdditionalEffect(PAttacker, PDefender, Action, finaldamage);
            if (enspellhandlehelpers::ShouldRewriteAddEffectToRecoverHP(
                    static_cast<std::uint16_t>(Action->addEffectMessage), Action->addEffectParam))
            {
                Action->addEffectMessage = MsgBasic::AddEffectRecoversHP;
            }
        }
        else
        {
            bool hasDrainDaze = PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::DrainDaze);
            bool hasAspirDaze = PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AspirDaze);
            bool hasHasteDaze = PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::HasteDaze);

            if (hasDrainDaze || hasAspirDaze || hasHasteDaze)
            {
                int32 delay = PAttacker->GetWeaponDelay(false) / 10; // TODO: What kind of value did this ACTUALLY expect?

                xi::StatusEffect daze       = xi::StatusEffect::None;
                uint32           attackerID = 0;
                uint16           power      = 0;

                if (hasDrainDaze)
                {
                    daze = xi::StatusEffect::DrainDaze;
                }
                else if (hasAspirDaze)
                {
                    daze = xi::StatusEffect::AspirDaze;
                }
                else if (hasHasteDaze)
                {
                    daze = xi::StatusEffect::HasteDaze;
                }

                attackerID = PDefender->StatusEffectContainer->GetStatusEffect(daze)->GetSubID();

                if (PAttacker->objtype == TYPE_PC && PAttacker->PParty != nullptr)
                {
                    if (PChar)
                    {
                        // clang-format off
                        PChar->ForPartyWithTrusts([&](CBattleEntity* PMember)
                        {
                            if (attackerID == PMember->id)
                            {
                                power = enspellhandlehelpers::DazePowerFromMembership(
                                    true, PDefender->StatusEffectContainer->GetStatusEffect(daze)->GetPower());
                            }
                        });
                        // clang-format on
                    }
                }
                else if (PAttacker->objtype == TYPE_TRUST)
                {
                    if (auto* PMaster = dynamic_cast<CCharEntity*>(PAttacker->PMaster))
                    {
                        // clang-format off
                        PMaster->ForPartyWithTrusts([&](CBattleEntity* PMember)
                        {
                            if (attackerID == PMember->id)
                            {
                                power = enspellhandlehelpers::DazePowerFromMembership(
                                    true, PDefender->StatusEffectContainer->GetStatusEffect(daze)->GetPower());
                            }
                        });
                        // clang-format on
                    }
                }
                else if (PAttacker->PMaster == nullptr)
                {
                    power = enspellhandlehelpers::DazePowerFromMembership(
                        attackerID == PAttacker->id,
                        PDefender->StatusEffectContainer->GetStatusEffect(daze)->GetPower());
                }

                const auto drainDaze  = (daze == xi::StatusEffect::DrainDaze) ? enspellhandlehelpers::SambaDaze::Drain : enspellhandlehelpers::SambaDaze::None;
                const auto aspirDaze  = (daze == xi::StatusEffect::AspirDaze) ? enspellhandlehelpers::SambaDaze::Aspir : enspellhandlehelpers::SambaDaze::None;
                const auto hasteDazeK = (daze == xi::StatusEffect::HasteDaze) ? enspellhandlehelpers::SambaDaze::Haste : enspellhandlehelpers::SambaDaze::None;

                if (enspellhandlehelpers::ShouldApplyDrainSamba(drainDaze, power))
                {
                    const int upper = enspellhandlehelpers::DrainSambaRollUpper(power, delay);
                    const int8 roll = xirand::GetRandomNumber(1, upper);
                    const int8 lvlDiff = static_cast<int8>(PDefender->GetMLevel() - PAttacker->GetMLevel());
                    const int8 Samba = enspellhandlehelpers::DrainSambaAmount(power, delay, roll, lvlDiff, finaldamage);

                    Action->additionalEffect = ActionProcAddEffect::HPDrain;
                    Action->addEffectMessage = MsgBasic::AddEffectHPDrained;
                    Action->addEffectParam   = Samba;

                    PAttacker->addHP(Samba); // does not do any additional damage to targets HP, only heals the attacker

                    if (PChar != nullptr)
                    {
                        PChar->updatemask |= UPDATE_HP;
                    }
                }
                else if (enspellhandlehelpers::ShouldApplyAspirSamba(aspirDaze, power, PDefender->GetMaxMP()))
                {
                    const int upper = enspellhandlehelpers::AspirSambaRollUpper(power, delay);
                    const int8 roll = xirand::GetRandomNumber(1, upper);
                    const int8 Samba = enspellhandlehelpers::AspirSambaAmount(power, delay, roll, finaldamage);

                    Action->additionalEffect = ActionProcAddEffect::MPDrain;
                    Action->addEffectMessage = MsgBasic::AddEffectMPDrained;

                    int16 mpDrained = PDefender->addMP(-Samba);

                    PAttacker->addMP(mpDrained);
                    Action->addEffectParam = mpDrained;

                    if (PChar != nullptr)
                    {
                        PChar->updatemask |= UPDATE_HP;
                    }
                }
                else if (enspellhandlehelpers::ShouldApplyHasteSamba(hasteDazeK, power))
                {
                    Action->additionalEffect = ActionProcAddEffect::Haste;
                    // Ability haste added in scripts\globals\effects\haste_samba_haste_effect.lua
                    PAttacker->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::HasteSambaHaste, 0, power, 0s, 10s);
                    // Status effect removed in CAttackRound constructor (i.e. after next attack round is calculated)
                }
            }
        }
    }
}

namespace
{
// Shared injects for getHitRateModifiers (melee or ranged).
auto BuildHitRateModParams(CBattleEntity* PAttacker, CBattleEntity* PDefender, const bool isRanged, const bool isWeaponskill)
    -> physicalhitratehelpers::HitRateModParams
{
    using namespace physicalhitratehelpers;
    HitRateModParams m{};
    m.isRanged      = isRanged;
    m.isWeaponskill = isWeaponskill;

    m.isBehind23       = behind(PAttacker->loc.p, PDefender->loc.p, BehindAngle);
    m.isFacing64       = facing(PAttacker->loc.p, PDefender->loc.p, FacingConeYonin);
    m.attackerIsFacing = facing(PAttacker->loc.p, PDefender->loc.p, FacingDefault);
    m.targetIsFacing   = facing(PDefender->loc.p, PAttacker->loc.p, FacingDefault);
    m.attackerIsPC     = PAttacker->objtype == TYPE_PC;
    m.targetIsPC       = PDefender->objtype == TYPE_PC;

    if (!isRanged)
    {
        if (auto* flourish = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::BuildingFlourish))
        {
            m.hasBuildingFlourish      = true;
            m.buildingFlourishPower    = flourish->GetPower();
            m.buildingFlourishSubPower = flourish->GetSubPower();
        }
        if (auto* innin = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Innin))
        {
            m.hasInnin   = true;
            m.inninPower = innin->GetPower();
        }
        if (m.attackerIsPC)
        {
            auto* PChar = static_cast<CCharEntity*>(PAttacker);
            m.attackerClosedPositionMerit = PChar->PMeritPoints->GetMeritValue(MERIT_CLOSED_POSITION, PChar);
        }
    }

    // Ambush trait (melee + ranged).
    if (m.attackerIsPC)
    {
        auto* PChar = static_cast<CCharEntity*>(PAttacker);
        m.hasAmbushTrait = charutils::hasTrait(PChar, TRAIT_AMBUSH);
        if (m.hasAmbushTrait)
        {
            m.ambushMerit = PChar->PMeritPoints->GetMeritValue(MERIT_AMBUSH, PChar);
        }
    }

    if (auto* yonin = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Yonin))
    {
        m.hasYonin   = true;
        m.yoninPower = yonin->GetPower();
    }

    // LSB reads Innin/Yonin JP levels from the *target*.
    if (m.targetIsPC)
    {
        auto* PTargetChar = static_cast<CCharEntity*>(PDefender);
        m.inninJP         = PTargetChar->PJobPoints->GetJobPointValue(JP_INNIN_EFFECT);
        m.yoninJP         = PTargetChar->PJobPoints->GetJobPointValue(JP_YONIN_EFFECT);
        m.targetClosedPositionMerit = PTargetChar->PMeritPoints->GetMeritValue(MERIT_CLOSED_POSITION, PTargetChar);
    }

    if (auto* flash = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Flash))
    {
        // Mirror CLuaStatusEffect::getTimeRemaining (milliseconds).
        std::int64_t remainingMs = 0;
        if (flash->GetDuration() > 0s)
        {
            const auto duration = flash->GetStartTime() - timer::now() + flash->GetDuration();
            remainingMs         = std::max<std::int64_t>(timer::count_milliseconds(duration), 0);
        }
        m.flashPenalty = FlashPenalty(remainingMs);
    }

    return m;
}

auto IsUsingH2H(CBattleEntity* PEntity) -> bool
{
    auto* weapon = dynamic_cast<CItemWeapon*>(PEntity->m_Weapons[SLOT_MAIN]);
    if (PEntity->objtype == TYPE_PC)
    {
        if (weapon == nullptr)
        {
            return true; // bare handed
        }
        return weapon->getSkillType() == SKILLTYPE::SKILL_HAND_TO_HAND;
    }
    return weapon != nullptr && weapon->getSkillType() == SKILLTYPE::SKILL_HAND_TO_HAND;
}

auto IsWeaponTwoHanded(CBattleEntity* PEntity) -> bool
{
    auto* weapon = dynamic_cast<CItemWeapon*>(PEntity->m_Weapons[SLOT_MAIN]);
    return weapon != nullptr && weapon->isTwoHanded();
}

auto IsAvatar(CBattleEntity* PEntity) -> bool
{
    return PEntity->objtype == TYPE_PET &&
           static_cast<CPetEntity*>(PEntity)->getPetType() == PET_TYPE::AVATAR;
}

auto ApplyLevelCorrection(CBattleEntity* PAttacker) -> bool
{
    const bool useAdoulin = settings::get<bool>("main.USE_ADOULIN_WEAPON_SKILL_CHANGES");
    return levelcorrectionhelpers::IsLevelCorrectedZone(useAdoulin, PAttacker->getZone());
}

auto ResolveRangedSweetSpotEnd(CBattleEntity* PAttacker) -> double
{
    using namespace physicalhitratehelpers;
    auto* weapon = dynamic_cast<CItemWeapon*>(PAttacker->m_Weapons[SLOT_RANGED]);
    if (weapon == nullptr)
    {
        return ResolveSweetSpot(false, 0, 0, 0).end;
    }
    return ResolveSweetSpot(true, weapon->getID(), weapon->getSkillType(), weapon->getSubSkillType()).end;
}
} // namespace

uint8 GetRangedHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender, bool isBarrage, int16 accBonus)
{
    // isBarrage is unused by production Lua (parity).
    (void)isBarrage;

    using namespace physicalhitratehelpers;

    const double dist = distance(PAttacker->loc.p, PDefender->loc.p);
    if (dist > MaxRangedDistance)
    {
        return 0;
    }

    const auto mods = HitRateModifiers(BuildHitRateModParams(PAttacker, PDefender, true, false));

    const bool isPC = PAttacker->objtype == TYPE_PC;
    const auto sweetEnd = ResolveRangedSweetSpotEnd(PAttacker);
    const int  distPenalty = AccuracyDistancePenalty(
        isPC,
        dist,
        sweetEnd,
        static_cast<double>(PDefender->modelSize),
        static_cast<double>(PAttacker->modelSize),
        PAttacker->GetMLevel());

    RangedHitRateParams p{};
    p.acc                    = PAttacker->RACC(0);
    p.eva                    = PDefender->EVA();
    p.bonus                  = accBonus;
    p.accBonus               = mods.accBonus;
    p.evaBonus               = mods.evaBonus;
    p.distancePenalty        = distPenalty;
    p.distance               = dist;
    p.applyLevelCorrection   = ApplyLevelCorrection(PAttacker);
    p.attackerLevel          = PAttacker->GetMLevel();
    p.defenderLevel          = PDefender->GetMLevel();
    p.attackerIsPC           = isPC;
    p.attackerIsAvatar       = IsAvatar(PAttacker);

    return HitRateToPercent(RangedHitRate(p));
}

uint8 GetRangedHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender, bool isBarrage)
{
    return GetRangedHitRate(PAttacker, PDefender, isBarrage, 0);
}

float GetRangedDamageRatio(CBattleEntity* PAttacker, CBattleEntity* PDefender, bool isCritical, int16 bonusRangedAttack)
{
    using namespace pdifhelpers;

    auto* targ_weapon = dynamic_cast<CItemWeapon*>(PAttacker->m_Weapons[SLOT_RANGED]);
    if (!targ_weapon)
    {
        // No ranged weapon, check ammo slot for throwing
        targ_weapon = dynamic_cast<CItemWeapon*>(PAttacker->m_Weapons[SLOT_AMMO]);

        if (!targ_weapon)
        {
            ShowError("battleutils::GetRangedDamageRatio(): No ranged weapon or ammo");
            return 1.0f;
        }
    }

    const std::uint8_t weaponType = targ_weapon->getSkillType();
    const bool         isPC      = PAttacker->objtype == TYPE_PC;
    bool applyLC = ApplyLevelCorrection(PAttacker);
    if (isPC && PAttacker->getMod(Mod::RA_IGNORE_LVL_DIFF) > 0)
    {
        applyLC = false;
    }

    // Distance attack penalty (PC only in pure helper; Lua gates non-PC via isPC).
    int distancePenalty = 0;
    if (isPC)
    {
        const double dist     = distance(PAttacker->loc.p, PDefender->loc.p);
        const auto   sweet    = physicalhitratehelpers::ResolveSweetSpot(
            true, targ_weapon->getID(), weaponType, targ_weapon->getSubSkillType());
        // cSkillMax = getMaxSkillLevel(mainLvl, WAR, EVASION)
        const int cSkillMax = GetMaxSkill(SKILL_EVASION, JOB_WAR, PAttacker->GetMLevel());
        distancePenalty     = physicalhitratehelpers::AttackDistancePenalty(
            true,
            dist,
            sweet.start,
            sweet.end,
            static_cast<double>(PDefender->modelSize),
            static_cast<double>(PAttacker->modelSize),
            cSkillMax);
    }

    const double flourishBonus = 1.0; // isWeaponskill=false for this call site
    const double actorAttack   = std::max(1.0, std::floor((static_cast<double>(PAttacker->RATT(0)) + bonusRangedAttack - distancePenalty) * 1.0 * flourishBonus));
    const double targetDefense = EffectiveDefense(static_cast<double>(PDefender->DEF()), false, 0.0);

    double lower = 0.0;
    double upper = 0.0;
    RangedBounds(actorAttack, targetDefense, applyLC, isPC, PAttacker->GetMLevel(), PDefender->GetMLevel(), WeaponCap(weaponType), PAttacker->getMod(Mod::DAMAGE_LIMIT), PAttacker->getMod(Mod::DAMAGE_LIMITP), lower, upper);

    int lo = 0;
    int hi = 0;
    double ratio = 0.0;
    if (RatioRollRange(lower, upper, lo, hi))
    {
        ratio = static_cast<double>(xirand::GetRandomNumber(lo, hi + 1)) / 1000.0;
    }

    const int critInc = PAttacker->getMod(Mod::CRIT_DMG_INCREASE) + PAttacker->getMod(Mod::RANGED_CRIT_DMG_INCREASE);
    const int critDef = PDefender->getMod(Mod::CRIT_DEF_BONUS);
    const double pDif = FinishRangedPDIF(ratio, isCritical, critInc, critDef);
    return static_cast<float>(std::max(pDif, 0.0));
}

int16 CalculateBaseTP(CBattleEntity* PEntity, int32 delay)
{
    // Mirrors xi.combat.tp.calculateTPReturn entity classification + delay bands.
    bool isMob          = false;
    bool isCharmedPCPet = false;
    if (PEntity != nullptr)
    {
        isMob = PEntity->objtype == TYPE_MOB;
        if (isMob)
        {
            const bool hasPCMaster = PEntity->PMaster != nullptr && PEntity->PMaster->objtype == TYPE_PC;
            isCharmedPCPet         = tpreturnhelpers::IsCharmedPCPet(true, PEntity->isCharmed, hasPCMaster);
        }
    }

    const bool usePC = tpreturnhelpers::UsePCOrPetTPFormula(isMob, isCharmedPCPet);
    return tpreturnhelpers::CalculateTPReturn(usePC, delay);
}

auto GetBaseDelay(CBattleEntity* PEntity) -> uint16
{
    basedelayhelpers::MeleeBaseDelayParams p{};
    auto* PCharEntity = dynamic_cast<CCharEntity*>(PEntity);
    auto* PMobEntity  = dynamic_cast<CMobEntity*>(PEntity);
    p.isPC = PCharEntity != nullptr;
    p.isMob = PMobEntity != nullptr;

    if (PCharEntity)
    {
        auto* PMainWeapon = dynamic_cast<CItemWeapon*>(PCharEntity->getEquip(SLOT_MAIN));
        auto* PSubWeapon  = dynamic_cast<CItemWeapon*>(PCharEntity->getEquip(SLOT_SUB));
        if (PMainWeapon)
        {
            p.hasMainWeapon = true;
            p.mainDelay     = PMainWeapon->getBaseDelay();
            p.isH2H         = PMainWeapon->getSkillType() == SKILLTYPE::SKILL_HAND_TO_HAND;
            if (PSubWeapon)
            {
                p.hasSubWeapon = true;
                p.subDelay     = PSubWeapon->getBaseDelay();
            }
        }
    }
    else if (PMobEntity)
    {
        auto* PWeapon = dynamic_cast<CItemWeapon*>(PMobEntity->m_Weapons[SLOT_MAIN]);
        if (PWeapon)
        {
            p.hasMainWeapon = true;
            p.mainDelay     = PWeapon->getBaseDelay();
        }
    }

    return basedelayhelpers::GetBaseDelay(p);
}

auto GetBaseRangedDelay(CBattleEntity* PEntity) -> uint16
{
    basedelayhelpers::RangedBaseDelayParams p{};
    auto* PCharEntity = dynamic_cast<CCharEntity*>(PEntity);
    auto* PMobEntity  = dynamic_cast<CMobEntity*>(PEntity);
    p.isPC  = PCharEntity != nullptr;
    p.isMob = PMobEntity != nullptr;

    if (PCharEntity)
    {
        auto* PRangedWeapon = dynamic_cast<CItemWeapon*>(PCharEntity->getEquip(SLOT_RANGED));
        auto* PAmmo         = dynamic_cast<CItemWeapon*>(PCharEntity->getEquip(SLOT_AMMO));
        if (PRangedWeapon)
        {
            p.hasRanged        = true;
            p.rangedDelay      = PRangedWeapon->getBaseDelay();
            p.rangedIsRanged   = PRangedWeapon->isRanged();
            p.rangedIsThrowing = PRangedWeapon->isThrowing();
        }
        if (PAmmo)
        {
            p.hasAmmo      = true;
            p.ammoDelay    = PAmmo->getBaseDelay();
            p.ammoIsRanged = PAmmo->isRanged();
        }
    }
    else if (PMobEntity)
    {
        auto* PWeapon = dynamic_cast<CItemWeapon*>(PMobEntity->m_Weapons[SLOT_MAIN]);
        if (PWeapon)
        {
            p.hasMobMain   = true;
            p.mobMainDelay = PWeapon->getBaseDelay();
        }
    }

    return basedelayhelpers::GetBaseRangedDelay(p);
}


namespace
{
auto BattleEntityUsesPCOrPetTPFormula(CBattleEntity* PEntity) -> bool
{
    if (PEntity == nullptr)
    {
        return true;
    }
    if (PEntity->objtype != TYPE_MOB)
    {
        return true;
    }
    const bool hasPCMaster = PEntity->PMaster != nullptr && PEntity->PMaster->objtype == TYPE_PC;
    return tpreturnhelpers::IsCharmedPCPet(true, PEntity->isCharmed, hasPCMaster);
}

auto BattleEntityIsUsingH2H(CBattleEntity* PEntity) -> bool
{
    if (auto* PChar = dynamic_cast<CCharEntity*>(PEntity))
    {
        auto* PMain = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_MAIN));
        if (PMain)
        {
            return PMain->getSkillType() == SKILLTYPE::SKILL_HAND_TO_HAND;
        }
        return true; // bare handed
    }
    if (auto* PMob = dynamic_cast<CMobEntity*>(PEntity))
    {
        auto* PWeapon = dynamic_cast<CItemWeapon*>(PMob->m_Weapons[SLOT_MAIN]);
        return PWeapon != nullptr && PWeapon->getSkillType() == SKILLTYPE::SKILL_HAND_TO_HAND;
    }
    if (auto* PPet = dynamic_cast<CPetEntity*>(PEntity))
    {
        auto* PWeapon = dynamic_cast<CItemWeapon*>(PPet->m_Weapons[SLOT_MAIN]);
        return PWeapon != nullptr && PWeapon->getSkillType() == SKILLTYPE::SKILL_HAND_TO_HAND;
    }
    return false;
}

auto BuildModifiedDelayParams(CBattleEntity* PEntity, const std::int32_t delay) -> tpfromdamagehelpers::ModifiedDelayParams
{
    tpfromdamagehelpers::ModifiedDelayParams p{};
    p.delay     = delay;
    p.dualWield = PEntity->IsDualWielding();
    p.dualWieldMod = PEntity->getMod(Mod::DUAL_WIELD);
    p.usingH2H  = BattleEntityIsUsingH2H(PEntity);
    p.martialArtsMod = PEntity->getMod(Mod::MARTIAL_ARTS);
    p.delayP    = PEntity->getMod(Mod::DELAYP);

    if (PEntity->objtype == TYPE_PC)
    {
        p.actor = tpfromdamagehelpers::ModifiedDelayActor::PC;
        auto* PChar = static_cast<CCharEntity*>(PEntity);
        p.subEquipped = PChar->getEquip(SLOT_SUB) != nullptr;
        p.h2hSkillRankZero = PChar->RealSkills.rank[SKILL_HAND_TO_HAND] == 0;
    }
    else if (PEntity->objtype == TYPE_MOB)
    {
        p.actor = tpfromdamagehelpers::ModifiedDelayActor::Mob;
    }
    else
    {
        p.actor = tpfromdamagehelpers::ModifiedDelayActor::Other;
    }
    return p;
}
} // namespace

auto CalculateTPFromDamageDealt(CBattleEntity* PAttacker, const bool& isZanshin, const SLOTTYPE& slot) -> int32
{
    if (PAttacker == nullptr)
    {
        ShowWarning("battleutils::CalculateTPFromDamageDealt() - PAttacker was null.");
        return 0;
    }

    const bool hasMeikyo = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MeikyoShisui);
    const bool usePC     = BattleEntityUsesPCOrPetTPFormula(PAttacker);
    const auto storeTP   = PAttacker->getMod(Mod::STORETP);

    if (slot == SLOT_RANGED || slot == SLOT_AMMO)
    {
        const auto baseRanged = static_cast<std::int32_t>(GetBaseRangedDelay(PAttacker));
        return tpfromdamagehelpers::SingleRangedHitTPReturn(hasMeikyo, usePC, baseRanged, storeTP);
    }

    const auto baseDelay = static_cast<std::int32_t>(GetBaseDelay(PAttacker));
    const auto modParams = BuildModifiedDelayParams(PAttacker, baseDelay);
    const auto modOut    = tpfromdamagehelpers::GetModifiedDelayAndCanZanshin(modParams);

    std::int32_t ikishoten = 0;
    if (PAttacker->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(PAttacker);
        ikishoten   = PChar->PMeritPoints->GetMeritValue(MERIT_IKISHOTEN, PChar);
    }

    return tpfromdamagehelpers::SingleMeleeHitTPReturn(
        hasMeikyo, isZanshin, usePC, modOut.delay, modOut.canZanshin, ikishoten, storeTP);
}

auto CalculateTPFromDamageTaken(CBattleEntity* PAttacker, CBattleEntity* PDefender, int32 damage, uint16 delay) -> int32
{
    if (PAttacker == nullptr || PDefender == nullptr)
    {
        ShowWarning("battleutils::CalculateTPFromDamageTaken() - PAttacker or PDefender was null.");
        return 0;
    }

    if (tpfromdamagehelpers::ShouldZeroPhysicalTPGain(
            false, damage, PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MeikyoShisui)))
    {
        return 0;
    }

    const auto modParams = BuildModifiedDelayParams(PAttacker, static_cast<std::int32_t>(delay));
    const auto modOut    = tpfromdamagehelpers::GetModifiedDelayAndCanZanshin(modParams);
    const bool usePC     = BattleEntityUsesPCOrPetTPFormula(PAttacker);
    const auto baseTP    = static_cast<std::int32_t>(tpreturnhelpers::CalculateTPReturn(usePC, modOut.delay));

    std::int32_t subtleMerit = 0;
    if (PAttacker->objtype == TYPE_PC)
    {
        auto* PChar  = static_cast<CCharEntity*>(PAttacker);
        subtleMerit  = PChar->PMeritPoints->GetMeritValue(MERIT_SUBTLE_BLOW_EFFECT, PChar);
    }

    const bool tandemActive = petutils::IsTandemActive(PAttacker);
    std::int32_t tandemBonus = 0;
    if (tandemActive)
    {
        const bool hasMasterPC = PAttacker->PMaster != nullptr && PAttacker->PMaster->objtype == TYPE_PC;
        const auto masterPower = hasMasterPC ? PAttacker->PMaster->getMod(Mod::TANDEM_BLOW_POWER) : 0;
        const auto selfPower   = PAttacker->getMod(Mod::TANDEM_BLOW_POWER);
        tandemBonus            = tpfromdamagehelpers::TandemBlowBonus(true, hasMasterPC, masterPower, selfPower);
    }

    tpfromdamagehelpers::PhysicalTPGainParams gain{};
    gain.baseTPGain      = baseTP;
    gain.targetIsMob     = PDefender->objtype == TYPE_MOB;
    gain.actorIsMob      = PAttacker->objtype == TYPE_MOB;
    gain.dAGI            = static_cast<std::int32_t>(PAttacker->AGI()) - static_cast<std::int32_t>(PDefender->AGI());
    gain.inhibitTP       = PDefender->getMod(Mod::INHIBIT_TP);
    gain.storeTP         = PDefender->getMod(Mod::STORETP);
    gain.subtleBlow      = PAttacker->getMod(Mod::SUBTLE_BLOW);
    gain.subtleBlowMerit = subtleMerit;
    gain.subtleBlowII    = PAttacker->getMod(Mod::SUBTLE_BLOW_II);
    gain.tandemBlowBonus = tandemBonus;

    return tpfromdamagehelpers::PhysicalTPGain(gain);
}

bool TryInterruptSpell(CBattleEntity* PAttacker, CBattleEntity* PDefender, CSpell* PSpell)
{
    // Early return: Spell can't be interrupted.
    if ((SKILLTYPE)PSpell->getSkillType() == SKILL_SINGING)
    {
        return false;
    }

    // Early return: Manafont prevents interruptions.
    if (PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Manafont))
    {
        return false;
    }

    spellinterrupthelpers::Params params{};
    params.attackerLevel     = PAttacker->GetMLevel();
    params.defenderLevel     = PDefender->GetMLevel();
    params.defenderIsMob     = PDefender->objtype == TYPE_MOB;
    params.defenderIsPC      = PDefender->objtype == TYPE_PC;
    params.spellInterruptMod = PDefender->getMod(Mod::SPELLINTERRUPT);
    params.roll              = xirand::GetRandomNumber<float>(1.0f);

    if (params.defenderIsPC)
    {
        auto* PChar = static_cast<CCharEntity*>(PDefender);
        float skillCap   = GetMaxSkill((SKILLTYPE)PSpell->getSkillType(), PChar->GetMJob(), PChar->GetMLevel());
        float skillLevel = PChar->GetSkill(PSpell->getSkillType());

        // If skill cap is 0, player may be using a spell from their subjob.
        if (skillCap == 0)
        {
            skillCap = GetMaxSkill((SKILLTYPE)PSpell->getSkillType(), PChar->GetSJob(), PChar->GetMLevel());
        }

        params.skillCap       = skillCap;
        params.skillLevel     = skillLevel;
        params.meritReduction = static_cast<std::uint8_t>(
            PChar->PMeritPoints->GetMeritValue(MERIT_SPELL_INTERUPTION_RATE, PChar));
    }

    if (CStatusEffect* PAqua = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Aquaveil))
    {
        params.hasAquaveil   = true;
        params.aquaveilPower = PAqua->GetPower();
    }

    const auto result = spellinterrupthelpers::Evaluate(params);

    if (result.aquaveilConsumed)
    {
        if (result.aquaveilDelete)
        {
            PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Aquaveil);
        }
        else if (CStatusEffect* PAqua = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Aquaveil))
        {
            PAqua->SetPower(result.aquaveilNewPower);
        }
    }

    return result.interrupted;
}

/************************************************************************
 *                                                                       *
 *  Calculate damage based on damage and resistance to damage type       *
 *                                                                       *
 ************************************************************************/

auto TakePhysicalDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, PHYSICAL_ATTACK_TYPE physicalAttackType, int32 damage, bool isBlocked, uint8 slot, uint16 tpMultiplier, CBattleEntity* taChar, bool giveTPtoVictim, bool giveTPtoAttacker, bool isCounter, bool isCovered, CBattleEntity* POriginalTarget) -> int32
{
    auto* weapon     = GetEntityWeapon(PAttacker, (SLOTTYPE)slot);
    giveTPtoAttacker = takephysicalhelpers::GiveTPToAttacker(
        giveTPtoAttacker, PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MeikyoShisui));
    giveTPtoVictim = takephysicalhelpers::GiveTPToVictim(
        giveTPtoVictim, static_cast<uint8>(physicalAttackType));
    bool           isRanged   = takedamagehelpers::IsWSRangedSlot(slot);
    int32          baseDamage = damage;
    ATTACK_TYPE    attackType = ATTACK_TYPE::PHYSICAL;
    xi::DamageType damageType = xi::DamageType::None;
    if (takephysicalhelpers::ShouldUseFormlessStrikesPath(
            PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::FormlessStrikes), isCounter))
    {
        attackType = ATTACK_TYPE::SPECIAL;
        // https://www.bg-wiki.com/ffxi/Formless_Strikes
        // Merit value of 1 is +5%, so 60% normal power
        const uint8 formlessMerit = (PAttacker->objtype == TYPE_PC)
                                        ? static_cast<CCharEntity*>(PAttacker)->PMeritPoints->GetMeritValue(MERIT_FORMLESS_STRIKES, static_cast<CCharEntity*>(PAttacker))
                                        : static_cast<uint8>(0);
        const uint8 formlessMod   = takephysicalhelpers::FormlessStrikesMod(PAttacker->objtype == TYPE_PC, formlessMerit);

        damage = takephysicalhelpers::ApplyFormlessPower(damage, formlessMod);
        damage = takephysicalhelpers::ApplyUDMGBreath(damage, PDefender->getMod(Mod::UDMGBREATH));
        damage = takephysicalhelpers::ApplyDMGBreath(damage, PDefender->getMod(Mod::DMGBREATH), PDefender->getMod(Mod::DMG));

        // TODO: Breaths can have elements. Where are those handled for absorption and nullification.

        // Preserve absorb-then-null else-if RNG order.
        const bool absorbProc = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::ABSORB_DMG_CHANCE);
        bool       nullAll    = false;
        bool       nullBreath = false;
        if (!absorbProc)
        {
            nullAll = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_DAMAGE);
            if (!nullAll)
            {
                nullBreath = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_BREATH_DAMAGE);
            }
        }
        std::int32_t formlessOut = damage;
        const auto   formlessRes = takephysicalhelpers::FormlessAbsorbNull(damage, absorbProc, nullAll, nullBreath, formlessOut);
        damage                   = formlessOut;
        if (formlessRes == takephysicalhelpers::FormlessAbsorbNullResult::PassThrough)
        {
            damage = HandleSevereDamage(PDefender, damage, false);
        }

        damage = CheckAndApplyDamageCap(damage, PDefender);
    }
    else
    {
        damageType = weapon ? weapon->getDmgType() : xi::DamageType::None;

        if (isRanged)
        {
            attackType = ATTACK_TYPE::RANGED;
            damage     = RangedDmgTaken(PDefender, damage, damageType, isCovered);
        }
        else
        {
            damage = PhysicalDmgTaken(PDefender, damage, damageType, isCovered);
        }

        // absorb mods are handled in the above functions, but they do not affect counters
        // this is a little hacky, but will work for now
        damage = takephysicalhelpers::FlipCounterAbsorb(damage, isCounter);

        if (takephysicalhelpers::UseTypeSpecificSDT(isCounter, giveTPtoAttacker)) // counters are always considered blunt (assuming h2h) damage, except retaliation (which is the only counter
                                            // that gives TP to the attacker)
        {
            switch (damageType)
            {
                case xi::DamageType::Piercing:
                    damage = takephysicalhelpers::ApplySDT(damage, PDefender->getMod(Mod::PIERCE_SDT));
                    break;
                case xi::DamageType::Slashing:
                    damage = takephysicalhelpers::ApplySDT(damage, PDefender->getMod(Mod::SLASH_SDT));
                    break;
                case xi::DamageType::Blunt:
                    damage = takephysicalhelpers::ApplySDT(damage, PDefender->getMod(Mod::IMPACT_SDT));
                    break;
                case xi::DamageType::HandToHand:
                    damage = takephysicalhelpers::ApplySDT(damage, PDefender->getMod(Mod::HTH_SDT));
                    break;
                default:
                    break;
            }
        }
        else
        {
            damage = takephysicalhelpers::ApplySDT(damage, PDefender->getMod(Mod::HTH_SDT));
        }

        if (isBlocked)
        {
            // TODO: get trust/pet/etc absorb percents
            // shield def bonus is a flat raw damage reduction that occurs before absorb
            // however do not reduce below 0 or if damage is negative
            damage = takephysicalhelpers::ApplyShieldDefBonus(damage, PDefender->getMod(Mod::SHIELD_DEF_BONUS));

            // Shield Mastery
            if (takephysicalhelpers::ShouldAddShieldMasteryTP(
                    damage, PDefender->getMod(Mod::STONESKIN), PDefender->getMod(Mod::SHIELD_MASTERY_TP)))
            {
                // If the attack was blocked and has shield mastery, add shield mastery TP bonus
                // unblocked damage (before block but as if affected by phalanx) must be greater than zero
                PDefender->addTP(PDefender->getMod(Mod::SHIELD_MASTERY_TP));
            }

            bool    hasShield         = false;
            uint8   shieldAbsorption  = 0;
            const auto* PCharDefender = dynamic_cast<CCharEntity*>(PDefender);
            if (PCharDefender)
            {
                CItemEquipment* slotSub = PCharDefender->getEquip(SLOT_SUB);
                if (slotSub && slotSub->IsShield())
                {
                    hasShield        = true;
                    shieldAbsorption = slotSub->getShieldAbsorption();
                }
            }
            const uint8 absorb = takephysicalhelpers::ShieldBlockAbsorb(
                PCharDefender != nullptr, hasShield, shieldAbsorption);

            // Reprisal
            if (takephysicalhelpers::ShouldApplyReprisalSpikes(
                    damage, PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Reprisal)))
            {
                // Reflect a portion of the blocked damage back. This is calculated before Stoneskin, Phalanx, Sentinel or Invincible
                CStatusEffect* reprisalEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Reprisal);

                if (reprisalEffect != nullptr)
                {
                    const int16 effectPower = takephysicalhelpers::ReprisalEffectPower(
                        static_cast<int16>(reprisalEffect->GetPower()), PDefender->getMod(Mod::REPRISAL_SPIKES_BONUS));
                    const bool invOrSent = PDefender->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::Invincible, xi::StatusEffect::Sentinel });
                    const int32 blockedDamage = takephysicalhelpers::ReprisalBlockedDamage(damage, baseDamage, absorb, invOrSent);
                    const int32 spikesDamage  = takephysicalhelpers::ReprisalSpikesDamage(blockedDamage, effectPower);

                    // Set Reprisal spike damage
                    PDefender->setModifier(Mod::SPIKES_DMG, spikesDamage);
                }
            }
            damage = takephysicalhelpers::ApplyBlockAbsorb(damage, absorb);
        }
    }

    if (takedamagehelpers::ShouldApplyWSPhalanxStoneskin(damage))
    {
        damage = takedamagehelpers::ApplyPhalanx(damage, PDefender->getMod(Mod::PHALANX));

        damage = HandleStoneskin(PDefender, damage);
        HandleAfflatusMiseryDamage(PDefender, damage);
    }
    damage = takedamagehelpers::ClampWSDamage(damage);

    damage = CheckAndApplyDamageCap(damage, PDefender);

    // Scarlet Delirium: Updates status effect power with damage bonus
    battleutils::HandleScarletDelirium(PDefender, damage);

    int32 corrected = PDefender->takeDamage(damage, PAttacker, attackType, damageType);
    damage          = takedamagehelpers::CorrectedDamageAfterTake(damage, corrected);

    // Only claim a mob and if the allegiance is not PLAYER. This prevents mobs from calling ClaimMob on other mobs or themselves.
    if (takephysicalhelpers::ShouldClaimOnPhysicalDamage(
            PDefender->objtype == TYPE_MOB, PDefender->allegiance == PAttacker->allegiance))
    {
        battleutils::ClaimMob(PDefender, PAttacker);
    }

    if (damage > 0)
    {
        PDefender->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Damage);

        // Check for bind breaking
        BindBreakCheck(PAttacker, PDefender);

        switch (PDefender->objtype)
        {
            case TYPE_MOB:
                if (taChar == nullptr)
                {
                    ((CMobEntity*)PDefender)->PEnmityContainer->UpdateEnmityFromDamage(PAttacker, damage);
                }
                else
                {
                    ((CMobEntity*)PDefender)->PEnmityContainer->UpdateEnmityFromDamage(taChar, damage);
                }

                if (((CMobEntity*)PDefender)->m_HiPCLvl < PAttacker->GetMLevel())
                {
                    ((CMobEntity*)PDefender)->m_HiPCLvl = PAttacker->GetMLevel();
                }

                // if the mob is charmed by player
                if (PDefender->PMaster != nullptr && PDefender->PMaster->objtype == TYPE_PC)
                {
                    ((CPetEntity*)PDefender)
                        ->loc.zone->UpdateEntityPacket(PDefender, ENTITY_UPDATE, UPDATE_COMBAT);

                    if (PAttacker->objtype == TYPE_MOB)
                    {
                        // charmed mob should lose enmity from normal attacks
                        ((CMobEntity*)PAttacker)->PEnmityContainer->UpdateEnmityFromAttack(PDefender, damage);
                    }
                }
                break;

            case TYPE_PET:
                ((CPetEntity*)PDefender)->loc.zone->UpdateEntityPacket(PDefender, ENTITY_UPDATE, UPDATE_COMBAT);

                if (PAttacker->objtype == TYPE_MOB)
                {
                    // pets should lose enmity from normal attacks
                    ((CMobEntity*)PAttacker)->PEnmityContainer->UpdateEnmityFromAttack(PDefender, damage);
                }
                break;
            case TYPE_PC:
                if (PAttacker->objtype == TYPE_MOB)
                {
                    if (isCovered)
                    {
                        ((CMobEntity*)PAttacker)->PEnmityContainer->UpdateEnmityFromCover(POriginalTarget, PDefender);
                    }
                    else
                    {
                        ((CMobEntity*)PAttacker)->PEnmityContainer->UpdateEnmityFromAttack(PDefender, damage);
                    }
                }
                break;
            default:
                break;
        }

        // try to interrupt spell if not a ranged attack and not blocked by Shield Mastery
        if (takephysicalhelpers::ShouldTryHitInterruptPhysical(
                isRanged,
                isBlocked,
                PDefender->objtype == TYPE_PC,
                PDefender->objtype == TYPE_PC && charutils::hasTrait(static_cast<CCharEntity*>(PDefender), TRAIT_SHIELD_MASTERY)))
        {
            PDefender->TryHitInterrupt(PAttacker);
        }

        if (giveTPtoAttacker)
        {
            bool isZanshin = physicalAttackType == PHYSICAL_ATTACK_TYPE::ZANSHIN;

            int16 attackerTPReturn = CalculateTPFromDamageDealt(PAttacker, isZanshin, static_cast<SLOTTYPE>(slot));

            PAttacker->addTP((int16)(tpMultiplier * attackerTPReturn));
        }

        if (giveTPtoVictim)
        {
            int32 delay = 0;

            if (isRanged && PAttacker->objtype == TYPE_PC)
            {
                delay = GetBaseRangedDelay(PAttacker);
            }
            else
            {
                delay = GetBaseDelay(PAttacker);
            }

            int16 defenderTPReturn = CalculateTPFromDamageTaken(PAttacker, PDefender, damage, delay);

            PDefender->addTP((int16)(tpMultiplier * defenderTPReturn));
        }
    }
    else if (PDefender->objtype == TYPE_MOB)
    {
        ((CMobEntity*)PDefender)->PEnmityContainer->UpdateEnmityFromDamage(PAttacker, 0);
    }

    return damage;
}

/************************************************************************
 *                                                                       *
 *  Handle Damage from Weaponskills (dmg type reductions calced in lua)  *
 *                                                                       *
 ************************************************************************/

auto TakeWeaponskillDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, int32 damage, ATTACK_TYPE attackType, xi::DamageType damageType, uint8 slot, bool primary, float tpMultiplier, uint16 bonusTP, float targetTPMultiplier) -> int32
{
    const bool isRanged = takedamagehelpers::IsWSRangedSlot(slot);

    // DefenseBoost infront nullification + null damage rolls.
    uint16 defenseSubPower = 0;
    bool   inFrontDefBoost = false;
    const bool hasDefenseBoost =
        PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::DefenseBoost);
    if (hasDefenseBoost)
    {
        defenseSubPower  = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::DefenseBoost)->GetSubPower();
        inFrontDefBoost  = infront(PAttacker->loc.p, PDefender->loc.p, defenseSubPower);
    }
    // Preserve null RNG only for the matching attack type arm (short-circuit else-if).
    bool nullRangedProc = false;
    bool nullPhysProc   = false;
    if (attackType == ATTACK_TYPE::RANGED)
    {
        nullRangedProc = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_RANGED_DAMAGE);
    }
    else if (attackType == ATTACK_TYPE::PHYSICAL)
    {
        nullPhysProc = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_PHYSICAL_DAMAGE);
    }
    damage = takedamagehelpers::ApplyWSDefenseAndNull(
        damage,
        takedamagehelpers::WSDefenseBoostNullifies(
            static_cast<uint8>(attackType), hasDefenseBoost, defenseSubPower, inFrontDefBoost),
        static_cast<uint8>(attackType),
        nullRangedProc,
        nullPhysProc);

    if (takedamagehelpers::ShouldApplyWSPhalanxStoneskin(damage))
    {
        damage = takedamagehelpers::ApplyPhalanx(damage, PDefender->getMod(Mod::PHALANX));
        damage = HandleStoneskin(PDefender, damage);
    }

    if (takedamagehelpers::ShouldApplyOverwhelm(isRanged))
    {
        damage = getOverWhelmDamageBonus(PAttacker, PDefender, damage);
    }

    HandleAfflatusMiseryDamage(PDefender, damage);
    damage = takedamagehelpers::ClampWSDamage(damage);

    damage = CheckAndApplyDamageCap(damage, PDefender);

    int32 corrected = PDefender->takeDamage(damage, PAttacker, attackType, damageType);
    damage          = takedamagehelpers::CorrectedDamageAfterTake(damage, corrected);

    if (takedamagehelpers::ShouldClaimOnWSDamage(PAttacker->objtype == TYPE_PC))
    {
        battleutils::ClaimMob(PDefender, PAttacker);
    }

    int16 standbyTp = 0;

    if (takedamagehelpers::ShouldProcessWSHitEffects(damage))
    {
        PDefender->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Damage);

        // Check for bind breaking
        BindBreakCheck(PAttacker, PDefender);

        switch (PDefender->objtype)
        {
            case TYPE_MOB:
                // if the mob is charmed by player
                if (takedamagehelpers::ShouldUpdateCharmedMobPacket(
                        true, PDefender->PMaster != nullptr, PDefender->PMaster && PDefender->PMaster->objtype == TYPE_PC))
                {
                    ((CPetEntity*)PDefender)
                        ->loc.zone->UpdateEntityPacket(PDefender, ENTITY_UPDATE, UPDATE_COMBAT);
                }

                if (takedamagehelpers::ShouldUpdateMobHiPCLvl(((CMobEntity*)PDefender)->m_HiPCLvl, PAttacker->GetMLevel()))
                {
                    ((CMobEntity*)PDefender)->m_HiPCLvl = PAttacker->GetMLevel();
                }

                break;

            case TYPE_PET:
                if (takedamagehelpers::ShouldUpdatePetCombatPacket(true))
                {
                    ((CPetEntity*)PDefender)->loc.zone->UpdateEntityPacket(PDefender, ENTITY_UPDATE, UPDATE_COMBAT);
                }
                break;

            default:
                break;
        }

        // try to interrupt spell
        PDefender->TryHitInterrupt(PAttacker);

        // Add tp to attacker
        // Calculate TP Return from WS
        {
            const int16 baseTpFromDealt = primary
                                              ? CalculateTPFromDamageDealt(PAttacker, false, static_cast<SLOTTYPE>(slot))
                                              : static_cast<int16>(0);
            standbyTp = takedamagehelpers::WSStandbyTP(primary, bonusTP, tpMultiplier, baseTpFromDealt);
        }

        // Add TP to defender
        int32 delay = 0;

        if (isRanged)
        {
            delay = GetBaseRangedDelay(PAttacker);
        }
        else
        {
            delay = GetBaseDelay(PAttacker);
        }

        const int16 baseTp = CalculateTPFromDamageTaken(PAttacker, PDefender, damage, delay);

        PDefender->addTP(takedamagehelpers::WSDefenderTP(tpMultiplier, targetTPMultiplier, baseTp));
    }
    else if (takedamagehelpers::ShouldUpdateEnmityFromZeroWS(damage, PDefender->objtype == TYPE_MOB))
    {
        ((CMobEntity*)PDefender)->PEnmityContainer->UpdateEnmityFromDamage(PAttacker, 0);
    }

    // Apply TP
    PAttacker->addTP(takedamagehelpers::WSAttackerAddTP(PAttacker->getMod(Mod::SAVETP), standbyTp));

    // Remove Hagakure Effect if present
    if (takedamagehelpers::ShouldRemoveHagakure(
            PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Hagakure)))
    {
        PAttacker->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Hagakure);
    }

    return damage;
}

/************************************************************************
 *                                                                       *
 *  Handle Damage from Spells (dmg type reductions calced in lua)        *
 *                                                                       *
 ************************************************************************/

void TakeSpellDamage(CBattleEntity* PDefender, CBattleEntity* PAttacker, CSpell* PSpell, int32 damage, ATTACK_TYPE attackType, xi::DamageType damageType)
{
    // Scarlet Delirium: Updates status effect power with damage bonus
    battleutils::HandleScarletDelirium(PDefender, damage);

    PDefender->takeDamage(damage, PAttacker, attackType, damageType);

    // Remove effects from damage
    if (takedamagehelpers::ShouldApplySpellDamageEffects(PSpell->canTargetEnemy(), damage))
    {
        PDefender->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Damage);

        // Check for bind breaking
        BindBreakCheck(PAttacker, PDefender);

        // Add TP for damaging spells (Only player chars who have the Occult Accumen trait)
        {
            const bool isPC = PAttacker->objtype == TYPE_PC;
            const bool meikyo = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MeikyoShisui);
            const auto skillType = static_cast<std::uint8_t>(PSpell->getSkillType());
            const auto mpCost = static_cast<std::int32_t>(PSpell->getMPCost());
            const auto occult = PAttacker->getMod(Mod::OCCULT_ACUMEN);
            const auto store  = PAttacker->getMod(Mod::STORETP);
            PAttacker->addTP(tpfromdamagehelpers::SpellTP(isPC, meikyo, skillType, mpCost, occult, store));
        }

        // Targets of damaging spells gain TP
        {
            if (!tpfromdamagehelpers::ShouldZeroMagicalTPGain(
                    false, damage, PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MeikyoShisui)))
            {
                std::int32_t subtleMerit = 0;
                if (PAttacker->objtype == TYPE_PC)
                {
                    auto* PChar = static_cast<CCharEntity*>(PAttacker);
                    subtleMerit = PChar->PMeritPoints->GetMeritValue(MERIT_SUBTLE_BLOW_EFFECT, PChar);
                }

                const bool tandemActive = petutils::IsTandemActive(PAttacker);
                std::int32_t tandemBonus = 0;
                if (tandemActive)
                {
                    const bool hasMasterPC = PAttacker->PMaster != nullptr && PAttacker->PMaster->objtype == TYPE_PC;
                    const auto masterPower = hasMasterPC ? PAttacker->PMaster->getMod(Mod::TANDEM_BLOW_POWER) : 0;
                    const auto selfPower   = PAttacker->getMod(Mod::TANDEM_BLOW_POWER);
                    tandemBonus            = tpfromdamagehelpers::TandemBlowBonus(true, hasMasterPC, masterPower, selfPower);
                }

                tpfromdamagehelpers::MagicalTPGainParams gain{};
                gain.targetIsMob     = PDefender->objtype == TYPE_MOB;
                gain.dAGI            = static_cast<std::int32_t>(PAttacker->AGI()) - static_cast<std::int32_t>(PDefender->AGI());
                gain.inhibitTP       = PDefender->getMod(Mod::INHIBIT_TP);
                gain.storeTP         = PDefender->getMod(Mod::STORETP);
                gain.subtleBlow      = PAttacker->getMod(Mod::SUBTLE_BLOW);
                gain.subtleBlowMerit = subtleMerit;
                gain.subtleBlowII    = PAttacker->getMod(Mod::SUBTLE_BLOW_II);
                gain.tandemBlowBonus = tandemBonus;
                PDefender->addTP(tpfromdamagehelpers::MagicalTPGain(gain));
            }
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Handle Damage from Swipe/Lunge (dmg type reductions calced in lua)   *
 *                                                                       *
 ************************************************************************/

auto TakeSwipeLungeDamage(CBattleEntity* PDefender, CBattleEntity* PAttacker, int32 damage, ATTACK_TYPE attackType, xi::DamageType damageType) -> int32
{
    damage = CheckAndApplyDamageCap(damage, PDefender);

    PDefender->takeDamage(damage, PAttacker, attackType, damageType);

    // Remove effects from damage
    if (takedamagehelpers::ShouldApplySwipeLungeHitEffects(damage))
    {
        PDefender->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Damage);
        // Check for bind breaking
        BindBreakCheck(PAttacker, PDefender);

        // Do targets get TP?
    }

    return damage;
}

/************************************************************************
 *                                                                       *
 *  Calculate Probability attack will hit (20% min - 95~99% max cap)     *
 *  attackNumber: 0=main, 1=sub, 2=kick                                  *
 *                                                                       *
 ************************************************************************/

uint8 GetHitRateEx(CBattleEntity* PAttacker, CBattleEntity* PDefender, uint8 attackNumber,
                   int16 offsetAccuracy) // subWeaponAttack is for calculating acc of dual wielded sub weapon
{
    bool hasSneakAttack      = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::SneakAttack);
    bool hasTrickAttack      = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::TrickAttack);
    bool isBehind            = behind(PAttacker->loc.p, PDefender->loc.p, 64);
    bool hasAssassin         = PAttacker->hasTrait(TRAIT_ASSASSIN);
    bool hasValidSneakAttack = hasSneakAttack && isBehind;
    bool hasValidTrickAttack = hasTrickAttack && hasAssassin;
    // Match original || short-circuit: only resolve TA char when SA is not already valid.
    bool hasValidTAChar = false;
    if (!hasValidSneakAttack && hasValidTrickAttack)
    {
        hasValidTAChar = getAvailableTrickAttackChar(PAttacker, PDefender) != nullptr;
    }

    double hitRate = 0.0;
    if (!hasValidSneakAttack && !hasValidTAChar)
    {
        using namespace physicalhitratehelpers;

        const bool isPC   = PAttacker->objtype == TYPE_PC;
        const bool isPet  = PAttacker->objtype == TYPE_PET;
        const bool slotLeftOrHigher = attackNumber >= AttackSlotLeft;

        const auto mods = HitRateModifiers(BuildHitRateModParams(PAttacker, PDefender, false, false));

        MeleeHitRateParams p{};
        p.acc                  = PAttacker->ACC(attackNumber, 0);
        p.eva                  = PDefender->EVA();
        p.bonus                = offsetAccuracy;
        p.accBonus             = mods.accBonus;
        p.evaBonus             = mods.evaBonus;
        p.cap                  = HitRateCap(isPet, isPC, IsUsingH2H(PAttacker), IsWeaponTwoHanded(PAttacker), slotLeftOrHigher);
        p.applyLevelCorrection = ApplyLevelCorrection(PAttacker);
        p.attackerLevel        = PAttacker->GetMLevel();
        p.defenderLevel        = PDefender->GetMLevel();
        p.attackerIsPC         = isPC;
        p.attackerIsAvatar     = IsAvatar(PAttacker);

        hitRate = MeleeHitRate(p);
    }

    return paralyzeshadowhelpers::GetHitRateEx(hasValidSneakAttack, hasValidTAChar, hitRate);
}

uint8 GetHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    return GetHitRateEx(PAttacker, PDefender, 0, 0); // assume attack 0(main)
}

uint8 GetHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender, uint8 attackNumber)
{
    return GetHitRateEx(PAttacker, PDefender, attackNumber, 0);
}

uint8 GetHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender, uint8 attackNumber, int16 offsetAccuracy)
{
    return GetHitRateEx(PAttacker, PDefender, attackNumber, offsetAccuracy);
}

/************************************************************************
 *                                                                       *
 *  Calculate Crit Hit Rate                                              *
 *                                                                       *
 ************************************************************************/

uint8 GetCritHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender, bool ignoreSneakTrickAttack, SLOTTYPE weaponSlot)
{
    if (PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MightyStrikes, 0) ||
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MightyStrikes))
    {
        return 100;
    }

    const bool isPC = PAttacker->objtype == TYPE_PC;
    const bool hasSneakAttack =
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::SneakAttack);
    const bool behindOrHide =
        behind(PAttacker->loc.p, PDefender->loc.p, 64) ||
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Hide);
    const bool isTHFMain   = isPC && PAttacker->GetMJob() == JOB_THF;
    const bool hasAssassin = isPC && charutils::hasTrait(static_cast<CCharEntity*>(PAttacker), TRAIT_ASSASSIN);
    const bool hasTrickAttack =
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::TrickAttack);
    // TA char only resolved when the TA arm would be considered (preserve getAvailableTrickAttackChar call order).
    const bool taArmEligible =
        isPC && isTHFMain && hasAssassin && !ignoreSneakTrickAttack && hasTrickAttack;
    CBattleEntity* taChar =
        taArmEligible ? battleutils::getAvailableTrickAttackChar(PAttacker, PDefender) : nullptr;

    // SA is evaluated before TA in LSB (else-if chain). When SA arm matches entity
    // conditions, TA is not considered even if ignore would only apply to one.
    const bool saArmEligible = isPC && !ignoreSneakTrickAttack && hasSneakAttack;

    const auto path = saArmEligible
                          ? (behindOrHide ? crithitratehelpers::MeleeCritPath::Forced100
                                          : crithitratehelpers::MeleeCritPath::BareFive)
                          : (taArmEligible
                                 ? (taChar != nullptr ? crithitratehelpers::MeleeCritPath::Forced100
                                                     : crithitratehelpers::MeleeCritPath::BareFive)
                                 : crithitratehelpers::MeleeCritPath::Assembly);

    if (path == crithitratehelpers::MeleeCritPath::Forced100)
    {
        return 100;
    }
    if (path == crithitratehelpers::MeleeCritPath::BareFive)
    {
        // LSB quirk: SA/TA arm entered but position/TA-char failed → bare 5, no assembly.
        return 5;
    }

    std::int32_t attackerMerit = 0;
    std::int32_t fencerRate    = 0;
    if (isPC)
    {
        auto* PCharAttacker = static_cast<CCharEntity*>(PAttacker);
        attackerMerit       = PCharAttacker->PMeritPoints->GetMeritValue(MERIT_CRIT_HIT_RATE, PCharAttacker);

        CItemWeapon*    PMain      = dynamic_cast<CItemWeapon*>(PCharAttacker->m_Weapons[SLOT_MAIN]);
        CItemEquipment* PSub       = PCharAttacker->getEquip(SLOT_SUB);
        CItemWeapon*    PSubWeapon = dynamic_cast<CItemWeapon*>(PCharAttacker->m_Weapons[SLOT_SUB]);
        if (crithitratehelpers::FencerCritEligible(
                PMain != nullptr,
                PMain && PMain->isTwoHanded(),
                PMain && PMain->isHandToHand(),
                PSub != nullptr,
                PSubWeapon != nullptr,
                PSubWeapon ? static_cast<std::uint8_t>(PSubWeapon->getSkillType()) : static_cast<std::uint8_t>(0),
                PSub && PSub->IsShield()))
        {
            fencerRate = PCharAttacker->getMod(Mod::FENCER_CRITHITRATE);
        }
    }

    std::int32_t defenderMerit = 0;
    if (PDefender->objtype == TYPE_PC)
    {
        defenderMerit = static_cast<CCharEntity*>(PDefender)->PMeritPoints->GetMeritValue(
            MERIT_ENEMY_CRIT_RATE, static_cast<CCharEntity*>(PDefender));
    }

    std::int32_t inninPower = 0;
    if (PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Innin) &&
        behind(PAttacker->loc.p, PDefender->loc.p, 64))
    {
        inninPower = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Innin)->GetPower();
    }
    std::int32_t yoninPower = 0;
    if (PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Yonin) &&
        infront(PDefender->loc.p, PAttacker->loc.p, 64))
    {
        yoninPower = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Yonin)->GetPower();
    }

    std::int32_t weaponOnly = 0;
    if (auto* player = dynamic_cast<CCharEntity*>(PAttacker))
    {
        auto* weapon = dynamic_cast<CItemWeapon*>(player->getEquip(weaponSlot));
        if (weapon && weapon->getModifier(Mod::CRITHITRATE_ONLY_WEP) > 0)
        {
            weaponOnly = weapon->getModifier(Mod::CRITHITRATE_ONLY_WEP);
        }
    }

    return crithitratehelpers::MeleeCritHitRate(
        attackerMerit,
        fencerRate,
        defenderMerit,
        inninPower,
        yoninPower,
        GetDexCritBonus(PAttacker, PDefender),
        PAttacker->getMod(Mod::CRITHITRATE),
        PDefender->getMod(Mod::CRITICAL_HIT_EVASION),
        weaponOnly);
}

int8 GetDexCritBonus(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    return crithitratehelpers::DexCritBonus(PAttacker->DEX(), PDefender->AGI());
}

/************************************************************************
 *                                                                       *
 *  Calculate Ranged Crit Hit Rate                                       *
 *                                                                       *
 ************************************************************************/

uint8 GetRangedCritHitRate(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    std::int32_t attackerMerit = 0;
    if (PAttacker->objtype == TYPE_PC)
    {
        auto* PCharAttacker = static_cast<CCharEntity*>(PAttacker);
        attackerMerit       = PCharAttacker->PMeritPoints->GetMeritValue(MERIT_CRIT_HIT_RATE, PCharAttacker);
    }

    std::int32_t defenderMerit = 0;
    if (PDefender->objtype == TYPE_PC)
    {
        defenderMerit = static_cast<CCharEntity*>(PDefender)->PMeritPoints->GetMeritValue(
            MERIT_ENEMY_CRIT_RATE, static_cast<CCharEntity*>(PDefender));
    }

    std::int32_t inninPower = 0;
    if (PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Innin) &&
        behind(PAttacker->loc.p, PDefender->loc.p, 64))
    {
        inninPower = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Innin)->GetPower();
    }
    std::int32_t yoninPower = 0;
    if (PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Yonin) &&
        infront(PDefender->loc.p, PAttacker->loc.p, 64))
    {
        yoninPower = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Yonin)->GetPower();
    }

    return crithitratehelpers::RangedCritHitRate(
        attackerMerit,
        defenderMerit,
        inninPower,
        yoninPower,
        PAttacker->getMod(Mod::CRITHITRATE),
        PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::MightyStrikes),
        GetAGICritBonus(PAttacker, PDefender),
        PDefender->getMod(Mod::CRITICAL_HIT_EVASION));
}

int8 GetAGICritBonus(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    return crithitratehelpers::AgiCritBonus(PAttacker->AGI(), PDefender->AGI());
}

/************************************************************************
 *                                                                       *
 *   Formula for calculating damage ratio                                *
 *                                                                       *
 ************************************************************************/

float GetDamageRatio(CBattleEntity* PAttacker, CBattleEntity* PDefender, bool isCritical, float bonusAttPercent, SKILLTYPE weaponType, SLOTTYPE weaponSlot, bool isCannonball)
{
    using namespace pdifhelpers;

    const bool isPC    = PAttacker->objtype == TYPE_PC;
    const bool applyLC = ApplyLevelCorrection(PAttacker);

    // isWeaponskill=false for this call site → flourishBonus 1.0
    double tpFactor         = 0.0;
    bool   tpIgnoresDefense = false;
    if (PAttacker->objtype == TYPE_PET && static_cast<CPetEntity*>(PAttacker)->getPetType() == PET_TYPE::AUTOMATON)
    {
        auto* PAuto   = static_cast<CAutomatonEntity*>(PAttacker);
        auto* PMaster = PAttacker->PMaster;
        const bool hasAttuner = PAuto->hasAttachment(attunerhelpers::AttunerAttachmentSlot());
        const bool hasMaster  = PMaster != nullptr;
        int        fireCount  = 0;
        bool       overdrive  = false;
        if (hasMaster)
        {
            fireCount = static_cast<int>(PMaster->StatusEffectContainer->GetEffectsCount(xi::StatusEffect::FireManeuver));
            overdrive = PMaster->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Overdrive);
        }
        const auto defIgnore = attunerhelpers::AttunerDefIgnore(
            true,
            hasAttuner,
            hasMaster,
            PAttacker->GetMLevel(),
            PDefender->GetMLevel(),
            fireCount,
            overdrive);
        tpFactor += defIgnore;
        if (tpFactor > 0.0)
        {
            tpIgnoresDefense = true;
        }
    }

    double actorAttack = 0.0;
    if (isCannonball)
    {
        actorAttack = static_cast<double>(PAttacker->DEF());
    }
    else
    {
        const double flourishBonus = 1.0;
        actorAttack                = std::max(1.0, std::floor(static_cast<double>(PAttacker->ATT(weaponSlot)) * static_cast<double>(bonusAttPercent) * flourishBonus));
    }
    const double targetDefense = EffectiveDefense(static_cast<double>(PDefender->DEF()), tpIgnoresDefense, tpFactor);

    const int spikeRoll    = xirand::GetRandomNumber(1, 10001); // [1,10000]
    const int upperMaxCoin = xirand::GetRandomNumber(0, 2);    // 0 or 1

    double lower  = 0.0;
    double upper  = 0.0;
    bool   spiked = false;
    MeleeBounds(actorAttack, targetDefense, isCritical, applyLC, isPC, PAttacker->GetMLevel(), PDefender->GetMLevel(), WeaponCap(static_cast<std::uint8_t>(weaponType)), PAttacker->getMod(Mod::DAMAGE_LIMIT), PAttacker->getMod(Mod::DAMAGE_LIMITP), spikeRoll, upperMaxCoin, lower, upper, spiked);
    if (spiked)
    {
        return 1.0f;
    }
    if (upper == 0.0)
    {
        return 0.0f;
    }

    int lo = 0;
    int hi = 0;
    int ratioRoll = 0;
    if (RatioRollRange(lower, upper, lo, hi))
    {
        ratioRoll = xirand::GetRandomNumber(lo, hi + 1); // inclusive
    }
    const int meleeRandStep = xirand::GetRandomNumber(0, 6); // 0..5

    const double pDif = FinishMeleePDIF(static_cast<double>(ratioRoll) / 1000.0, meleeRandStep, isCritical, PAttacker->getMod(Mod::CRIT_DMG_INCREASE), PDefender->getMod(Mod::CRIT_DEF_BONUS));
    return static_cast<float>(std::max(pDif, 0.0));
}

/************************************************************************
 *                                                                       *
 *   Formula for Strength                                                *
 *                                                                       *
 ************************************************************************/

auto GetFSTR(CBattleEntity* PAttacker, CBattleEntity* PDefender, uint8 SlotID) -> int32
{
    if (SlotID != SLOT_RANGED && SlotID != SLOT_AMMO && SlotID != SLOT_MAIN && SlotID != SLOT_SUB)
    {
        ShowError("battleutils::GetFSTR() failed to run lua calls");
        return 0;
    }

    const auto actor = fstrhelpers::ClassifyActor(
        PAttacker->objtype == TYPE_MOB,
        PAttacker->objtype == TYPE_PET);

    const std::int32_t mainLvl      = PAttacker->GetMLevel();
    const std::int32_t attackerSTR  = PAttacker->STR();
    const std::int32_t defenderVIT  = PDefender->VIT();

    std::int32_t weaponRank = 0;
    if (SlotID == SLOT_RANGED || SlotID == SLOT_AMMO)
    {
        weaponRank = PAttacker->GetRangedWeaponRank();
    }
    else
    {
        weaponRank = PAttacker->GetMainWeaponRank();
    }

    return fstrhelpers::GetFSTR(
        SlotID, actor, mainLvl, attackerSTR, defenderVIT, weaponRank);
}

/************************************************************************
 *                                                                       *
 *  Multihit calculator                                                  *
 *                                                                       *
 ************************************************************************/

uint8 getHitCount(uint8 hits)
{
    return hitcounthelpers::GetHitCount(hits, static_cast<std::uint8_t>(xirand::GetRandomNumber(100)));
}

/************************************************************************
 *                                                                       *
 *  Returns the number of hits for multihit weapons if applicable        *
 *  (Keeping this for backwards compatibility with the old system)       *
 *                                                                       *
 ************************************************************************/

uint8 CheckMultiHits(CBattleEntity* PEntity, CItemWeapon* PWeapon)
{
    if (multihitshelpers::ShouldRejectNullWeapon(PWeapon == nullptr))
    {
        return 0;
    }

    // checking players weapon hit count
    const uint8 baseHits = PWeapon->getHitCount();

    std::int16_t tripleAttack = PEntity->getMod(Mod::TRIPLE_ATTACK);
    std::int16_t doubleAttack = PEntity->getMod(Mod::DOUBLE_ATTACK);
    std::int16_t quadAttack   = PEntity->getMod(Mod::QUAD_ATTACK);

    std::int16_t tripleMerit = 0;
    std::int16_t doubleMerit = 0;
    bool         hasTriple   = false;
    bool         hasDouble   = false;

    // check for merit upgrades
    if (PEntity->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(PEntity);
        hasTriple   = charutils::hasTrait(PChar, TRAIT_TRIPLE_ATTACK);
        hasDouble   = charutils::hasTrait(PChar, TRAIT_DOUBLE_ATTACK);
        if (hasTriple)
        {
            tripleMerit = static_cast<std::int16_t>(PChar->PMeritPoints->GetMeritValue(MERIT_TRIPLE_ATTACK_RATE, PChar));
        }
        if (hasDouble)
        {
            doubleMerit = static_cast<std::int16_t>(PChar->PMeritPoints->GetMeritValue(MERIT_DOUBLE_ATTACK_RATE, PChar));
        }
    }

    std::int16_t doubleRate = 0;
    std::int16_t tripleRate = 0;
    std::int16_t quadRate   = 0;
    multihitshelpers::StackMultiHitRates(
        doubleAttack, tripleAttack, quadAttack, doubleMerit, tripleMerit, hasDouble, hasTriple, doubleRate, tripleRate, quadRate);

    // Exclusive QA/TA/DA with short-circuit RNG order.
    std::uint8_t quadRoll   = 0;
    std::uint8_t tripleRoll = 0;
    std::uint8_t doubleRoll = 0;
    quadRoll                = static_cast<std::uint8_t>(xirand::GetRandomNumber(100));
    if (static_cast<std::int16_t>(quadRoll) >= quadRate)
    {
        tripleRoll = static_cast<std::uint8_t>(xirand::GetRandomNumber(100));
        if (static_cast<std::int16_t>(tripleRoll) >= tripleRate)
        {
            doubleRoll = static_cast<std::uint8_t>(xirand::GetRandomNumber(100));
        }
    }

    auto num = multihitshelpers::ExpandMultiHits(baseHits, doubleRate, tripleRate, quadRate, quadRoll, tripleRoll, doubleRoll);

    // Hasso Zanshin bonus: requires HASSO_ZANSHIN_BONUS mod (applied by Hasso effect when SAM is main job)
    const auto hassoBonus = PEntity->getMod(Mod::HASSO_ZANSHIN_BONUS);
    const auto hasHasso   = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Hasso);
    if (hassoBonus > 0 && hasHasso)
    {
        std::int16_t zanshinMerit = 0;
        if (PEntity->objtype == TYPE_PC)
        {
            zanshinMerit = static_cast<std::int16_t>(
                static_cast<CCharEntity*>(PEntity)->PMeritPoints->GetMeritValue(MERIT_ZASHIN_ATTACK_RATE, static_cast<CCharEntity*>(PEntity)));
        }
        const auto chance = multihitshelpers::HassoZanshinChance(
            PEntity->getMod(Mod::ZANSHIN), zanshinMerit, PEntity->objtype == TYPE_PC);
        const auto roll = static_cast<std::uint8_t>(xirand::GetRandomNumber(100));
        num             = multihitshelpers::ApplyHassoZanshinHit(num, hassoBonus, true, chance, roll);
    }

    return multihitshelpers::CapMultiHits(num);
}

/************************************************************************
 *                                                                       *
 *  Chance paralysis will cause you to be paralyzed                      *
 *                                                                       *
 ************************************************************************/

bool IsParalyzed(CBattleEntity* PAttacker)
{
    return paralyzeshadowhelpers::IsParalyzed(PAttacker->getMod(Mod::PARALYZE), xirand::GetRandomNumber(100));
}

/************************************************************************
 *                                                                       *
 *  Chance Shadows (Blink/Utsusemi) will proc                            *
 *                                                                       *
 ************************************************************************/

bool IsAbsorbByShadow(CBattleEntity* PDefender, CBattleEntity* PAttacker)
{
    // utsus always overwrites blink, so if utsus>0 then we know theres no blink.
    const uint16 utsusemi          = PDefender->getMod(Mod::UTSUSEMI);
    const uint16 blink             = PDefender->getMod(Mod::BLINK);
    const int    blinkFailRoll     = (utsusemi == 0) ? xirand::GetRandomNumber(100) : 0;
    const bool   defenderIsPC      = PDefender->objtype == TYPE_PC;
    const bool   hasCopyImage      = defenderIsPC && PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::CopyImage) != nullptr;
    const bool   attackerIsMob     = dynamic_cast<CMobEntity*>(PAttacker) != nullptr;

    const auto decision = paralyzeshadowhelpers::IsAbsorbByShadow(
        utsusemi, blink, blinkFailRoll, defenderIsPC, hasCopyImage, attackerIsMob);

    if (!decision.absorbed)
    {
        return false;
    }

    const Mod modShadow = (decision.usedMod == paralyzeshadowhelpers::ShadowModKind::Blink) ? Mod::BLINK : Mod::UTSUSEMI;
    PDefender->setModifier(modShadow, decision.remaining);

    if (decision.delCopyImage)
    {
        PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::CopyImage);
    }
    if (decision.delBlink)
    {
        PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Blink);
    }
    if (decision.setIcon)
    {
        if (CStatusEffect* PStatusEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::CopyImage))
        {
            if (decision.applyCEEnmity)
            {
                // player loses 25 CE if attack absorbed by utsusemi shadow
                if (auto* PMob = dynamic_cast<CMobEntity*>(PAttacker))
                {
                    PMob->PEnmityContainer->UpdateEnmity(PDefender, paralyzeshadowhelpers::UtsusemiAbsorbCEDelta, 0);
                }
            }
            PStatusEffect->SetIcon(decision.icon);
            PDefender->StatusEffectContainer->UpdateStatusIcons();
        }
    }

    return true;
}

/************************************************************************
 *                                                                       *
 *  Intimidation from Killer Effects (chance to intimidate)              *
 *                                                                       *
 ************************************************************************/

auto IsIntimidated(CBattleEntity* PAttacker, CBattleEntity* PDefender) -> bool
{
    std::int16_t killerEffect = 0;
    if (const auto killerMod = intimidatehelpers::IntimidateKillerMod(PAttacker->m_EcoSystem))
    {
        killerEffect = PDefender->getMod(*killerMod);
    }

    std::int16_t doubtPower = 0;
    if (CStatusEffect* PDoubtEffect = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Doubt))
    {
        doubtPower = PDoubtEffect->GetPower();
    }

    std::int16_t intimidatePower = 0;
    if (CStatusEffect* PIntimidateEffect = PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Intimidate))
    {
        intimidatePower = PIntimidateEffect->GetPower();
    }

    const auto chance = intimidatehelpers::IntimidateChance(
        PAttacker == PDefender, killerEffect, doubtPower, intimidatePower);
    return intimidatehelpers::IsIntimidated(
        chance, static_cast<std::uint8_t>(xirand::GetRandomNumber(100)));
}

/************************************************************************
 *                                                                       *
 *  Get SkillChain Effect                                                *
 *                                                                       *
 ************************************************************************/
#define PAIR(x, y) (((x) << 8) + (y))

auto GetSkillchainSubeffect(const SKILLCHAIN_ELEMENT skillchain) -> ActionProcSkillChain
{
    if (skillchain < SC_NONE || skillchain > SC_DARKNESS_II)
    {
        ShowWarning("battleutils::GetSkillchainSubeffect() - Invalid Element passed to function.");
        return ActionProcSkillChain::None;
    }

    return skillchaintableshelpers::GetSkillchainSubeffect(static_cast<std::uint8_t>(skillchain));
}

uint8 GetSkillchainTier(SKILLCHAIN_ELEMENT skillchain)
{
    if (skillchain < SC_NONE || skillchain > SC_DARKNESS_II)
    {
        ShowWarning("battleutils::GetSkillchainTier() - Invalid Element passed to function.");
        return 0;
    }

    return skillchaintableshelpers::GetSkillchainTier(static_cast<std::uint8_t>(skillchain));
}

SKILLCHAIN_ELEMENT FormSkillchain(const std::list<SKILLCHAIN_ELEMENT>& resonance, const std::list<SKILLCHAIN_ELEMENT>& skill)
{
    std::list<std::uint8_t> res;
    std::list<std::uint8_t> sk;
    for (const auto& e : resonance)
    {
        res.push_back(static_cast<std::uint8_t>(e));
    }
    for (const auto& e : skill)
    {
        sk.push_back(static_cast<std::uint8_t>(e));
    }
    return static_cast<SKILLCHAIN_ELEMENT>(skillchaintableshelpers::FormSkillchain(res, sk));
}

auto GetSkillChainEffect(const CBattleEntity* PDefender, uint8 primary, uint8 secondary, uint8 tertiary) -> ActionProcSkillChain
{
    CStatusEffect* PSCEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Skillchain, 0);
    CStatusEffect* PCBEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Chainbound, 0);

    const bool hasSC = PSCEffect != nullptr;
    const bool hasCB = PCBEffect != nullptr;
    const bool cbElapsedOK = hasCB && (PCBEffect->GetStartTime() + 2s < timer::now());
    const bool scElapsedOK = hasSC && (PSCEffect->GetStartTime() + 3s < timer::now());

    const auto formFn = [](const std::list<std::uint8_t>& resonance, const std::list<std::uint8_t>& skill) -> std::uint8_t {
        std::list<SKILLCHAIN_ELEMENT> res;
        std::list<SKILLCHAIN_ELEMENT> sk;
        for (const auto e : resonance)
        {
            res.emplace_back(static_cast<SKILLCHAIN_ELEMENT>(e));
        }
        for (const auto e : skill)
        {
            sk.emplace_back(static_cast<SKILLCHAIN_ELEMENT>(e));
        }
        return static_cast<std::uint8_t>(FormSkillchain(res, sk));
    };
    const auto tierFn = [](const std::uint8_t el) -> std::uint8_t {
        return GetSkillchainTier(static_cast<SKILLCHAIN_ELEMENT>(el));
    };

    const auto decision = skillchaineffecthelpers::ResolveSkillChainEffect(
        hasSC,
        hasCB,
        cbElapsedOK,
        scElapsedOK,
        hasCB ? PCBEffect->GetPower() : static_cast<std::uint16_t>(0),
        hasSC ? PSCEffect->GetTier() : static_cast<std::uint8_t>(0),
        hasSC ? PSCEffect->GetPower() : static_cast<std::uint16_t>(0),
        hasSC ? PSCEffect->GetSubPower() : static_cast<std::uint16_t>(0),
        primary,
        secondary,
        tertiary,
        formFn,
        tierFn);

    if (decision.outcome == skillchaineffecthelpers::SkillChainEffectOutcome::OpenInitial)
    {
        PDefender->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Skillchain, 0, decision.combined, 0s, 10s, 0, 0, 0);
        return ActionProcSkillChain::None;
    }

    if (decision.outcome == skillchaineffecthelpers::SkillChainEffectOutcome::EarlyNone && !decision.consumeChainbound)
    {
        if (hasSC && scElapsedOK && PSCEffect->GetTier() == 0 && !PSCEffect->GetPower())
        {
            ShowWarning("PSCEffect Power was 0.");
        }
        return ActionProcSkillChain::None;
    }

    if (decision.ensureSkillchainEffect)
    {
        PDefender->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Skillchain, 0, decision.combined, 0s, 10s, 0, 0, 0);
    }
    if (decision.consumeChainbound)
    {
        PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Chainbound);
    }
    PSCEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Skillchain, 0);

    if (!PSCEffect)
    {
        return ActionProcSkillChain::None;
    }

    Mod resistanceRankMods[] = { Mod::FIRE_RES_RANK, Mod::ICE_RES_RANK, Mod::WIND_RES_RANK, Mod::EARTH_RES_RANK, Mod::THUNDER_RES_RANK, Mod::ICE_RES_RANK, Mod::LIGHT_RES_RANK, Mod::DARK_RES_RANK };

    // Reset the effects resistance rank mods
    for (const auto& resistanceRank : resistanceRankMods)
    {
        PSCEffect->setMod(resistanceRank, 0);
    }

    if (decision.outcome == skillchaineffecthelpers::SkillChainEffectOutcome::Link)
    {
        const auto skillchain = static_cast<SKILLCHAIN_ELEMENT>(decision.formed);

        PSCEffect->SetStartTime(timer::now());
        if (decision.shrinkDurationBy1s)
        {
            PSCEffect->SetDuration(PSCEffect->GetDuration() - 1s);
        }
        PSCEffect->SetTier(decision.newTier);
        PSCEffect->SetPower(decision.newPower);
        // Cap after host Add/readback so Chainbound path uses the fresh effect subPower.
        PSCEffect->SetSubPower(skillchaineffecthelpers::CapLinkedSubPower(PSCEffect->GetSubPower()));

        // Set new resistance rank modifiers
        // https://www.bg-wiki.com/ffxi/Resist#Modifying_Resistance_Rank
        for (auto& element : GetSkillchainMagicElement(skillchain))
        {
            const Mod resistanceRankMod = GetResistanceRankModFromElement(element);
            PSCEffect->setMod(resistanceRankMod, -1);
        }

        return GetSkillchainSubeffect(skillchain);
    }

    // ResetOpen (and EarlyNone after chainbound consumption with no PSC readback)
    PSCEffect->SetStartTime(timer::now());
    PSCEffect->SetDuration(10s);
    PSCEffect->SetTier(decision.newTier);
    PSCEffect->SetPower(decision.newPower);
    PSCEffect->SetSubPower(decision.newSubPower);

    return ActionProcSkillChain::None;
}

std::vector<ELEMENT> GetSkillchainMagicElement(SKILLCHAIN_ELEMENT skillchain)
{
    const auto elems = skillchaintableshelpers::GetSkillchainMagicElement(static_cast<std::uint8_t>(skillchain));
    std::vector<ELEMENT> out;
    out.reserve(elems.size());
    for (const auto e : elems)
    {
        out.push_back(static_cast<ELEMENT>(e));
    }
    return out;
}

Mod GetResistanceRankModFromElement(ELEMENT& element)
{
    if (const auto mod = skillchaintableshelpers::GetResistanceRankModFromElement(static_cast<std::uint8_t>(element)))
    {
        return *mod;
    }
    // LSB used unordered_map::at — invalid element would throw. Preserve fire as safe fallback
    // only if pure table misses (should not happen for valid ELEMENT_*).
    ShowWarning("battleutils::GetResistanceRankModFromElement() - Invalid element.");
    return Mod::FIRE_RES_RANK;
}

auto TakeSkillchainDamage(CBattleEntity* PAttacker, CBattleEntity* PDefender, int32 lastSkillDamage, CBattleEntity* taChar) -> int32
{
    if (PAttacker == nullptr || PDefender == nullptr)
    {
        ShowWarning("battleutils::TakeSkillchainDamage() - PAttacker or PDefender was null.");
        return 0;
    }

    // Pure product floors (skillchain.lua); complex multipliers still Lua-injected.
    skillchaindamagehelpers::ProductParams pp{};
    auto* PSCEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Skillchain, 0);
    pp.hasEffect    = PSCEffect != nullptr;
    if (PSCEffect)
    {
        pp.type  = static_cast<std::uint8_t>(PSCEffect->GetPower());
        pp.level = static_cast<std::uint8_t>(PSCEffect->GetTier());
        pp.count = static_cast<std::uint8_t>(PSCEffect->GetSubPower());
    }
    pp.baseDamage = lastSkillDamage;

    // Element selection: lowest resistance rank among SC magic elements.
    if (pp.hasEffect && pp.type != 0)
    {
        pp.element = skillchaindamagehelpers::SelectElement(
            pp.type,
            [PDefender](std::uint8_t el) -> int {
                const auto elem = static_cast<ELEMENT>(el);
                if (const auto mod = skillchaintableshelpers::GetResistanceRankModFromElement(static_cast<std::uint8_t>(elem)))
                {
                    return PDefender->getMod(*mod);
                }
                return 0;
            });
    }

    // Nullification pure (isMagic=true, isBreath=false).
    if (pp.element != 0)
    {
        using namespace skillchaininjecthelpers;
        const int rollAll = xirand::GetRandomNumber(1, 101);
        const int rollMag = xirand::GetRandomNumber(1, 101);
        const int rollEl  = xirand::GetRandomNumber(1, 101);
        const bool nullAll = RollProc1to100(rollAll, PDefender->getMod(Mod::NULL_DAMAGE));
        const bool nullMag = RollProc1to100(rollMag, PDefender->getMod(Mod::NULL_MAGICAL_DAMAGE));
        const auto nullMod = NullModForElement(pp.element);
        const bool nullEl  = (nullMod != Mod::NONE) && RollProc1to100(rollEl, PDefender->getMod(nullMod));
        pp.nullified       = (NullificationFactor(nullAll, nullMag, nullEl) == 0.0);
    }

    // Multiplier injects pure (parity with skillchain.lua).
    pp.bonusMult   = 1.0 + static_cast<double>(PAttacker->getMod(Mod::SKILLCHAINBONUS)) / 100.0;
    pp.damageMult  = 1.0 + static_cast<double>(PAttacker->getMod(Mod::SKILLCHAINDMG)) / 10000.0;
    pp.magicDamage = PAttacker->getMod(Mod::MAGIC_DAMAGE);

    if (pp.element != 0 && !pp.nullified)
    {
        using namespace skillchaininjecthelpers;

        // Day/weather (alwaysApply=false; 33% random proc or force mods).
        {
            DayWeatherParams dwp{};
            dwp.spellElement         = pp.element;
            dwp.weather              = static_cast<std::uint8_t>(GetWeather(PAttacker, false));
            dwp.dayElement           = static_cast<std::uint8_t>(GetDayElement());
            dwp.alwaysApply          = false;
            dwp.randomProc           = xirand::GetRandomNumber(1, 101) <= 33;
            dwp.forceDWBonusPenalty  = PAttacker->getMod(Mod::FORCE_DW_BONUS_PENALTY) > 0;
            const auto forceMod      = ForceDWBonusModForElement(pp.element);
            dwp.forceElementBonus    = (forceMod != Mod::NONE) && PAttacker->getMod(forceMod) > 0;
            dwp.iridescence          = PAttacker->getMod(Mod::IRIDESCENCE);
            dwp.dayWeatherProcBonus  = PAttacker->getMod(Mod::DAY_WEATHER_PROC_BONUS);
            dwp.dayNukeBonus         = PAttacker->getMod(Mod::DAY_NUKE_BONUS);
            pp.dayWeatherMult        = DayWeatherMultiplier(dwp);
        }

        // Staff / affinity
        {
            const auto staffMod = StaffModForElement(pp.element);
            const auto affMod   = AffinityModForElement(pp.element);
            pp.staffMult    = StaffBonus(pp.element, staffMod != Mod::NONE ? PAttacker->getMod(staffMod) : 0);
            pp.affinityMult = AffinityBonus(pp.element, affMod != Mod::NONE ? PAttacker->getMod(affMod) : 0);
        }

        // Magical damage taken adjustment
        pp.magicTakenMult = MagicalDamageAdjustment(
            PDefender->getMod(Mod::DMG),
            PDefender->getMod(Mod::DMGMAGIC),
            PDefender->getMod(Mod::DMGMAGIC_II),
            PDefender->getMod(Mod::UDMGMAGIC));

        // Absorption (Liement + chance ladder)
        {
            const auto dmgType = static_cast<xi::DamageType>(
                static_cast<std::uint8_t>(xi::DamageType::Elemental) + pp.element);
            const double liement = CheckLiementAbsorb(PDefender, dmgType);
            const int    rollAll = xirand::GetRandomNumber(1, 101);
            const int    rollMag = xirand::GetRandomNumber(1, 101);
            const int    rollEl  = xirand::GetRandomNumber(1, 101);
            const bool   absAll  = RollProc1to100(rollAll, PDefender->getMod(Mod::ABSORB_DMG_CHANCE));
            const bool   absMag  = RollProc1to100(rollMag, PDefender->getMod(Mod::MAGIC_ABSORB));
            const auto   absMod  = AbsorbModForElement(pp.element);
            const bool   absEl   = (absMod != Mod::NONE) && RollProc1to100(rollEl, PDefender->getMod(absMod));
            pp.absorbMult        = AbsorptionFactor(liement, absAll, absMag, absEl);
        }

        // Res-rank for selected element.
        {
            const auto elem = static_cast<ELEMENT>(pp.element);
            if (const auto mod = skillchaintableshelpers::GetResistanceRankModFromElement(static_cast<std::uint8_t>(elem)))
            {
                pp.resRank = PDefender->getMod(*mod);
            }
        }

        // Innin merit on attacker (PC).
        std::int32_t inninMerit = 0;
        if (PAttacker->objtype == TYPE_PC)
        {
            auto* PChar = static_cast<CCharEntity*>(PAttacker);
            inninMerit  = PChar->PMeritPoints->GetMeritValue(MERIT_INNIN_EFFECT, PChar);
        }
        pp.inninMult     = 1.0 + static_cast<double>(inninMerit) / 100.0;
        pp.sengikoriMult = 1.0 + static_cast<double>(PDefender->getMod(Mod::SENGIKORI_SC_DMG_DEBUFF)) / 100.0;
    }

    const auto product = skillchaindamagehelpers::Product(pp);
    int32      damage  = product.damage;

    if (product.consumeSengikori)
    {
        PDefender->setModifier(Mod::SENGIKORI_SC_DMG_DEBUFF, 0);
    }

    if (product.applied)
    {
        if (damage > 0)
        {
            damage = skillchaindamagehelpers::ClampSCDamage(
                skillchaindamagehelpers::ApplyPhalanx(damage, PDefender->getMod(Mod::PHALANX)));
            damage = skillchaindamagehelpers::ClampSCDamage(HandleOneForAll(PDefender, damage));
            damage = skillchaindamagehelpers::ClampSCDamage(HandleStoneskin(PDefender, damage));
            damage = CheckAndApplyDamageCap(damage, PDefender);

            const auto dmgType = static_cast<xi::DamageType>(
                static_cast<std::uint8_t>(xi::DamageType::Elemental) + product.element);
            PDefender->takeDamage(damage, PAttacker, ATTACK_TYPE::SPECIAL, dmgType);
        }
        else
        {
            // Absorb path: addHP of positive amount (Lua: addHP(-finalDamage) with finalDamage ≤ 0).
            PDefender->addHP(-damage);
        }
    }

    battleutils::ClaimMob(PDefender, PAttacker);
    PDefender->updatemask |= UPDATE_STATUS;

    PDefender->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Damage);

    switch (PDefender->objtype)
    {
        case TYPE_PC:
        {
            if (PDefender->animation == ANIMATION_SIT || (PDefender->animation >= ANIMATION_SITCHAIR_0 && PDefender->animation <= ANIMATION_SITCHAIR_10))
            {
                PDefender->animation = ANIMATION_NONE;
                PDefender->updatemask |= UPDATE_HP;
            }
            break;
        }

        case TYPE_MOB:
        {
            static_cast<CMobEntity*>(PDefender)->PEnmityContainer->UpdateEnmityFromDamage(taChar ? taChar : PAttacker, std::abs(damage));
            break;
        }

        default:
        {
            break;
        }
    }

    return damage;
}

CItemEquipment* GetEntityArmor(CBattleEntity* PEntity, SLOTTYPE Slot)
{
    if (!entityequiphelpers::IsValidArmorSlot(static_cast<std::uint8_t>(Slot)))
    {
        ShowWarning("Invalid Slot Type (%d) passed to function.", static_cast<uint8>(Slot));
        return nullptr;
    }

    if (entityequiphelpers::ShouldReturnPCArmor(PEntity->objtype == TYPE_PC))
    {
        return (((CCharEntity*)PEntity)->getEquip(Slot));
    }

    return nullptr;
}

CItemWeapon* GetEntityWeapon(CBattleEntity* PEntity, SLOTTYPE Slot)
{
    if (!entityequiphelpers::IsValidWeaponSlot(static_cast<std::uint8_t>(Slot)))
    {
        ShowWarning("battleutils::GetEntityWeapon() - Received invalid slot type.");
        return nullptr;
    }

    return dynamic_cast<CItemWeapon*>(PEntity->m_Weapons[Slot]);
}

void MakeEntityStandUp(CBattleEntity* PEntity)
{
    if (PEntity == nullptr)
    {
        ShowWarning("battleutils::MakeEntityStandUp() - PEntity was null.");
        return;
    }

    const auto action = entityactionhelpers::ClassifyStandUp(
        PEntity->objtype == TYPE_PC, static_cast<uint8>(PEntity->animation));
    if (action == entityactionhelpers::StandUpAction::None)
    {
        return;
    }

    CCharEntity* PPlayer = static_cast<CCharEntity*>(PEntity);
    if (action == entityactionhelpers::StandUpAction::CancelHealing)
    {
        PPlayer->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Healing);
        PPlayer->updatemask |= UPDATE_HP;
        return;
    }

    // LeaveSit
    PPlayer->animation = ANIMATION_NONE;
    PPlayer->updatemask |= UPDATE_HP;

    CPetEntity* PPet = dynamic_cast<CPetEntity*>(PPlayer->PPet);
    if (entityactionhelpers::ShouldStandUpPet(
            PPet != nullptr, PPet ? static_cast<uint8>(PPet->getPetType()) : static_cast<uint8>(0)))
    {
        PPet->animation = ANIMATION_NONE;
        PPet->updatemask |= UPDATE_HP;
    }
}

/************************************************************************
 *                                                                       *
 *  Handle NIN tool usage                                                *
 *  (for all entities except characters, default to true)                *
 *                                                                       *
 ************************************************************************/

bool HasNinjaTool(CBattleEntity* PEntity, CSpell* PSpell, bool ConsumeTool)
{
    if (PEntity == nullptr || PSpell == nullptr)
    {
        ShowWarning("battleutils::HasNinjaTool() - PEntity or PSpell was null.");
        return false;
    }

    if (PEntity->objtype != TYPE_PC)
    {
        return ninjatoolhelpers::NonPCAlwaysHasTool();
    }

    CCharEntity* PChar = static_cast<CCharEntity*>(PEntity);

    const uint16 preferred = PSpell->getMPCost();
    uint8        SlotID    = 0;
    const bool   preferredAvailable =
        ERROR_SLOTID != (SlotID = PChar->getStorage(LOC_INVENTORY)->SearchItem(preferred));

    bool         substituteAvailable = false;
    std::uint16_t substituteID       = 0;
    if (!preferredAvailable && PChar->GetMJob() == JOB_NIN)
    {
        if (const auto sub = ninjatoolhelpers::Substitute(preferred))
        {
            substituteID = *sub;
            substituteAvailable =
                ERROR_SLOTID != (SlotID = PChar->getStorage(LOC_INVENTORY)->SearchItem(substituteID));
        }
    }

    const auto resolved = ninjatoolhelpers::ResolveWithSubstitute(
        preferred,
        preferredAvailable,
        PChar->GetMJob() == JOB_NIN,
        substituteAvailable);

    if (!resolved.ok)
    {
        return false;
    }

    // SlotID already holds the inventory slot of the resolved tool.
    const uint16 toolID = resolved.toolID;

    const bool hasFutae = PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Futae);

    std::uint16_t meritBonus = 0;
    if (charutils::hasTrait(PChar, TRAIT_NINJA_TOOL_EXPERT))
    {
        meritBonus = static_cast<std::uint16_t>(
            PChar->PMeritPoints->GetMeritValue(MERIT_NINJA_TOOL_EXPERTISE, PChar));
    }
    const auto chance = ninjatoolhelpers::ExpertiseChance(
        static_cast<std::uint16_t>(PChar->getMod(Mod::NINJA_TOOL)), meritBonus);

    if (ConsumeTool)
    {
        const int roll = xirand::GetRandomNumber(100);
        // Futae path ignores expertise roll; expertise path uses roll.
        // ConsumeQty handles both: for Futae+wheel returns 2 without consulting roll.
        const int qty = ninjatoolhelpers::ConsumeQty(toolID, hasFutae, chance, roll);
        if (qty > 0)
        {
            charutils::UpdateItem(PChar, LOC_INVENTORY, SlotID, static_cast<int16>(-qty));
            PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
        }
    }

    return true;
}

/*
    pass result of worldAngle(anchorEntity, firstEntity) instead of calculating everytime to allow TA to be more efficient
    Note the order of worldAngle calls, the anchor must be first in all comparisons
    Calculates world angle between other entity and anchor entity,
    then determines if the difference of those angles is within acceptable range for moves that require the three to be "in a straight line"
    Used for Trick Attack and Cover: separate checks for distance/party membership must be done to confirm eligability
*/
inline bool areInLine(uint8 firstEntityWorldAngle, CBattleEntity* anchorEntity, CBattleEntity* otherEntity)
{
    // X-degree angle threshold, centered on the firstPlayer's world angle
    // X = 10 => 10/2 * 255 / 360 ~ 3.5 rotation diff, rounded to 4 which really gives X~11.3
    int16 angleDiff = angleDifference(firstEntityWorldAngle, worldAngle(anchorEntity->loc.p, otherEntity->loc.p));

    // Useful for debugging if trick attack/cover aren't reliably calculating eligability, but chatty otherwise
    // ShowDebug("InLine check angleDiff: %d", angleDiff);

    return trickattackhelpers::AreInLineFromDiff(angleDiff);
}

/*
 *  Find if any party members are in position for trick attack.  Do this by comparing the world angle between:
 *  1. the TA user and the TA target
 *  2. the TA user and the Mob
 *  First, build a list of players that are closer to the mob than the TA user,
 *   then sort by distance and choose the first that succeeds in meeting the criteria of areInline function
 */

CBattleEntity* getAvailableTrickAttackChar(CBattleEntity* taUser, CBattleEntity* PMob)
{
    TracyZoneScoped;

    if (!trickattackhelpers::ShouldEvaluateTrickAttack(
            taUser->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::TrickAttack)))
    {
        return nullptr;
    }

    // angle and distance between mob and TA user
    uint8                                         angleTAmob = worldAngle(PMob->loc.p, taUser->loc.p);
    auto                                          distTAmob  = distance(taUser->loc.p, PMob->loc.p);
    std::vector<std::pair<float, CBattleEntity*>> taTargetList;

    if (taUser->PParty != nullptr)
    {
        // Collect all potential TA targets who are closer to the mob than the TA user

        // clang-format off
            taUser->ForAlliance([&PMob, distTAmob, &taTargetList](CBattleEntity* PMember)
            {
                float distTAtarget = distance(PMember->loc.p, PMob->loc.p);
                // require closer target not be closer than .5 yalms (.5*.5=.25 distsquared) to mob
                if (trickattackhelpers::TrickAttackCandidateDistanceOK(distTAtarget, distTAmob))
                {
                    taTargetList.emplace_back(distTAtarget, PMember);
                }

                if (auto* PChar = dynamic_cast<CCharEntity*>(PMember))
                {
                    for (auto* PTrust : PChar->PTrusts)
                    {
                        distTAtarget = distance(PTrust->loc.p, PMob->loc.p);
                        // require closer target not be closer than .5 yalms (.5*.5=.25 distsquared) to mob
                        if (trickattackhelpers::TrickAttackCandidateDistanceOK(distTAtarget, distTAmob))
                        {
                            taTargetList.emplace_back(distTAtarget, PTrust);
                        }
                    }
                }
            });
        // clang-format on
    }

    // Check TA user's fellow
    /*
    if (auto* PChar = dynamic_cast<CCharEntity*>(taUser))
    {
        if (PChar->PFellow)
        {
            if (auto* fellow = dynamic_cast<CBattleEntity*>(PChar->PFellow))
            {
                float distTAtarget = distance(fellow->loc.p, PMob->loc.p);
                // require closer target not be closer than .5 yalms (.5*.5=.25 distsquared) to mob
                if (trickattackhelpers::TrickAttackCandidateDistanceOK(distTAtarget, distTAmob))
                {
                    taTargetList.emplace_back(distTAtarget, fellow);
                }
            }
        }
    }
    */

    if (!taTargetList.empty())
    {
        // sorts by distance then by pointer id (only if floats are equal)
        std::sort(taTargetList.begin(), taTargetList.end());
        for (const auto& [dist, potentialTAtarget] : taTargetList)
        {
            if (trickattackhelpers::TrickAttackCandidateSkip(
                    taUser->id == potentialTAtarget->id, potentialTAtarget->isDead()))
            {
                continue;
            }

            if (areInLine(angleTAmob, PMob, potentialTAtarget))
            {
                return potentialTAtarget;
            }
        }
    }

    // No Trick attack party member available
    return nullptr;
}

/************************************************************************
 *                                                                       *
 *  Add enmity to PSource for all the MOB targets who have               *
 *  PTarget on their enmity list.                                        *
 *                                                                       *
 ************************************************************************/

void GenerateCureEnmity(CBattleEntity* PSource, CBattleEntity* PTarget, int32 amount, int32 fixedCE, int32 fixedVE)
{
    if (!traitsenmityhelpers::ShouldGenerateCureEnmity(PSource != nullptr, PTarget != nullptr))
    {
        ShowWarning("battleutils::GenerateCureEnmity - PSource or PTarget was null.");
        return;
    }

    for (auto* PEntity : *PTarget->PNotorietyContainer)
    {
        CMobEntity* PCurrentMob = dynamic_cast<CMobEntity*>(PEntity);
        if (traitsenmityhelpers::ShouldUpdateCureEnmity(
                PCurrentMob != nullptr,
                PCurrentMob ? PCurrentMob->m_HiPCLvl : static_cast<uint8>(0),
                PCurrentMob && PCurrentMob->PEnmityContainer->HasID(PTarget->id)))
        {
            PCurrentMob->PEnmityContainer->UpdateEnmityFromCure(PSource, PTarget->GetMLevel(), amount, fixedCE, fixedVE);
        }
    }
}

// Generate enmity for all targets in range
void GenerateInRangeEnmity(CBattleEntity* PSource, int32 CE, int32 VE)
{
    if (!traitsenmityhelpers::ShouldGenerateInRangeEnmity(PSource != nullptr))
    {
        ShowWarning("battleutils::GenerateInRangeEnmity() - PSource received as null.");
        return;
    }

    bool useSource = false;
    bool useMaster = false;
    traitsenmityhelpers::ResolveInRangeEnmitySource(
        PSource->objtype == TYPE_PC,
        PSource->PMaster != nullptr,
        PSource->PMaster && PSource->PMaster->objtype == TYPE_PC,
        useSource,
        useMaster);

    CCharEntity* PIterSource = nullptr;
    if (useSource)
    {
        PIterSource = static_cast<CCharEntity*>(PSource);
    }
    else if (useMaster)
    {
        PIterSource = static_cast<CCharEntity*>(PSource->PMaster);
    }

    if (PIterSource)
    {
        FOR_EACH_PAIR_CAST_SECOND(CMobEntity*, PCurrentMob, PIterSource->SpawnMOBList)
        {
            if (traitsenmityhelpers::ShouldUpdateInRangeEnmity(
                    PCurrentMob->m_HiPCLvl, PCurrentMob->PEnmityContainer->HasID(PSource->id)))
            {
                PCurrentMob->PEnmityContainer->UpdateEnmity(PSource, CE, VE, false, false, false);
            }
        }
    }
}

// handle "type 1" enmity reset
void handleKillshotEnmity(CBattleEntity* PAttacker, CBattleEntity* PTarget)
{
    // Handle killshot enmity reset if applicable
    if (PAttacker->objtype == TYPE_MOB && PTarget && PTarget->isDead())
    {
        auto* PMob     = static_cast<CMobEntity*>(PAttacker);
        auto* PHighest = PMob->PEnmityContainer->GetHighestEnmity();
        if (enmitycombathelpers::ShouldClearKillshotEnmity(
                true, true, true, PHighest != nullptr, PHighest && PHighest->targid == PTarget->targid))
        {
            PMob->PEnmityContainer->Clear(PTarget->id);
        }
    }
}

void handleSecondaryTargetEnmity(CBattleEntity* PAttacker, CBattleEntity* PTarget)
{
    const auto action = enmitycombathelpers::ClassifySecondaryTargetEnmity(
        PAttacker->objtype == TYPE_MOB, PTarget != nullptr, PTarget && PTarget->isDead());
    if (action == enmitycombathelpers::SecondaryTargetEnmityAction::None)
    {
        return;
    }
    auto* PMob = static_cast<CMobEntity*>(PAttacker);
    // Secondary targets won't get targeted anymore if they were killed from this action
    // Inactive targets will get set back to active if hit (and not dead)
    PMob->PEnmityContainer->SetActive(PTarget->id, action == enmitycombathelpers::SecondaryTargetEnmityAction::Activate);
}

/************************************************************************
 *                                                                       *
 *  Transfer Enmity (used with ACCOMPLICE & COLLABORATOR ability type)   *
 *                                                                       *
 ************************************************************************/

void TransferEnmity(CBattleEntity* PHateReceiver, CBattleEntity* PHateGiver, CMobEntity* PMob, uint8 percentToTransfer)
{
    // Ensure the players have a battle target..
    if (!enmitycombathelpers::ShouldTransferEnmity(PMob != nullptr, PMob && PMob->PEnmityContainer != nullptr))
    {
        return;
    }

    PMob->PEnmityContainer->LowerEnmityByPercent(PHateGiver, percentToTransfer, PHateReceiver);
}

/************************************************************************
 *                                                                       *
 *  Handle Soul Eater effect                                             *
 *                                                                       *
 ************************************************************************/

uint16 doSoulEaterEffect(CCharEntity* m_PChar, uint32 damage)
{
    if (m_PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Souleater))
    {
        const float bonusDamage = combatbonustailshelpers::SoulEaterBonusDamage(
            m_PChar->health.hp,
            m_PChar->getMaxGearMod(Mod::SOULEATER_EFFECT),
            m_PChar->getMod(Mod::SOULEATER_EFFECT_II));

        if (bonusDamage >= 1)
        {
            const float costScale = combatbonustailshelpers::SoulEaterHPCostScale(
                m_PChar->getMod(Mod::STALWART_SOUL));
            m_PChar->addHP(-HandleStoneskin(m_PChar, static_cast<int32>(bonusDamage * costScale)));

            damage = combatbonustailshelpers::ApplySoulEaterToDamage(
                damage, bonusDamage, m_PChar->GetMJob() == JOB_DRK);
        }
    }
    return damage;
}

uint16 doConsumeManaEffect(CCharEntity* m_PChar)
{
    const bool hasEffect = m_PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::ConsumeMana);
    const auto bonusDmg  = combatbonustailshelpers::ConsumeManaBonus(hasEffect, m_PChar->health.mp);
    if (hasEffect)
    {
        m_PChar->health.mp = 0;
        m_PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::ConsumeMana);
    }
    return static_cast<uint16>(bonusDmg);
}

/************************************************************************
 *                                                                       *
 *  Calculate Samurai Store TP value (from merit)                        *
 *                                                                       *
 ************************************************************************/

uint8 getStoreTPbonusFromMerit(CBattleEntity* PEntity)
{
    const bool isPC      = PEntity->objtype == TYPE_PC;
    const bool isSAMMain = isPC && static_cast<CCharEntity*>(PEntity)->GetMJob() == JOB_SAM;
    const std::int16_t merit = isSAMMain
        ? static_cast<std::int16_t>(static_cast<CCharEntity*>(PEntity)->PMeritPoints->GetMeritValue(
              MERIT_STORE_TP_EFFECT, static_cast<CCharEntity*>(PEntity)))
        : 0;
    return combatstatusmitigationhelpers::StoreTPBonusFromMerit(isPC, isSAMMain, merit);
}

/************************************************************************
 *                                                                       *
 *  Calculate Samurai Overwhelm damage bonus                             *
 *                                                                       *
 ************************************************************************/

int32 getOverWhelmDamageBonus(CBattleEntity* PAttacker, CBattleEntity* PDefender, int32 damage)
{
    if (auto PChar = dynamic_cast<CCharEntity*>(PAttacker)) // Some mobskills use TakeWeaponskillDamage function, which calls upon this one.
    {
        const bool inFront   = infront(PChar->loc.p, PDefender->loc.p, 64);
        const uint8 meritCount = PChar->PMeritPoints->GetMeritValue(MERIT_OVERWHELM, PChar);
        damage = combatbonustailshelpers::OverwhelmDamageBonus(damage, meritCount, inFront);
    }
    return damage;
}

uint8 getBarrageShotCount(CBattleEntity* PBattleEntity)
{
    /*
    Ranger level 30, four shots.
    Ranger level 50, five shots.
    Ranger level 75, six shots.
    Ranger level 90, seven shots.
    Ranger level 99, eight shots.
    */

    // TODO: verify all RNG trusts that use Barrage have RNG main job
    const auto lvl = barragehelpers::BarrageLevelForJob(
        static_cast<std::uint8_t>(PBattleEntity->GetMJob()),
        PBattleEntity->GetMLevel(),
        PBattleEntity->GetSLevel());
    // Level gate before BARRAGE_COUNT (matches original early return when lvl < 30).
    if (lvl < 30)
    {
        return 0;
    }
    auto shotCount = barragehelpers::BarrageShotCount(lvl);

    shotCount += PBattleEntity->getMod(Mod::BARRAGE_COUNT);

    // only archery + marksmanship can use barrage
    if (PBattleEntity->objtype == TYPE_PC)
    {
        if (auto* PChar = dynamic_cast<CCharEntity*>(PBattleEntity); PChar)
        {
            CItemWeapon* PItem = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_RANGED));

            if (!barragehelpers::BarrageSkillOK(
                    PItem != nullptr,
                    PItem ? static_cast<std::uint8_t>(PItem->getSkillType()) : static_cast<std::uint8_t>(0)))
            {
                return 0;
            }

            // make sure we have enough ammo for all these shots
            CItemWeapon* PAmmo = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_AMMO));

            // TODO: Check if this should be here. Recycle can proc and potentially allow more shots to land
            // This function is additive to the first shot. So one ammo is already consumed before we get here
            shotCount = barragehelpers::BarrageAmmoClamp(
                shotCount,
                PAmmo != nullptr,
                PAmmo ? static_cast<std::uint8_t>(PAmmo->getQuantity()) : static_cast<std::uint8_t>(0));
        }
    }

    return shotCount;
}

void applyCharm(CBattleEntity* PCharmer, CBattleEntity* PVictim, timer::duration charmTime)
{
    PVictim->isCharmed = true;

    if (PVictim->objtype == TYPE_MOB)
    {
        CCharEntity* PChar = dynamic_cast<CCharEntity*>(PCharmer);
        CMobEntity*  PMob  = dynamic_cast<CMobEntity*>(PVictim);
        PVictim->PMaster   = PCharmer;
        PCharmer->PPet     = PVictim;

        // make the mob disengage
        if (PCharmer->PPet->PAI->IsEngaged())
        {
            PCharmer->PPet->PAI->Disengage();
        }

        // clear the victims emnity list
        PMob->PEnmityContainer->Clear();

        // set the mobs ai to petAi
        PCharmer->PPet->PAI->SetController(std::make_unique<CPetController>(PMob));
        PCharmer->PPet->charmTime = timer::now() + charmTime;

        // this will make him transition back to roaming if sleeping
        PCharmer->PPet->animation = ANIMATION_NONE;
        PCharmer->updatemask |= UPDATE_HP;

        if (PChar)
        {
            charutils::BuildingCharAbilityTable(PChar);
            std::memset(&PChar->m_PetCommands, 0, sizeof(PChar->m_PetCommands));
            PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
            PChar->pushPacket<CCharStatusPacket>(PChar);
            PChar->pushPacket<CPetSyncPacket>(PChar);
        }
        // clang-format off
            PCharmer->ForAlliance([&PVictim](CBattleEntity* PMember)
            {
                if (static_cast<CCharEntity*>(PMember)->PClaimedMob == PVictim)
                {
                    static_cast<CCharEntity*>(PMember)->PClaimedMob = nullptr;
                }
            });
        // clang-format on
        PMob->m_OwnerID.clean();
        PVictim->updatemask |= UPDATE_STATUS;
    }

    else if (PVictim->objtype == TYPE_PC)
    {
        if (PVictim->PPet)
        {
            petutils::DespawnPet(PVictim);
        }

        static_cast<CCharEntity*>(PVictim)->ClearTrusts();

        PVictim->PAI->SetController(std::make_unique<CPlayerCharmController>(static_cast<CCharEntity*>(PVictim)));

        battleutils::RelinquishClaim(static_cast<CCharEntity*>(PVictim));
        PVictim->PMaster = PCharmer;
        PVictim->updatemask |= UPDATE_ALL_CHAR;

        // Prevent auto attacks for a little bit to simulate retail
        // On retail, you don't engage for a little bit, which we have no mechanism for yet
        // TODO: implement the delays on engage (also applies to mobs) and verify exact timings for those things.
        PVictim->PAI->Inactive(5000ms, false);
    }
    PVictim->allegiance = PCharmer->allegiance;
    PVictim->updatemask |= UPDATE_HP;
}

void unCharm(CBattleEntity* PEntity)
{
    if (PEntity->objtype == TYPE_PC)
    {
        PEntity->isCharmed = false;
        PEntity->PAI->SetController(std::make_unique<CPlayerController>(static_cast<CCharEntity*>(PEntity)));

        PEntity->PMaster = nullptr;
        if (PEntity->PAI->IsEngaged())
        {
            PEntity->PAI->Disengage();
        }
        PEntity->updatemask |= UPDATE_ALL_CHAR;
    }
}

void ClaimMob(CBattleEntity* PDefender, CBattleEntity* PAttacker, bool passing)
{
    TracyZoneScoped;

    const bool defenderIsMob = PDefender != nullptr && PDefender->objtype == TYPE_MOB;
    if (claimhelpers::ClassifyClaimMobEarly(defenderIsMob, false, true, false, claimhelpers::ClaimType::Exclusive, false) ==
            claimhelpers::ClaimMobEarlyAction::SkipNonMob ||
        !defenderIsMob)
    { // Do not try to claim anything but mobs (trusts, pets, players don't count)
        return;
    }

    if (PDefender->allegiance == PAttacker->allegiance)
    { // mobs that are allied with the attacker do not need to be claimed and will not update enmity
        return;
    }

    if (auto* mob = dynamic_cast<CMobEntity*>(PDefender))
    {
        CBattleEntity* original      = PAttacker;
        const bool     attackerIsPC  = PAttacker->objtype == TYPE_PC;
        const bool     hasPCMaster   = PAttacker->PMaster && PAttacker->PMaster->objtype == TYPE_PC;
        if (!attackerIsPC)
        {
            if (hasPCMaster)
            { // claim by master
                PAttacker = PAttacker->PMaster;
            }
            else
            {
                return;
            }
        }
        CBattleEntity* battleTarget = original->GetBattleTarget();
        if (claimhelpers::ShouldUpdateEnmityOnClaim(passing))
        {
            mob->PEnmityContainer->UpdateEnmity(original, 0, 0, true, true);
        }

        if (claimhelpers::ClassifyClaimMobEarly(
                true, false, true, true, static_cast<claimhelpers::ClaimType>(mob->getMobMod(MOBMOD_CLAIM_TYPE)), true) ==
            claimhelpers::ClaimMobEarlyAction::SkipUnclaimable)
        {
            return;
        }

        if (PAttacker)
        {
            CCharEntity* attacker = static_cast<CCharEntity*>(PAttacker);
            if (claimhelpers::ShouldDirtyExpOnClaim(passing))
            {
                battleutils::DirtyExp(PDefender, PAttacker);
            }
            const bool btAllows = claimhelpers::BattleTargetAllowsClaim(
                battleTarget != nullptr,
                battleTarget == PDefender,
                battleTarget != nullptr && battleTarget == attacker->PClaimedMob,
                PDefender->isDead());
            if (btAllows)
            {
                if (claimhelpers::ShouldUnclaimPreviousMob(
                        PDefender->isAlive(),
                        attacker->PClaimedMob != nullptr,
                        attacker->PClaimedMob == PDefender,
                        attacker->PClaimedMob && attacker->PClaimedMob->isAlive(),
                        attacker->PClaimedMob ? attacker->PClaimedMob->m_OwnerID.id : 0,
                        attacker->id))
                { // unclaim any other living mobs owned by attacker
                    static_cast<CMobController*>(attacker->PClaimedMob->PAI->GetController())->TapDeclaimTime();
                    attacker->PClaimedMob = nullptr;
                }
                const auto path = claimhelpers::ClassifyClaimOwnership(
                    true, mob->GetCallForHelpFlag(), battleutils::HasClaim(PAttacker, PDefender), PDefender->isDead());
                switch (path)
                {
                    case claimhelpers::ClaimOwnershipPath::CFHBlocked:
                    case claimhelpers::ClaimOwnershipPath::SkipBattleTarget:
                        break;
                    case claimhelpers::ClaimOwnershipPath::AllianceUpdate:
                    { // mob is currently claimed by your alliance, update ownership
                        mob->m_OwnerID.id     = PAttacker->id;
                        mob->m_OwnerID.targid = PAttacker->targid;
                        if (claimhelpers::ShouldAssignPClaimedMob(PDefender->isAlive()))
                        { // ignore killing blow
                            mob->updatemask |= UPDATE_STATUS;
                            attacker->PClaimedMob = PDefender;
                        }
                        break;
                    }
                    case claimhelpers::ClaimOwnershipPath::KillingBlowUnclaimed:
                    { // always give rewards on the killing blow
                        mob->m_OwnerID.id     = PAttacker->id;
                        mob->m_OwnerID.targid = PAttacker->targid;
                        return;
                    }
                    case claimhelpers::ClaimOwnershipPath::HighestEnmityScan:
                    {
                        CBattleEntity* highestClaim = mob->PEnmityContainer->GetHighestEnmity();
                        if (highestClaim && highestClaim->objtype == TYPE_TRUST)
                        {
                            highestClaim = static_cast<CTrustEntity*>(highestClaim)->PMaster;
                        }
                        // clang-format off
                            PAttacker->ForAlliance([&](CBattleEntity* PMember)
                            {
                                if (claimhelpers::HighestEnmityAllowsClaim(
                                        highestClaim != nullptr, highestClaim == PMember, highestClaim == PMember->PPet))
                                { // someone in your alliance is top of hate list, claim for your alliance
                                    mob->m_OwnerID.id     = PAttacker->id;
                                    mob->m_OwnerID.targid = PAttacker->targid;
                                    if (claimhelpers::ShouldAssignPClaimedMob(PDefender->isAlive()))
                                    { // ignore killing blow
                                        mob->updatemask |= UPDATE_STATUS;
                                        attacker->PClaimedMob = PDefender;
                                    }
                                }
                            });
                        // clang-format on
                        break;
                    }
                }
            }
        }
    }
}

void DirtyExp(CBattleEntity* PDefender, CBattleEntity* PAttacker)
{
    if (PDefender->objtype == TYPE_MOB)
    {
        CMobEntity* mob = static_cast<CMobEntity*>(PDefender);
        if (PAttacker->objtype != TYPE_PC)
        {
            if (PAttacker->PMaster && PAttacker->PMaster->objtype == TYPE_PC)
            {
                PAttacker = PAttacker->PMaster;
            }
            else
            {
                PAttacker = nullptr;
            }
        }
        if (PAttacker)
        {
            std::vector<claimhelpers::DirtyExpMember> members;
            // clang-format off
                PAttacker->ForAlliance([&members, &mob](CBattleEntity* PMember)
                {
                    claimhelpers::DirtyExpMember m{};
                    m.sameZone = PMember->getZone() == mob->getZone();
                    m.inRange  = distance(PMember->loc.p, mob->loc.p) < claimhelpers::DirtyExpDistance;
                    m.mLevel   = PMember->GetMLevel();
                    members.push_back(m);
                });
            // clang-format on
            std::uint8_t hiParty = 0;
            std::uint8_t hiPCLvl = 0;
            claimhelpers::DirtyExpMerge(mob->m_HiPartySize, mob->m_HiPCLvl, members.data(), members.size(), hiParty, hiPCLvl);
            mob->m_HiPartySize = hiParty;
            mob->m_HiPCLvl     = hiPCLvl;
        }
    }
}

void RelinquishClaim(CCharEntity* PChar)
{
    CBattleEntity* mob = PChar->PClaimedMob;
    if (claimhelpers::ShouldRelinquishOwnedMob(mob != nullptr, mob && mob->isAlive(), mob ? mob->m_OwnerID.id : 0, PChar->id))
    { // if we currently own a mob
        bool found = false;
        // clang-format off
            static_cast<CBattleEntity*>(PChar)->ForAlliance([&PChar, &mob, &found](CBattleEntity* PMember)
            {
                CCharEntity* member = static_cast<CCharEntity*>(PMember);
                if (claimhelpers::RelinquishPassCandidate(
                        member == PChar,
                        found,
                        member->getZone() == PChar->getZone(),
                        member->isAlive(),
                        member->PClaimedMob == nullptr,
                        member->PClaimedMob == mob))
                { // check if we can pass claim to someone else
                    found = true;
                    battleutils::ClaimMob(mob, PMember, true);
                }
            });
        // clang-format on
        if (!found)
        { // if mob didn't pass to someone else, unclaim it
            static_cast<CMobController*>(mob->PAI->GetController())->TapDeclaimTime();
        }
    }
    PChar->PClaimedMob = nullptr;
}

// Checks to see if the mob has a damage cap value
// This is used for instances like Suttung, Antaeus, Crustacean Conundrum bcnm, Colonization reives
int32 CheckAndApplyDamageCap(int32 damage, CBattleEntity* PDefender)
{
    const int32 damageCap     = PDefender->getMod(Mod::RECEIVED_DAMAGE_CAP);
    int32       damageVariant = PDefender->getMod(Mod::RECEIVED_DAMAGE_VARIANT);

    if (damageCap == 0 || damage < damageCap)
    {
        return damage;
    }

    if (damageVariant > damageCap)
    {
        ShowWarning("battleutils::CheckAndApplyDamageCap - RECEIVED_DAMAGE_VARIANT is > than RECEIVED_DAMAGE_CAP");
        damageVariant = 0;
    }

    // [min, max) on RNG excludes max → add +1 for inclusive variant range
    const int32 subtract = xirand::GetRandomNumber<int32>(0, damageVariant + 1);
    return combatstatusmitigationhelpers::CheckAndApplyDamageCap(damage, damageCap, damageVariant, subtract);
}

// TODO: Study using lua functions.
auto MagicDmgTaken(CBattleEntity* PDefender, int32 damage, ELEMENT element) -> int32
{
    Mod absorb[8]    = { Mod::FIRE_ABSORB, Mod::ICE_ABSORB, Mod::WIND_ABSORB, Mod::EARTH_ABSORB, Mod::LTNG_ABSORB, Mod::WATER_ABSORB, Mod::LIGHT_ABSORB, Mod::DARK_ABSORB };
    Mod nullarray[8] = { Mod::FIRE_NULL, Mod::ICE_NULL, Mod::WIND_NULL, Mod::EARTH_NULL, Mod::LTNG_NULL, Mod::WATER_NULL, Mod::LIGHT_NULL, Mod::DARK_NULL };

    xi::DamageType damageType = static_cast<xi::DamageType>((uint8)xi::DamageType::Elemental + (uint8)element);

    // Liement here
    float liement = CheckLiementAbsorb(PDefender, damageType);
    if (dmgtakenhelpers::ShouldLiementShortCircuit(liement))
    {
        return dmgtakenhelpers::ApplyLiement(damage, liement);
    }

    damage = dmgtakenhelpers::MagicResist(
        damage,
        PDefender->getMod(Mod::UDMGMAGIC),
        PDefender->getMod(Mod::DMGMAGIC),
        PDefender->getMod(Mod::DMG),
        PDefender->getMod(Mod::DMGMAGIC_II));

    // Absorb/null with short-circuit RNG order matching original || chain.
    bool absorbAll     = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::ABSORB_DMG_CHANCE);
    bool absorbMagic   = false;
    bool absorbElement = false;
    if (!absorbAll)
    {
        absorbMagic = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::MAGIC_ABSORB);
        if (!absorbMagic && element)
        {
            absorbElement = xirand::GetRandomNumber(100) < PDefender->getMod(absorb[element - 1]);
        }
    }

    bool nullAll     = false;
    bool nullMagic   = false;
    bool nullElement = false;
    const auto absorbOutcome = dmgtakenhelpers::MagicalOutcome(
        absorbAll, absorbMagic, absorbElement, element != 0, false, false, false);
    if (absorbOutcome == dmgtakenhelpers::AbsorbNullOutcome::Pass)
    {
        nullAll = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_DAMAGE);
        if (!nullAll)
        {
            nullMagic = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_MAGICAL_DAMAGE);
            if (!nullMagic && element)
            {
                nullElement = xirand::GetRandomNumber(100) < PDefender->getMod(nullarray[element - 1]);
            }
        }
    }

    const auto outcome = dmgtakenhelpers::MagicalOutcome(
        absorbAll, absorbMagic, absorbElement, element != 0, nullAll, nullMagic, nullElement);

    if (outcome == dmgtakenhelpers::AbsorbNullOutcome::Pass)
    {
        damage = HandleSevereDamage(PDefender, damage, false);
    }
    else
    {
        damage = dmgtakenhelpers::ApplyAbsorbNull(damage, outcome);
    }

    damage = CheckAndApplyDamageCap(damage, PDefender);

    return damage;
}

auto PhysicalDmgTaken(CBattleEntity* PDefender, int32 damage, xi::DamageType damageType, bool IsCovered) -> int32
{
    damage = dmgtakenhelpers::PhysicalResist(
        damage,
        PDefender->getMod(Mod::UDMGPHYS),
        PDefender->getMod(Mod::DMGPHYS),
        PDefender->getMod(Mod::DMG),
        PDefender->getMod(Mod::DMGPHYS_II),
        PDefender->getMod(Mod::AUTO_EQUALIZER),
        PDefender->GetMaxHP());

    bool absorbAll  = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::ABSORB_DMG_CHANCE);
    bool absorbPhys = false;
    if (!absorbAll)
    {
        absorbPhys = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::PHYS_ABSORB);
    }

    bool nullAll  = false;
    bool nullPhys = false;
    if (dmgtakenhelpers::PhysicalOutcome(absorbAll, absorbPhys, false, false) == dmgtakenhelpers::AbsorbNullOutcome::Pass)
    {
        nullAll = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_DAMAGE);
        if (!nullAll)
        {
            nullPhys = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_PHYSICAL_DAMAGE);
        }
    }

    const auto outcome = dmgtakenhelpers::PhysicalOutcome(absorbAll, absorbPhys, nullAll, nullPhys);
    if (outcome == dmgtakenhelpers::AbsorbNullOutcome::Pass)
    {
        damage = HandleSevereDamage(PDefender, damage, true);

        ConvertDmgToMP(PDefender, damage, IsCovered);

        damage = HandleFanDance(PDefender, damage);
    }
    else
    {
        damage = dmgtakenhelpers::ApplyAbsorbNull(damage, outcome);
    }

    damage = CheckAndApplyDamageCap(damage, PDefender);

    return damage;
}

auto RangedDmgTaken(CBattleEntity* PDefender, int32 damage, xi::DamageType damageType, bool IsCovered) -> int32
{
    damage = dmgtakenhelpers::RangedResist(
        damage,
        PDefender->getMod(Mod::UDMGRANGE),
        PDefender->getMod(Mod::DMGRANGE),
        PDefender->getMod(Mod::DMG),
        PDefender->getMod(Mod::AUTO_EQUALIZER),
        PDefender->GetMaxHP());

    // TODO: Consider new modifier for ranged specific absorb (PHYS_ABSORB used today).
    bool absorbAll  = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::ABSORB_DMG_CHANCE);
    bool absorbPhys = false;
    if (!absorbAll)
    {
        absorbPhys = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::PHYS_ABSORB);
    }

    bool nullAll    = false;
    bool nullRanged = false;
    if (dmgtakenhelpers::RangedOutcome(absorbAll, absorbPhys, false, false) == dmgtakenhelpers::AbsorbNullOutcome::Pass)
    {
        nullAll = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_DAMAGE);
        if (!nullAll)
        {
            nullRanged = xirand::GetRandomNumber(100) < PDefender->getMod(Mod::NULL_RANGED_DAMAGE);
        }
    }

    const auto outcome = dmgtakenhelpers::RangedOutcome(absorbAll, absorbPhys, nullAll, nullRanged);
    if (outcome == dmgtakenhelpers::AbsorbNullOutcome::Pass)
    {
        damage = HandleSevereDamage(PDefender, damage, true);

        ConvertDmgToMP(PDefender, damage, IsCovered);

        damage = HandleFanDance(PDefender, damage);
    }
    else
    {
        damage = dmgtakenhelpers::ApplyAbsorbNull(damage, outcome);
    }

    damage = CheckAndApplyDamageCap(damage, PDefender);

    return damage;
}

void HandleIssekiganEnmityBonus(CBattleEntity* PDefender, CBattleEntity* PAttacker)
{
    // Issekigan is Known to Grant 300 CE per parry, but unknown how it effects VE (per bgwiki). So VE is left alone for now.
    // JP is known to give 10 VE per point
    // Only give jpBonus if the defender is a player, as mobs don't have job points.
    const uint8 issekiganJP = (PDefender->objtype == TYPE_PC)
                                  ? static_cast<CCharEntity*>(PDefender)->PJobPoints->GetJobPointValue(JP_ISSEKIGAN_EFFECT)
                                  : static_cast<uint8>(0);
    const auto  decision    = enmitycombathelpers::IssekiganEnmityBonus(
        PAttacker->objtype == TYPE_MOB,
        PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Issekigan),
        PDefender->objtype == TYPE_PC,
        issekiganJP);
    if (decision.applied)
    {
        static_cast<CMobEntity*>(PAttacker)->PEnmityContainer->UpdateEnmity(PDefender, decision.ce, decision.ve, false, false);
    }
}

void HandleAfflatusMiseryAccuracyBonus(CBattleEntity* PAttacker)
{
    const bool hasMisery  = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AfflatusMisery);
    const bool hasAuspice = PAttacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Auspice);
    // We keep track of the running total of Accuracy Bonus as part of the Sub Power of the Effect
    // This is used to re-adjust Mod::ACC when the effect wears off
    const uint16 accBonus = hasMisery
                                ? PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::AfflatusMisery)->GetSubPower()
                                : 0;
    const auto   decision = combatstatustailshelpers::AfflatusMiseryAccuracyBonus(hasMisery, hasAuspice, accBonus);
    if (decision.applied)
    {
        // Per BGWiki, this bonus is thought to cap at +30
        PAttacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::AfflatusMisery)->SetSubPower(decision.newSubPower);
        // Update the Accuracy Modifer as well, so that this is reflected throughout the battle system
        PAttacker->addModifier(Mod::ACC, decision.accDelta);
    }
}

void HandleAfflatusMiseryDamage(CBattleEntity* PDefender, int32 damage)
{
    if (combatstatustailshelpers::ShouldSetAfflatusMiseryDamage(
            PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AfflatusMisery), damage))
    {
        PDefender->setModifier(Mod::AFFLATUS_MISERY, damage);
    }
}

void HandleTacticalParry(CBattleEntity* PEntity)
{
    CCharEntity* PChar = dynamic_cast<CCharEntity*>(PEntity);
    int16        tpBonus = 0;
    if (combatstatustailshelpers::TacticalTPBonus(
            PChar != nullptr,
            PChar != nullptr && charutils::hasTrait(PChar, TRAIT_TACTICAL_PARRY),
            PChar != nullptr ? PChar->getMod(Mod::TACTICAL_PARRY) : static_cast<int16>(0),
            tpBonus))
    {
        PChar->addTP(tpBonus);
    }
}

void HandleTacticalGuard(CBattleEntity* PEntity)
{
    CCharEntity* PChar = dynamic_cast<CCharEntity*>(PEntity);
    int16        tpBonus = 0;
    if (combatstatustailshelpers::TacticalTPBonus(
            PChar != nullptr,
            PChar != nullptr && charutils::hasTrait(PChar, TRAIT_TACTICAL_GUARD),
            PChar != nullptr ? PChar->getMod(Mod::TACTICAL_GUARD) : static_cast<int16>(0),
            tpBonus))
    {
        PChar->addTP(tpBonus);
    }
}

float HandleTranquilHeart(CBattleEntity* PEntity)
{
    const bool hasTrait = PEntity->objtype == TYPE_PC &&
                          charutils::hasTrait(static_cast<CCharEntity*>(PEntity), TRAIT_TRANQUIL_HEART);
    const int16 healingSkill = hasTrait ? PEntity->GetSkill(SKILL_HEALING_MAGIC) : 0;
    return combatbonustailshelpers::TranquilHeartReduction(hasTrait, healingSkill);
}

void BindBreakCheck(CBattleEntity* PAttacker, CBattleEntity* PDefender)
{
    if (PDefender->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Bind))
    {
        // Previously there was a tiered comparative level check here which gave different rates
        // depending on the level difference between the attacker and the defender.
        // These rates seemed very low, and have been removed, absent true research on retail.
        // EMobDifficulty mobCheck = charutils::CheckMob(PAttacker->GetMLevel(), PDefender->GetMLevel());
        // The level comparison and switch has been removed.

        if (combatbonustailshelpers::BindBreaks(static_cast<std::uint16_t>(xirand::GetRandomNumber(1000))))
        {
            PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Bind);
        }
    }
}

int32 HandleOneForAll(CBattleEntity* PDefender, int32 damage)
{
    auto* PEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::OneForAll);
    const bool has = PEffect != nullptr;
    const auto power = has ? PEffect->GetPower() : 0;
    return combatstatusmitigationhelpers::HandleOneForAll(damage, power, has);
}

int32 HandleStoneskin(CBattleEntity* PDefender, int32 damage)
{
    const int16 skin = PDefender->getMod(Mod::STONESKIN);
    const auto  r    = combatstatusmitigationhelpers::HandleStoneskin(damage, skin);
    if (r.removeEffect)
    {
        PDefender->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Stoneskin);
    }
    else if (skin > 0 && r.skinLeft != skin)
    {
        // Partial absorb: delModifier(STONESKIN, damage absorbed)
        PDefender->delModifier(Mod::STONESKIN, skin - r.skinLeft);
    }
    return r.remainingDamage;
}

auto HandleSevereDamage(CBattleEntity* PDefender, int32 damage, bool isPhysical) -> int32
{
    damage = HandleSevereDamageEffect(PDefender, xi::StatusEffect::Migawari, damage, true);
    // TODO: Earthen Armor effect
    // TODO: Sentinel's Scherzo effect

    if (isPhysical && PDefender->objtype == TYPE_PET && PDefender->getMod(Mod::AUTO_SCHURZEN) != 0 &&
        damage >= PDefender->health.hp)
    {
        auto* master = static_cast<CPetEntity*>(PDefender)->PMaster;
        const bool hasEarth = master != nullptr &&
                              master->StatusEffectContainer->GetEffectsCount(xi::StatusEffect::EarthManeuver) >= 1;
        const auto rewritten = combatstatusmitigationhelpers::ApplySchurzenCap(
            damage, PDefender->health.hp, true, true, hasEarth);
        if (hasEarth)
        {
            master->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::EarthManeuver);
            damage = rewritten;
        }
    }

    return damage;
}

int32 HandleFanDance(CBattleEntity* PDefender, int32 damage)
{
    auto* PEffect = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::FanDance);
    const bool has = PEffect != nullptr;
    const auto power = has ? PEffect->GetPower() : 0;
    const auto r = combatstatusmitigationhelpers::HandleFanDance(damage, power, has);
    if (has && r.newPower != power)
    {
        PEffect->SetPower(r.newPower);
    }
    return r.newDamage;
}

void HandleScarletDelirium(CBattleEntity* PDefender, int32 damage)
{
    CStatusEffect* effectScarDel = PDefender->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::ScarletDelirium);

    // Damage bonus calculation, update Effect Power
    if (effectScarDel && effectScarDel->GetPower() == 0)
    {
        const auto power   = combatstatusmitigationhelpers::ScarletDeliriumPower(damage, PDefender->GetMaxHP());
        const auto jpValue = effectScarDel->GetSubPower();
        const auto duration = std::chrono::seconds(combatstatusmitigationhelpers::ScarletDeliriumDurationSec(jpValue));

        // Convert status effect from "Absorb damage" mode to "Provide damage bonus" mode
        PDefender->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::ScarletDelirium);
        PDefender->StatusEffectContainer->AddStatusEffectSilent(
            xi::StatusEffect::ScarletDelirium1,
            static_cast<uint16>(xi::StatusEffect::ScarletDelirium1),
            power,
            0s,
            duration);
    }
}

auto HandleSevereDamageEffect(CBattleEntity* PDefender, xi::StatusEffect effect, int32 damage, bool removeEffect) -> int32
{
    auto* PEffect = PDefender->StatusEffectContainer->GetStatusEffect(effect);
    const bool has = PEffect != nullptr;
    const auto power = has ? PEffect->GetPower() : 0;
    const auto sub   = has ? PEffect->GetSubPower() : 0;
    const auto r = combatstatusmitigationhelpers::HandleSevereDamageEffect(
        damage, PDefender->GetMaxHP(), power, sub, has);
    if (r.triggered && removeEffect)
    {
        PDefender->StatusEffectContainer->DelStatusEffect(effect);
    }
    return r.newDamage;
}

/************************************************************************
 *                                                                       *
 *  Handle the /assist command                                           *
 *                                                                       *
 ************************************************************************/

void assistTarget(CCharEntity* PChar, uint16 TargID)
{
    // get the entity we want to assist
    CBattleEntity* EntityToAssist = (CBattleEntity*)PChar->GetEntity(TargID, TYPE_MOB | TYPE_PC);
    const bool     hasEntity      = EntityToAssist != nullptr;
    const bool     targetIsPC     = hasEntity && EntityToAssist->objtype == TYPE_PC;
    const bool     hasBTID        = hasEntity && EntityToAssist->GetBattleTargetID() != 0;
    CBattleEntity* EntityToLockon = hasEntity ? EntityToAssist->GetBattleTarget() : nullptr;
    const auto     lockOn         = entityactionhelpers::ClassifyAssistTarget(
        hasEntity, targetIsPC, hasBTID, EntityToLockon != nullptr);
    if (lockOn == entityactionhelpers::AssistLockOn::LockOnResolved)
    {
        // PC path uses resolved battle target; non-PC path uses GetBattleTarget() as LSB did.
        CBattleEntity* target = targetIsPC ? EntityToLockon : EntityToAssist->GetBattleTarget();
        if (target != nullptr)
        {
            // lock on to the new target!
            PChar->pushPacket<GP_SERV_COMMAND_ASSIST>(PChar, target);
        }
    }
}

ELEMENT GetDayElement()
{
    const auto weekday = static_cast<std::uint8_t>(vanadiel_time::get_weekday());
    return static_cast<ELEMENT>(entityequiphelpers::DayElementFromWeekday(weekday));
}

auto GetWeather(CBattleEntity* PEntity, bool ignoreScholar) -> Weather
{
    if (weathergethelpers::ShouldReturnNoneWeather(
            PEntity != nullptr, zoneutils::GetZone(PEntity ? PEntity->getZone() : 0) != nullptr))
    {
        return Weather::None;
    }

    return GetWeather(PEntity, ignoreScholar, zoneutils::GetZone(PEntity->getZone())->weather().current());
}

auto GetWeather(CBattleEntity* PEntity, bool ignoreScholar, Weather zoneWeather) -> Weather
{
    if (weathergethelpers::ShouldReturnNoneWeather(PEntity != nullptr, true))
    {
        return Weather::None;
    }

    auto scholarSpell = Weather::None;

    if (!ignoreScholar) // Do not need to check for status effects if we're ignoring scholar spells
    {
        weathergethelpers::ScholarStorms storms{};
        storms.firestorm    = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Firestorm);
        storms.rainstorm    = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Rainstorm);
        storms.sandstorm    = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Sandstorm);
        storms.windstorm    = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Windstorm);
        storms.hailstorm    = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Hailstorm);
        storms.thunderstorm = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Thunderstorm);
        storms.aurorastorm  = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Aurorastorm);
        storms.voidstorm    = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Voidstorm);
        scholarSpell        = weathergethelpers::ScholarWeatherFromStorms(storms);
    }

    return weathergethelpers::ResolveWeather(zoneWeather, scholarSpell, ignoreScholar);
}

auto WeatherMatchesElement(const Weather weather, const uint8 element) -> bool
{
    return weathermatcheshelpers::WeatherMatchesElement(weather, element);
}

void DrawIn(CBattleEntity* PTarget, const position_t pos, const float offset, const float degrees)
{
    const float radian     = drawinhelpers::DegreesToRadians(degrees);
    position_t  nearEntity = nearPosition(pos, offset, radian);

    // Target may be in the middle of zoning (Alliance-based Draw-In)
    if (drawinhelpers::ShouldAbortDrawInNoZone(PTarget->loc.zone != nullptr))
    {
        return;
    }

    // If geometry blocks the path from the source eyeline to the draw-in point, abort the
    // draw-in - navmesh snapToValidPosition below will handle snapping to a valid position.
    const auto src = Vector3{ pos.x, drawinhelpers::DrawInRaySourceY(pos.y), pos.z };
    const auto dst = Vector3{ nearEntity.x, nearEntity.y, nearEntity.z };
    if (drawinhelpers::ShouldAbortDrawInRayBlock(PTarget->loc.zone->xiMesh()->rayIntersect(src, dst)))
    {
        return;
    }

    // Snap nearEntity to a guaranteed valid position
    PTarget->loc.zone->navMesh()->snapToValidPosition(nearEntity);

    // Move the target a little higher, just in case
    nearEntity.y = drawinhelpers::DrawInAfterSnapY(nearEntity.y);

    if (drawinhelpers::ShouldApplyDrawInMove(
            PTarget->status == STATUS_TYPE::CUTSCENE_ONLY,
            PTarget->isDead(),
            PTarget->isMounted()))
    {
        // draw in!
        PTarget->loc.zone->PushPacket(PTarget, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_WPOS>(PTarget, nearEntity));
        PTarget->loc.zone->PushPacket(PTarget, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(PTarget, PTarget, 0, 0, MsgBasic::DrawnIn));
    }
}

/************************************************************************
 *                                                                       *
 *  Add the COR Wild Card effect to a specific character                 *
 *                                                                       *
 ************************************************************************/

void DoWildCardToEntity(CCharEntity* PCaster, CCharEntity* PTarget, const uint8 roll)
{
    // No matter the roll, all basic abilities are reset
    PTarget->PRecastContainer->ResetAbilities();

    // Wild Card is excluded from 1HR reset.
    // TODO: COR Job Points allow Wild Card to reset itself 1-20% of the time
    const auto effect = wildcardrandomdealhelpers::ResolveWildCard(
        roll,
        PTarget->GetMJob() == JOB_COR,
        PTarget->health.maxmp,
        PTarget->health.mp);

    if (effect.setTP)
    {
        PTarget->health.tp = effect.tpValue;
    }
    if (effect.delSpecial2)
    {
        PTarget->PRecastContainer->Del(RECAST_ABILITY, Recast::Special2);
    }
    if (effect.delSpecial)
    {
        PTarget->PRecastContainer->Del(RECAST_ABILITY, Recast::Special);
    }
    if (effect.setMPHalfFloor)
    {
        PTarget->health.mp = PTarget->health.maxmp / 2;
    }
    if (effect.addFullMP)
    {
        PTarget->addMP(PTarget->health.maxmp);
    }

    PTarget->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PTarget);
}

/************************************************************************
 *                                                                       *
 *   Does the random deal effect to a specific character (reset ability) *
 *                                                                       *
 ************************************************************************/
bool DoRandomDealToEntity(CCharEntity* PChar, CBattleEntity* PTarget)
{
    if (PChar == nullptr || PTarget == nullptr)
    {
        // Invalid User or Target
        return false;
    }

    RecastList_t* recastList = PTarget->PRecastContainer->GetRecastList(RECAST_ABILITY);

    std::vector<wildcardrandomdealhelpers::RandomDealRecast> recasts;
    recasts.reserve(recastList->size());
    for (uint8 i = 0; i < recastList->size(); ++i)
    {
        Recast_t* recast = &recastList->at(i);
        recasts.push_back(wildcardrandomdealhelpers::RandomDealRecast{
            static_cast<uint16>(recast->ID),
            recast->RecastTime > 0s,
        });
    }

    std::vector<int> resetCandidateList;
    std::vector<int> activeCooldownList;
    wildcardrandomdealhelpers::BuildRandomDealLists(recasts, resetCandidateList, activeCooldownList);

    if (wildcardrandomdealhelpers::ShouldEvadeRandomDeal(
            static_cast<int>(resetCandidateList.size()), static_cast<int>(activeCooldownList.size())))
    {
        // Evade because we have no abilities that can be reset
        return false;
    }

    const uint8 loadedDeck     = PChar->PMeritPoints->GetMeritValue(MERIT_LOADED_DECK, PChar);
    const uint8 resetTwoChance = wildcardrandomdealhelpers::RandomDealTwoChance(PChar->getMod(Mod::RANDOM_DEAL_BONUS));

    // Host mutates lists in place for shuffle, then applies DeleteByIndex using
    // pure path classification for chance gates (RNG still host-side, exact order).
    if (loadedDeck > 0) // Loaded Deck Merit Version
    {
        if (activeCooldownList.size() > 1)
        {
            // Shuffle active cooldowns and take first (loaded deck)
            xirand::ShuffleInPlace(activeCooldownList);
        }

        const auto chance = (activeCooldownList.size() > 1)
                                ? static_cast<uint8>(100)
                                : wildcardrandomdealhelpers::LoadedDeckChance(loadedDeck);

        // chance >= GetRandomNumber(100)  ⇔  !(chance < roll)
        if (chance < static_cast<uint8>(xirand::GetRandomNumber(100)))
        {
            // Evade because we failed to reset with loaded deck
            return false;
        }

        PTarget->PRecastContainer->DeleteByIndex(RECAST_ABILITY, static_cast<uint8>(activeCooldownList.at(0)));

        // Reset 2 abilities by chance
        if (activeCooldownList.size() > 1 && resetTwoChance >= static_cast<uint8>(xirand::GetRandomNumber(100)))
        {
            PTarget->PRecastContainer->DeleteByIndex(RECAST_ABILITY, static_cast<uint8>(activeCooldownList.at(1)));
        }
        if (PChar != PTarget)
        {
            if (auto PCharTarget = dynamic_cast<CCharEntity*>(PTarget))
            {
                // Update target's recast state: caster's will be handled in CCharEntity::OnAbility.
                PCharTarget->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PCharTarget);
            }
        }
        return true;
    }

    // Standard Version
    if (resetCandidateList.size() > 1)
    {
        // Shuffle if more than 1 ability
        xirand::ShuffleInPlace(resetCandidateList);
    }

    // Reset first ability (shuffled or only)
    PTarget->PRecastContainer->DeleteByIndex(RECAST_ABILITY, static_cast<uint8>(resetCandidateList.at(0)));

    // Reset 2 abilities by chance (could be 2 abilities that don't need resets)
    // Note: GetRandomNumber(1, 100) inclusive range — differs from loaded-deck [0,100).
    if (resetCandidateList.size() > 1 && activeCooldownList.size() > 1 &&
        static_cast<int>(resetTwoChance) >= xirand::GetRandomNumber(1, 100))
    {
        PTarget->PRecastContainer->DeleteByIndex(RECAST_ABILITY, static_cast<uint8>(resetCandidateList.at(1)));
    }

    if (PChar != PTarget)
    {
        if (auto PCharTarget = dynamic_cast<CCharEntity*>(PTarget))
        {
            // Update target's recast state: caster's will be handled in CCharEntity::OnAbility.
            PCharTarget->pushPacket<GP_SERV_COMMAND_ABIL_RECAST>(PCharTarget);
        }
    }

    return true;
}

// turn towards target unless mob behavior ignores this (but can be forced to anyway)
void turnTowardsTarget(CBaseEntity* PEntity, CBaseEntity* PTarget, bool force)
{
    CMobEntity* PMob = dynamic_cast<CMobEntity*>(PEntity);
    // Big mobs typically should ignore this -- Such as dragons/wyrms or other big things.
    // Some TP moves like Petro Eyes from normal dragons _also_ ignore their standard behavior, so we must allow it sometimes.
    if (!entityactionhelpers::ShouldTurnTowardsTarget(
            PEntity != nullptr,
            PTarget != nullptr,
            PMob != nullptr,
            PMob ? static_cast<uint16>(PMob->m_Behavior) : static_cast<uint16>(0),
            force))
    {
        return;
    }

    PEntity->loc.p.rotation = worldAngle(PEntity->loc.p, PTarget->loc.p);
    PEntity->updatemask |= UPDATE_POS;
    PEntity->loc.zone->UpdateEntityPacket(PTarget, ENTITY_UPDATE, UPDATE_POS);
}

/************************************************************************
 *                                                                       *
 *  Reduce input delay by Snapshot and Velocity shot                     *
 *                                                                       *
 ************************************************************************/

int16 GetRangedDelayReduction(CBattleEntity* battleEntity, int16 delay)
{
    auto SnapShotReductionPercent{ battleEntity->getMod(Mod::SNAPSHOT) };

    if (auto* PChar = dynamic_cast<CCharEntity*>(battleEntity))
    {
        if (charutils::hasTrait(PChar, TRAIT_SNAPSHOT))
        {
            SnapShotReductionPercent += PChar->PMeritPoints->GetMeritValue(MERIT_SNAPSHOT, PChar);
        }
    }

    // https://www.bg-wiki.com/ffxi/Snapshot — cap + velocity applied in pure helper
    const bool hasVelocity = battleEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::VelocityShot);
    return entityactionhelpers::RangedDelayReduction(
        delay,
        SnapShotReductionPercent,
        hasVelocity,
        hasVelocity ? battleEntity->getMod(Mod::VELOCITY_SNAPSHOT_BONUS) : static_cast<int16>(0));
}

/************************************************************************
 *                                                                       *
 *  Get any ranged attack bonuses here                                   *
 *                                                                       *
 ************************************************************************/

int32 GetRangedAttackBonuses(CBattleEntity* battleEntity)
{
    return entityactionhelpers::RangedAttackBonuses(
        battleEntity->objtype == TYPE_PC,
        battleEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::VelocityShot),
        battleEntity->getMod(Mod::VELOCITY_RATT_BONUS));
}

/************************************************************************
 *                                                                       *
 *  Get any ranged accuracy bonuses here                                 *
 *                                                                       *
 ************************************************************************/

int32 GetRangedAccuracyBonuses(CBattleEntity* battleEntity)
{
    return entityactionhelpers::RangedAccuracyBonuses(
        battleEntity->objtype == TYPE_PC,
        battleEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Barrage),
        battleEntity->getMod(Mod::BARRAGE_ACC));
}

void AddTraits(CBattleEntity* PEntity, TraitList_t* traitList, uint8 level)
{
    auto* PChar = dynamic_cast<CCharEntity*>(PEntity);

    for (auto&& PTrait : *traitList)
    {
        traitsenmityhelpers::TraitCandidate candidate{
            PTrait->getID(),
            PTrait->getLevel(),
            PTrait->getRank(),
            static_cast<uint16>(PTrait->getMod()),
            static_cast<uint16>(PTrait->getMeritID()),
        };

        std::vector<traitsenmityhelpers::ExistingTrait> existing;
        existing.reserve(PEntity->TraitList.size());
        for (auto* PExistingTrait : PEntity->TraitList)
        {
            existing.push_back(traitsenmityhelpers::ExistingTrait{
                PExistingTrait->getID(),
                PExistingTrait->getRank(),
                static_cast<uint16>(PExistingTrait->getMod()),
                static_cast<uint16>(PExistingTrait->getMeritID()),
            });
        }

        auto meritCount = [PChar](uint16 meritID) -> uint8 {
            if (!PChar || meritID == 0)
            {
                return 0;
            }
            return PChar->PMeritPoints->GetMerit(static_cast<MERIT_TYPE>(meritID))->count;
        };

        const auto decision = traitsenmityhelpers::ResolveTraitAdd(
            level, candidate, existing, PChar != nullptr, meritCount, meritCount);

        if (!decision.eligible)
        {
            continue;
        }
        if (decision.delExistingIndex >= 0)
        {
            // Check if we still have the merit required for this trait / rank upgrade
            PEntity->delTrait(PEntity->TraitList.at(static_cast<std::size_t>(decision.delExistingIndex)));
        }
        if (decision.shouldAdd)
        {
            PEntity->addTrait(PTrait);
        }
    }
}

bool HasClaim(CBattleEntity* PEntity, CBattleEntity* PTarget)
{
    if (PEntity == nullptr)
    {
        ShowWarning("PEntity is null.");
        return false;
    }

    if (PTarget == nullptr)
    {
        ShowWarning("PTarget is null.");
        return false;
    }

    CBattleEntity* PMaster = PEntity;

    if (PEntity->PMaster != nullptr)
    {
        PMaster = PEntity->PMaster;
    }

    bool allianceHasOwner = false;
    // clang-format off
        PMaster->ForAlliance([&PTarget, &allianceHasOwner](CBattleEntity* PChar)
        {
            if (PChar->id == PTarget->m_OwnerID.id)
            {
                allianceHasOwner = true;
            }
        });
    // clang-format on

    return claimhelpers::HasClaim(PMaster->id, PTarget->m_OwnerID.id, allianceHasOwner);
}

timer::duration CalculateSpellCastTime(CBattleEntity* PEntity, CMagicState* PMagicState)
{
    CSpell* PSpell = PMagicState->GetSpell();
    if (spellcasthelpers::ShouldReturnZeroNullSpell(PSpell == nullptr))
    {
        return 0s;
    }

    // Check Quick Magic procs (host RNG + SetInstantCast side effect)
    const int16 quickMagicRate = PEntity->getMod(Mod::QUICK_MAGIC);
    const bool  quickMagicProc = xirand::GetRandomNumber(100) < quickMagicRate;
    if (quickMagicProc)
    {
        PMagicState->SetInstantCast(true);
    }

    spellcasthelpers::SpellCastParams p{};
    p.baseMs     = std::chrono::duration_cast<std::chrono::milliseconds>(PSpell->getCastTime()).count();
    p.spellGroup = static_cast<std::uint16_t>(PSpell->getSpellGroup());
    p.aoe        = static_cast<std::uint8_t>(PSpell->getAOE());
    p.skillType  = static_cast<std::uint8_t>(PSpell->getSkillType());
    p.isCure     = PSpell->isCure();
    p.isNa       = PSpell->isNa();
    p.isPC       = PEntity->objtype == TYPE_PC;
    p.quickMagicProc = quickMagicProc;

    p.hassoOrSeigan =
        PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::Hasso, xi::StatusEffect::Seigan });
    p.alacrity = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Alacrity);
    p.celerity = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Celerity);
    p.darkArtsOrAddendumBlack =
        PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::DarkArts, xi::StatusEffect::AddendumBlack });
    p.lightArtsOrAddendumWhite =
        PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::LightArts, xi::StatusEffect::AddendumWhite });
    p.pianissimo  = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Pianissimo);
    p.nightingale = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Nightingale);
    p.troubadour  = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Troubadour);

    if (p.nightingale && p.isPC)
    {
        auto* PChar = static_cast<CCharEntity*>(PEntity);
        p.nightingaleInstant =
            xirand::GetRandomNumber(100) < (PChar->PMeritPoints->GetMeritValue(MERIT_NIGHTINGALE, PChar) - 25);
    }

    p.blackMagicCast         = PEntity->getMod(Mod::BLACK_MAGIC_CAST);
    p.whiteMagicCast         = PEntity->getMod(Mod::WHITE_MAGIC_CAST);
    p.darkMagicCast          = PEntity->getMod(Mod::DARK_MAGIC_CAST);
    p.grimoireSpellcasting   = PEntity->getMod(Mod::GRIMOIRE_SPELLCASTING);
    p.alacrityCelerityEffect = static_cast<std::uint16_t>(PEntity->getMod(Mod::ALACRITY_CELERITY_EFFECT));
    p.weatherMatchesElement =
        battleutils::WeatherMatchesElement(battleutils::GetWeather(PEntity, false), static_cast<uint8>(PSpell->getElement()));

    if (p.isPC)
    {
        auto* PChar = static_cast<CCharEntity*>(PEntity);
        p.strategemEffectII           = PChar->PJobPoints->GetJobPointValue(JP_STRATEGEM_EFFECT_II);
        p.summoningMagicCastTimeMerit = PChar->PMeritPoints->GetMeritValue(MERIT_SUMMONING_MAGIC_CAST_TIME, PChar);
        p.ninjutsuCastTimeBonus       = PChar->PJobPoints->GetJobPointValue(JP_NINJITSU_CAST_TIME_BONUS);
        p.cureCastTimeMerit           = static_cast<std::int16_t>(PChar->PMeritPoints->GetMeritValue(MERIT_CURE_CAST_TIME, PChar));
        p.widenedCompassJP            = static_cast<std::int16_t>(PChar->PJobPoints->GetJobPointValue(JP_WIDENED_COMPASS_EFFECT));
    }

    p.summoningMagicCast     = PEntity->getMod(Mod::SUMMONING_MAGIC_CAST);
    p.songSpellcastingTime   = static_cast<std::uint16_t>(PEntity->getMod(Mod::SONG_SPELLCASTING_TIME));
    p.fastCast               = PEntity->getMod(Mod::FASTCAST);
    p.uFastCast              = PEntity->getMod(Mod::UFASTCAST);
    p.inspirationFastCast    = PEntity->getMod(Mod::INSPIRATION_FAST_CAST);
    p.elementalCelerity      = PEntity->getMod(Mod::ELEMENTAL_CELERITY);
    p.cureCastTime           = PEntity->getMod(Mod::CURE_CAST_TIME);
    p.divineBenison          = PEntity->getMod(Mod::DIVINE_BENISON);

    const auto ms = spellcasthelpers::CalculateSpellCastMs(p);
    return std::chrono::milliseconds(ms);
}

uint16 CalculateSpellCost(CBattleEntity* PEntity, CSpell* PSpell)
{
    if (spellcosthelpers::ShouldReturnZeroNullSpell(PSpell == nullptr))
    {
        ShowWarning("battleutils::CalculateMPCost Spell is nullptr");
        return 0;
    }

    // ninja tools or bard song
    if (spellcosthelpers::ShouldReturnZeroNoMPCost(PSpell->hasMPCost()))
    {
        return 0;
    }

    const bool noMPDepletion = xirand::GetRandomNumber(100) < PEntity->getMod(Mod::NO_SPELL_MP_DEPLETION);

    return spellcosthelpers::CalculateSpellCost(
        static_cast<std::uint16_t>(PSpell->getID()),
        static_cast<std::uint16_t>(PSpell->getSpellGroup()),
        static_cast<std::uint8_t>(PSpell->getAOE()),
        PSpell->getMPCost(),
        static_cast<std::uint16_t>(PEntity->health.maxmp),
        PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Manifestation),
        PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Parsimony),
        PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Accession),
        PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Penury),
        PEntity->getMod(Mod::BLACK_MAGIC_COST),
        PEntity->getMod(Mod::WHITE_MAGIC_COST),
        PEntity->getMod(Mod::MP_COST_REDUCTION),
        noMPDepletion);
}

bool CanAffordSpell(CBattleEntity* PEntity, CSpell* PSpell, uint8 flags)
{
    if (PEntity == nullptr)
    {
        return false;
    }

    const bool  hasManafont = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Manafont);
    auto*       PMob        = dynamic_cast<CMobEntity*>(PEntity);
    const bool  isMob       = PMob != nullptr;
    const int16 noSpellCost = isMob ? PMob->getMobMod(MOBMOD_NO_SPELL_COST) : static_cast<int16>(0);
    const bool  hasMPCost   = PSpell->hasMPCost();
    const auto  spellCost   = hasMPCost ? CalculateSpellCost(PEntity, PSpell) : static_cast<uint16>(0);

    return canaffordspellhelpers::CanAffordSpell(
        hasManafont,
        flags,
        isMob,
        noSpellCost,
        hasMPCost,
        spellCost,
        PEntity->health.mp);
}

timer::duration CalculateSpellRecastTime(CBattleEntity* PEntity, CSpell* PSpell)
{
    if (spellrecasthelpers::ShouldReturnZeroNullSpell(PSpell == nullptr))
    {
        return 0s;
    }

    spellrecasthelpers::SpellRecastParams p{};
    p.baseMs     = std::chrono::duration_cast<std::chrono::milliseconds>(PSpell->getRecastTime()).count();
    p.spellID    = static_cast<std::uint16_t>(PSpell->getID());
    p.spellGroup = static_cast<std::uint16_t>(PSpell->getSpellGroup());
    p.aoe        = static_cast<std::uint8_t>(PSpell->getAOE());
    p.skillType  = static_cast<std::uint8_t>(PSpell->getSkillType());

    p.recastReductionCap = settings::get<float>("map.SPELL_RECAST_REDUCTION_CAP");

    p.fastCast            = PEntity->getMod(Mod::FASTCAST);
    p.inspirationFastCast = PEntity->getMod(Mod::INSPIRATION_FAST_CAST);
    p.hasteMagic          = PEntity->getMod(Mod::HASTE_MAGIC);
    p.hasteGear           = PEntity->getMod(Mod::HASTE_GEAR);

    p.elementalMagicRecast  = PEntity->getMod(Mod::ELEMENTAL_MAGIC_RECAST);
    p.blueMagicRecast       = PEntity->getMod(Mod::BLUE_MAGIC_RECAST);
    p.healingMagicRecast    = PEntity->getMod(Mod::HEALING_MAGIC_RECAST);
    p.enfeeblingMagicRecast = PEntity->getMod(Mod::ENFEEBLING_MAGIC_RECAST);
    p.enhancingMagicRecast  = PEntity->getMod(Mod::ENHANCING_MAGIC_RECAST);

    p.blackMagicRecast     = PEntity->getMod(Mod::BLACK_MAGIC_RECAST);
    p.whiteMagicRecast     = PEntity->getMod(Mod::WHITE_MAGIC_RECAST);
    p.grimoireSpellcasting = PEntity->getMod(Mod::GRIMOIRE_SPELLCASTING);

    p.songRecastDelaySec = PEntity->getMod(Mod::SONG_RECAST_DELAY);
    p.isPC               = PEntity->objtype == TYPE_PC;
    if (p.isPC)
    {
        auto* PChar = static_cast<CCharEntity*>(PEntity);
        p.finaleRecastMeritSec  = PChar->PMeritPoints->GetMeritValue(MERIT_FINALE_RECAST, PChar);
        p.lullabyRecastMeritSec = PChar->PMeritPoints->GetMeritValue(MERIT_LULLABY_RECAST, PChar);
    }

    p.alacrityCelerityEffect = static_cast<std::uint16_t>(PEntity->getMod(Mod::ALACRITY_CELERITY_EFFECT));

    p.nightingale = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Nightingale);
    p.composure   = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Composure);
    p.hassoOrSeigan =
        PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::Hasso, xi::StatusEffect::Seigan });
    p.manifestation = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Manifestation);
    p.accession     = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Accession);
    p.darkArtsOrAddendumBlack =
        PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::DarkArts, xi::StatusEffect::AddendumBlack });
    p.lightArtsOrAddendumWhite =
        PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::LightArts, xi::StatusEffect::AddendumWhite });
    p.alacrity = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Alacrity);
    p.celerity = PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Celerity);

    p.mainJob = static_cast<std::uint8_t>(PEntity->GetMJob());
    p.weatherMatchesElement =
        battleutils::WeatherMatchesElement(battleutils::GetWeather(PEntity, false), static_cast<uint8>(PSpell->getElement()));

    const auto ms = spellrecasthelpers::CalculateSpellRecastMs(p);
    return std::chrono::milliseconds(ms);
}

int16 CalculateWeaponSkillTP(CBattleEntity* PEntity, CWeaponSkill* PWeaponSkill, int16 spentTP)
{
    const bool isPC       = PEntity->objtype == TYPE_PC;
    const bool isRangedWS = wstphelpers::IsRangedWeaponskillID(PWeaponSkill->getID());

    std::int16_t subTPBonus    = 0;
    std::int16_t mainTPBonus   = 0;
    std::int16_t rangedTPBonus = 0;
    bool         fencerElig    = false;
    std::int16_t fencerTPBonus = 0;

    if (isPC)
    {
        auto* PChar = static_cast<CCharEntity*>(PEntity);

        // remove TP Bonus from offhand weapon
        // TODO -- don't remove TP bonus if this TP bonus is from an augment (or perhaps add a second TP bonus stat.)
        if (PChar->m_Weapons[SLOT_SUB])
        {
            subTPBonus = static_cast<std::int16_t>(
                battleutils::GetScaledItemModifier(PEntity, PChar->m_Weapons[SLOT_SUB], Mod::TP_BONUS));
        }

        // if ranged WS, remove TP bonus from mainhand weapon
        if (isRangedWS)
        {
            if (PChar->m_Weapons[SLOT_MAIN])
            {
                mainTPBonus = static_cast<std::int16_t>(
                    battleutils::GetScaledItemModifier(PEntity, PChar->m_Weapons[SLOT_MAIN], Mod::TP_BONUS));
            }
        }
        else
        {
            // if melee WS, remove TP bonus from ranged weapon
            // TODO -- don't remove TP bonus if this TP bonus is from an augment (or perhaps add a second TP bonus stat.)
            if (PChar->m_Weapons[SLOT_RANGED])
            {
                rangedTPBonus = static_cast<std::int16_t>(
                    battleutils::GetScaledItemModifier(PEntity, PChar->m_Weapons[SLOT_RANGED], Mod::TP_BONUS));
            }

            // Add Fencer TP Bonus
            CItemWeapon*    PMain      = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[SLOT_MAIN]);
            CItemEquipment* PSub       = PChar->getEquip(SLOT_SUB);
            CItemWeapon*    PSubWeapon = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[SLOT_SUB]);

            fencerElig = wstphelpers::FencerEligible(
                PMain != nullptr,
                PMain && PMain->isTwoHanded(),
                PMain && PMain->isHandToHand(),
                PSub != nullptr,
                PSubWeapon != nullptr,
                PSubWeapon ? static_cast<std::uint8_t>(PSubWeapon->getSkillType()) : static_cast<std::uint8_t>(0),
                PSub && PSub->IsShield());
            if (fencerElig)
            {
                fencerTPBonus = PEntity->getMod(Mod::FENCER_TP_BONUS);
            }
        }
    }

    return wstphelpers::CalculateWeaponSkillTP(
        spentTP,
        PEntity->getMod(Mod::TP_BONUS),
        isPC,
        isRangedWS,
        subTPBonus,
        mainTPBonus,
        rangedTPBonus,
        fencerElig,
        fencerTPBonus);
}

bool RemoveAmmo(CCharEntity* PChar, int quantity)
{
    CItemWeapon* PItem = (CItemWeapon*)PChar->getEquip(SLOT_AMMO);

    if (!rangedammohelpers::RemoveAmmoShouldAct(PItem != nullptr))
    {
        return false;
    }

    if (rangedammohelpers::RemoveAmmoEmptiesSlot(true, PItem->getQuantity(), quantity))
    {
        auto  eloc = PChar->equipLocation(SLOT_AMMO);
        uint8 slot = eloc ? eloc->Slot : 0;
        uint8 loc  = eloc ? static_cast<uint8>(eloc->Container) : 0;
        charutils::UnequipItem(PChar, SLOT_AMMO);
        PChar->RequestPersist(CHAR_PERSIST::EQUIP);
        charutils::UpdateItem(PChar, loc, slot, -quantity);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
        return true;
    }

    auto ammoLoc = PChar->equipLocation(SLOT_AMMO);
    charutils::UpdateItem(PChar, static_cast<uint8>(ammoLoc->Container), ammoLoc->Slot, -quantity);
    PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
    return false;
}

int32 GetMeritValue(CBattleEntity* PEntity, MERIT_TYPE merit)
{
    if (PEntity->objtype == TYPE_PC)
    {
        return static_cast<CCharEntity*>(PEntity)->PMeritPoints->GetMeritValue(merit, static_cast<CCharEntity*>(PEntity));
    }
    return 0;
}

int32 GetScaledItemModifier(CBattleEntity* PEntity, CItemEquipment* PItem, Mod mod)
{
    if (scaleditemmodifierhelpers::ShouldRejectNull(PEntity == nullptr, PItem == nullptr))
    {
        ShowWarning("battleutils::GetScaledItemModifier() - PEntity or PItem received as null.");
        return 0;
    }

    return scaleditemmodifierhelpers::ScaledItemModifier(
        PEntity->GetMLevel(),
        PItem->getReqLvl(),
        mod,
        PItem->getModifier(mod));
}

auto GetSpikesDamageType(const ActionReactKind spikesType) -> xi::DamageType
{
    return damageaffinityhelpers::GetSpikesDamageType(spikesType);
}

auto GetEnspellDamageType(ENSPELL enspellType) -> xi::DamageType
{
    return damageaffinityhelpers::GetEnspellDamageType(static_cast<std::uint8_t>(enspellType));
}

auto GetRuneEnhancementDamageType(xi::StatusEffect runeEffect) -> xi::DamageType
{
    return damageaffinityhelpers::GetRuneEnhancementDamageType(runeEffect);
}

auto GetRuneEnhancementElement(xi::StatusEffect runeEffect) -> ELEMENT
{
    return static_cast<ELEMENT>(damageaffinityhelpers::GetRuneEnhancementElement(runeEffect));
}

CBattleEntity* GetCoverAbilityUser(CBattleEntity* PCoverAbilityTarget, CBattleEntity* PMob)
{
    CBattleEntity* PCoverAbilityUser    = nullptr;
    uint32         coverAbilityTargetID = PCoverAbilityTarget->id;

    // If the cover ability target is in a party, try to find a cover ability user
    if (entityactionhelpers::ShouldSearchCoverParty(PCoverAbilityTarget->PParty != nullptr))
    {
        for (auto* PMember : PCoverAbilityTarget->PParty->members)
        {
            if (entityactionhelpers::CoverMemberEligible(
                    coverAbilityTargetID == PMember->GetLocalVar("COVER_ABILITY_TARGET"),
                    PMember->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Cover),
                    PMember->isAlive()))
            {
                PCoverAbilityUser = PMember;
                break;
            }
        }

        if (PCoverAbilityUser != nullptr)
        {
            // using same variable names as trick attack function, for consistent understanding
            uint8 angleTAmob = worldAngle(PMob->loc.p, PCoverAbilityUser->loc.p);
            float distTAmob  = distance(PCoverAbilityUser->loc.p, PMob->loc.p);

            // check if cover user is within melee range and that cover target is in-line behind
            if (entityactionhelpers::CoverGeometryOK(
                    distTAmob,
                    PMob->GetMeleeRange(PCoverAbilityUser),
                    worldAngleMinDistance,
                    distance(PCoverAbilityTarget->loc.p, PMob->loc.p),
                    areInLine(angleTAmob, PMob, PCoverAbilityTarget)))
            {
                return PCoverAbilityUser;
            }
        }
    }
    return nullptr;
}

bool IsMagicCovered(CCharEntity* PCoverAbilityUser)
{
    return entityactionhelpers::IsMagicCovered(
        PCoverAbilityUser != nullptr,
        PCoverAbilityUser ? PCoverAbilityUser->getMod(Mod::COVER_MAGIC_AND_RANGED) : static_cast<int16>(0));
}

void ConvertDmgToMP(CBattleEntity* PDefender, int32 damage, bool IsCovered)
{
    // If attack was covered, get cover ability user's COVER_TO_MP mod
    // Get ABSORB_PHYSDMG_TO_MP mod
    const int16 absorbedMP = entityactionhelpers::AbsorbedPhysDmgToMP(
        damage,
        IsCovered,
        IsCovered ? PDefender->getMod(Mod::COVER_TO_MP) : static_cast<int16>(0),
        PDefender->getMod(Mod::ABSORB_PHYSDMG_TO_MP));

    if (entityactionhelpers::ShouldApplyAbsorbedMP(absorbedMP))
    {
        PDefender->addMP(absorbedMP);
    }
}

auto CheckLiementAbsorb(CBattleEntity* PBattleEntity, xi::DamageType DamageType) -> float
{
    if (!PBattleEntity)
    {
        return 1.0f;
    }

    auto* liementEffect = PBattleEntity->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Liement, 0);
    const bool active = liementEffect != nullptr;
    const std::uint16_t absorbPower    = active ? liementEffect->GetPower() : 0;
    const std::uint16_t absorbTypeBits = active ? liementEffect->GetSubPower() : 0;

    const auto result = damageaffinityhelpers::CheckLiementAbsorb(
        active,
        absorbPower,
        absorbTypeBits,
        static_cast<std::uint16_t>(DamageType));

    if (result.consume)
    {
        // Liement absorbs once and disappears.
        PBattleEntity->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Liement);
    }
    return result.multiplier;
}

void addEcosystemKillerEffects(CBattleEntity* PBattleEntity)
{
    if (const auto killerMod = combatbonustailshelpers::KillerMod(PBattleEntity->m_EcoSystem))
    {
        PBattleEntity->addModifier(*killerMod, combatbonustailshelpers::KillerBonus);
    }
}

}; // namespace battleutils
