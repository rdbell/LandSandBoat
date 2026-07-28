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

#include "char_entity.h"
#include "level_sync_departure.h"

#include "can_attack_capacity.h"
#include "char_action_boundary_capacity.h"
#include "char_aman_host_capacity.h"
#include "char_automaton_capacity.h"
#include "char_bazaar_capacity.h"
#include "char_combat_transition_capacity.h"
#include "char_death_apply_capacity.h"
#include "char_death_homepoint_capacity.h"
#include "char_death_plan_capacity.h"
#include "char_raise_complete_capacity.h"
#include "char_is_mob_owner_capacity.h"
#include "char_ability_preflight_capacity.h"
#include "char_ability_recast_capacity.h"
#include "char_ability_stealth_capacity.h"
#include "char_ability_paralyze_capacity.h"
#include "char_activity_capacity.h"
#include "char_shield_capacity.h"
#include "char_gender_capacity.h"
#include "char_ability_result_capacity.h"
#include "char_ability_pet_capacity.h"
#include "char_ability_response_capacity.h"
#include "ability_aoe_capacity.h"
#include "char_weaponskill_range_capacity.h"
#include "char_weaponskill_self_capacity.h"
#include "char_weaponskill_primary_capacity.h"
#include "char_cast_finish_capacity.h"
#include "char_immanence_capacity.h"
#include "char_cast_skillup_capacity.h"
#include "char_var_cache_capacity.h"
#include "char_timed_death_capacity.h"
#include "char_entity_update_capacity.h"
#include "char_equipment_capacity.h"
#include "char_equip_flush_capacity.h"
#include "char_error_delivery_capacity.h"
#include "char_event_activation_capacity.h"
#include "char_event_lock_capacity.h"
#include "char_event_idle_capacity.h"
#include "char_event_packet_capacity.h"
#include "char_event_queue_capacity.h"
#include "char_event_skip_capacity.h"
#include "char_highest_job_capacity.h"
#include "char_item_finish_preflight_capacity.h"
#include "char_item_finish_complete_capacity.h"
#include "char_item_finish_targets_capacity.h"
#include "char_moghancement_state_capacity.h"
#include "char_moghancement_furniture_capacity.h"
#include "char_moghancement_craft_capacity.h"
#include "char_moghancement_general_capacity.h"
#include "char_moghancement_resistance_capacity.h"
#include "char_moghancement_update_capacity.h"
#include "char_name_capacity.h"
#include "char_packet_queue_capacity.h"
#include "char_pet_zoning_capacity.h"
#include "char_persistence_capacity.h"
#include "char_playtime_capacity.h"
#include "char_raise_apply_capacity.h"
#include "char_raise_plan_capacity.h"
#include "char_post_tick_refresh_capacity.h"
#include "char_post_tick_update_capacity.h"
#include "char_resource_capacity.h"
#include "char_runtime_state_capacity.h"
#include "char_start_synth_capacity.h"
#include "char_target_resolver_capacity.h"
#include "char_storage_capacity.h"
#include "char_tick_capacity.h"
#include "char_trait_sync_capacity.h"
#include "char_trigger_area_capacity.h"
#include "char_trust_roster_capacity.h"
#include "char_valid_target_capacity.h"
#include "common/logging.h"
#include "common/timer.h"
#include "common/utils.h"

#include <cstring>

#include "enums/item_lockflg.h"
#include "items/item_access.h"
#include "packets/basic.h"
#include "packets/char_status.h"
#include "packets/char_sync.h"
#include "packets/char_update.h"
#include "packets/entity_update.h"
#include "packets/s2c/0x01d_item_same.h"
#include "packets/s2c/0x01f_item_list.h"
#include "packets/s2c/0x02a_talknumwork.h"
#include "packets/s2c/0x032_event.h"
#include "packets/s2c/0x033_eventstr.h"
#include "packets/s2c/0x034_eventnum.h"
#include "packets/s2c/0x036_talknum.h"
#include "packets/s2c/0x050_equip_list.h"
#include "packets/s2c/0x051_grap_list.h"
#include "packets/s2c/0x052_eventucoff.h"
#include "packets/s2c/0x053_systemmes.h"
#include "packets/s2c/0x055_scenarioitem.h"
#include "packets/s2c/0x058_assist.h"
#include "packets/s2c/0x0aa_magic_data.h"
#include "packets/s2c/0x0ac_command_data.h"
#include "packets/s2c/0x0df_group_attr.h"

#include "ai/ai_container.h"
#include "ai/controllers/player_controller.h"
#include "ai/helpers/targetfind.h"
#include "ai/states/ability_state.h"
#include "ai/states/attack_state.h"
#include "ai/states/item_state.h"
#include "ai/states/magic_state.h"
#include "ai/states/range_state.h"
#include "ai/states/weaponskill_state.h"

#include "ability.h"
#include "aman.h"
#include "attack.h"
#include "automaton_entity.h"
#include "battlefield.h"
#include "char_recast_container.h"

#include "action/action.h"
#include "action/interrupts.h"
#include "blue_spell.h"
#include "conquest_system.h"
#include "enums/key_items.h"
#include "enums/recast.h"
#include "ipc_client.h"
#include "item_container.h"
#include "items/item_equipment.h"
#include "items/item_furnishing.h"
#include "items/item_usable.h"
#include "items/item_weapon.h"
#include "items/transactions/synth.h"
#include "job_points.h"
#include "latent_effect_container.h"
#include "linkshell.h"
#include "mob_modifier.h"
#include "mobskill.h"
#include "modifier.h"
#include "notoriety_container.h"
#include "packets/s2c/0x020_item_attr.h"
#include "packets/s2c/0x028_battle2.h"
#include "packets/s2c/0x029_battle_message.h"
#include "packets/s2c/0x063_miscdata_status_icons.h"
#include "petskill.h"
#include "spell.h"
#include "status_effect_container.h"
#include "trade_container.h"
#include "treasure_pool.h"
#include "trust_entity.h"
#include "unitychat.h"
#include "universal_container.h"
#include "utils/attackutils.h"
#include "utils/battleutils.h"
#include "utils/charutils.h"
#include "utils/gardenutils.h"
#include "utils/messageutils.h"
#include "utils/moduleutils.h"
#include "utils/petutils.h"
#include "weapon_skill.h"

CCharEntity::CCharEntity()
: m_PlayTime(0s)
{
    TracyZoneScoped;

    objtype     = TYPE_PC;
    m_EcoSystem = xi::Ecosystem::Humanoid;

    eventPreparation = new EventPrep();
    currentEvent     = new EventInfo();

    inSequence       = false;
    gotMessage       = false;
    m_Locked         = false;
    m_zoneInCutscene = false;

    accid        = 0;
    m_GMlevel    = 0;
    m_isGMHidden = false;

    allegiance = ALLEGIANCE_TYPE::PLAYER;

    TradeContainer = new CTradeContainer();
    Container      = new CTradeContainer();
    UContainer     = new CUContainer();

    m_Inventory  = std::make_unique<CItemContainer>(LOC_INVENTORY);
    m_Mogsafe    = std::make_unique<CItemContainer>(LOC_MOGSAFE);
    m_Storage    = std::make_unique<CItemContainer>(LOC_STORAGE);
    m_Tempitems  = std::make_unique<CItemContainer>(LOC_TEMPITEMS);
    m_Moglocker  = std::make_unique<CItemContainer>(LOC_MOGLOCKER);
    m_Mogsatchel = std::make_unique<CItemContainer>(LOC_MOGSATCHEL);
    m_Mogsack    = std::make_unique<CItemContainer>(LOC_MOGSACK);
    m_Mogcase    = std::make_unique<CItemContainer>(LOC_MOGCASE);
    m_Wardrobe   = std::make_unique<CItemContainer>(LOC_WARDROBE);
    m_Mogsafe2   = std::make_unique<CItemContainer>(LOC_MOGSAFE2);
    m_Wardrobe2  = std::make_unique<CItemContainer>(LOC_WARDROBE2);
    m_Wardrobe3  = std::make_unique<CItemContainer>(LOC_WARDROBE3);
    m_Wardrobe4  = std::make_unique<CItemContainer>(LOC_WARDROBE4);
    m_Wardrobe5  = std::make_unique<CItemContainer>(LOC_WARDROBE5);
    m_Wardrobe6  = std::make_unique<CItemContainer>(LOC_WARDROBE6);
    m_Wardrobe7  = std::make_unique<CItemContainer>(LOC_WARDROBE7);
    m_Wardrobe8  = std::make_unique<CItemContainer>(LOC_WARDROBE8);
    m_RecycleBin = std::make_unique<CItemContainer>(LOC_RECYCLEBIN);

    keys = {};

    m_SpellList.reset();
    std::memset(&m_LearnedAbilities, 0, sizeof(m_LearnedAbilities));
    std::memset(&m_TitleList, 0, sizeof(m_TitleList));
    std::memset(&m_ZonesVisitedList, 0, sizeof(m_ZonesVisitedList));
    std::memset(&m_Abilities, 0, sizeof(m_Abilities));
    std::memset(&m_TraitList, 0, sizeof(m_TraitList));
    std::memset(&m_PetCommands, 0, sizeof(m_PetCommands));
    std::memset(&m_WeaponSkills, 0, sizeof(m_WeaponSkills));
    std::memset(&m_SetBlueSpells, 0, sizeof(m_SetBlueSpells));
    std::memset(&m_FieldChocobo, 0, sizeof(m_FieldChocobo));
    std::memset(&m_unlockedAttachments, 0, sizeof(m_unlockedAttachments));

    std::memset(&m_questLog, 0, sizeof(m_questLog));
    std::memset(&m_missionLog, 0, sizeof(m_missionLog));
    m_eminenceCache.activemap.reset();

    std::memset(&m_claimedDeeds, 0, sizeof(m_claimedDeeds));

    for (uint8 i = 0; i <= 3; ++i)
    {
        m_missionLog[i].current = 0xFFFF;
    }

    m_missionLog[4].current = 0;   // MISSION_TOAU
    m_missionLog[5].current = 0;   // MISSION_WOTG
    m_missionLog[6].current = 101; // MISSION_COP
    for (auto& i : m_missionLog)
    {
        i.statusUpper = 0;
        i.statusLower = 0;
    }

    m_PMonstrosity = nullptr;

    m_Costume            = 0;
    m_Costume2           = 0;
    m_hasTractor         = 0;
    m_hasRaise           = 0;
    m_weaknessLvl        = 0;
    m_hasArise           = false;
    m_LevelRestriction   = 0;
    servmesLastOffset_   = std::nullopt;
    m_AHHistoryTimestamp = timer::time_point::min();
    m_DeathTimestamp     = timer::time_point::min();

    m_EquipFlag         = 0;
    m_EquipBlock        = 0;
    m_StatsDebilitation = 0;

    MeritMode    = false;
    PMeritPoints = nullptr;
    PJobPoints   = nullptr;

    m_isStyleLocked = false;
    m_isBlockingAid = false;

    BazaarID.clean();

    WideScanTarget = std::nullopt;

    lastTradeInvite = {};
    TradePending.clean();
    InvitePending.clean();

    PLinkshell1   = nullptr;
    PLinkshell2   = nullptr;
    PUnityChat    = nullptr;
    PTreasurePool = nullptr;

    PClaimedMob            = nullptr;
    PRecastContainer       = std::make_unique<CCharRecastContainer>(this);
    PLatentEffectContainer = new CLatentEffectContainer(this);

    requestedWarp       = false;
    requestedZoneChange = false;

    retriggerLatents = false;

    resetPetZoningInfo();
    petZoningInfo.petID = 0;

    m_SaveTime    = timer::time_point::min();
    m_reloadParty = false;

    m_moghouseID     = 0;
    m_moghancementID = 0;

    m_Substate = CHAR_SUBSTATE::SUBSTATE_NONE;

    playerConfig = {};

    PAI = std::make_unique<CAIContainer>(this, nullptr, std::make_unique<CPlayerController>(this), std::make_unique<CTargetFind>(this));

    hookedFish   = nullptr;
    lastCastTime = 0;
    nextFishTime = 0;
    fishingToken = 0;
    hookDelay    = 13;

    profile = {};
    search  = {};
    std::memset(&styleItems, 0, sizeof(styleItems));

    m_StartActionPos   = {};
    m_ActionOffsetPos  = {};
    m_previousLocation = {};
    m_PrevZonelineID   = 0;

    m_jobMasterDisplay = false;
    m_EffectsChanged   = false;

    visibleGmLevel        = 0;
    wallhackEnabled       = false;
    isFrozenFlagged       = false;
    isSettingBazaarPrices = false;
    isLinkDead            = false;
    pendingPositionUpdate = false;
}

