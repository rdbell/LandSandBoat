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
#include "common/timer.h"

#include "alliance.h"
#include "entities/battle_entity.h"
#include "ipc_client.h"
#include "job_points.h"
#include "latent_effect_container.h"
#include "party.h"
#include "party_capacity.h"
#include "status_effect_container.h"
#include "treasure_pool.h"
#include "utils/blueutils.h"
#include "utils/charutils.h"
#include "utils/jailutils.h"
#include "utils/zoneutils.h"
#include <cstring>
#include <vector>

#include "packets/c2s/0x077_group_change2.h"
#include "packets/char_status.h"
#include "packets/char_sync.h"
#include "packets/s2c/0x009_message.h"
#include "packets/s2c/0x029_battle_message.h"
#include "packets/s2c/0x076_group_effects.h"
#include "packets/s2c/0x0ac_command_data.h"
#include "packets/s2c/0x0b4_config.h"
#include "packets/s2c/0x0c8_group_tbl.h"
#include "packets/s2c/0x0dd_group_list.h"

// should have brace-or-equal initializers when MSVC supports it
struct CParty::partyInfo_t
{
    uint32      id         = {};
    uint32      partyid    = {};
    uint32      allianceid = {};
    std::string name       = {};
    uint16      flags      = {};
    uint16      zone       = {};
    uint16      prev_zone  = {};
};

// Constructor
CParty::CParty(CBattleEntity* PEntity)
: m_PartyID(0)
, m_PartyType(PARTY_MOBS)
, m_PartyNumber(0)
{
    m_PLeader        = nullptr;
    m_PAlliance      = nullptr;
    m_PSyncTarget    = nullptr;
    m_PQuarterMaster = nullptr;
    m_EffectsChanged = false;

    if (partyhelpers::ShouldInitPartyFromEntity(PEntity == nullptr, PEntity != nullptr && PEntity->PParty != nullptr))
    {
        m_PartyID   = partyhelpers::PartyIDFromEntity(PEntity->id);
        m_PartyType = partyhelpers::ResolvePartyTypeIsPC(PEntity->objtype == TYPE_PC) ? PARTY_PCS : PARTY_MOBS;

        AddMember(PEntity);
        SetLeader(PEntity->name);
    }
    else
    {
        ShowWarning("%s", partyhelpers::FormatCPartyCtorNullWarning());
    }
}

CParty::CParty(uint32 id)
: m_PartyID(id)
, m_PartyType(PARTY_PCS)
, m_PartyNumber(0)
{
    m_PAlliance = nullptr;

    m_PLeader        = nullptr;
    m_PSyncTarget    = nullptr;
    m_PQuarterMaster = nullptr;

    m_EffectsChanged = false;
}

// Dirty, ugly hack to prevent bad refs keeping garbage pointers in memory pointing to things that _could_ still be valid, causing mayhem
CParty::~CParty()
{
    m_PLeader        = nullptr;
    m_PartyID        = 0;
    m_PAlliance      = nullptr;
    m_PSyncTarget    = nullptr;
    m_PQuarterMaster = nullptr;
}

void CParty::DisbandParty(bool playerInitiated)
{
    if (partyhelpers::ShouldDetachAllianceOnDisband(m_PAlliance != nullptr))
    {
        m_PAlliance->removeParty(this);
    }

    m_PSyncTarget = nullptr;
    m_PLeader     = nullptr;
    m_PAlliance   = nullptr;

    const auto memberPath = partyhelpers::ClassifyDisbandPartyMemberPath(
        m_PartyType == PARTY_PCS,
        m_PartyType == PARTY_MOBS);

    if (memberPath == partyhelpers::disband_party_member_path::PC_FULL)
    {
        SetQuarterMaster("");

        this->PushPacket(0, 0, std::make_unique<GP_SERV_COMMAND_GROUP_TBL>(nullptr));

        for (auto& member : members)
        {
            CCharEntity* PChar = (CCharEntity*)member;
            PChar->ClearTrusts();

            PChar->PParty = nullptr;
            PChar->PLatentEffectContainer->CheckLatentsPartyJobs();
            PChar->PLatentEffectContainer->CheckLatentsPartyMembers(members.size(), 0);
            PChar->PLatentEffectContainer->CheckLatentsPartyAvatar();
            PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PChar, 0, 0, PChar->getZone());

            // TODO: TreasurePool should stay with the last character, but now it is not critical

            if (partyhelpers::ShouldReplaceSoloTreasurePool(
                    PChar->PTreasurePool != nullptr,
                    PChar->PTreasurePool != nullptr && PChar->PTreasurePool->getPoolType() == TreasurePoolType::Zone))
            {
                PChar->PTreasurePool->delMember(PChar);
                PChar->PTreasurePool = new CTreasurePool(TreasurePoolType::Solo);
                PChar->PTreasurePool->addMember(PChar);
                PChar->PTreasurePool->updatePool(PChar);
            }
            CStatusEffect* sync = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
            if (partyhelpers::ShouldStartSyncDisableCountdown(sync != nullptr, sync != nullptr && sync->GetDuration() == 0s))
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, partyhelpers::LevelSyncDisableDurationSeconds, MsgStd::LevelSyncRemoveLeftParty);
                sync->SetStartTime(timer::now());
                sync->SetDuration(std::chrono::seconds(partyhelpers::LevelSyncDisableDurationSeconds));
            }

            db::preparedStmt("DELETE FROM accounts_parties WHERE charid = ?", PChar->id);
        }

        // make sure message server isn't notified of a disband if this came from the message server already
        if (partyhelpers::ShouldNotifyPartyDisbandIPC(playerInitiated))
        {
            message::send(ipc::PartyDisband{
                .partyId = m_PartyID,
            });
        }
    }
    else if (memberPath == partyhelpers::disband_party_member_path::MOB_CLEAR)
    {
        for (auto& member : members) // this should really only trigger when a dynamic entity dies and nothing else qualifies for it's party anymore (such as !fafnir in zones without dragons)
        {
            member->PParty = nullptr;
        }
    }

    // TODO: This entire system needs rewriting to both:
    //     : - Make it stable
    //     : - Get rid of `delete this` and manage memory nicely
    delete this; // cpp.sh allow
}

