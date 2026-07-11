/*
===========================================================================
  Copyright (c) 2021 Ixion Dev Teams
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

#include "common/utils.h"

#include <cstring>

#include "entities/char_entity.h"
#include "ipc_client.h"
#include "unitychat.h"
#include "unitychat_capacity.h"
#include "utils/jailutils.h"

CUnityChat::CUnityChat(uint32 leader)
: m_leader(leader)
{
}

uint32 CUnityChat::getLeader() const
{
    return m_leader;
}

void CUnityChat::AddMember(CCharEntity* PChar)
{
    db::preparedStmt("UPDATE accounts_sessions SET unitychat = ? WHERE charid = ? LIMIT 1", this->getLeader(), PChar->id);
    PChar->PUnityChat = this;
    members.emplace_back(PChar);
}

bool CUnityChat::DelMember(CCharEntity* PChar)
{
    for (uint32 i = 0; i < members.size(); ++i)
    {
        if (members.at(i) == PChar)
        {
            db::preparedStmt("UPDATE accounts_sessions SET unitychat = 0 WHERE charid = ? LIMIT 1", PChar->id);
            PChar->PUnityChat = nullptr;
            members.erase(members.begin() + i);
            break;
        }
    }
    return unitychathelpers::DelMemberRemaining(members.size());
}

void CUnityChat::PushPacket(uint32 senderID, const std::unique_ptr<CBasicPacket>& packet)
{
    for (auto& member : members)
    {
        if (unitychathelpers::ShouldReceiveUnityPacket(
                member->id == senderID,
                member->status == STATUS_TYPE::DISAPPEAR,
                jailutils::InPrison(member)))
        {
            member->pushPacket(packet->copy());
        }
    }
}

namespace unitychat
{

std::map<uint32, std::unique_ptr<CUnityChat>> UnityChatList;

CUnityChat* LoadUnityChat(uint32 leader)
{
    auto PUnity           = std::make_unique<CUnityChat>(leader);
    UnityChatList[leader] = std::move(PUnity);
    return UnityChatList[leader].get();
}

void UnloadUnityChat(uint32 leader)
{
    if (unitychathelpers::ShouldUnloadUnityChat(UnityChatList.find(leader) != UnityChatList.end()))
    {
        UnityChatList.erase(leader);
    }
}

bool AddOnlineMember(CCharEntity* PChar, uint32 leader)
{
    if (unitychathelpers::ShouldRejectNullOnlineMember(PChar == nullptr))
    {
        ShowWarning("%s", unitychathelpers::FormatOnlineMemberNullWarning());
        return unitychathelpers::OnlineMemberAlwaysReturnsFalse();
    }

    CUnityChat* PUnity       = nullptr;
    const bool  foundInCache = UnityChatList.find(leader) != UnityChatList.end();
    if (!unitychathelpers::ShouldLoadUnityChatOnOnlineAdd(foundInCache, leader))
    {
        if (foundInCache)
        {
            PUnity = UnityChatList.find(leader)->second.get();
        }
    }
    else
    {
        PUnity = LoadUnityChat(leader);
    }
    if (unitychathelpers::ShouldAddMemberAfterOnlineLookup(PUnity != nullptr))
    {
        PUnity->AddMember(PChar);
    }
    return unitychathelpers::OnlineMemberAlwaysReturnsFalse();
}

bool DelOnlineMember(CCharEntity* PChar, uint32 leader)
{
    if (unitychathelpers::ShouldRejectNullOnlineMember(PChar == nullptr))
    {
        ShowWarning("%s", unitychathelpers::FormatOnlineMemberNullWarning());
        return unitychathelpers::OnlineMemberAlwaysReturnsFalse();
    }

    try
    {
        CUnityChat* PUnityChat = UnityChatList.at(leader).get();
        if (unitychathelpers::ShouldEraseUnityChatAfterDelOnline(PUnityChat->DelMember(PChar)))
        {
            UnityChatList.erase(leader);
        }
    }
    catch (std::out_of_range& exception)
    {
        ShowError("%s", unitychathelpers::FormatDelOnlineMemberException(exception.what()));
    }
    return unitychathelpers::OnlineMemberAlwaysReturnsFalse();
}

CUnityChat* GetUnityChat(uint32 leader)
{
    if (unitychathelpers::ShouldReturnCachedUnityChat(UnityChatList.find(leader) != UnityChatList.end()))
    {
        return UnityChatList.find(leader)->second.get();
    }
    else
    {
        return nullptr;
    }
}

}; // namespace unitychat