CCharEntity::~CCharEntity()
{
    TracyZoneScoped;

    clearPacketList();

    if (PTreasurePool != nullptr)
    {
        // remove myself
        PTreasurePool->delMember(this);
    }

    ClearTrusts(); // trusts don't survive zone lines

    if (PLinkshell1 != nullptr)
    {
        PLinkshell1->DelMember(this);
    }

    if (PLinkshell2 != nullptr)
    {
        PLinkshell2->DelMember(this);
    }

    if (PUnityChat != nullptr)
    {
        PUnityChat->DelMember(this);
    }

    if (isDead() && !charutils::IsCharacterPersistenceSuppressedForTests())
    {
        charutils::SaveDeathTime(this);
    }

    uint8 sameZonePeerCount = 0;
    if (m_LevelRestriction != 0 && PParty && PParty->GetSyncTarget())
    {
        for (uint32 i = 0; i < PParty->members.size(); ++i)
        {
            if (PParty->members.at(i) != this && PParty->members.at(i)->getZone() == PParty->GetSyncTarget()->getZone())
            {
                sameZonePeerCount++;
            }
        }
    }
    const auto plan = partyhelpers::PlanLevelSyncDeparture(
        m_LevelRestriction != 0,
        PParty != nullptr,
        PParty && (PParty->GetSyncTarget() == this || PParty->GetLeader() == this),
        PParty && PParty->GetSyncTarget() != nullptr,
        sameZonePeerCount);
    switch (plan.action)
    {
        case partyhelpers::LevelSyncDepartureAction::DeactivateLeftArea:
            PParty->SetSyncTarget("", MsgStd::LevelSyncDeactivateLeftArea);
            break;
        case partyhelpers::LevelSyncDepartureAction::RemoveTooFewMembers:
            PParty->SetSyncTarget("", MsgStd::LevelSyncRemoveTooFewMembers);
            break;
        case partyhelpers::LevelSyncDepartureAction::None:
            break;
    }
    if (plan.clearDepartingEffects)
    {
        StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelSync);
        StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelRestriction);
    }

    if (PParty && loc.destination != 0 && !inMogHouse())
    {
        if (PParty->m_PAlliance)
        {
            message::send(ipc::AllianceReload{
                .allianceId = PParty->m_PAlliance->m_AllianceID,
            });
        }
        else
        {
            message::send(ipc::PartyReload{
                .partyId = PParty->GetPartyID(),
            });
        }
    }

    SpawnPCList.clear();
    SpawnNPCList.clear();
    SpawnMOBList.clear();
    SpawnPETList.clear();
    SpawnTRUSTList.clear();

    if (PParty)
    {
        PParty->PopMember(this);
    }

    if (!charutils::IsCharacterPersistenceSuppressedForTests())
    {
        charutils::WriteHistory(this);
    }

    this->clearTransactions();

    destroy(TradeContainer);
    destroy(Container);
    destroy(UContainer);
    destroy(PLatentEffectContainer);

    destroy(eventPreparation);
    destroy(currentEvent);

    while (!eventQueue.empty())
    {
        auto head = eventQueue.front();
        eventQueue.pop_front();
        destroy(head);
    }
}

uint8 CCharEntity::GetGender()
{
    return chargenderhelpers::FromRace(look.race);
}

bool CCharEntity::isPacketListEmpty()
{
    return PacketList.empty();
}

auto CCharEntity::getPacketList() const -> const std::deque<std::unique_ptr<CBasicPacket>>&
{
    return PacketList;
}

auto CCharEntity::getPacketListCopy() -> std::deque<std::unique_ptr<CBasicPacket>>
{
    std::deque<std::unique_ptr<CBasicPacket>> PacketListCopy;
    for (const auto& packet : PacketList)
    {
        PacketListCopy.emplace_back(packet->copy());
    }
    return PacketListCopy;
}

void CCharEntity::clearPacketList()
{
    while (!PacketList.empty())
    {
        std::ignore = popPacket();
    }
}

void CCharEntity::pushPacket(std::unique_ptr<CBasicPacket>&& packet)
{
    TracyZoneScoped;
    TracyZoneString(getName());
    TracyZoneHex16(packet->getType());

    if (isPacketFiltered(packet))
    {
        // packet will destruct itself when it goes out of scope
        return;
    }

    moduleutils::OnPushPacket(this, packet);

    charpacketqueuehelpers::OnPush(
        packet->getType(),
        packet->getType() == 0x5B ? packet->ref<uint32>(0x10) : 0,
        this->id,
        [&](const bool pending) { pendingPositionUpdate = pending; });

    PacketList.emplace_back(std::move(packet));
}

void CCharEntity::updateEntityPacket(CBaseEntity* PEntity, ENTITYUPDATE type, uint8 updatemask)
{
    auto       itr              = EntityUpdatePackets.find(PEntity->id);
    const bool hasPendingPacket = itr != EntityUpdatePackets.end() && itr->second != nullptr;

    auto* PChar = [&]() -> CCharEntity*
    {
        if (PEntity->objtype == TYPE_PC)
        {
            return static_cast<CCharEntity*>(PEntity);
        }

        return nullptr;
    }();

    charentityupdatehelpers::Apply(
        hasPendingPacket,
        PChar != nullptr,
        [&]() { static_cast<CCharUpdatePacket*>(itr->second)->updateWith(PChar, type, updatemask); },
        [&]() { static_cast<CEntityUpdatePacket*>(itr->second)->updateWith(PEntity, type, updatemask); },
        [&]()
        {
            auto packet                    = std::make_unique<CCharUpdatePacket>(PChar, type, updatemask);
            EntityUpdatePackets[PChar->id] = packet.get();
            PacketList.emplace_back(std::move(packet));
        },
        [&]()
        {
            auto packet                      = std::make_unique<CEntityUpdatePacket>(PEntity, type, updatemask);
            EntityUpdatePackets[PEntity->id] = packet.get();
            PacketList.emplace_back(std::move(packet));
        });
}

auto CCharEntity::popPacket() -> std::unique_ptr<CBasicPacket>
{
    auto PPacket = std::move(PacketList.front());
    PacketList.pop_front();

    const auto packetType = PPacket->getType();
    uint32     packetEntityID{};
    if (packetType == 0x0D || packetType == 0x0E)
    {
        packetEntityID = PPacket->ref<uint32>(0x04);
    }
    else if (packetType == 0x5B)
    {
        packetEntityID = PPacket->ref<uint32>(0x10);
    }
    charpacketqueuehelpers::OnPop(
        packetType,
        packetEntityID,
        this->id,
        [&](const uint32 entityID) { EntityUpdatePackets.erase(entityID); },
        [&](const bool pending) { pendingPositionUpdate = pending; });

    return PPacket;
}

size_t CCharEntity::getPacketCount()
{
    return PacketList.size();
}

void CCharEntity::erasePackets(uint8 num)
{
    for (auto i = 0; i < num; i++)
    {
        std::ignore = popPacket();
    }
}

bool CCharEntity::isPacketFiltered(std::unique_ptr<CBasicPacket>& packet)
{
    return charpacketqueuehelpers::Filtered(packet->getType(), playerConfig.MessageFilter.others_synthesis_and_fishing_results);
}

bool CCharEntity::isNewPlayer() const
{
    return !playerConfig.NewAdventurerOffFlg;
}

bool CCharEntity::isSeekingParty() const
{
    return playerConfig.InviteFlg;
}

bool CCharEntity::isAnon() const
{
    return playerConfig.AnonymityFlg;
}

bool CCharEntity::isAway() const
{
    return playerConfig.AwayFlg;
}

bool CCharEntity::hasAutoTargetEnabled() const
{
    return !playerConfig.AutoTargetOffFlg;
}

auto CCharEntity::isCrafting() const -> bool
{
    return charactivityhelpers::IsCrafting(animation, this->activeTransaction<SynthTransaction>() != nullptr);
}

auto CCharEntity::isFishing() const -> bool
{
    return charactivityhelpers::IsFishing(animation);
}

void CCharEntity::setPetZoningInfo()
{
    if (PPet == nullptr || PPet->objtype != TYPE_PET)
    {
        return;
    }

    auto PPetEntity = dynamic_cast<CPetEntity*>(PPet);
    if (PPetEntity == nullptr)
    {
        return;
    }
    const auto petType = PPetEntity->getPetType();
    const auto petID   = PPetEntity->petID();
    const bool transientAvatar = petID == PETID_ALEXANDER || petID == PETID_ODIN || petID == PETID_ATOMOS;
    charpetzoninghelpers::ApplyCapture(
        petType == PET_TYPE::JUG_PET,
        petType == PET_TYPE::AVATAR,
        petType == PET_TYPE::AUTOMATON,
        petType == PET_TYPE::WYVERN,
        transientAvatar,
        [&]() { return settings::get<bool>("map.KEEP_JUGPET_THROUGH_ZONING"); },
        [&]() { petZoningInfo.petID = petID; },
        [&]()
        {
            petZoningInfo.jugSpawnTime = PPetEntity->getJugSpawnTime();
            petZoningInfo.jugDuration  = PPetEntity->getJugDuration();
        },
        [&]()
        {
            petZoningInfo.petLevel = PPetEntity->getSpawnLevel();
            petZoningInfo.petHP    = PPet->health.hp;
            petZoningInfo.petTP    = PPet->health.tp;
            petZoningInfo.petMP    = PPet->health.mp;
            petZoningInfo.petType  = petType;
        },
        [&]() { petZoningInfo.respawnPet = true; });
}

void CCharEntity::resetPetZoningInfo()
{
    charpetzoninghelpers::ApplyReset(
        [&]() { petZoningInfo.petLevel = 0; },
        [&]() { petZoningInfo.petHP = 0; },
        [&]() { petZoningInfo.petTP = 0; },
        [&]() { petZoningInfo.petMP = 0; },
        [&]() { petZoningInfo.respawnPet = false; },
        [&]() { petZoningInfo.petType = PET_TYPE::AVATAR; },
        [&]() { petZoningInfo.jugSpawnTime = timer::time_point{}; },
        [&]() { petZoningInfo.jugDuration = 0s; });
}

auto CCharEntity::shouldPetPersistThroughZoning() const -> bool
{
    PET_TYPE petType{};
    auto     PPetEntity = dynamic_cast<CPetEntity*>(PPet);

    if (PPetEntity == nullptr && !petZoningInfo.respawnPet)
    {
        return false;
    }

    if (PPetEntity != nullptr)
    {
        petType = PPetEntity->getPetType();
    }
    else // petZoningInfo.respawnPet == true
    {
        petType = petZoningInfo.petType;
    }

    return charpetzoninghelpers::ShouldPersist(
        PPetEntity != nullptr,
        petZoningInfo.respawnPet,
        petType == PET_TYPE::WYVERN,
        petType == PET_TYPE::AVATAR,
        petType == PET_TYPE::AUTOMATON,
        petType == PET_TYPE::JUG_PET,
        [&]() { return settings::get<bool>("map.KEEP_JUGPET_THROUGH_ZONING"); });
}

void CCharEntity::setAutomatonFrame(const AutomatonFrame frame)
{
    automatonInfo_.equip.frame = frame;
}

void CCharEntity::setAutomatonHead(const AutomatonHead head)
{
    automatonInfo_.equip.head = head;
}

void CCharEntity::setAutomatonAttachment(const uint8 slotid, const uint8 id)
{
    automatonInfo_.equip.attachments[slotid] = id;
}

void CCharEntity::setAutomatonElementMax(const uint8 element, const uint8 max)
{
    automatonInfo_.elementMax[element] = max;
}

void CCharEntity::addAutomatonElementCapacity(const uint8 element, const int8 value)
{
    automatonInfo_.elementEquip[element] += value;
}

void CCharEntity::setAutomatonElementalCapacityBonus(const uint8 bonus)
{
    charautomatonhelpers::ApplyElementalCapacityBonus(
        bonus, automatonInfo_.elementalCapacityBonus, automatonInfo_.elementMax);
}

auto CCharEntity::getAutomatonFrame() const -> AutomatonFrame
{
    return automatonInfo_.equip.frame;
}

auto CCharEntity::getAutomatonHead() const -> AutomatonHead
{
    return automatonInfo_.equip.head;
}

auto CCharEntity::getAutomatonAttachment(const uint8 slotid) const -> uint8
{
    return automatonInfo_.equip.attachments[slotid];
}

auto CCharEntity::hasAutomatonAttachment(const uint8 attachment) const -> bool
{
    return charautomatonhelpers::HasAttachment(automatonInfo_.equip.attachments, attachment);
}

auto CCharEntity::getAutomatonElementMax(const uint8 element) const -> uint8
{
    return automatonInfo_.elementMax[element];
}

auto CCharEntity::getAutomatonElementCapacity(const uint8 element) const -> uint8
{
    return automatonInfo_.elementEquip[element];
}