// Assign roles to group members (players only)
void CParty::AssignPartyRole(const std::string& MemberName, const GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND role)
{
    const bool isPCParty = m_PartyType == PARTY_PCS;

    // Early pure gate for mob parties (preserves no membership SQL on mob path).
    if (!isPCParty)
    {
        ShowWarningFmt("{}", partyhelpers::FormatAssignRoleMobPartyWarning(static_cast<uint8>(role), MemberName));
        return;
    }

    // Make sure that the character is actually a part of this party
    const auto rset = db::preparedStmt("SELECT chars.charid FROM chars JOIN accounts_parties ON accounts_parties.charid = chars.charid WHERE charname = ? AND partyid = ?", MemberName, m_PartyID);

    const auto action = partyhelpers::ClassifyAssignPartyRole(
        isPCParty,
        static_cast<bool>(rset),
        rset && rset->rowsCount() != 0,
        static_cast<uint8>(role));

    switch (action)
    {
        case partyhelpers::assign_party_role_action::REJECT_MOB_PARTY:
            // Unreachable when isPCParty is true; kept for switch exhaustiveness.
            ShowWarningFmt("{}", partyhelpers::FormatAssignRoleMobPartyWarning(static_cast<uint8>(role), MemberName));
            return;
        case partyhelpers::assign_party_role_action::REJECT_NOT_MEMBER:
        case partyhelpers::assign_party_role_action::REJECT_UNKNOWN_ROLE:
            return;
        case partyhelpers::assign_party_role_action::SET_LEADER:
            SetLeader(MemberName);
            break;
        case partyhelpers::assign_party_role_action::SET_QUARTERMASTER:
            SetQuarterMaster(MemberName);
            break;
        case partyhelpers::assign_party_role_action::CLEAR_QUARTERMASTER:
            SetQuarterMaster("");
            break;
        case partyhelpers::assign_party_role_action::SET_LEVEL_SYNC:
            SetSyncTarget(MemberName, MsgStd::LevelSyncSet);
            break;
        case partyhelpers::assign_party_role_action::DISABLE_LEVEL_SYNC:
            SetSyncTarget("", MsgStd::LevelSyncRemoveLeftParty);
            break;
    }

    if (partyhelpers::ShouldNotifyAllianceReloadOnRole(m_PAlliance != nullptr))
    {
        message::send(ipc::AllianceReload{
            .allianceId = m_PAlliance->m_AllianceID,
        });
    }
    else
    {
        message::send(ipc::PartyReload{
            .partyId = m_PartyID,
        });
    }
}

// get number of members in specified zone
uint8 CParty::MemberCount(uint16 ZoneID)
{
    uint8 count = 0;

    for (auto member : members)
    {
        const bool  zoneMatches = member->getZone() == ZoneID;
        const bool  isPC        = member->objtype == TYPE_PC;
        uint8       trustCount  = 0;
        if (isPC)
        {
            auto* charMember = static_cast<CCharEntity*>(member);
            trustCount       = static_cast<uint8>(charMember->PTrusts.size());
        }
        count = partyhelpers::AccumulateMemberCount(
            count,
            partyhelpers::MemberCountContribution(zoneMatches, isPC, trustCount));
    }
    return count;
}

// Returns entity pointer to party member by name (used for /pcmd kick or otherwise)
CBattleEntity* CParty::GetMemberByName(const std::string& memberName)
{
    const auto gate = partyhelpers::ClassifyGetMemberByName(m_PartyType == PARTY_PCS, memberName.empty());
    if (gate == partyhelpers::get_member_by_name_gate::REJECT_MOB_PARTY)
    {
        ShowWarning("%s", partyhelpers::FormatGetMemberMobPartyWarning(memberName));
        return nullptr;
    }
    if (gate == partyhelpers::get_member_by_name_gate::REJECT_EMPTY)
    {
        return nullptr;
    }

    for (auto& member : members)
    {
        if (partyhelpers::MemberNameMatches(memberName, member->getName()))
        {
            return member;
        }
    }

    return nullptr;
}

void CParty::RemoveMember(CBattleEntity* PEntity)
{
    const auto gate = partyhelpers::ClassifyRemoveMember(
        PEntity == nullptr,
        PEntity != nullptr && PEntity->PParty != this,
        PEntity != nullptr && m_PLeader == PEntity);

    if (gate == partyhelpers::remove_member_gate::REJECT_NULL_OR_MISMATCH)
    {
        ShowWarning("%s", partyhelpers::FormatRemoveMemberNullWarning());
        return;
    }

    if (gate == partyhelpers::remove_member_gate::REMOVE_AS_LEADER)
    {
        RemovePartyLeader(PEntity);

        // Remove their trusts
        CCharEntity* PChar = dynamic_cast<CCharEntity*>(PEntity);
        if (PChar)
        {
            PChar->ClearTrusts();
        }
    }
    else
    {
        auto memberToDelete = std::find(members.begin(), members.end(), PEntity);

        if (memberToDelete != members.end())
        {
            if (partyhelpers::ShouldRunPCRemoveCleanup(m_PartyType == PARTY_PCS, PEntity->objtype == TYPE_PC))
            {
                CCharEntity* PChar = static_cast<CCharEntity*>(PEntity);

                if (partyhelpers::ShouldClearQuarterMasterOnRemove(m_PQuarterMaster == PChar))
                {
                    SetQuarterMaster("");
                }
                if (partyhelpers::ShouldDisableSyncOnRemove(m_PSyncTarget == PChar))
                {
                    SetSyncTarget("", MsgStd::LevelSyncRemoveLeftParty);
                    CStatusEffect* sync = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
                    if (partyhelpers::ShouldStartSyncDisableCountdown(sync != nullptr, sync != nullptr && sync->GetDuration() == 0s))
                    {
                        PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, partyhelpers::LevelSyncDisableDurationSeconds, MsgStd::LevelSyncRemoveLeftParty);
                        sync->SetStartTime(timer::now());
                        sync->SetDuration(std::chrono::seconds(partyhelpers::LevelSyncDisableDurationSeconds));
                    }
                    DisableSync();
                }
                {
                    CStatusEffect* sync = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
                    if (partyhelpers::ShouldApplyLeavingSyncCountdown(
                            m_PSyncTarget != nullptr,
                            m_PSyncTarget == PChar,
                            PChar->status != STATUS_TYPE::DISAPPEAR,
                            sync != nullptr && sync->GetDuration() == 0s))
                    {
                        PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, partyhelpers::LevelSyncDisableDurationSeconds, MsgStd::LevelSyncRemoveLeftParty);
                        sync->SetStartTime(timer::now());
                        sync->SetDuration(std::chrono::seconds(partyhelpers::LevelSyncDisableDurationSeconds));
                    }
                }

                size_t trustCount = 0;
                if (m_PLeader != nullptr)
                {
                    trustCount = static_cast<CCharEntity*>(m_PLeader)->PTrusts.size();
                }

                PChar->PLatentEffectContainer->CheckLatentsPartyMembers(members.size(), trustCount);

                PChar->pushPacket<GP_SERV_COMMAND_GROUP_TBL>(nullptr);
                PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PChar, 0, 0, PChar->getZone());
                PChar->pushPacket<CCharStatusPacket>(PChar);

                db::preparedStmt("DELETE FROM accounts_parties WHERE charid = ?", PChar->id);

                if (m_PAlliance)
                {
                    message::send(ipc::AllianceReload{
                        .allianceId = m_PAlliance->m_AllianceID,
                    });
                }
                else
                {
                    message::send(ipc::PartyReload{
                        .partyId = m_PartyID,
                    });
                }

                if (PChar->PTreasurePool != nullptr && PChar->PTreasurePool->getPoolType() != TreasurePoolType::Zone)
                {
                    PChar->PTreasurePool->delMember(PChar);
                    PChar->PTreasurePool = new CTreasurePool(TreasurePoolType::Solo);
                    PChar->PTreasurePool->addMember(PChar);
                    PChar->PTreasurePool->updatePool(PChar);
                }
            }

            members.erase(memberToDelete);
            PEntity->PParty = nullptr;
        }
    }
}

