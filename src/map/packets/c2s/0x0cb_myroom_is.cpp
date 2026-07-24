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

#include "0x0cb_myroom_is.h"

#include "entities/char_entity.h"
#include "enums/msg_std.h"
#include "utils/charutils.h"

namespace
{

const auto isRentARoom = [](const CCharEntity* PChar)
{
    switch (PChar->profile.nation)
    {
        case NATION_SANDORIA:
            return PChar->loc.zone->GetRegionID() != REGION_TYPE::SANDORIA;
        case NATION_BASTOK:
            return PChar->loc.zone->GetRegionID() != REGION_TYPE::BASTOK;
        case NATION_WINDURST:
            return PChar->loc.zone->GetRegionID() != REGION_TYPE::WINDURST;
        default:
            return true;
    }
};

} // namespace

// Go host pure half: packetsystem.ValidateMyRoomIs / ProcessMyRoomIs / NewMyRoomIsHandler (6525); plan mappacket.ClientMyRoomIsRuntimePlanFor.
auto GP_CLI_COMMAND_MYROOM_IS::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustEqual(PChar->m_moghouseID, PChar->id, "Character not in their mog house")
        .oneOf<GP_CLI_COMMAND_MYROOM_IS_KIND>(this->Kind)
        .oneOf<GP_CLI_COMMAND_MYROOM_IS_PARAM2>(this->Param2);
}

void GP_CLI_COMMAND_MYROOM_IS::process(MapSession* PSession, CCharEntity* PChar) const
{
    // Note: If you're in a Rent-a-Room, these commands are not available to the client.
    // However, retail will honor each of them if injected.
    if (isRentARoom(PChar))
    {
        ShowWarning(fmt::format("Player {} modifying Rent-a-Room state.", PChar->getName()));
    }

    switch (static_cast<GP_CLI_COMMAND_MYROOM_IS_KIND>(this->Kind))
    {
        case GP_CLI_COMMAND_MYROOM_IS_KIND::Open:
            // Not implemented
            // NOTE: If you zone or move floors while in the MH and you have someone visiting, they will be booted.
            // NOTE: When you zone or move floors your "open MH" flag will be reset.
            break;
        case GP_CLI_COMMAND_MYROOM_IS_KIND::Close:
            // Not implemented
            break;
        case GP_CLI_COMMAND_MYROOM_IS_KIND::Remodel:
        {
            const auto plan = myroomishelpers::PlanRemodel(this->Param2, {
                .nation                       = PChar->profile.nation,
                .mhflag                       = PChar->profile.mhflag,
                .hasMogPatioDesignDocument    = charutils::hasKeyItem(PChar, KeyItem::MOG_PATIO_DESIGN_DOCUMENT),
            });

            if (plan.warnSecondFloorLocked)
            {
                ShowWarning(fmt::format("Player {} remodeling MH2F without it unlocked.", PChar->getName()));
            }

            if (plan.warnMogPatioLocked)
            {
                ShowWarning(fmt::format("Player {} remodeling MH2F to Patio without owning the KI to unlock it.", PChar->getName()));
            }

            PChar->profile.mhflag = plan.mhflag;
            charutils::SaveCharStats(PChar);

            // Note: The forced zone may bypass this message.
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(MsgStd::SuccessfulRemodel);

            // If the model changes AND you're on MH2F; force a rezone so the model change can take effect.
            if (plan.requestZoneTransition)
            {
                const auto zoneid = PChar->getZone();

                PChar->loc.destination = zoneid;
                PChar->status          = STATUS_TYPE::DISAPPEAR;

                PChar->clearPacketList();
                PChar->requestedZoneChange = true;
            }
        }
        break;
    }
}