/************************************************************************
 *
 * Return the container with the specified ID.If the ID goes beyond, then *
 * We protect the server from falling the use of temporary container *
 * Items as a plug (from this container items can not *
 * Move, wear, transmit, sell, etc.).Display *
 * Fatal error message.*
 *
 ************************************************************************/

auto CCharEntity::getStorage(const uint8 locationId) const -> CItemContainer*
{
    const std::array<CItemContainer*, MAX_CONTAINER_ID> containers = {
        m_Inventory.get(), m_Mogsafe.get(), m_Storage.get(), m_Tempitems.get(),
        m_Moglocker.get(), m_Mogsatchel.get(), m_Mogsack.get(), m_Mogcase.get(),
        m_Wardrobe.get(), m_Mogsafe2.get(), m_Wardrobe2.get(), m_Wardrobe3.get(),
        m_Wardrobe4.get(), m_Wardrobe5.get(), m_Wardrobe6.get(), m_Wardrobe7.get(),
        m_Wardrobe8.get(), m_RecycleBin.get()
    };
    auto container = charstoragehelpers::Resolve(locationId, containers);
    if (container.has_value())
    {
        return *container;
    }

    ShowWarning("Unhandled or Invalid Location ID (%d) passed to function.", locationId);
    return nullptr;
}

auto CCharEntity::aman() -> CAMANContainer&
{
    return charamanhosthelpers::Get(m_AMAN, [&]() { return CAMANContainer(this); });
}

auto CCharEntity::lastProposalCloseTime() const -> timer::time_point
{
    return charruntimestatehelpers::Get(lastProposalCloseTime_);
}

void CCharEntity::setLastProposalCloseTime(timer::time_point t)
{
    charruntimestatehelpers::Set(lastProposalCloseTime_, t);
}

auto CCharEntity::inMogHouse() const -> bool
{
    return charruntimestatehelpers::InMogHouse(m_moghouseID);
}

auto CCharEntity::gmCallContainer() -> GMCallContainer&
{
    return gmCallContainer_;
}

auto CCharEntity::maze() -> maze_t&
{
    return maze_;
}

int8 CCharEntity::getShieldSize()
{
    CItemEquipment* PItem = getEquip(SLOT_SUB);
    return charshieldhelpers::ShieldSize(
        PItem != nullptr,
        PItem != nullptr && PItem->IsShield(),
        PItem != nullptr ? static_cast<int8>(PItem->getShieldSize()) : 0);
}

int16 CCharEntity::getShieldDefense()
{
    CItemEquipment* PItem = getEquip(SLOT_SUB);
    return charshieldhelpers::ShieldDefense(
        PItem != nullptr,
        PItem != nullptr && PItem->IsShield(),
        PItem != nullptr ? PItem->getModifier(Mod::DEF) : 0);
}

bool CCharEntity::hasBazaar()
{
    return charbazaarhelpers::HasBazaar(
        isSettingBazaarPrices,
        [&]() { return getStorage(LOC_INVENTORY); },
        [](const CItemContainer* inventory) { return inventory->GetSize(); },
        [](const CItemContainer* inventory, const uint8 slotId)
        {
            const auto* item = inventory->GetItem(slotId);
            return item == nullptr ? 0 : item->getCharPrice();
        });
}

void CCharEntity::SetName(const std::string& name)
{
    this->name = charnamehelpers::Normalize(name);
}

int16 CCharEntity::addTP(int16 tp)
{
    return charresourcehelpers::Apply<int16>(
        tp,
        [&](const int16 requested) { return CBattleEntity::addTP(requested); },
        [&]() { PLatentEffectContainer->CheckLatentsTP(); });
}

int32 CCharEntity::addHP(int32 hp)
{
    return charresourcehelpers::Apply<int32>(
        hp,
        [&](const int32 requested) { return CBattleEntity::addHP(requested); },
        [&]() { PLatentEffectContainer->CheckLatentsHP(); });
}

int32 CCharEntity::addMP(int32 mp)
{
    return charresourcehelpers::Apply<int32>(
        mp,
        [&](const int32 requested) { return CBattleEntity::addMP(requested); },
        [&]() { PLatentEffectContainer->CheckLatentsMP(); });
}

bool CCharEntity::getStyleLocked() const
{
    return charruntimestatehelpers::Get(m_isStyleLocked);
}

void CCharEntity::setStyleLocked(bool isStyleLocked)
{
    charruntimestatehelpers::Set(m_isStyleLocked, isStyleLocked);
}

bool CCharEntity::getBlockingAid() const
{
    return charruntimestatehelpers::Get(m_isBlockingAid);
}

void CCharEntity::setBlockingAid(bool isBlockingAid)
{
    charruntimestatehelpers::Set(m_isBlockingAid, isBlockingAid);
}

void CCharEntity::SetPlayTime(timer::duration playTime)
{
    charplaytimehelpers::Set(playTime, m_PlayTime, m_SaveTime, []() { return timer::now(); });
}

timer::duration CCharEntity::GetPlayTime(bool needUpdate)
{
    return charplaytimehelpers::Get(needUpdate, m_PlayTime, m_SaveTime, []() { return timer::now(); });
}

auto CCharEntity::getEquip(const SLOTTYPE slot) const -> CItemEquipment*
{
    if (slot >= EquipSlotCount)
    {
        ShowWarningFmt("getEquip: slot {} out of range", slot);
        return nullptr;
    }

    return static_cast<CItemEquipment*>(charequipmenthelpers::Get(equipped_, slot));
}

auto CCharEntity::equipLocation(const uint8 equipSlot) const -> std::optional<ItemLocation>
{
    if (equipSlot >= EquipSlotCount)
    {
        ShowWarningFmt("equipLocation: slot {} out of range", equipSlot);
        return std::nullopt;
    }

    return charequipmenthelpers::Location(
        equipped_,
        equipSlot,
        [](CItem* item)
        {
            return ItemLocation{
                static_cast<CONTAINER_ID>(item->getLocationID()),
                item->getSlotID(),
            };
        });
}

auto CCharEntity::bindEquip(const uint8 equipSlot, CItem* item) -> bool
{
    if (equipSlot >= EquipSlotCount)
    {
        ShowWarningFmt("bindEquip: slot {} out of range", equipSlot);
        return false;
    }

    if (!item)
    {
        ShowWarningFmt("bindEquip: null item for slot {}", equipSlot);
        return false;
    }

    return charequipmenthelpers::Bind(
        equipped_,
        equipSlot,
        item,
        [](CItem* toEquip) { return xi::items::mark(toEquip, ItemState::Equipped); },
        [](CItem* toFree) { return xi::items::mark(toFree, ItemState::Free); });
}

void CCharEntity::clearEquip(const uint8 equipSlot)
{
    if (equipSlot >= EquipSlotCount)
    {
        return;
    }

    charequipmenthelpers::Clear(
        equipped_, equipSlot, [](CItem* item) { return xi::items::mark(item, ItemState::Free); });
}

void CCharEntity::ReloadPartyInc()
{
    m_reloadParty = true;
}

void CCharEntity::ReloadPartyDec()
{
    m_reloadParty = false;
}

bool CCharEntity::ReloadParty() const
{
    return m_reloadParty;
}

void CCharEntity::RemoveTrust(CTrustEntity* PTrust)
{
    chartrustrosterhelpers::Remove(
        PTrusts,
        PTrust,
        [](CTrustEntity* trust) { return trust->PAI->IsSpawned(); },
        [](CTrustEntity* trust) { trust->PAI->Despawn(); },
        [&]() { ReloadPartyInc(); });
}

void CCharEntity::ClearTrusts()
{
    chartrustrosterhelpers::Clear(
        PTrusts,
        [](CTrustEntity* trust) { trust->PAI->Despawn(); },
        [&]() { ReloadPartyInc(); });
}

void CCharEntity::RequestPersist(CHAR_PERSIST toPersist)
{
    charpersistencehelpers::Request(dataToPersist, static_cast<uint8>(toPersist));
}

bool CCharEntity::PersistData()
{
    // TODO: Add a SaveCharLinkshells callback if CHAR_PERSIST::LINKSHELL is introduced.
    return charpersistencehelpers::Flush(
        charVarChanges,
        dataToPersist,
        static_cast<uint8>(CHAR_PERSIST::EQUIP),
        static_cast<uint8>(CHAR_PERSIST::POSITION),
        static_cast<uint8>(CHAR_PERSIST::EFFECTS),
        [&](const std::string& name)
        {
            const auto& [value, expiry] = charVarCache[name];
            charutils::PersistCharVar(this->id, name.c_str(), value, expiry);
        },
        [&]() { charutils::SaveCharEquip(this); },
        [&]() { charutils::SaveCharLook(this); },
        [&]() { charutils::SaveCharPosition(this); },
        [&]() { StatusEffectContainer->SaveStatusEffects(true); });
}

bool CCharEntity::PersistData(timer::time_point tick)
{
    return charpersistencehelpers::FlushAt(
        tick, nextDataPersistTime, TIME_BETWEEN_PERSIST, [&]() { return PersistData(); });
}

auto CCharEntity::Tick(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;

    co_await CBattleEntity::Tick(tick);

    // Send an update packet at a regular interval to keep the player's death variables synced.
    chartickhelpers::AfterBase(
        tick,
        chartickhelpers::HasDeathTimestamp(m_DeathTimestamp, timer::time_point::min()),
        m_deathSyncTime,
        updatemask,
        UPDATE_STATUS,
        death_update_frequency,
        [&]() { return inMogHouse(); },
        [&]() { gardenutils::UpdateGardening(this, SendPacket::Yes); });

    co_return;
}

void CCharEntity::PostTick()
{
    TracyZoneScoped;

    charposttickrefreshhelpers::Apply(
        m_EffectsChanged,
        [&]() { CBattleEntity::PostTick(); },
        [&]() { return ReloadParty(); },
        [&]() { charutils::ReloadParty(this); },
        [&]() { pushPacket<CCharStatusPacket>(this); },
        [&]() { pushPacket<CCharSyncPacket>(this); },
        [&]() { charutils::SendExtendedJobPackets(this); },
        [&]() { pushPacket<GP_SERV_COMMAND_MISCDATA::STATUS_ICONS>(this); },
        [&]() { return PParty != nullptr; },
        [&]() { PParty->PushEffectsPacket(); });

    charposttickupdatehelpers::Apply(
        timer::now(),
        m_nextUpdateTimer,
        updatemask,
        sendServerStatus_,
        UPDATE_POS,
        UPDATE_HP,
        250ms,
        m_isGMHidden,
        [&]() { return loc.zone != nullptr; },
        [&](const uint8 updateMask) { loc.zone->UpdateEntityPacket(this, ENTITY_UPDATE, updateMask); },
        [&]() { return isCharmed; },
        [&](const uint8 updateMask) { updateEntityPacket(this, ENTITY_UPDATE, updateMask); },
        [&]()
        {
            // clang-format off
            ForAlliance([&](auto PEntity)
            {
                static_cast<CCharEntity*>(PEntity)->pushPacket<GP_SERV_COMMAND_GROUP_ATTR>(this);
            });
            // clang-format on
        },
        [&]() { pushPacket<CCharStatusPacket>(this); },
        [&]() { inventorySyncState_.flushDirtyItems(this); });
}

// Flush all pending equipment changes at end of network cycle after all SmallPackets have been processed
void CCharEntity::flushEquipChanges()
{
    charequipflushhelpers::Apply(
        inventorySyncState_.pendingEquipChanges(),
        inventorySyncState_.dirtyContainers(),
        LOC_INVENTORY,
        ItemLockFlg::Normal,
        ItemLockFlg::NoDrop,
        [&](const CONTAINER_ID container) { return inventorySyncState_.isSynced(container); },
        [](const CItem* item) { return item->isSubType(ITEM_CHARGED); },
        [&](const uint8 containerSlotId, const SLOTTYPE equipSlot, const CONTAINER_ID container)
        { pushPacket<GP_SERV_COMMAND_EQUIP_LIST>(containerSlotId, equipSlot, container); },
        [&]() { pushPacket<GP_SERV_COMMAND_GRAP_LIST>(this); },
        [&](CItem* item, const CONTAINER_ID container, const uint8 slotId)
        { pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(item, container, slotId); },
        [&](CItem* item, const ItemLockFlg lockFlag)
        { pushPacket<GP_SERV_COMMAND_ITEM_LIST>(item, lockFlag); },
        [&](const CONTAINER_ID container) { pushPacket<GP_SERV_COMMAND_ITEM_SAME>(container, this); },
        [&]() { pushPacket<GP_SERV_COMMAND_ITEM_SAME>(this); },
        [&]() { pushPacket<GP_SERV_COMMAND_MAGIC_DATA>(this); },
        [&]() { pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(this); },
        [&]() { inventorySyncState_.clearEquipChanges(); });
}