void CParty::DelMember(CBattleEntity* PEntity)
{
    if (partyhelpers::ClassifyEntityPartyMatch(PEntity == nullptr, PEntity != nullptr && PEntity->PParty != this) ==
        partyhelpers::entity_party_gate::REJECT_NULL_OR_MISMATCH)
    {
        ShowWarning("%s", partyhelpers::FormatDelMemberNullWarning());
        return;
    }

    if (partyhelpers::ClassifyDelMemberPath(m_PLeader == PEntity) == partyhelpers::del_member_path::AS_LEADER)
    {
        if (partyhelpers::ShouldReloadPartyAfterLeaderDel(RemovePartyLeader(PEntity))) // Only reload if not disbanded
        {
            this->ReloadParty();
        }
    }
    else
    {
        auto memberToDelete = std::find(members.begin(), members.end(), PEntity);

        if (memberToDelete != members.end())
        {
            if (partyhelpers::ShouldRunPCRemoveCleanup(m_PartyType == PARTY_PCS, PEntity->objtype == TYPE_PC))
            {
                CCharEntity* PChar = static_cast<CCharEntity*>(PEntity);

                if (partyhelpers::ShouldClearQuarterMasterOnRemove(m_PQuarterMaster == PChar))
                {
                    SetQuarterMaster("");
                }
                if (partyhelpers::ShouldDisableSyncOnRemove(m_PSyncTarget == PChar))
                {
                    SetSyncTarget("", MsgStd::LevelSyncRemoveLeftParty);
                    CStatusEffect* sync = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
                    if (partyhelpers::ShouldStartSyncDisableCountdown(sync != nullptr, sync != nullptr && sync->GetDuration() == 0s))
                    {
                        PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, partyhelpers::LevelSyncDisableDurationSeconds, MsgStd::LevelSyncRemoveLeftParty);
                        sync->SetStartTime(timer::now());
                        sync->SetDuration(std::chrono::seconds(partyhelpers::LevelSyncDisableDurationSeconds));
                    }
                    DisableSync();
                }
                {
                    CStatusEffect* sync = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
                    if (partyhelpers::ShouldApplyLeavingSyncCountdown(
                            m_PSyncTarget != nullptr,
                            m_PSyncTarget == PChar,
                            PChar->status != STATUS_TYPE::DISAPPEAR,
                            sync != nullptr && sync->GetDuration() == 0s))
                    {
                        PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, partyhelpers::LevelSyncDisableDurationSeconds, MsgStd::LevelSyncRemoveLeftParty);
                        sync->SetStartTime(timer::now());
                        sync->SetDuration(std::chrono::seconds(partyhelpers::LevelSyncDisableDurationSeconds));
                    }
                }
                PChar->PLatentEffectContainer->CheckLatentsPartyMembers(members.size(), 0);

                PChar->pushPacket<GP_SERV_COMMAND_GROUP_TBL>(nullptr);
                PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PChar, 0, 0, PChar->getZone());
                PChar->pushPacket<CCharStatusPacket>(PChar);
                PChar->PParty = nullptr;

                if (PChar->PTreasurePool != nullptr && PChar->PTreasurePool->getPoolType() != TreasurePoolType::Zone)
                {
                    PChar->PTreasurePool->delMember(PChar);
                    PChar->PTreasurePool = new CTreasurePool(TreasurePoolType::Solo);
                    PChar->PTreasurePool->addMember(PChar);
                    PChar->PTreasurePool->updatePool(PChar);
                }
            }
            else
            {
                PEntity->PParty = nullptr;
            }
            members.erase(memberToDelete);
        }
        this->ReloadParty();
    }
}

void CParty::PopMember(CBattleEntity* PEntity)
{
    if (partyhelpers::ClassifyEntityPartyMatch(PEntity == nullptr, PEntity != nullptr && PEntity->PParty != this) ==
        partyhelpers::entity_party_gate::REJECT_NULL_OR_MISMATCH)
    {
        ShowWarning("%s", partyhelpers::FormatPopMemberNullWarning());
        return;
    }

    auto memberToDelete = std::find(members.begin(), members.end(), PEntity);

    if (memberToDelete != members.end())
    {
        members.erase(memberToDelete);
    }

    // free memory, party will re reinsatiated when they zone back in
    if (partyhelpers::ShouldDeleteEmptyPartyOnPop(members.empty()))
    {
        if (m_PAlliance)
        {
            if (partyhelpers::ShouldClearAllianceMainOnPop(true, m_PAlliance->getMainParty() == this))
            {
                m_PAlliance->setMainParty(nullptr);
            }

            auto it = m_PAlliance->partyList.begin();
            while (it != m_PAlliance->partyList.end())
            {
                if (this == *it)
                {
                    it = m_PAlliance->partyList.erase(it);
                    continue;
                }
                it++;
            }
        }
        delete this; // cpp.sh allow
    }
    PEntity->PParty = nullptr;
}

bool CParty::RemovePartyLeader(CBattleEntity* PEntity)
{
    const bool isMobParty = m_PartyType == PARTYTYPE::PARTY_MOBS;
    const bool isEmpty    = members.empty();

    if (isEmpty)
    {
        ShowWarning("%s", partyhelpers::FormatRemovePartyLeaderEmptyWarning());
        return partyhelpers::RemovePartyLeaderReturnValue(partyhelpers::remove_party_leader_plan::EMPTY_LIST);
    }

    // PC parties: attempt DB promote of a non-leader session member.
    if (partyhelpers::ShouldAttemptPCLeaderPromote(isMobParty))
    {
        const auto rset = db::preparedStmt("SELECT charname FROM accounts_sessions JOIN chars ON accounts_sessions.charid = chars.charid "
                                           "JOIN accounts_parties ON accounts_parties.charid = chars.charid WHERE partyid = ? AND NOT partyflag & ? "
                                           "ORDER BY timestamp ASC LIMIT 1",
                                           m_PartyID,
                                           PARTY_LEADER);
        if (rset && rset->rowsCount() && rset->next())
        {
            std::string newLeader = rset->get<std::string>("charname");
            SetLeader(newLeader);
        }
    }

    bool hasOtherMember = false;
    for (auto member : members)
    {
        if (member != PEntity)
        {
            hasOtherMember = true;
            break;
        }
    }

    const bool stillLeader = m_PLeader == PEntity;
    const auto plan        = partyhelpers::ClassifyRemovePartyLeader(isEmpty, isMobParty, hasOtherMember, stillLeader);

    switch (plan)
    {
        case partyhelpers::remove_party_leader_plan::MOB_PROMOTE_AND_DEL:
            // mob party, mob destructor being called and is leader of a party
            for (auto member : members)
            {
                if (member != PEntity) // assign leader to next party member
                {
                    m_PLeader = member;
                    DelMember(PEntity);
                    return partyhelpers::RemovePartyLeaderReturnValue(plan);
                }
            }
            // Fall through if no other member (should not happen when hasOtherMember).
            DisbandParty();
            return partyhelpers::RemovePartyLeaderReturnValue(partyhelpers::remove_party_leader_plan::DISBAND);

        case partyhelpers::remove_party_leader_plan::DISBAND:
            DisbandParty();
            return partyhelpers::RemovePartyLeaderReturnValue(plan);

        case partyhelpers::remove_party_leader_plan::REMOVE_MEMBER:
            RemoveMember(PEntity);
            return partyhelpers::RemovePartyLeaderReturnValue(plan);

        case partyhelpers::remove_party_leader_plan::EMPTY_LIST:
        default:
            return partyhelpers::RemovePartyLeaderReturnValue(partyhelpers::remove_party_leader_plan::EMPTY_LIST);
    }
}

