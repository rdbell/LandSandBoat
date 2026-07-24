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

#include "0x03d_black_edit.h"

#include "common/database.h"
#include "packets/s2c/0x042_black_edit.h"
#include "utils/blacklistutils.h"
#include "utils/charutils.h"

// Go host pure half: packetsystem.ValidateBlackEdit / ProcessBlackEdit /
// NewBlackEditHandler (6458); plan mappacket.ClientBlackEditRuntimePlanFor.

namespace
{

const auto sendFailPacket = [](CCharEntity* PChar)
{
    PChar->pushPacket<GP_SERV_COMMAND_BLACK_EDIT>(0, "", GP_SERV_COMMAND_BLACK_EDIT_MODE::Error);
};

} // namespace

auto GP_CLI_COMMAND_BLACK_EDIT::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf<GP_CLI_COMMAND_BLACK_EDIT_MODE>(this->Mode);
}

void GP_CLI_COMMAND_BLACK_EDIT::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto name = db::escapeString(asStringFromUntrustedSource(this->Data.Name, 15));

    const auto [charid, accid] = charutils::getCharIdAndAccountIdFromName(name);
    const auto operation       = blackedithelpers::OperationFor(static_cast<GP_CLI_COMMAND_BLACK_EDIT_MODE>(this->Mode));
    const auto sendResponse    = [&](const blackedithelpers::Response response)
    {
        switch (response)
        {
            case blackedithelpers::Response::Error:
                sendFailPacket(PChar);
                break;
            case blackedithelpers::Response::Add:
                PChar->pushPacket<GP_SERV_COMMAND_BLACK_EDIT>(accid, name, GP_SERV_COMMAND_BLACK_EDIT_MODE::Add);
                break;
            case blackedithelpers::Response::Delete:
                PChar->pushPacket<GP_SERV_COMMAND_BLACK_EDIT>(accid, name, GP_SERV_COMMAND_BLACK_EDIT_MODE::Delete);
                break;
            case blackedithelpers::Response::None:
                break;
        }
    };

    if (!charid)
    {
        sendResponse(blackedithelpers::ResponseFor(false, operation, false));
        return;
    }

    bool mutationSucceeded = false;
    switch (operation)
    {
        case blackedithelpers::Operation::Add:
            mutationSucceeded = blacklistutils::AddBlacklisted(PChar->id, charid);
            break;
        case blackedithelpers::Operation::Remove:
            mutationSucceeded = blacklistutils::DeleteBlacklisted(PChar->id, charid);
            break;
        case blackedithelpers::Operation::None:
            break;
    }

    sendResponse(blackedithelpers::ResponseFor(true, operation, mutationSucceeded));
}