auto CCharEntity::inventorySyncState() -> InventorySyncState&
{
    return inventorySyncState_;
}

void CCharEntity::addTrait(CTrait* PTrait)
{
    chartraitsynchelpers::Apply(
        PTrait->getID(),
        [&]() { CBattleEntity::addTrait(PTrait); },
        [&](const uint16 traitID) { charutils::addTrait(this, traitID); });
}

void CCharEntity::delTrait(CTrait* PTrait)
{
    chartraitsynchelpers::Apply(
        PTrait->getID(),
        [&]() { CBattleEntity::delTrait(PTrait); },
        [&](const uint16 traitID) { charutils::delTrait(this, traitID); });
}

bool CCharEntity::ValidTarget(CBattleEntity* PInitiator, uint16 targetFlags)
{
    TracyZoneScoped;

    const charvalidtargethelpers::Context ctx{
        .confrontationMatches = StatusEffectContainer->GetConfrontationEffect() == PInitiator->StatusEffectContainer->GetConfrontationEffect(),
        .dead                 = isDead(),
        .targetPlayerDead     = (targetFlags & TARGET_PLAYER_DEAD) != 0,
        .targetPlayer         = (targetFlags & TARGET_PLAYER) != 0,
        .sameAllegiance       = allegiance == PInitiator->allegiance,
    };
    return charvalidtargethelpers::Apply(
        ctx,
        [&]() { return CBattleEntity::ValidTarget(PInitiator, targetFlags); },
        [&]()
        {
            return charvalidtargethelpers::Relations{
                .sameParty          = PParty && PInitiator->PParty && PInitiator->PParty == PParty,
                .sameAlliance       = PParty && PParty->m_PAlliance && PInitiator->PParty && PInitiator->PParty->m_PAlliance && PParty->m_PAlliance == PInitiator->PParty->m_PAlliance,
                .partyPetMaster     = PInitiator->PMaster && PInitiator->PMaster->PParty && PInitiator->PMaster->PParty == PParty,
                .soloPetMaster      = PParty == nullptr && PInitiator->PMaster == this,
                .targetsParty       = (targetFlags & TARGET_PLAYER_PARTY) != 0,
                .targetsAlliance    = (targetFlags & TARGET_PLAYER_ALLIANCE) != 0,
                .pianissimoTarget   = (targetFlags & TARGET_PLAYER_PARTY_PIANISSIMO) != 0,
                .entrustTarget      = (targetFlags & TARGET_PLAYER_PARTY_ENTRUST) != 0,
                .differentCharacter = PInitiator != this,
                .initiatorIsTrust   = PInitiator->objtype == TYPE_TRUST,
            };
        },
        [&]() { return PInitiator->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Pianissimo); },
        [&]() { return PInitiator->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Entrust); });
}

bool CCharEntity::CanUseSpell(CSpell* PSpell)
{
    TracyZoneScoped;

    return charactionboundaryhelpers::CanUseSpell(
        [&]() { return charutils::hasSpell(this, static_cast<uint16>(PSpell->getID())); },
        [&]() { return CBattleEntity::CanUseSpell(PSpell); });
}

void CCharEntity::OnChangeTarget(CBattleEntity* PNewTarget)
{
    TracyZoneScoped;

    charcombattransitionhelpers::ChangeTarget(
        [&]() { battleutils::RelinquishClaim(this); },
        [&]() { pushPacket<GP_SERV_COMMAND_ASSIST>(this, PNewTarget); },
        [&]() { PLatentEffectContainer->CheckLatentsTargetChange(); });
}

void CCharEntity::OnEngage(CAttackState& state)
{
    TracyZoneScoped;

    charcombattransitionhelpers::Engage(
        m_charHistory.battlesFought,
        [&]() { CBattleEntity::OnEngage(state); },
        [&]() { PLatentEffectContainer->CheckLatentsTargetChange(); });
}

void CCharEntity::OnDisengage(CAttackState& state)
{
    TracyZoneScoped;

    charcombattransitionhelpers::Disengage(
        [&]() { battleutils::RelinquishClaim(this); },
        [&]() { CBattleEntity::OnDisengage(state); },
        [&]() { return state.HasErrorMsg(); },
        [&]() { pushPacket(state.GetErrorMsg()); },
        [&](const bool weaponDrawn) { PLatentEffectContainer->CheckLatentsWeaponDraw(weaponDrawn); });
}

bool CCharEntity::CanAttack(CBattleEntity* PTarget, std::unique_ptr<CBasicPacket>& errMsg)
{
    TracyZoneScoped;

    using namespace canattackhelpers;

    const float dist = distance(loc.p, PTarget->loc.p);
    const auto  fail = EvaluateCharCanAttack(
        PTarget->PAI->IsUntargetable(),
        IsMobOwner(PTarget),
        StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::CharmI, xi::StatusEffect::CharmIi }),
        dist,
        facing(this->loc.p, PTarget->loc.p, CharFacingCone),
        GetMeleeRange(PTarget));

    if (CharCanAttackSucceeded(fail))
    {
        return true;
    }

    if (fail == CanAttackFail::Untargetable)
    {
        return false;
    }

    errMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(
        this, PTarget, 0, 0, static_cast<MsgBasic>(static_cast<uint16>(fail)));

    if (ShouldDisengageOnCharCanAttackFail(fail))
    {
        PAI->Disengage();
    }
    return false;
}

bool CCharEntity::OnAttack(CAttackState& state, action_t& action)
{
    TracyZoneScoped;

    auto* controller{ static_cast<CPlayerController*>(PAI->GetController()) };
    return charactionboundaryhelpers::Attack(
        [&]() { return timer::now(); },
        [&](const timer::time_point attackTime) { controller->setLastAttackTime(attackTime); },
        [&]() { return CBattleEntity::OnAttack(state, action); });
}

void CCharEntity::OnCastFinished(CMagicState& state, action_t& action)
{
    TracyZoneScoped;

    auto* PSpell  = state.GetSpell();
    auto* PTarget = static_cast<CBattleEntity*>(state.GetTarget());

    // not ideal, since Trick Attack character (taChar) is also calculated on the lua side for the base spell.
    // Only blue spells that act as a physical WS can TA.
    CBattleEntity* taChar = nullptr;

    if (charcastfinishhelpers::ShouldResolveTrickAttack(
            StatusEffectContainer->HasStatusEffect(xi::StatusEffect::TrickAttack),
            PSpell->getSpellGroup() == SPELLGROUP_BLUE,
            PSpell->dealsDamage()))
    {
        taChar = battleutils::getAvailableTrickAttackChar(this, PTarget);
    }

    auto* controller{ static_cast<CPlayerController*>(PAI->GetController()) };
    controller->setLastSpellFinishedTime(timer::now());

    CBattleEntity::OnCastFinished(state, action);

    for (auto&& actionTarget : action.targets)
    {
        for (auto&& actionResult : actionTarget.results)
        {
            if (charcastfinishhelpers::ShouldApplyBlueSkillchain(
                    actionResult.param,
                    PSpell->dealsDamage(),
                    PSpell->getSpellGroup() == SPELLGROUP_BLUE,
                    StatusEffectContainer->HasStatusEffect(xi::StatusEffect::ChainAffinity) ||
                        StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AzureLore),
                    static_cast<CBlueSpell*>(PSpell)->getPrimarySkillchain() != 0))
            {
                auto*      PBlueSpell = static_cast<CBlueSpell*>(PSpell);
                const auto effect     = battleutils::GetSkillChainEffect(PTarget, PBlueSpell->getPrimarySkillchain(), PBlueSpell->getSecondarySkillchain(), 0);
                if (effect != ActionProcSkillChain::None)
                {
                    actionResult.recordSkillchain(effect, battleutils::TakeSkillchainDamage(this, PTarget, actionResult.param, taChar));
                }

                health.tp = charcastfinishhelpers::RemainingTPAfterBlueSkillchain(
                    health.tp,
                    StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::Sekkanoki, xi::StatusEffect::MeikyoShisui }));

                StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::ChainAffinity);
            }

            // Immanence will create or extend a skillchain for elemental spells
            if (charcastfinishhelpers::ShouldApplyImmanenceSkillchain(
                    PTarget->health.hp > 0,
                    actionResult.param,
                    PSpell->dealsDamage(),
                    PSpell->getSpellGroup() == SPELLGROUP_BLACK,
                    StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Immanence)))
            {
                const auto mapping = charimmanencehelpers::MapFamily(static_cast<uint16>(PSpell->getSpellFamily()));
                auto       effect  = ActionProcSkillChain::None;

                if (mapping.applies)
                {
                    effect = battleutils::GetSkillChainEffect(
                        PTarget, static_cast<SKILLCHAIN_ELEMENT>(mapping.skillchainElement), 0, 0);
                    StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Immanence);
                }

                if (effect != ActionProcSkillChain::None)
                {
                    actionResult.recordSkillchain(effect, battleutils::TakeSkillchainDamage(this, PTarget, actionResult.param, nullptr));

                    // Closing a skillchain with an immanence Helix will make the magic burst window longer
                    auto scEffect = PTarget->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Skillchain, 0);
                    if (mapping.isHelix && scEffect)
                    {
                        scEffect->SetDuration(scEffect->GetDuration() + std::chrono::seconds(charimmanencehelpers::HelixDurationExtensionSeconds()));
                    }
                }
            }
        }
    }
    charutils::RemoveStratagems(this, PSpell);
    if (PSpell->tookEffect())
    {
        charutils::TrySkillUP(this, (SKILLTYPE)PSpell->getSkillType(), PTarget->GetMLevel());

        CItemWeapon* PItem = static_cast<CItemWeapon*>(getEquip(SLOT_RANGED));

        if (PItem && PItem->isType(ITEM_EQUIPMENT))
        {
            SKILLTYPE Skilltype = (SKILLTYPE)PItem->getSkillType();

            if (charcastskilluphelpers::ShouldSkillUpRanged(static_cast<uint8>(PSpell->getSkillType()), static_cast<uint8>(Skilltype)))
            {
                charutils::TrySkillUP(this, Skilltype, PTarget->GetMLevel());
            }
        }
    }
}

void CCharEntity::OnCastInterrupted(CMagicState& state, action_t& action, MsgBasic msg, bool blockedCast)
{
    TracyZoneScoped;

    charactionboundaryhelpers::CastInterrupted(
        [&]() { CBattleEntity::OnCastInterrupted(state, action, msg, blockedCast); },
        [&]() { return state.HasErrorMsg(); },
        [&]() { return state.GetErrorMsg(); },
        [&](std::unique_ptr<CBasicPacket> message)
        {
            // TODO: May need special handling if interrupt was handled elsewhere
            pushPacket(std::move(message));
        });
}