std::vector<CParty::partyInfo_t> CParty::GetPartyInfo() const
{
    std::vector<CParty::partyInfo_t> memberinfo;

    const auto queryPlan = partyhelpers::PlanGetPartyInfoQuery(
        m_PartyType == PARTY_PCS,
        m_PAlliance != nullptr,
        m_PAlliance ? m_PAlliance->m_AllianceID : 0,
        m_PartyID);

    if (!queryPlan.query)
    {
        ShowWarning("%s", partyhelpers::FormatGetPartyInfoMobWarning());
        return memberinfo;
    }

    const auto rset = db::preparedStmt("SELECT chars.charid, partyid, allianceid, charname, partyflag, pos_zone, pos_prevzone FROM accounts_parties "
                                       "LEFT JOIN chars ON accounts_parties.charid = chars.charid WHERE "
                                       "(allianceid <> 0 AND allianceid = ?) OR partyid = ? ORDER BY partyflag & ?, timestamp",
                                       queryPlan.allianceID,
                                       queryPlan.partyID,
                                       queryPlan.orderFlags);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            memberinfo.emplace_back(CParty::partyInfo_t{
                .id         = rset->get<uint32>("charid"),
                .partyid    = rset->get<uint32>("partyid"),
                .allianceid = rset->get<uint32>("allianceid"),
                .name       = rset->get<std::string>("charname"),
                .flags      = rset->get<uint16>("partyflag"),
                .zone       = rset->get<uint16>("pos_zone"),
                .prev_zone  = rset->get<uint16>("pos_prevzone"),
            });
        }
    }

    return memberinfo;
}

void CParty::AddMember(CBattleEntity* PEntity)
{
    const bool alreadyInList = PEntity != nullptr && std::find(members.begin(), members.end(), PEntity) != members.end();
    const auto gate          = partyhelpers::ClassifyAddMember(
        PEntity == nullptr,
        PEntity != nullptr && PEntity->PParty != nullptr,
        alreadyInList,
        PEntity != nullptr && PEntity->objtype == TYPE_PC,
        m_PartyType == PARTY_PCS,
        IsFull(),
        HasTrusts());

    switch (gate)
    {
        case partyhelpers::add_member_gate::REJECT_NULL_OR_HAS_PARTY:
            ShowWarning("%s", partyhelpers::FormatAddMemberNullWarning());
            return;
        case partyhelpers::add_member_gate::REJECT_ALREADY_MEMBER:
            ShowWarning("%s", partyhelpers::FormatAddMemberAlreadyInListWarning());
            return;
        case partyhelpers::add_member_gate::REJECT_FULL:
            ShowWarning("%s", partyhelpers::FormatAddMemberFullWarning());
            return;
        case partyhelpers::add_member_gate::REJECT_TRUSTS:
            ShowWarning("%s", partyhelpers::FormatAddMemberTrustsWarning());
            return;
        case partyhelpers::add_member_gate::PROCEED:
            break;
    }

    PEntity->PParty = this;
    members.emplace_back(PEntity);

    if (partyhelpers::ShouldStampLeaderCreatedPartyTime(PEntity->objtype == TYPE_PC, members.size()))
    {
        auto* PLeader = dynamic_cast<CCharEntity*>(CParty::GetLeader());

        if (PLeader)
        {
            PLeader->m_LeaderCreatedPartyTime = timer::now();
        }
    }

    if (partyhelpers::ShouldRunPCAddPostProcess(m_PartyType == PARTY_PCS))
    {
        CCharEntity* PChar = dynamic_cast<CCharEntity*>(PEntity);

        if (!PChar)
        {
            ShowWarning("%s", partyhelpers::FormatAddMemberNonPlayerWarning(PEntity->getName()));
            return;
        }

        uint32 allianceid = 0;
        if (m_PAlliance)
        {
            allianceid = m_PAlliance->m_AllianceID;
        }

        db::preparedStmt("INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES (?, ?, ?, ?)",
                         PChar->id,
                         m_PartyID,
                         allianceid,
                         GetMemberFlags(PChar));

        if (partyhelpers::ShouldNotifyAllianceReloadOnRole(m_PAlliance != nullptr))
        {
            message::send(ipc::AllianceReload{
                .allianceId = m_PAlliance->m_AllianceID,
            });
        }
        else
        {
            message::send(ipc::PartyReload{
                .partyId = m_PartyID,
            });
        }

        ReloadTreasurePool(PChar);

        if (partyhelpers::ShouldClearSeekingParty(PChar->isSeekingParty()))
        {
            PChar->playerConfig.InviteFlg = false;
            PChar->updatemask |= UPDATE_HP;

            charutils::SaveCharStats(PChar);
            charutils::SavePlayerSettings(PChar);

            PChar->pushPacket<GP_SERV_COMMAND_CONFIG>(PChar);
            PChar->pushPacket<CCharStatusPacket>(PChar);
            PChar->pushPacket<CCharSyncPacket>(PChar);
        }

        PChar->PTreasurePool->updatePool(PChar);

        // Apply level sync if the party is level synced
        if (partyhelpers::ShouldApplyPartyLevelSyncOnJoin(m_PSyncTarget != nullptr))
        {
            if (PChar->getZone() == m_PSyncTarget->getZone())
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, m_PSyncTarget->GetMLevel(), MsgStd::LevelSyncActivated);
                PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::OnZone);
                PChar->StatusEffectContainer->AddStatusEffectSilent(xi::StatusEffect::LevelSync, static_cast<uint16>(xi::StatusEffect::LevelSync), m_PSyncTarget->GetMLevel(), 0s, 0s);
                PChar->loc.zone->PushPacket(PChar, CHAR_INRANGE, std::make_unique<CCharSyncPacket>(PChar));
            }
        }

        // You lose all your summoned trusts upon joining a party
        PChar->ClearTrusts();

        PChar->m_charHistory.joinedParties++;
    }
}

void CParty::AddMember(uint32 id)
{
    if (!partyhelpers::ShouldRunOutOfZoneAddMember(m_PartyType == PARTY_PCS))
    {
        return;
    }

    // Out-of-zone add is always a PC joining a PC party.
    if (partyhelpers::ShouldRejectPCAddFull(true, true, IsFull()))
    {
        ShowWarning("%s", partyhelpers::FormatAddMemberOutOfZoneFullWarning());
        return;
    }

    if (partyhelpers::ShouldRejectPCAddTrusts(true, true, HasTrusts()))
    {
        ShowWarning("%s", partyhelpers::FormatAddMemberTrustsWarning());
        return;
    }

    const bool hasAlliance = m_PAlliance != nullptr;
    uint32     allianceid  = 0;
    if (hasAlliance)
    {
        allianceid = m_PAlliance->m_AllianceID;
    }
    const uint16 Flags = partyhelpers::OutOfZoneAddMemberFlags(hasAlliance, this->m_PartyNumber);

    db::preparedStmt("INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES (?, ?, ?, ?)",
                     id,
                     m_PartyID,
                     allianceid,
                     Flags);

    if (partyhelpers::ShouldNotifyAllianceReloadOnRole(hasAlliance))
    {
        message::send(ipc::AllianceReload{
            .allianceId = m_PAlliance->m_AllianceID,
        });
    }
    else
    {
        message::send(ipc::PartyReload{
            .partyId = m_PartyID,
        });
    }

    /*if (PChar->nameflags.flags & FLAG_INVITE)
    {
        PChar->nameflags.flags ^= FLAG_INVITE;
        PChar->updatemask |= UPDATE_HP;

        charutils::SaveCharStats(PChar);

        PChar->status = STATUS_UPDATE;
        PChar->pushPacket<GP_SERV_COMMAND_CONFIG>(PChar);
        PChar->pushPacket<CCharStatusPacket>(PChar);
        PChar->pushPacket<CCharSyncPacket>(PChar);
    }
    PChar->PTreasurePool->UpdatePool(PChar);*/
}

