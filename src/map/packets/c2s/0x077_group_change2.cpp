/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

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

#include "0x077_group_change2.h"

#include "ipc_client.h"
#include "linkshell.h"

#include "common/ipc_structs.h"
#include "entities/char_entity.h"
#include "items/item_linkshell.h"

namespace
{

const std::set validLinkshellOperations = {
    GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell1,
    GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell2,
};

} // namespace

auto groupchange2helpers::MakeDispatchPlan(const uint8 kind, const uint8 changeKind, const RuntimeState& state) -> DispatchPlan
{
    switch (static_cast<GP_CLI_COMMAND_GROUP_CHANGE2_KIND>(kind))
    {
        case GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Party:
            return state.hasParty && (changeKind == 0 || (changeKind >= 4 && changeKind <= 7)) ? DispatchPlan{ Action::AssignPartyRole } : DispatchPlan{};
        case GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell1:
            return state.linkshell1Ready && (changeKind == 2 || changeKind == 3) ? DispatchPlan{ Action::SendLinkshellRankChange, 1 } : DispatchPlan{};
        case GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell2:
            return state.linkshell2Ready && (changeKind == 2 || changeKind == 3) ? DispatchPlan{ Action::SendLinkshellRankChange, 2 } : DispatchPlan{};
        case GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Alliance:
            return state.hasParty && state.hasAlliance && changeKind == 1 ? DispatchPlan{ Action::AssignAllianceLeaderAndReload } : DispatchPlan{};
    }
    return {};
}

// Go host pure half: packetsystem.ValidateGroupChange2 / ProcessGroupChange2 /
// NewGroupChange2Handler (6484); plan mappacket.ClientGroupChange2Packet.DispatchPlan.
auto GP_CLI_COMMAND_GROUP_CHANGE2::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    auto pv = PacketValidator(PChar)
                  .blockedBy({ BlockedState::InEvent })
                  .oneOf<GP_CLI_COMMAND_GROUP_CHANGE2_KIND>(this->Kind)
                  .oneOf<GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND>(this->ChangeKind);

    switch (static_cast<GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND>(this->ChangeKind))
    {
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetPartyLeader:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetQuartermaster:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetLottery:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetLevelSync:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::DisableLevelSync:
        {
            pv
                .mustEqual(this->Kind, GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Party, "Invalid operation")
                .isPartyLeader();
        }
        break;
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::PearlToSack:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SackToPearl:
        {
            pv
                .oneOf("Kind", static_cast<GP_CLI_COMMAND_GROUP_CHANGE2_KIND>(this->Kind), validLinkshellOperations)
                .hasLinkshellRank(this->Kind, LSTYPE_LINKSHELL);
        }
        break;
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetAllianceLeader:
        {
            pv
                .mustEqual(this->Kind, GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Alliance, "Invalid operation")
                .isAllianceLeader();
        }
        break;
    }

    return pv;
}

void GP_CLI_COMMAND_GROUP_CHANGE2::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto memberName = db::escapeString(asStringFromUntrustedSource(this->sName, sizeof(this->sName)));
    const auto item1      = reinterpret_cast<CItemLinkshell*>(PChar->getEquip(SLOT_LINK1));
    const auto item2      = reinterpret_cast<CItemLinkshell*>(PChar->getEquip(SLOT_LINK2));
    const auto plan       = groupchange2helpers::MakeDispatchPlan(this->Kind, this->ChangeKind, {
                                                                                                    .hasParty        = PChar->PParty != nullptr,
                                                                                                    .hasAlliance     = PChar->PParty != nullptr && PChar->PParty->m_PAlliance != nullptr,
                                                                                                    .linkshell1Ready = PChar->PLinkshell1 != nullptr && item1 != nullptr,
                                                                                                    .linkshell2Ready = PChar->PLinkshell2 != nullptr && item2 != nullptr,
                                                                                                });
    switch (plan.action)
    {
        case groupchange2helpers::Action::AssignPartyRole:
        {
            ShowDebug(fmt::format("(Party) Altering permissions of {} to {}", memberName, this->ChangeKind));
            PChar->PParty->AssignPartyRole(memberName, static_cast<GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND>(this->ChangeKind));
        }
        break;
        case groupchange2helpers::Action::SendLinkshellRankChange:
        {
            CItemLinkshell*   PItemLinkshell = nullptr;
            const CLinkshell* PLinkshell     = nullptr;
            switch (plan.linkshellSlot)
            {
                case 1:
                {
                    PItemLinkshell = reinterpret_cast<CItemLinkshell*>(PChar->getEquip(SLOT_LINK1));
                    PLinkshell     = PChar->PLinkshell1;
                }
                break;
                case 2:
                {
                    PItemLinkshell = reinterpret_cast<CItemLinkshell*>(PChar->getEquip(SLOT_LINK2));
                    PLinkshell     = PChar->PLinkshell2;
                }
                break;
                default:
                    // Conditions leading here are checked beforehand
                    break;
            }

            if (PLinkshell && PItemLinkshell)
            {
                message::send(ipc::LinkshellRankChange{
                    .requesterId   = PChar->id,
                    .requesterRank = PItemLinkshell->GetLSType(),
                    .memberName    = memberName,
                    .linkshellId   = PLinkshell->getID(),
                    .newRank       = this->ChangeKind,
                });
            }
        }
        break;
        case groupchange2helpers::Action::AssignAllianceLeaderAndReload:
        {
            ShowDebug(fmt::format("(Alliance) Changing leader to {}", memberName));
            PChar->PParty->m_PAlliance->assignAllianceLeader(memberName);

            message::send(ipc::AllianceReload{
                .allianceId = PChar->PParty->m_PAlliance->m_AllianceID,
            });
        }
        break;
        case groupchange2helpers::Action::None:
            break;
    }
}