void CCharEntity::OnWeaponSkillFinished(CWeaponSkillState& state, action_t& action)
{
    TracyZoneScoped;

    CBattleEntity::OnWeaponSkillFinished(state, action);

    auto* PWeaponSkill  = state.GetSkill();
    auto* PBattleTarget = static_cast<CBattleEntity*>(state.GetTarget());

    int16 tp = state.GetSpentTP();
    tp       = battleutils::CalculateWeaponSkillTP(this, PWeaponSkill, tp);

    PLatentEffectContainer->CheckLatentsTP();
    PLatentEffectContainer->CheckLatentsWS(true);

    SLOTTYPE damslot = SLOT_MAIN;

    if (charweaponskillrangehelpers::InRange(
            distance(loc.p, PBattleTarget->loc.p),
            PWeaponSkill->getRange(),
            this->modelHitboxSize,
            PBattleTarget->modelHitboxSize))
    {
        damslot = static_cast<SLOTTYPE>(charweaponskillrangehelpers::DamageSlot(PWeaponSkill->getID()));

        PAI->TargetFind->reset();
        // TODO: revise parameters
        if (PWeaponSkill->isAoE())
        {
            PAI->TargetFind->findWithinArea(PBattleTarget, AOE_RADIUS::TARGET, PWeaponSkill->getRadius(), FINDFLAGS_NONE, TARGET_NONE);
        }
        else
        {
            PAI->TargetFind->findSingleTarget(PBattleTarget, FINDFLAGS_NONE, TARGET_NONE);
        }

        if (PAI->TargetFind->m_targets.size() == 0)
        {
            // There used to be an assumed interrupt handler here.
            // Add a test and capture before reintroducing.
            return;
        }

        for (auto&& PTarget : PAI->TargetFind->m_targets)
        {
            bool             primary      = PTarget == PBattleTarget;
            action_target_t& actionTarget = action.addTarget(PTarget->id);
            action_result_t& actionResult = actionTarget.addResult();

            uint16         tpHitsLanded    = 0;
            uint16         extraHitsLanded = 0;
            int32          damage          = 0;
            CBattleEntity* taChar          = battleutils::getAvailableTrickAttackChar(this, PTarget);

            actionResult.resolution                         = ActionResolution::Hit;
            actionResult.animation                          = PWeaponSkill->getAnimationId();
            std::tie(damage, tpHitsLanded, extraHitsLanded) = luautils::OnUseWeaponSkill(this, PTarget, PWeaponSkill, tp, primary, action, taChar);

            if (!battleutils::isValidSelfTargetWeaponskill(PWeaponSkill->getID()))
            {
                if (charweaponskillprimaryhelpers::ShouldNotifyHit(
                        false,
                        primary,
                        PBattleTarget->objtype == TYPE_MOB))
                {
                    luautils::OnWeaponskillHit(PBattleTarget, this, PWeaponSkill->getID());
                }
            }
            else
            {
                const auto selfResult   = charweaponskillselfhelpers::BuildResult(primary, damage);
                actionResult.messageID  = static_cast<MsgBasic>(selfResult.messageID);
                actionResult.resolution = ActionResolution::Hit;
                actionResult.param      = PTarget->addMP(selfResult.healAmount);
            }

            if (primary)
            {
                // See battleentity.h for REACTION class
                // On retail, weaponskills will contain 0x08, 0x10 (HIT, ABILITY) on hit and may include the following:
                // 0x01, 0x02, 0x04 (MISS, GUARDED, BLOCK)
                // TODO: refactor this so lua returns the number of hits so we don't have to check the reaction bits.
                const bool isNegated = charweaponskillprimaryhelpers::IsNegatedResolution(
                    actionResult.resolution == ActionResolution::Miss,
                    actionResult.resolution == ActionResolution::Parry);
                if (!isNegated)
                {
                    auto wspoints = settings::get<uint8>("map.WS_POINTS_BASE");

                    if (PBattleTarget->health.hp > 0 && PWeaponSkill->getPrimarySkillchain() != 0)
                    {
                        // NOTE: GetSkillChainEffect is INSIDE this if statement because it
                        //  ALTERS the state of the resonance, which misses and non-elemental skills should NOT do.
                        const auto effect = battleutils::GetSkillChainEffect(
                            PBattleTarget,
                            PWeaponSkill->getPrimarySkillchain(),
                            PWeaponSkill->getSecondarySkillchain(),
                            PWeaponSkill->getTertiarySkillchain());
                        if (effect != ActionProcSkillChain::None)
                        {
                            actionResult.recordSkillchain(effect, battleutils::TakeSkillchainDamage(this, PBattleTarget, damage, taChar));

                            // Despite appearances, ws_points_skillchain is not a multiplier it is just an amount "per skillchain level"
                            const auto wsPointsSkillchain = settings::get<uint8>("map.WS_POINTS_SKILLCHAIN");
                            wspoints = charweaponskillprimaryhelpers::TotalWSPoints(
                                wspoints,
                                true,
                                static_cast<uint8>(effect),
                                wsPointsSkillchain);
                        }
                    }
                    // check for ws points
                    // TODO: As a general rule, mobs not granting EXP do not give WSP
                    // The following exceptions apply:
                    // - PC targeted weaponskills always give WSP
                    // - A handful of content: Besieged, DI
                    if (charweaponskillprimaryhelpers::ShouldAwardWSPoints(
                            true,
                            false,
                            charutils::CheckMob(this->GetMLevel(), PTarget) > EMobDifficulty::TooWeak))
                    {
                        charutils::AddWeaponSkillPoints(this, damslot, wspoints);
                    }
                }
            }
        }
        battleutils::ClaimMob(PBattleTarget, this);
    }
    else
    {
        ActionInterrupts::WeaponSkillOutOfRange(this, PBattleTarget);
    }

    PLatentEffectContainer->CheckLatentsWS(false);
    this->processActionEffectFlags(action);
}

void CCharEntity::OnAbility(CAbilityState& state, action_t& action)
{
    TracyZoneScoped;

    auto* PAbility = state.GetAbility();
    auto* PTarget  = static_cast<CBattleEntity*>(state.GetTarget());
    const auto preflight = charabilitypreflighthelpers::Evaluate(
        this->PRecastContainer->HasRecast(RECAST_ABILITY, PAbility->getRecastId(), PAbility->getRecastTime()),
        this->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Amnesia),
        PAbility->getValidTarget(),
        [&]() { pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, this, 0, 0, MsgBasic::WaitLonger); },
        [&]() { pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, this, 0, 0, MsgBasic::UnableToUseJobAbility2); },
        [&]() { PAI->TargetFind->reset(); },
        [&](const uint8 findFlags, const uint16 validTarget) {
            PAI->TargetFind->findSingleTarget(PTarget, findFlags, validTarget);
            return static_cast<int>(PAI->TargetFind->m_targets.size());
        });
    if (preflight != charabilitypreflighthelpers::Result::Proceed)
    {
        return;
    }

    // Retained for later AoE target-find (same TARGET_PLAYER_DEAD → FINDFLAGS_DEAD policy).
    uint8 findFlags = 0;
    if ((PAbility->getValidTarget() & TARGET_PLAYER_DEAD) == TARGET_PLAYER_DEAD)
    {
        findFlags |= FINDFLAGS_DEAD;
    }

    std::unique_ptr<CBasicPacket> errMsg;
    if (IsValidTarget(PTarget->targid, PAbility->getValidTarget(), errMsg))
    {
        // Recast planning pure half (merit/charge/tabula/BP). Charge pointer retained for ApplyAbilityRecast.
        // TODO: gear Recast- mod is a separate static reduction path not covered here.
        auto* charge = ability::GetCharge(this, static_cast<uint16>(PAbility->getRecastId()));

        std::int32_t meritRecastReductionSec = 0;
        if (PAbility->getMeritModID() > 0 && !(PAbility->getAddType() & ADDTYPE_MERIT))
        {
            meritRecastReductionSec = PMeritPoints->GetMeritValue(static_cast<MERIT_TYPE>(PAbility->getMeritModID()), this);
        }

        std::int16_t avatarsFavorPower = 0;
        if (CStatusEffect* avatarsFavor = this->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::AvatarsFavor))
        {
            avatarsFavorPower = avatarsFavor->GetPower();
        }

        const auto recastPlan = charabilityrecasthelpers::BuildInitial({
            .abilityID               = PAbility->getID(),
            .recastID                = static_cast<uint16>(PAbility->getRecastId()),
            .addType                 = PAbility->getAddType(),
            .meritModID              = PAbility->getMeritModID(),
            .meritRecastReductionSec = meritRecastReductionSec,
            .hasCharge               = charge != nullptr,
            .chargeTimeSec           = charge != nullptr ? timer::count_seconds(charge->chargeTime) : 0,
            .abilityRecastSec        = timer::count_seconds(PAbility->getRecastTime()),
            .sicMeritReductionSec    = PMeritPoints->GetMeritValue(MERIT_SIC_RECAST, this),
            .strategemRecastModSec   = this->getMod(Mod::STRATAGEM_RECAST),
            .hasTabulaRasa           = this->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::TabulaRasa),
            .bpDelayMod              = this->getMod(Mod::BP_DELAY),
            .bpDelayIIMod            = this->getMod(Mod::BP_DELAY_II),
            .avatarsFavorPower       = avatarsFavorPower,
        });

        auto baseChargeTime = std::chrono::seconds(recastPlan.baseChargeTimeSec);
        action.recast       = std::chrono::seconds(recastPlan.recastSec);
        if (recastPlan.setBPRecastTime)
        {
            // Localvar will set the BP ability timer when the move consumes MP
            // The delay is snapshot when the player uses the ability: https://www.bg-wiki.com/ffxi/Blood_Pact_Ability_Delay
            this->SetLocalVar("bpRecastTime", recastPlan.bpRecastTime);
        }

        // Check paralysis and consume recast for non-SP abilities
        if (charabilityparalyzehelpers::Apply(
                battleutils::IsParalyzed(this),
                static_cast<uint16>(PAbility->getRecastId()),
                [&]() { charutils::ApplyAbilityRecast(this, PAbility, charge, baseChargeTime, action.recast); },
                [&]() { ActionInterrupts::AbilityParalyzed(this, PTarget); }))
        {
            return;
        }

        // remove invisible if aggressive
        {
            const auto stealth = charabilitystealthhelpers::PlanCleanup(PAbility->getID(), PAbility->getValidTarget());
            if (stealth.removeInvisible)
            {
                charutils::RemoveInvisible(this);
            }
            if (stealth.removeDetectable)
            {
                StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Detectable);
            }
            if (stealth.removeIllusion)
            {
                StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Illusion);
            }
        }

        {
            CItem* PItem = getEquip(SLOT_HEAD);
            const bool rewardHead = PItem != nullptr && charabilityrecasthelpers::IsRewardRecastHead(PItem->getID());
            action.recast = std::chrono::seconds(charabilityrecasthelpers::AdjustPostParalysis(
                timer::count_seconds(action.recast),
                PAbility->getID(),
                rewardHead,
                this->getMod(Mod::SIC_READY_RECAST)));
        }

        action.actorId    = this->id;
        action.actiontype = PAbility->getActionType();
        action.actionid   = PAbility->getID();

        // Calculate ability AoE type and radius (pure ability_aoe_capacity).
        const auto aoeResult = abilityaoehelpers::TypeAndRadius(
            PAbility->getID(),
            static_cast<std::uint16_t>(PAbility->getRecastId()),
            PAbility->getAOE(),
            static_cast<int>(PAbility->getRadius()),
            this->getMod(Mod::LIEMENT_EXTENDS_TO_AREA),
            StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Contradance),
            this->getMod(Mod::ROLL_RANGE));
        const auto  aoeType   = static_cast<AOE_TYPE>(aoeResult.type);
        const float aoeRadius = static_cast<float>(aoeResult.radius);

        // TODO: get rid of this to script, too
        if (PAbility->isPetAbility())
        {
            CPetEntity* PPetEntity = dynamic_cast<CPetEntity*>(PPet);
            CPetSkill*  PPetSkill  = battleutils::GetPetSkill(PAbility->getID());

            // is a real pet (charmed pets won't return a valid PPetEntity)
            // and has pet ability in the pet_skills sql table
            if (PPetEntity && PPetSkill) // don't display msg and notify pet
            {
                const bool isJugPet = PPetEntity->getPetType() == PET_TYPE::JUG_PET;
                // For jug pet abilities, the JobAbility FINISH packet targets the player, not the pet
                action_target_t& actionTarget = action.addTarget(charabilitypethelpers::ActionPacketTargetID(isJugPet, this->id, PTarget->id));
                action_result_t& actionResult = actionTarget.addResult();
                actionResult.animation        = ActionAnimation::PetSkillStart;
                actionResult.resolution       = ActionResolution::Hit;

                // set primary target for jug ready abilities (JA targets the player, but the pet acts like a mob and makes its own decision on the skill target)
                const auto PPetTarget = charabilitypethelpers::PetSkillTargetID(
                    isJugPet,
                    (PPetSkill->getValidTargets() & TARGET_ENEMY) != 0,
                    PTarget->targid,
                    PPetEntity->GetBattleTargetID(),
                    PPetEntity->targid);

                // OnAbilityCheck succeeded and petskill is found, tell pet to perform it
                // TODO: This ends up sending the pet action packet before PC...
                PPetEntity->PAI->PetSkill(PPetTarget, PPetSkill->getID());
            }
        }
        // TODO: make this generic enough to not require an if
        else if (aoeType != AOE_TYPE::NONE)
        {
            PAI->TargetFind->reset();
            PAI->TargetFind->findWithinArea(this, AOE_RADIUS::ATTACKER, aoeRadius, findFlags, PAbility->getValidTarget());

            auto prevMsg = MsgBasic::None;
            for (auto&& PTargetFound : PAI->TargetFind->m_targets)
            {
                action_target_t& actionTarget = action.addTarget(PTargetFound->id);
                action_result_t& actionResult = actionTarget.addResult();
                actionResult.resolution       = ActionResolution::Hit;
                actionResult.animation        = PAbility->getAnimationID();
                actionResult.messageID        = PAbility->getMessage();

                int32 value = luautils::OnUseAbility(this, PTargetFound, PAbility, &action);

                const auto resolved = charabilityresulthelpers::ResolveAoETarget(
                    prevMsg == MsgBasic::None,
                    static_cast<uint16>(PAbility->getMessage()),
                    value,
                    [](const uint16 message) { return static_cast<uint16>(messageutils::GetAoEVariant(static_cast<MsgBasic>(message))); },
                    [](const uint16 message) { return static_cast<uint16>(messageutils::GetAbsorbVariant(static_cast<MsgBasic>(message))); });
                actionResult.messageID = static_cast<MsgBasic>(resolved.messageID);
                actionResult.param     = resolved.param;

                prevMsg = actionResult.messageID;

                state.ApplyEnmity();
            }
        }
        else
        {
            action_target_t& actionTarget = action.addTarget(PTarget->id);
            action_result_t& actionResult = actionTarget.addResult();
            actionResult.resolution       = ActionResolution::Hit;
            actionResult.animation        = PAbility->getAnimationID();
            MsgBasic prevMsg              = actionResult.messageID;

            // Check for special situations from Steal (The Tenshodo Showdown quest)
            if (PAbility->getID() == ABILITY_STEAL)
            {
                // Force a specific result to be stolen based on the mob LUA
                actionResult.param = luautils::OnSteal(this, PTarget, PAbility, &action);
            }

            int32 value = luautils::OnUseAbility(this, PTarget, PAbility, &action);

            // TODO: Some abilities legitimately have no message (e.g., Full Circle)
            const auto resolved = charabilityresulthelpers::ResolveSingle(
                static_cast<uint16>(prevMsg),
                static_cast<uint16>(actionResult.messageID),
                static_cast<uint16>(PAbility->getMessage()),
                value,
                [](const uint16 message) { return static_cast<uint16>(messageutils::GetAbsorbVariant(static_cast<MsgBasic>(message))); });
            actionResult.messageID = static_cast<MsgBasic>(resolved.messageID);
            actionResult.param     = resolved.param;

            state.ApplyEnmity();
        }

        // Some mobs respond to abilities (ex. Absolute Virtue / Ob)
        for (CBattleEntity* PBattleEntity : *PNotorietyContainer)
        {
            auto* PMob = dynamic_cast<CMobEntity*>(PBattleEntity);
            if (charabilityresponsehelpers::ShouldNotify(
                    PMob != nullptr,
                    PMob != nullptr && PMob->getMobMod(MOBMOD_ABILITY_RESPONSE) != 0,
                    PMob != nullptr && PMob->getZone() == this->getZone()))
            {
                luautils::OnPlayerAbilityUse(PMob, this, PAbility);
            }
        }

        // Cleanup "consumed" abilities after action like Contradance
        StatusEffectContainer->DelStatusEffect(PAbility->getPostActionEffectCleanup());

        charutils::ApplyAbilityRecast(this, PAbility, charge, baseChargeTime, action.recast);
        this->processActionEffectFlags(action);
    }
    else if (errMsg)
    {
        pushPacket(std::move(errMsg));
    }
}