void CParty::PushMember(CBattleEntity* PEntity)
{
    if (partyhelpers::ClassifyPushMember(PEntity == nullptr, PEntity != nullptr && PEntity->PParty != nullptr) ==
        partyhelpers::push_member_gate::REJECT_NULL_OR_HAS_PARTY)
    {
        ShowWarning("%s", partyhelpers::FormatPushMemberNullWarning());
        return;
    }

    PEntity->PParty = this;
    members.emplace_back(PEntity);

    auto info = GetPartyInfo();

    for (auto&& memberinfo : info)
    {
        if (partyhelpers::MemberInfoMatchesEntity(memberinfo.id, PEntity->id))
        {
            if (partyhelpers::ShouldAssignLeaderFromFlags(memberinfo.flags))
            {
                m_PLeader = PEntity;
            }
            if (partyhelpers::ShouldAssignQuarterMasterFromFlags(memberinfo.flags))
            {
                m_PQuarterMaster = PEntity;
            }
            if (partyhelpers::ShouldAssignSyncTargetFromFlags(memberinfo.flags))
            {
                m_PSyncTarget = PEntity;
            }
        }
    }

    ReloadTreasurePool((CCharEntity*)PEntity);
}

void CParty::SetPartyID(uint32 id)
{
    m_PartyID = id;
}

uint32 CParty::GetPartyID() const
{
    return m_PartyID;
}

CBattleEntity* CParty::GetLeader()
{
    return m_PLeader;
}

CBattleEntity* CParty::GetSyncTarget()
{
    return m_PSyncTarget;
}

CBattleEntity* CParty::GetQuaterMaster()
{
    return m_PQuarterMaster;
}

uint16 CParty::GetMemberFlags(CBattleEntity* PEntity)
{
    if (partyhelpers::ShouldRejectGetMemberFlags(PEntity == nullptr, PEntity != nullptr && PEntity->PParty != this))
    {
        ShowWarning("%s", partyhelpers::FormatGetMemberFlagsNullWarning());
        return 0;
    }

    const bool isLeader = PEntity == m_PLeader;
    const bool isAllianceLeader = partyhelpers::IsAllianceLeaderForFlags(
        PEntity->PParty->m_PAlliance != nullptr,
        isLeader,
        PEntity->PParty->m_PAlliance != nullptr && PEntity->PParty->m_PAlliance->getMainParty() == PEntity->PParty);

    return partyhelpers::MemberFlags(
        PEntity->PParty->m_PartyNumber,
        isLeader,
        PEntity == m_PQuarterMaster,
        PEntity == m_PSyncTarget,
        isAllianceLeader);
}

// update the party for all members
void CParty::ReloadParty()
{
    if (partyhelpers::ShouldSkipMobReloadParty(m_PartyType == PARTYTYPE::PARTY_MOBS)) // Mob parties don't need to send packets
    {
        return;
    }

    auto info = GetPartyInfo();

    // alliance
    if (partyhelpers::ClassifyReloadPartyPath(this->m_PAlliance != nullptr) == partyhelpers::reload_party_path::ALLIANCE)
    {
        for (auto&& party : m_PAlliance->partyList)
        {
            party->RefreshFlags(info);
            for (auto&& member : party->members)
            {
                CCharEntity* PChar = (CCharEntity*)member;
                PChar->ReloadPartyDec();
                PChar->pushPacket<GP_SERV_COMMAND_GROUP_TBL>(party);
                // auto effects = std::make_unique<GP_SERV_COMMAND_GROUP_EFFECTS>();
                partyhelpers::reload_party_member_list_position listPosition{};
                for (auto&& memberinfo : info)
                {
                    listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, memberinfo.flags);
                    auto* PPartyMember = zoneutils::GetChar(memberinfo.id);
                    if (PPartyMember)
                    {
                        PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PPartyMember, listPosition.listIndex, memberinfo.flags, PChar->getZone());
                        // if (memberinfo.partyid == party->GetPartyID() && PPartyMember != PChar)
                        //    effects->AddMemberEffects(PChar);
                    }
                    else
                    {
                        uint16 zoneid = partyhelpers::OfflineMemberZoneID(memberinfo.zone, memberinfo.prev_zone);
                        PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(memberinfo.id, memberinfo.name, memberinfo.flags, listPosition.listIndex, zoneid);
                    }
                    listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
                }
                // PChar->pushPacket(effects.release());
            }
        }
    }
    else
    {
        RefreshFlags(info);
        CBattleEntity* PLeader    = GetLeader();
        size_t         trustCount = 0;

        if (PLeader != nullptr)
        {
            trustCount = static_cast<CCharEntity*>(PLeader)->PTrusts.size();
        }

        // regular party
        for (auto& member : members)
        {
            CCharEntity* PChar = (CCharEntity*)member;

            PChar->PLatentEffectContainer->CheckLatentsPartyJobs();
            PChar->PLatentEffectContainer->CheckLatentsPartyMembers(members.size(), trustCount);
            PChar->PLatentEffectContainer->CheckLatentsPartyAvatar();
            PChar->ReloadPartyDec();
            PChar->pushPacket<GP_SERV_COMMAND_GROUP_TBL>(this, PLeader && PChar->getZone() == PLeader->getZone());
            // auto effects = std::make_unique<GP_SERV_COMMAND_GROUP_EFFECTS>();
            uint8 j = 0;
            for (auto&& memberinfo : info)
            {
                auto* PPartyMember = zoneutils::GetChar(memberinfo.id);
                const auto rowPlan = partyhelpers::PlanSoloReloadPartyRow(j, PPartyMember != nullptr, trustCount);
                if (PPartyMember)
                {
                    PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PPartyMember, j, memberinfo.flags, PChar->getZone());

                    // Inject the party leader's trusts into the party list
                    CBattleEntity* PLeader = GetLeader();
                    if (PLeader != nullptr)
                    {
                        for (auto* PTrust : ((CCharEntity*)PLeader)->PTrusts)
                        {
                            j++;
                            // trusts don't persist over zonelines, so we know their zone has be the same as the leader.
                            PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PTrust, j);
                        }
                    }
                }
                else
                {
                    uint16 zoneid = partyhelpers::OfflineMemberZoneID(memberinfo.zone, memberinfo.prev_zone);
                    PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(memberinfo.id, memberinfo.name, memberinfo.flags, j, zoneid);
                }
                j = rowPlan.nextIndex;
            }
        }
    }
}

// update party info for PChar
void CParty::ReloadPartyMembers(CCharEntity* PChar)
{
    if (partyhelpers::ShouldRejectNullReloadPartyMembers(PChar == nullptr))
    {
        ShowWarning("%s", partyhelpers::FormatReloadPartyMembersNullWarning());
        return;
    }

    PChar->ReloadPartyDec();
    PChar->pushPacket<GP_SERV_COMMAND_GROUP_TBL>(this);

    auto info = GetPartyInfo();
    RefreshFlags(info);
    partyhelpers::reload_party_member_list_position listPosition{};
    for (auto&& memberinfo : info)
    {
        listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, memberinfo.flags);
        CCharEntity* PPartyMember = zoneutils::GetChar(memberinfo.id);
        if (PPartyMember)
        {
            PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(PPartyMember, listPosition.listIndex, memberinfo.flags, PChar->getZone());
        }
        else
        {
            uint16 zoneid = partyhelpers::OfflineMemberZoneID(memberinfo.zone, memberinfo.prev_zone);
            PChar->pushPacket<GP_SERV_COMMAND_GROUP_LIST>(memberinfo.id, memberinfo.name, memberinfo.flags, listPosition.listIndex, zoneid);
        }
        listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    }
}

// update treasure pool for specified character
void CParty::ReloadTreasurePool(CCharEntity* PChar)
{
    if (partyhelpers::ShouldRejectNullReloadTreasurePool(PChar == nullptr))
    {
        ShowWarning("%s", partyhelpers::FormatReloadTreasurePoolNullWarning());
        return;
    }

    if (partyhelpers::ShouldKeepZoneTreasurePool(
            PChar->PTreasurePool != nullptr,
            PChar->PTreasurePool != nullptr && PChar->PTreasurePool->getPoolType() == TreasurePoolType::Zone))
    {
        return;
    }

    const auto scan = partyhelpers::ClassifyReloadTreasureScan(
        PChar->PParty != nullptr,
        PChar->PParty != nullptr && PChar->PParty->m_PAlliance != nullptr);

    // alliance
    if (scan == partyhelpers::reload_treasure_scan::ALLIANCE)
    {
        for (std::size_t a = 0; a < PChar->PParty->m_PAlliance->partyList.size(); ++a)
        {
            for (std::size_t i = 0; i < PChar->PParty->m_PAlliance->partyList.at(a)->members.size(); ++i)
            {
                CCharEntity* PPartyMember = (CCharEntity*)PChar->PParty->m_PAlliance->partyList.at(a)->members.at(i);

                if (partyhelpers::ShouldJoinMemberTreasurePool(
                        PPartyMember == PChar,
                        PPartyMember->PTreasurePool != nullptr,
                        PPartyMember->getZone() == PChar->getZone()))
                {
                    if (partyhelpers::ShouldDelOwnPoolBeforeJoin(PChar->PTreasurePool != nullptr))
                    {
                        PChar->PTreasurePool->delMember(PChar);
                    }
                    PChar->PTreasurePool = PPartyMember->PTreasurePool;
                    PChar->PTreasurePool->addMember(PChar);
                    return;
                }
            }

        } // regular party
    }
    else if (scan == partyhelpers::reload_treasure_scan::PARTY)
    {
        for (auto& member : members)
        {
            CCharEntity* PPartyMember = (CCharEntity*)member;

            if (partyhelpers::ShouldJoinMemberTreasurePool(
                    PPartyMember == PChar,
                    PPartyMember->PTreasurePool != nullptr,
                    PPartyMember->getZone() == PChar->getZone()))
            {
                if (partyhelpers::ShouldDelOwnPoolBeforeJoin(PChar->PTreasurePool != nullptr))
                {
                    PChar->PTreasurePool->delMember(PChar);
                }
                PChar->PTreasurePool = PPartyMember->PTreasurePool;
                PChar->PTreasurePool->addMember(PChar);
                return;
            }
        }
    }

    if (partyhelpers::ShouldCreateSoloTreasurePool(PChar->PTreasurePool != nullptr))
    {
        PChar->PTreasurePool = new CTreasurePool(TreasurePoolType::Solo);
        PChar->PTreasurePool->addMember(PChar);
    }
}

void CParty::SetLeader(const std::string& MemberName)
{
    if (partyhelpers::ShouldUseMobPartyFirstMemberAsLeader(m_PartyType == PARTY_MOBS))
    {
        m_PLeader = members.at(0);
        return;
    }

    if (m_PartyType == PARTY_PCS)
    {
        uint32 newId = 0;

        const auto rset = db::preparedStmt("SELECT chars.charid from accounts_sessions JOIN chars ON chars.charid = accounts_sessions.charid WHERE charname = ?", MemberName);
        if (partyhelpers::ClassifySetLeaderLookup(static_cast<bool>(rset), rset && rset->rowsCount() && rset->next()) ==
            partyhelpers::set_leader_lookup_gate::FOUND)
        {
            newId = rset->get<uint32>(0);
        }
        else
        {
            return;
        }

        db::preparedStmt("UPDATE accounts_parties SET partyflag = partyflag & ~? WHERE partyid = ? AND partyflag & ?",
                         partyhelpers::ClearLeaderFlagsMask,
                         m_PartyID,
                         partyhelpers::PartyLeaderFlag);
        db::preparedStmt("UPDATE accounts_parties SET partyid = ? WHERE partyid = ?", newId, m_PartyID);
        db::preparedStmt("UPDATE accounts_parties SET allianceid = ? WHERE allianceid = ?", newId, m_PartyID);

        m_PLeader = GetMemberByName(MemberName);
        if (partyhelpers::ShouldRewriteAllianceIDOnLeaderChange(this->m_PAlliance != nullptr, this->m_PAlliance && this->m_PAlliance->m_AllianceID == m_PartyID))
        {
            m_PAlliance->m_AllianceID = newId;
        }

        m_PartyID = partyhelpers::NewPartyIDFromLeaderChar(newId);
        // SQL IF(allianceid = partyid, ALLIANCE_LEADER|PARTY_LEADER, PARTY_LEADER) stays server-side;
        // LeaderPartyFlags documents the two branch values for hosts.
        db::preparedStmt("UPDATE accounts_parties SET partyflag = partyflag | IF(allianceid = partyid, ?, ?) WHERE charid = ?",
                         partyhelpers::LeaderPartyFlags(true),
                         partyhelpers::LeaderPartyFlags(false),
                         newId);

        // Passing leader dismisses trusts
        for (auto* PMemberEntity : members)
        {
            if (auto* PMember = dynamic_cast<CCharEntity*>(PMemberEntity))
            {
                PMember->ClearTrusts();
            }
        }
    }
}