bool CCharEntity::IsMobOwner(CBattleEntity* PBattleTarget)
{
    TracyZoneScoped;

    if (PBattleTarget == nullptr)
    {
        ShowWarning("CCharEntity::IsMobOwner() - PBattleTarget was null.");
        return false;
    }

    bool nonExclusiveClaim = false;
    if (auto* PMob = dynamic_cast<CMobEntity*>(PBattleTarget))
    {
        nonExclusiveClaim = PMob->getMobMod(MOBMOD_CLAIM_TYPE) == static_cast<int16>(ClaimType::NonExclusive);
    }

    return charismobownerhelpers::Evaluate(
        PBattleTarget->m_OwnerID.id,
        this->id,
        PBattleTarget->objtype == TYPE_PC,
        nonExclusiveClaim,
        [&]()
        {
            bool found = false;
            // clang-format off
            ForAlliance([&PBattleTarget, &found](CBattleEntity* PEntity)
            {
                if (PEntity->id == PBattleTarget->m_OwnerID.id)
                {
                    found = true;
                }
            });
            // clang-format on
            return found;
        });
}

void CCharEntity::HandleErrorMessage(std::unique_ptr<CBasicPacket>& msg)
{
    TracyZoneScoped;

    charerrordeliveryhelpers::DeliverError(
        msg,
        [&]() { return isCharmed; },
        [&](std::unique_ptr<CBasicPacket> message) { pushPacket(std::move(message)); });
}

void CCharEntity::OnDeathTimer()
{
    TracyZoneScoped;

    chardeathhomepointhelpers::Expire(
        [&]() { charutils::SetCharVar(this, "expLost", 0); },
        [&]() { requestedWarp = true; }); // zone entities will warp us on the next tick
}

void CCharEntity::OnRaise()
{
    TracyZoneScoped;

    // TODO: Moghancement Experience needs to be factored in here somewhere.
    if (m_hasRaise > 0)
    {
        const bool mijinGakure = GetLocalVar("MijinGakure") != 0;
        const auto plan        = charraiseplanhelpers::Build({
                   .hasRaise      = m_hasRaise,
                   .weaknessLevel = m_weaknessLvl,
                   .hasArise      = m_hasArise,
                   .mijinGakure   = mijinGakure,
                   .mijinReraise  = mijinGakure && getMod(Mod::MIJIN_RERAISE) != 0,
                   .maxHP         = GetMaxHP(),
                   .mainLevel     = GetMLevel(),
                   .expRetain     = settings::get<uint8>("map.EXP_RETAIN"),
        });
        action_t action{
            .actorId    = id,
            .actiontype = ActionCategory::MagicFinish,
        };

        auto& actionTarget = action.addTarget(id);
        auto& actionResult = actionTarget.addResult();
        charraiseapplyhelpers::Apply(
            plan,
            [&](const uint8 level) { m_weaknessLvl = level; },
            [&](const uint8 level, const std::chrono::minutes duration)
            {
                StatusEffectContainer->AddStatusEffect(
                    xi::StatusEffect::Weakness,
                    static_cast<uint16>(xi::StatusEffect::Weakness),
                    level,
                    0s,
                    duration);
            },
            [&](const ActionAnimation animation) { actionResult.animation = animation; },
            [&](const uint16 hp) { addHP(hp); },
            [&]() { updatemask |= UPDATE_HP; },
            [&]() { loc.zone->PushPacket(this, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action)); },
            [&]() -> uint16 { return charutils::GetCharVar(this, "expLost"); },
            [&](const uint16 xp) { charutils::AddExperiencePoints(true, false, false, this, this, xp); },
            [&]() { charutils::SetCharVar(this, "expLost", 0); },
            [&]() { StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Reraise, static_cast<uint16>(xi::StatusEffect::Reraise), 3, 0s, 1h); },
            [&]() { SetLocalVar("MijinGakure", 0); },
            [&]() { m_hasArise = false; },
            [&]() { m_hasRaise = 0; });
    }
}

auto CCharEntity::OnItemFinish(CItemState& state, action_t& action) -> bool
{
    TracyZoneScoped;

    auto* PTarget = static_cast<CBattleEntity*>(state.GetTarget());
    auto* PItem   = state.GetItem();

    uint8 findFlags{};
    if (!charitemfinishpreflighthelpers::Apply(
            PItem->isType(ITEM_EQUIPMENT),
            PItem->getQuantity(),
            PItem->getReserve(),
            PItem->getValidTarget(),
            this->id,
            PItem->getID(),
            [&]() { ShowWarning("OnItemFinish: %s attempted to use reserved/insufficient %s (%u).", this->getName(), PItem->getName(), PItem->getID()); },
            [&]() { this->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, this, PItem->getID(), 0, MsgBasic::ItemFailsToActivate); },
            [&]() { PAI->TargetFind->reset(); },
            [&](const uint8 flags, const uint16 validTarget)
            {
                findFlags = flags;
                PAI->TargetFind->findSingleTarget(PTarget, flags, validTarget);
                return PAI->TargetFind->m_targets.size();
            },
            [&](const uint32 actorID, const uint16 itemID)
            {
                action.actorId    = actorID;
                action.actiontype = ActionCategory::ItemFinish;
                action.actionid   = itemID;
            }))
    {
        return false;
    }

    auto processAction = [&](CBaseEntity* PTargetFound) -> void
    {
        action_target_t& actionTarget = action.addTarget(PTargetFound->id);
        action_result_t& actionResult = actionTarget.addResult();
        actionResult.resolution       = ActionResolution::Hit;
        actionResult.animation        = PItem->getAnimationID();

        // TODO: guard charutils::UpdateItem against InTransaction items so a
        // Lua delItem inside OnItemUse can't decrement out-of-tx.
        int32 value = luautils::OnItemUse(this, PTargetFound, PItem, action);

        actionResult.param = value;
        // TODO: how to detect if item does damage?
        /*if (value < 0)
        {
            actionResult.messageID = messageutils::GetAbsorbVariant(actionResult.messageID);
            actionResult.param     = -actionResult.param;
        }*/
    };

    charitemfinishtargetshelpers::Apply(
        PItem->getAoE(),
        PTarget,
        findFlags,
        PItem->getValidTarget(),
        [&]() { PAI->TargetFind->reset(); },
        [&](const float distance, const uint8 flags, const uint16 validTarget) -> const auto&
        {
            PAI->TargetFind->findWithinArea(this, AOE_RADIUS::ATTACKER, distance, flags, validTarget);
            return PAI->TargetFind->m_targets;
        },
        processAction);

    return charitemfinishcompletehelpers::Apply(
        PItem->isType(ITEM_EQUIPMENT),
        PItem->getMaxCharges(),
        PItem->getCurrentCharges(),
        PItem->getSlotID(),
        PItem->getLocationID(),
        [&](const uint8 charges) { PItem->setCurrentCharges(charges); },
        [&]() { PItem->setLastUseTime(timer::now()); },
        [&]()
        {
            db::preparedStmt("UPDATE char_inventory "
                             "SET extra = ? "
                             "WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                             PItem->m_extra,
                             this->id,
                             PItem->getLocationID(),
                             PItem->getSlotID());
        },
        [&](const uint16 key) { this->PRecastContainer->Add(RECAST_ITEM, static_cast<Recast>(key), PItem->getReuseTime()); });
}

CBattleEntity* CCharEntity::IsValidTarget(uint16 targid, uint16 validTargetFlags, std::unique_ptr<CBasicPacket>& errMsg)
{
    TracyZoneScoped;

    auto* PTarget = CBattleEntity::IsValidTarget(targid, validTargetFlags, errMsg);
    const auto decision = chartargetresolverhelpers::Apply(
        PTarget != nullptr,
        PTarget != nullptr && PTarget->objtype == TYPE_PC,
        PTarget != nullptr && PTarget->isAlive(),
        validTargetFlags,
        [&]() { return charutils::IsAidBlocked(this, static_cast<CCharEntity*>(PTarget)); },
        [&]() { return IsMobOwner(PTarget); },
        [&]()
        {
            auto* PEntity = GetEntity(targid, TYPE_MOB | TYPE_PC | TYPE_PET | TYPE_TRUST);
            return PEntity && PEntity->objtype == TYPE_MOB && static_cast<CMobEntity*>(PEntity)->allegiance == ALLEGIANCE_TYPE::PLAYER &&
                   (static_cast<CMobEntity*>(PEntity)->m_Behavior & BEHAVIOR_NO_ASSIST);
        });
    switch (decision)
    {
        case chartargetresolverhelpers::Decision::Accept:
            return PTarget;
        case chartargetresolverhelpers::Decision::Blocked:
            errMsg = std::make_unique<GP_SERV_COMMAND_SYSTEMMES>(0, 0, MsgStd::TargetIsCurrentlyBlocking);
            static_cast<CCharEntity*>(PTarget)->pushPacket<GP_SERV_COMMAND_SYSTEMMES>(0, 0, MsgStd::BlockedByBlockaid);
            break;
        case chartargetresolverhelpers::Decision::CannotOnThatTarget:
            errMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, this, 0, 0, MsgBasic::CannotOnThatTarget);
            break;
        case chartargetresolverhelpers::Decision::AlreadyClaimed:
            errMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, PTarget, 0, 0, MsgBasic::AlreadyClaimed);
            break;
        case chartargetresolverhelpers::Decision::CannotAttack:
            errMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, this, 0, 0, MsgBasic::CannotAttackTarget);
            break;
    }
    return nullptr;
}

void CCharEntity::Die()
{
    TracyZoneScoped;

    auto* PLastAttacker = GetEntity(lastAttackerId_.targid);
    const auto plan = chardeathplanhelpers::Build({
        .lastAttackerMatches   = PLastAttacker && PLastAttacker->id == lastAttackerId_.id,
        .hasPet               = this->PPet != nullptr,
        .mijinGakure          = GetLocalVar("MijinGakure") != 0,
        .hasBattlefield       = PBattlefield != nullptr,
        .battlefieldLosesEXP  = PBattlefield != nullptr && (PBattlefield->GetRuleMask() & RULES_LOSE_EXP) == RULES_LOSE_EXP,
        .mainLevel            = GetMLevel(),
        .expLossLevel         = settings::get<uint8>("map.EXP_LOSS_LEVEL"),
        .expRetain            = settings::get<uint8>("map.EXP_RETAIN"),
        .experienceRetainedMod = getMod(Mod::EXPERIENCE_RETAINED),
    });
    chardeathapplyhelpers::Apply(
        plan,
        [&](const chardeathplanhelpers::Message message)
        {
            if (message == chardeathplanhelpers::Message::DefeatedBy)
            {
                loc.zone->PushPacket(this, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(PLastAttacker, this, 0, 0, MsgBasic::PlayerDefeatedBy));
            }
            else
            {
                loc.zone->PushPacket(this, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(this, this, 0, 0, MsgBasic::FallsToGround));
            }
        },
        [&]() { battleutils::RelinquishClaim(this); },
        [&]() { petutils::DespawnPet(this); },
        [&]() { Die(death_duration); },
        [&]() { SetDeathTime(timer::now()); },
        [&]() { setBlockingAid(false); },
        [&]() { conquest::LoseInfluencePoints(this); },
        [&](const float retainPercent) { charutils::DelExperiencePoints(this, retainPercent, 0); },
        [&]() { luautils::OnPlayerDeath(this); });
}

void CCharEntity::Die(timer::duration _duration)
{
    TracyZoneScoped;

    const auto plan = chartimeddeathhelpers::Build({
        .hasWeakness  = StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Weakness),
        .weaknessLevel = m_weaknessLvl,
        .hasRaise      = m_hasRaise,
        .reraiseI      = getMod(Mod::RERAISE_I) > 0,
        .reraiseII     = getMod(Mod::RERAISE_II) > 0,
        .reraiseIII    = getMod(Mod::RERAISE_III) > 0,
        .mijinReraise  = getMod(Mod::MIJIN_RERAISE) > 0,
    });
    chartimeddeathhelpers::Apply(
        plan,
        _duration,
        [&]() { ClearTrusts(); },
        [&]() { StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Weakness); },
        [&](const uint8 level) { m_weaknessLvl = level; },
        [&]() { m_deathSyncTime = timer::now() + death_update_frequency; },
        [&]() { PAI->ClearStateStack(); },
        [&](const timer::duration duration) { PAI->Internal_Die(duration); },
        [&]() { allegiance = ALLEGIANCE_TYPE::PLAYER; },
        [&](const uint8 level) { m_hasRaise = level; },
        [&]() { m_charHistory.timesKnockedOut++; },
        [&]() { CBattleEntity::Die(); });
}

void CCharEntity::Raise()
{
    TracyZoneScoped;

    charraisecompletehelpers::Complete(
        [&]() { OnRaise(); },
        [&]() { PAI->Accept_Raise(); },
        [&]() { SetDeathTime(timer::time_point::min()); });
}

void CCharEntity::SetDeathTime(timer::time_point timestamp)
{
    chardeathhomepointhelpers::SetDeathTime(m_DeathTimestamp, timestamp);
}

timer::duration CCharEntity::GetTimeSinceDeath() const
{
    return chardeathhomepointhelpers::TimeSinceDeath(
        m_DeathTimestamp,
        timer::time_point::min(),
        []() { return timer::now(); },
        timer::duration{ 0 });
}

timer::duration CCharEntity::GetTimeUntilDeathHomepoint() const
{
    return chardeathhomepointhelpers::TimeUntilDeathHomepoint(
        death_duration,
        [&]() { return GetTimeSinceDeath(); });
}

earth_time::time_point CCharEntity::GetTimeCreated()
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT UNIX_TIMESTAMP(timecreated) FROM chars WHERE charid = ? LIMIT 1", id);

    if (rset && rset->rowsCount() && rset->next())
    {
        return earth_time::time_point(std::chrono::seconds(rset->get<uint32>("UNIX_TIMESTAMP(timecreated)")));
    }

    return earth_time::time_point::min();
}

uint8 CCharEntity::getHighestJobLevel()
{
    return charhighestjobhelpers::HighestLevel(jobs.job);
}

bool CCharEntity::hasMoghancement(uint16 moghancementID) const
{
    return charmoghancementstatehelpers::Has(m_moghancementID, moghancementID);
}

void CCharEntity::UpdateMoghancement()
{
    TracyZoneScoped;

    std::vector<charmoghancementfurniturehelpers::Furniture> furniture;
    for (auto containerID : { LOC_MOGSAFE, LOC_MOGSAFE2 })
    {
        CItemContainer* PContainer = getStorage(containerID);
        for (int slotID = 1; slotID <= PContainer->GetSize(); ++slotID)
        {
            CItem* PItem = PContainer->GetItem(slotID);
            if (PItem != nullptr && PItem->isType(ITEM_FURNISHING))
            {
                CItemFurnishing* PFurniture = static_cast<CItemFurnishing*>(PItem);
                furniture.emplace_back(charmoghancementfurniturehelpers::Furniture{
                    .installed     = PFurniture->isInstalled(),
                    .secondFloor   = PFurniture->getOn2ndFloor(),
                    .element       = PFurniture->getElement(),
                    .aura          = PFurniture->getAura(),
                    .order         = PFurniture->getOrder(),
                    .moghancement  = PFurniture->getMoghancement(),
                });
            }
        }
    }

    const auto newMoghancementID = charmoghancementfurniturehelpers::Select(furniture);

    charmoghancementupdatehelpers::Update(
        newMoghancementID,
        m_moghancementID,
        [&](const uint16 id) { pushPacket<GP_SERV_COMMAND_TALKNUMWORK>(this, luautils::GetTextIDVariable(getZone(), "KEYITEM_OBTAINED"), id, 0, 0, 0, false); },
        [&](const uint16 id) { charutils::delKeyItem(this, static_cast<KeyItem>(id)); },
        [&](const uint16 id) { charutils::addKeyItem(this, static_cast<KeyItem>(id)); },
        [&](const uint8 table) { pushPacket<GP_SERV_COMMAND_SCENARIOITEM>(this, table); },
        [&]() { charutils::SaveKeyItems(this); },
        [&](const uint16 id) { SetMoghancement(id); },
        [&]() { charutils::SaveCharMoghancement(this); });
}

void CCharEntity::SetMoghancement(uint16 moghancementID)
{
    charmoghancementstatehelpers::Set(
        m_moghancementID,
        moghancementID,
        [&](const uint16 id, const bool adding) { changeMoghancement(id, adding); });
}