void CParty::SetSyncTarget(const std::string& MemberName, MsgStd message)
{
    CBattleEntity* PEntity = GetMemberByName(MemberName);

    const bool levelSyncEnabled = settings::get<bool>("map.LEVEL_SYNC_ENABLE");
    const bool designeeFound    = PEntity != nullptr;
    const bool designeeIsPC     = designeeFound && PEntity->objtype == TYPE_PC;

    bool   anyMemberHasBlockingStatus = false;
    uint8  designeeLevel              = 0;
    bool   sameZoneAsLeader           = false;
    CCharEntity* PChar                = nullptr;

    if (designeeIsPC)
    {
        PChar             = static_cast<CCharEntity*>(PEntity);
        designeeLevel     = PChar->GetMLevel();
        sameZoneAsLeader  = GetLeader() != nullptr && PChar->getZone() == GetLeader()->getZone();
        for (auto& member : members)
        {
            if (member->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::LevelRestriction, xi::StatusEffect::LevelSync, xi::StatusEffect::SjRestriction, xi::StatusEffect::Confrontation, xi::StatusEffect::Battlefield }))
            {
                anyMemberHasBlockingStatus = true;
                break;
            }
        }
    }

    const auto gate = partyhelpers::ClassifySetSyncTarget(
        levelSyncEnabled,
        designeeFound,
        designeeIsPC,
        designeeLevel,
        sameZoneAsLeader,
        anyMemberHasBlockingStatus);

    switch (gate)
    {
        case partyhelpers::set_sync_target_gate::DISABLED:
            return;

        case partyhelpers::set_sync_target_gate::REJECT_BELOW_MIN:
            ((CCharEntity*)GetLeader())->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>((CCharEntity*)GetLeader(), (CCharEntity*)GetLeader(), 0, partyhelpers::LevelSyncMinLevel, MsgStd::LevelSyncDesigneeBelowMin);
            return;

        case partyhelpers::set_sync_target_gate::REJECT_OTHER_AREA:
            ((CCharEntity*)GetLeader())->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>((CCharEntity*)GetLeader(), (CCharEntity*)GetLeader(), 0, 0, MsgStd::LevelSyncDesigneeInOtherArea);
            return;

        case partyhelpers::set_sync_target_gate::REJECT_STATUS:
            ((CCharEntity*)GetLeader())->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>((CCharEntity*)GetLeader(), (CCharEntity*)GetLeader(), 0, 0, MsgStd::LevelSyncPreventedByStatus);
            return;

        case partyhelpers::set_sync_target_gate::ENABLE:
        {
            m_PSyncTarget = PChar;
            for (auto& i : members)
            {
                const bool isPC         = i->objtype == TYPE_PC;
                CCharEntity* member     = isPC ? static_cast<CCharEntity*>(i) : nullptr;
                const bool notDisappear = member != nullptr && member->status != STATUS_TYPE::DISAPPEAR;
                const bool sameZone     = member != nullptr && member->getZone() == PChar->getZone();
                if (!partyhelpers::ShouldApplySyncEnableToMember(isPC, notDisappear, sameZone))
                {
                    continue;
                }

                member->pushPacket<GP_SERV_COMMAND_MESSAGE>(PChar->GetMLevel(), 0, 0, 0, message);
                member->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::OnZone);
                member->StatusEffectContainer->AddStatusEffectSilent(xi::StatusEffect::LevelSync, static_cast<uint16>(xi::StatusEffect::LevelSync), PChar->GetMLevel(), 0s, 0s);
                member->loc.zone->PushPacket(member, CHAR_INRANGE, std::make_unique<CCharSyncPacket>(member));
            }
            db::preparedStmt("UPDATE accounts_parties SET partyflag = partyflag & ~? WHERE partyid = ? AND partyflag & ?",
                             PARTY_SYNC,
                             m_PartyID,
                             PARTY_SYNC);
            db::preparedStmt("UPDATE accounts_parties SET partyflag = partyflag | ? WHERE partyid = ? AND charid = ?",
                             PARTY_SYNC,
                             m_PartyID,
                             PChar->id);
            return;
        }

        case partyhelpers::set_sync_target_gate::DISABLE:
        {
            if (m_PSyncTarget != nullptr)
            {
                // disable level sync
                for (auto& i : members)
                {
                    const bool isPC         = i->objtype == TYPE_PC;
                    CCharEntity* member     = isPC ? static_cast<CCharEntity*>(i) : nullptr;
                    const bool notDisappear = member != nullptr && member->status != STATUS_TYPE::DISAPPEAR;
                    if (!partyhelpers::ShouldApplySyncDisableToMember(isPC, notDisappear))
                    {
                        continue;
                    }

                    CStatusEffect* sync = member->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
                    if (partyhelpers::ShouldStartSyncDisableCountdown(sync != nullptr, sync != nullptr && sync->GetDuration() == 0s))
                    {
                        member->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(member, member, 0, partyhelpers::LevelSyncDisableDurationSeconds, message);
                        sync->SetStartTime(timer::now());
                        sync->SetDuration(std::chrono::seconds(partyhelpers::LevelSyncDisableDurationSeconds));
                    }
                }
            }
            m_PSyncTarget = nullptr;
            db::preparedStmt("UPDATE accounts_parties SET partyflag = partyflag & ~? WHERE partyid = ? AND partyflag & ?",
                             PARTY_SYNC,
                             m_PartyID,
                             PARTY_SYNC);
            return;
        }
    }
}

// FIXME: add case for "" membername
void CParty::SetQuarterMaster(const std::string& MemberName)
{
    CBattleEntity* PEntity = GetMemberByName(MemberName);
    m_PQuarterMaster       = PEntity;

    db::preparedStmt("UPDATE accounts_parties SET partyflag = partyflag & ~? WHERE partyid = ? AND partyflag & ?",
                     partyhelpers::PartyQMFlag,
                     m_PartyID,
                     partyhelpers::PartyQMFlag);

    if (partyhelpers::ShouldSetQuarterMasterDBFlag(PEntity != nullptr))
    {
        db::preparedStmt("UPDATE accounts_parties JOIN chars ON accounts_parties.charid = chars.charid "
                         "SET partyflag = partyflag | ? WHERE partyid = ? AND charname = ?",
                         partyhelpers::PartyQMFlag,
                         m_PartyID,
                         MemberName);
    }
}

// Send a packet to all members of the group if the zone is specified as 0
// or to the party members in the specified zone.
// Packet for PPartyMember is not sent in both cases
void CParty::PushPacket(uint32 senderID, uint16 ZoneID, const std::unique_ptr<CBasicPacket>& packet)
{
    for (auto& i : members)
    {
        if (i == nullptr)
        {
            continue;
        }

        const bool isPC = i->objtype == TYPE_PC;
        if (!isPC)
        {
            continue;
        }

        CCharEntity* member = static_cast<CCharEntity*>(i);
        if (partyhelpers::ShouldPushPartyPacketToMember(
                isPC,
                member->id,
                senderID,
                member->status != STATUS_TYPE::DISAPPEAR,
                jailutils::InPrison(member),
                ZoneID,
                member->getZone()))
        {
            member->pushPacket(packet->copy());
        }
    }
}