void CCharEntity::changeMoghancement(uint16 moghancementID, bool isAdding)
{
    TracyZoneScoped;

    if (moghancementID == 0)
    {
        return;
    }

    if (charmoghancementcrafthelpers::Apply(
            moghancementID,
            isAdding,
            [&](const Mod mod, const int16 amount) { addModifier(mod, amount); }))
    {
        return;
    }

    if (charmoghancementgeneralhelpers::Apply(
            moghancementID,
            profile.nation,
            isAdding,
            [&](const Mod mod, const int16 amount) { addModifier(mod, amount); }))
    {
        return;
    }

    if (charmoghancementresistancehelpers::Apply(
            moghancementID,
            isAdding,
            [&](const Mod mod, const int16 amount) { addModifier(mod, amount); }))
    {
        return;
    }

    // Apply the Moghancement
    int16 multiplier = isAdding ? 1 : -1;
    switch (moghancementID)
    {
        case MOGHANCEMENT_FIRE:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_FIRE, 5 * multiplier);
            break;
        case MOGHANCEMENT_ICE:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_ICE, 5 * multiplier);
            break;
        case MOGHANCEMENT_WIND:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_WIND, 5 * multiplier);
            break;
        case MOGHANCEMENT_EARTH:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_EARTH, 5 * multiplier);
            break;
        case MOGHANCEMENT_LIGHTNING:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_THUNDER, 5 * multiplier);
            break;
        case MOGHANCEMENT_WATER:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_WATER, 5 * multiplier);
            break;
        case MOGHANCEMENT_LIGHT:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_LIGHT, 5 * multiplier);
            break;
        case MOGHANCEMENT_DARK:
            addModifier(Mod::SYNTH_MATERIAL_LOSS_DARK, 5 * multiplier);
            break;

        case MOGHANCEMENT_FISHING:
            addModifier(Mod::FISH, 1 * multiplier);
            break;
        case MOGHANCEMENT_WOODWORKING:
            addModifier(Mod::WOOD, 1 * multiplier);
            break;
        case MOGHANCEMENT_SMITHING:
            addModifier(Mod::SMITH, 1 * multiplier);
            break;
        case MOGHANCEMENT_GOLDSMITHING:
            addModifier(Mod::GOLDSMITH, 1 * multiplier);
            break;
        case MOGHANCEMENT_CLOTHCRAFT:
            addModifier(Mod::CLOTH, 1 * multiplier);
            break;
        case MOGHANCEMENT_LEATHERCRAFT:
            addModifier(Mod::LEATHER, 1 * multiplier);
            break;
        case MOGHANCEMENT_BONECRAFT:
            addModifier(Mod::BONE, 1 * multiplier);
            break;
        case MOGHANCEMENT_ALCHEMY:
            addModifier(Mod::ALCHEMY, 1 * multiplier);
            break;
        case MOGHANCEMENT_COOKING:
            addModifier(Mod::COOK, 1 * multiplier);
            break;

        case MOGLIFICATION_FISHING:
            addModifier(Mod::FISH, 1 * multiplier);
            // TODO: "makes it slightly easier to reel in your catch"
            break;
        case MOGLIFICATION_WOODWORKING:
            addModifier(Mod::WOOD, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_WOODWORKING, 5 * multiplier);
            break;
        case MOGLIFICATION_SMITHING:
            addModifier(Mod::SMITH, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_SMITHING, 5 * multiplier);
            break;
        case MOGLIFICATION_GOLDSMITHING:
            addModifier(Mod::GOLDSMITH, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_GOLDSMITHING, 5 * multiplier);
            break;
        case MOGLIFICATION_CLOTHCRAFT:
            addModifier(Mod::CLOTH, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_CLOTHCRAFT, 5 * multiplier);
            break;
        case MOGLIFICATION_LEATHERCRAFT:
            addModifier(Mod::LEATHER, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_LEATHERCRAFT, 5 * multiplier);
            break;
        case MOGLIFICATION_BONECRAFT:
            addModifier(Mod::BONE, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_BONECRAFT, 5 * multiplier);
            break;
        case MOGLIFICATION_ALCHEMY:
            addModifier(Mod::ALCHEMY, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_ALCHEMY, 5 * multiplier);
            break;
        case MOGLIFICATION_COOKING:
            addModifier(Mod::COOK, 1 * multiplier);
            addModifier(Mod::SYNTH_MATERIAL_LOSS_COOKING, 5 * multiplier);
            break;

        // Mega Moglifications do not state anything about lowering material loss.
        case MEGA_MOGLIFICATION_FISHING:
            addModifier(Mod::FISH, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_WOODWORKING:
            addModifier(Mod::WOOD, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_SMITHING:
            addModifier(Mod::SMITH, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_GOLDSMITHING:
            addModifier(Mod::GOLDSMITH, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_CLOTHCRAFT:
            addModifier(Mod::CLOTH, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_LEATHERCRAFT:
            addModifier(Mod::LEATHER, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_BONECRAFT:
            addModifier(Mod::BONE, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_ALCHEMY:
            addModifier(Mod::ALCHEMY, 5 * multiplier);
            break;
        case MEGA_MOGLIFICATION_COOKING:
            addModifier(Mod::COOK, 5 * multiplier);
            break;

        case MOGHANCEMENT_EXPERIENCE:
            addModifier(Mod::EXPERIENCE_RETAINED, 5 * multiplier);
            break;
        case MOGHANCEMENT_GARDENING:
            addModifier(Mod::GARDENING_WILT_BONUS, 36 * multiplier);
            break;
        case MOGHANCEMENT_DESYNTHESIS:
            addModifier(Mod::SYNTH_SUCCESS_RATE_DESYNTHESIS, 2 * multiplier);
            break;
        case MOGHANCEMENT_CONQUEST:
            addModifier(Mod::CONQUEST_BONUS, 6 * multiplier);
            break;
        case MOGHANCEMENT_REGION:
            addModifier(Mod::CONQUEST_REGION_BONUS, 10 * multiplier);
            break;
        case MOGHANCEMENT_FISHING_ITEM:
            // TODO: Increases the chances of finding items when fishing
            break;
        case MOGHANCEMENT_SANDORIA_CONQUEST:
            if (profile.nation == 0)
            {
                addModifier(Mod::CONQUEST_BONUS, 6 * multiplier);
            }
            break;
        case MOGHANCEMENT_BASTOK_CONQUEST:
            if (profile.nation == 1)
            {
                addModifier(Mod::CONQUEST_BONUS, 6 * multiplier);
            }
            break;
        case MOGHANCEMENT_WINDURST_CONQUEST:
            if (profile.nation == 2)
            {
                addModifier(Mod::CONQUEST_BONUS, 6 * multiplier);
            }
            break;
        case MOGHANCEMENT_MONEY:
            addModifier(Mod::MOGHANCEMENT_GIL_BONUS_P, 10 * multiplier);
            break;
        case MOGHANCEMENT_CAMPAIGN:
            addModifier(Mod::CAMPAIGN_BONUS, 5 * multiplier);
            break;
        case MOGHANCEMENT_MONEY_II:
            addModifier(Mod::MOGHANCEMENT_GIL_BONUS_P, 15 * multiplier);
            break;
        case MOGHANCEMENT_SKILL_GAINS:
            // NOTE: Exact value is unknown but considering this only granted by a newish item it makes sense SE made it fairly strong
            addModifier(Mod::COMBAT_SKILLUP_RATE, 25 * multiplier);
            addModifier(Mod::MAGIC_SKILLUP_RATE, 25 * multiplier);
            break;
        case MOGHANCEMENT_BOUNTY:
            addModifier(Mod::EXP_BONUS, 10 * multiplier);
            addModifier(Mod::CAPACITY_BONUS, 10 * multiplier);
            break;
        case MOGLIFICATION_EXPERIENCE_BOOST:
            addModifier(Mod::EXP_BONUS, 15 * multiplier);
            break;
        case MOGLIFICATION_CAPACITY_BOOST:
            addModifier(Mod::CAPACITY_BONUS, 15 * multiplier);
            break;

        // NOTE: Exact values for resistances is unknown
        case MOGLIFICATION_RESIST_DEATH:
            addModifier(Mod::DEATHRES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_SLEEP:
            addModifier(Mod::SLEEPRES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_POISON:
            addModifier(Mod::POISONRES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_PARALYSIS:
            addModifier(Mod::PARALYZERES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_SILENCE:
            addModifier(Mod::SILENCERES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_PETRIFICATION:
            addModifier(Mod::PETRIFYRES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_VIRUS:
            addModifier(Mod::VIRUSRES, 10 * multiplier);
            break;
        case MOGLIFICATION_RESIST_CURSE:
            addModifier(Mod::CURSERES, 10 * multiplier);
            break;
        default:
            break;
    }
}

bool CCharEntity::OnAttackError(CAttackState& state)
{
    TracyZoneScoped;

    auto* controller{ static_cast<CPlayerController*>(PAI->GetController()) };
    return charerrordeliveryhelpers::AttackError(
        [&]() { return controller->getLastErrMsgTime(); },
        [&]() { return std::chrono::milliseconds(this->GetWeaponDelay(false)); },
        [&]() { return PAI->getTick(); },
        [&](const timer::time_point errorTime) { controller->setLastErrMsgTime(errorTime); });
}

bool CCharEntity::isInTriggerArea(uint32 triggerAreaID)
{
    return chartriggerareahelpers::Contains(charTriggerAreaIDs, triggerAreaID);
}

void CCharEntity::onTriggerAreaEnter(uint32 triggerAreaID)
{
    chartriggerareahelpers::Enter(charTriggerAreaIDs, triggerAreaID);
}

void CCharEntity::onTriggerAreaLeave(uint32 triggerAreaID)
{
    chartriggerareahelpers::Leave(charTriggerAreaIDs, triggerAreaID);
}

void CCharEntity::clearTriggerAreas()
{
    chartriggerareahelpers::Clear(charTriggerAreaIDs);
}

auto CCharEntity::isInEvent() const -> bool
{
    return chareventlockhelpers::IsInEvent(currentEvent->eventId);
}

bool CCharEntity::isNpcLocked()
{
    return chareventlockhelpers::IsNpcLocked(isInEvent(), inSequence);
}

void CCharEntity::endCurrentEvent()
{
    chareventqueuehelpers::EndCurrent(
        [&]() { currentEvent->reset(); },
        [&]() { eventPreparation->reset(); },
        [&]() { setLocked(false); },
        [&]() { m_zoneInCutscene = false; },
        [&]() { m_Substate = CHAR_SUBSTATE::SUBSTATE_NONE; },
        [&]() { tryStartNextEvent(); });
}

void CCharEntity::queueEvent(EventInfo* eventToQueue)
{
    chareventqueuehelpers::QueueEvent(
        eventQueue,
        eventToQueue,
        [](const EventInfo* event) { return event->eventId; },
        [&]() { ShowError("CCharEntity::queueEvent: Character attempted to start multiple of the same event."); },
        [&](EventInfo* event) { eventQueue.emplace_back(event); },
        [&]() { tryStartNextEvent(); });
}

void CCharEntity::tryStartNextEvent()
{
    TracyZoneScoped;

    if (chareventidlehelpers::ShouldDeferStart(isInEvent()))
    {
        return;
    }

    if (eventQueue.empty())
    {
        chareventidlehelpers::RestoreIdle(
            [&]() { updatemask |= UPDATE_POS; }, // TODO: decouple from this. We want the 250ms post-tick processing
            [&]() -> std::optional<uint16>
            {
                if (auto PStatusEffect = StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Mounted))
                {
                    return PStatusEffect->GetPower();
                }
                return std::nullopt;
            },
            [&](const uint16 power)
            {
                // Chocobo NPC (outside, gives you a mount) edge case
                animation = power == MOUNT_CHOCOBO || power == MOUNT_NOBLE_CHOCOBO ? ANIMATION_CHOCOBO : ANIMATION_MOUNT;
            },
            [&]() { return this->isDead(); },
            [&](const bool dead) { animation = dead ? ANIMATION_DEATH : ANIMATION_NONE; },
            [&]() { sendServerStatus_ = true; });
        return;
    }

    chareventactivationhelpers::ActivateState(
        [&]()
        {
            EventInfo* oldEvent = currentEvent;
            currentEvent        = eventQueue.front();
            eventQueue.pop_front();
            destroy(oldEvent);
        },
        [&]() { eventPreparation->reset(); },
        [&]() { m_Substate = CHAR_SUBSTATE::SUBSTATE_IN_CS; },
        [&]() { return animation == ANIMATION_HEALING; },
        [&]() { StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Healing); },
        [&]() { return PPet != nullptr; },
        [&]() { PPet->PAI->Disengage(); },
        [&]() { return currentEvent->targetEntity && currentEvent->targetEntity->objtype == TYPE_NPC; },
        [&]() { currentEvent->targetEntity->SetLocalVar("pauseNPCPathing", 1); },
        [&]() { return currentEvent->type == CUTSCENE; },
        [&](const bool locked) { setLocked(locked); });

    chareventpackethelpers::SendAndFinalize(
        [&]() { return currentEvent->strings.empty(); },
        [&]() { return !currentEvent->params.empty(); },
        [&]() { return currentEvent->textTable != -1; },
        [&]() { pushPacket<GP_SERV_COMMAND_EVENT>(this, currentEvent); },
        [&]() { pushPacket<GP_SERV_COMMAND_EVENTNUM>(this, currentEvent); },
        [&]() { pushPacket<GP_SERV_COMMAND_EVENTSTR>(this, currentEvent); },
        [&]() { animation = ANIMATION_EVENT; },
        [&]() { updatemask |= UPDATE_POS; }, // TODO: decouple from this. We want the 250ms post-tick processing.
        [&]() { sendServerStatus_ = true; }); // sendServerStatus_ is somewhat like an update mask on its own
}

void CCharEntity::skipEvent()
{
    TracyZoneScoped;

    chareventskiphelpers::Skip(
        [&]() { return isInEvent(); },
        [&]() { return m_Locked; },
        [&]() { return currentEvent->canSkip; },
        [&]() { pushPacket<GP_SERV_COMMAND_SYSTEMMES>(0, 0, MsgStd::EventSkipped); },
        [&]() { pushPacket<GP_SERV_COMMAND_EVENTUCOFF>(this, GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelEvent); },
        [&]() { m_Substate = CHAR_SUBSTATE::SUBSTATE_NONE; },
        [&]() { return currentEvent->interruptText; },
        [&](const uint16 text) { pushPacket<GP_SERV_COMMAND_TALKNUM>(currentEvent->targetEntity, text, false); },
        [&]() { endCurrentEvent(); });
}

void CCharEntity::setLocked(bool locked)
{
    TracyZoneScoped;

    // Player and pet enmity are handled in mobcontroler.cpp, CheckLock() fucntion.
    // Mob casting interruption handled in magic_state.cpp, CMagicState::Update boolean.
    chareventlockhelpers::SetLocked(
        m_Locked,
        locked,
        [&]() { PAI->Disengage(); },
        [&]() { return PPet != nullptr; },
        [&]() { PPet->PAI->Disengage(); },
        [&]() { battleutils::RelinquishClaim(this); });
}

auto CCharEntity::getCharVar(const std::string& varName) const -> int32
{
    const auto now = earth_time::timestamp();
    if (auto charVar = charVarCache.find(varName); charVar != charVarCache.end())
    {
        const auto& cachedVarData = charVar->second;

        // If the cached variable is not expired, return it.  Else, fall through so that the
        // database can be cleaned up.
        if (charvarcachehelpers::ShouldUseCacheHit(true, cachedVarData.second, now))
        {
            return cachedVarData.first;
        }
    }

    const auto value = charutils::FetchCharVar(this->id, varName);

    charVarCache[varName] = charvarcachehelpers::MakeEntry(value.first, value.second);
    return value.first;
}

auto CCharEntity::getCharVarsWithPrefix(const std::string& prefix) -> std::vector<std::pair<std::string, int32>>
{
    const auto currentTimestamp = earth_time::timestamp();

    std::vector<std::pair<std::string, int32>> charVars;

    const auto rset = db::preparedStmt("SELECT varname, value, expiry FROM char_vars WHERE charid = ? AND varname LIKE ?",
                                       this->id,
                                       fmt::format("{}%", prefix));
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            const auto varname = rset->get<std::string>("varname");
            const auto value   = rset->get<int32>("value");
            const auto expiry  = rset->get<uint32>("expiry");

            if (charvarcachehelpers::ShouldIncludeRow(expiry, currentTimestamp))
            {
                charVarCache[varname] = charvarcachehelpers::MakeEntry(value, expiry);

                charVars.emplace_back(varname, value);
            }
        }
    }

    return charVars;
}

auto CCharEntity::getCharVarsWithSuffix(const std::string& suffix) -> std::vector<std::pair<std::string, int32>>
{
    const auto currentTimestamp = earth_time::timestamp();

    std::vector<std::pair<std::string, int32>> charVars;

    const auto rset = db::preparedStmt("SELECT varname, value, expiry FROM char_vars WHERE charid = ? AND varname LIKE ?",
                                       this->id,
                                       fmt::format("%{}", suffix));
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            const auto varname = rset->get<std::string>("varname");
            const auto value   = rset->get<int32>("value");
            const auto expiry  = rset->get<uint32>("expiry");

            if (charvarcachehelpers::ShouldIncludeRow(expiry, currentTimestamp))
            {
                charVarCache[varname] = charvarcachehelpers::MakeEntry(value, expiry);

                charVars.emplace_back(varname, value);
            }
        }
    }

    return charVars;
}

void CCharEntity::setCharVar(const std::string& charVarName, int32 value, uint32 expiry /* = 0 */)
{
    charVarCache[charVarName] = charvarcachehelpers::MakeEntry(value, expiry);
    charutils::PersistCharVar(this->id, charVarName, value, expiry);
}

void CCharEntity::setVolatileCharVar(const std::string& charVarName, int32 value, uint32 expiry /* = 0 */)
{
    charVarCache[charVarName] = charvarcachehelpers::MakeEntry(value, expiry);
    charVarChanges.insert(charVarName);
}

void CCharEntity::updateCharVarCache(const std::string& charVarName, int32 value, uint32 expiry /* = 0 */)
{
    charVarCache[charVarName] = charvarcachehelpers::MakeEntry(value, expiry);
}

void CCharEntity::removeFromCharVarCache(const std::string& varName)
{
    charVarCache.erase(varName);
}

void CCharEntity::clearCharVarsWithPrefix(const std::string& prefix)
{
    if (charvarcachehelpers::ShouldRejectClearPrefix(prefix.size()))
    {
        ShowError("Prefix too short to clear with: '%s'", prefix);
        return;
    }

    auto iter = charVarCache.begin();
    while (iter != charVarCache.end())
    {
        if (charvarcachehelpers::StartsWithPrefix(iter->first, prefix))
        {
            iter->second = charvarcachehelpers::ClearedEntry();
        }
        ++iter;
    }

    db::preparedStmt("DELETE FROM char_vars WHERE charid = ? AND varname LIKE ?", this->id, fmt::format("{}%", prefix));
}

bool CCharEntity::startSynth(SKILLTYPE synthSkill)
{
    return charstartsynthhelpers::Apply(PAI != nullptr, [&]() { return PAI->Internal_Synth(synthSkill); });
}