void CParty::PushEffectsPacket()
{
    if (!partyhelpers::ShouldPushEffectsPacket(m_EffectsChanged))
    {
        return;
    }

    auto info = GetPartyInfo();

    for (auto& PMember : members)
    {
        auto*                     PMemberChar = static_cast<CCharEntity*>(PMember);
        std::vector<CCharEntity*> sameZoneMembers;

        for (auto& memberinfo : info)
        {
            auto*      PPartyMember = zoneutils::GetChar(memberinfo.id);
            const bool charFound    = PPartyMember != nullptr;
            const bool sameZone     = charFound && PPartyMember->getZone() == PMemberChar->getZone();
            if (partyhelpers::ShouldIncludeInGroupEffects(
                    memberinfo.partyid,
                    m_PartyID,
                    memberinfo.id,
                    PMemberChar->id,
                    charFound,
                    sameZone))
            {
                sameZoneMembers.push_back(PPartyMember);
            }
        }

        // Make and send packet for PMemberChar
        PMemberChar->pushPacket<GP_SERV_COMMAND_GROUP_EFFECTS>(sameZoneMembers);
    }
    m_EffectsChanged = false;
}

void CParty::EffectsChanged()
{
    m_EffectsChanged = true;
}

void CParty::DisableSync()
{
    m_PSyncTarget = nullptr;
    ReloadParty();
}

void CParty::RefreshSync()
{
    CCharEntity* sync      = (CCharEntity*)m_PSyncTarget;
    uint8        syncLevel = sync->jobs.job[sync->GetMJob()];
    if (partyhelpers::ShouldRemoveSyncForLowLevel(syncLevel))
    {
        SetSyncTarget("", MsgStd::LevelSyncRemoveLowLevel);
    }
    for (auto& i : members)
    {
        if (i->objtype != TYPE_PC)
        {
            continue;
        }

        CCharEntity* member = (CCharEntity*)i;
        const auto plan = partyhelpers::PlanRefreshSyncMember(
            true,
            member->getZone() == sync->getZone(),
            syncLevel,
            member->jobs.job[member->GetMJob()],
            member->GetMLevel());
        if (!plan.apply)
        {
            continue;
        }

        CStatusEffect* syncEffect = member->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelSync);
        if (syncEffect != nullptr)
        {
            syncEffect->SetPower(syncLevel);
        }

        if (plan.rebuild)
        {
            charutils::RemoveAllEquipMods(member);
            member->m_LevelRestriction = plan.newMainLevel;
            member->SetMLevel(plan.newMainLevel);
            member->SetSLevel(member->jobs.job[member->GetSJob()]);
            charutils::ApplyAllEquipMods(member);

            blueutils::ValidateBlueSpells(member);
            jobpointutils::RefreshGiftMods(member);
            charutils::BuildingCharSkillsTable(member);
            charutils::CalculateStats(member);
            charutils::BuildingCharTraitsTable(member);
            charutils::BuildingCharAbilityTable(member);
            charutils::BuildingCharWeaponSkills(member);
            charutils::CheckValidEquipment(member);
            member->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(member);
        }
        member->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(member, member, 0, syncLevel, MsgStd::LevelSyncActivated);
    }
    m_PSyncTarget = sync;
}

void CParty::SetPartyNumber(uint8 number)
{
    m_PartyNumber = number;
}

bool CParty::HasOnlyOneMember() const
{
    // Load party size to make sure that there is only one member in the party across all servers
    return partyhelpers::HasOnlyOnePartyMember(members.size(), LoadPartySize());
}

bool CParty::IsFull() const
{
    // Load party size to make sure that that all members are accounted for across all servers
    return partyhelpers::IsPartyFull(members.size(), LoadPartySize());
}

uint32 CParty::LoadPartySize() const
{
    const auto queryPlan = partyhelpers::PlanLoadPartySizeQuery(m_PartyType == PARTYTYPE::PARTY_PCS, m_PartyID);
    if (!queryPlan.query)
    {
        return partyhelpers::LoadPartySizeForType(false, members.size(), 0);
    }

    const auto rset = db::preparedStmt("SELECT COUNT(*) FROM accounts_parties WHERE partyid = ?", queryPlan.partyID);
    if (rset && rset->rowsCount() && rset->next())
    {
        return partyhelpers::LoadPartySizeForType(true, members.size(), rset->get<uint32>(0));
    }

    return partyhelpers::LoadPartySizeForType(true, members.size(), 0);
}

timer::time_point CParty::GetTimeLastMemberJoined()
{
    auto* PLeader                    = dynamic_cast<CCharEntity*>(CParty::GetLeader());
    auto  LeaderMemberLastJoinedTime = timer::now();

    if (PLeader)
    {
        LeaderMemberLastJoinedTime = PLeader->m_LeaderCreatedPartyTime;
    }

    return LeaderMemberLastJoinedTime;
}

bool CParty::HasTrusts()
{
    bool anyMemberHasTrusts = false;
    for (auto* PMember : members)
    {
        if (auto* PCharMember = dynamic_cast<CCharEntity*>(PMember))
        {
            if (!PCharMember->PTrusts.empty())
            {
                anyMemberHasTrusts = true;
                break;
            }
        }
    }
    return partyhelpers::PartyHasTrusts(anyMemberHasTrusts);
}

void CParty::RefreshFlags(std::vector<partyInfo_t>& info)
{
    // Clear pointers in case they no longer exist on this instance
    m_PLeader        = nullptr;
    m_PQuarterMaster = nullptr;
    m_PSyncTarget    = nullptr;

    for (auto&& memberinfo : info)
    {
        if (partyhelpers::ShouldRefreshFlagsForParty(memberinfo.partyid, m_PartyID))
        {
            if (partyhelpers::ShouldAssignLeaderFromFlags(memberinfo.flags))
            {
                bool found = false;
                for (auto* member : members)
                {
                    if (partyhelpers::MemberInfoMatchesEntity(member->id, memberinfo.id))
                    {
                        m_PLeader = member;
                        found     = true;
                    }
                }
                if (!found)
                {
                    m_PLeader = nullptr;
                }
            }
            if (partyhelpers::ShouldAssignQuarterMasterFromFlags(memberinfo.flags))
            {
                bool found = false;
                for (auto* member : members)
                {
                    if (partyhelpers::MemberInfoMatchesEntity(member->id, memberinfo.id))
                    {
                        m_PQuarterMaster = member;
                        found            = true;
                    }
                }
                if (!found)
                {
                    m_PQuarterMaster = nullptr;
                }
            }
            if (partyhelpers::ShouldAssignSyncTargetFromFlags(memberinfo.flags))
            {
                bool found = false;
                for (auto* member : members)
                {
                    if (partyhelpers::MemberInfoMatchesEntity(member->id, memberinfo.id))
                    {
                        m_PSyncTarget = member;
                        found         = true;
                    }
                }
                if (!found)
                {
                    m_PSyncTarget = nullptr;
                }
            }
            if (partyhelpers::ShouldAssignAllianceLeaderFromFlags(memberinfo.flags, m_PAlliance != nullptr))
            {
                bool found = false;
                for (auto* member : members)
                {
                    if (partyhelpers::MemberInfoMatchesEntity(member->id, memberinfo.id))
                    {
                        m_PAlliance->setMainParty(this);
                        found = true;
                    }
                }
                if (!found)
                {
                    m_PAlliance->setMainParty(nullptr);
                }
            }
        }
    }
}

std::size_t CParty::GetMemberCountAcrossAllProcesses()
{
    // TODO: We should detect whether or not we're a multi-process
    // setup. So we can avoid asking the database for more information
    // than we need to.
    return partyhelpers::CountPartyMembersAcrossAllProcesses(GetPartyInfo().size());
}
