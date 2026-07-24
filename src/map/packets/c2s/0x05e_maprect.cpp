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

#include "0x05e_maprect.h"

#include "maprect_moghouse_exit.h"

#include "common/utils.h"
#include "entities/char_entity.h"
#include "enums/msg_std.h"
#include "map/navmesh/navmesh.h"
#include "packets/s2c/0x053_systemmes.h"
#include "packets/s2c/0x065_wpos2.h"
#include "utils/charutils.h"
#include "utils/zoneutils.h"

// Go host pure half: packetsystem.ValidateMapRect / ProcessMapRect /
// NewMapRectHandler (6472); plans ClientMapRectClassifyRectID + MogHouseExitFor.

namespace
{

const auto denyZone = [](CCharEntity* PChar)
{
    // TODO: Retail handling:
    // - Tripped poshack check: Placed somewhere on the corresponding 'exit' zoneline
    // - Failed distance check: No movement
    // - Invalid zoneline (observed in Kamihr): Placed on a different zoneline
    PChar->loc.p.rotation += 128;

    PChar->pushPacket<GP_SERV_COMMAND_SYSTEMMES>(0, 0, MsgStd::CouldNotEnter);
    PChar->pushPacket<GP_SERV_COMMAND_WPOS2>(PChar, PChar->loc.p, POSMODE::RESET);

    PChar->status = STATUS_TYPE::NORMAL;
};

} // namespace

auto GP_CLI_COMMAND_MAPRECT::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf<GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT>(this->MyRoomExitBit)
        .oneOf<GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE>(this->MyRoomExitMode);
}

void GP_CLI_COMMAND_MAPRECT::process(MapSession* PSession, CCharEntity* PChar) const
{
    uint16_t startingZone = PChar->getZone();
    auto     startingPos  = PChar->loc.p;

    PChar->ClearTrusts();

    const auto rectClassification = maprect::ClassifyRectID(this->RectID);
    const auto isMogHouseExit     = rectClassification.isMogHouseExit;
    const auto isMogHouseEntrance = rectClassification.isMogHouseEntrance;

    if (PChar->status == STATUS_TYPE::NORMAL)
    {
        PChar->status       = STATUS_TYPE::DISAPPEAR;
        PChar->loc.boundary = 0;

        // Exiting Mog House
        if (isMogHouseExit)
        {
            const auto decision = maprect::MogHouseExitFor({
                .startingZone   = startingZone,
                .startingRegion = zoneutils::GetCurrentRegion(startingZone),
                .inMogHouse     = PChar->inMogHouse(),
                .mogHouseFlags  = PChar->profile.mhflag,
                .exitBit        = static_cast<GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT>(this->MyRoomExitBit),
                .exitMode       = static_cast<GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE>(this->MyRoomExitMode),
                .regionForZone  = zoneutils::GetCurrentRegion,
            });
            if (decision.action == maprect::MogHouseExitAction::LeaveMogHouse)
            {
                if (decision.clearMogHouseID)
                {
                    PChar->m_moghouseID = 0;
                }
                PChar->loc.destination = decision.destinationZone;
                PChar->loc.p           = {};
                if (decision.clearSecondFloorTracker)
                {
                    PChar->profile.mhflag &= ~(0x40);
                }
            }
            else if (decision.action == maprect::MogHouseExitAction::ChangeFloor)
            {
                PChar->loc.destination = decision.destinationZone;
                PChar->loc.p           = {};
                if (decision.toggleSecondFloorTracker)
                {
                    PChar->profile.mhflag ^= 0x40;
                }
            }
            else if (decision.action == maprect::MogHouseExitAction::Abort)
            {
                return;
            }
            else
            {
                PChar->status = STATUS_TYPE::NORMAL;
                ShowWarning("GP_CLI_COMMAND_MAPRECT: Moghouse zoneline abuse by %s", PChar->getName());
                return;
            }
        }
        else
        {
            // Ensure the zone line exists
            if (zoneLine_t* PZoneLine = PChar->loc.zone->GetZoneLine(this->RectID); !PZoneLine)
            {
                ShowError("GP_CLI_COMMAND_MAPRECT: Zone line %u not found", this->RectID);

                denyZone(PChar);
                return;
            }
            else if (PChar->m_PMonstrosity != nullptr) // Not allowed to use zonelines while MON
            {
                denyZone(PChar);
                return;
            }
            else
            {
                // 38-42y distance limit observed
                if (distance(PChar->loc.p, PZoneLine->originPos, true) > 40.0f)
                {
                    ShowWarning("GP_CLI_COMMAND_MAPRECT: %s too far from zoneline %u (%.1fy)",
                                PChar->getName(),
                                this->RectID,
                                distance(PChar->loc.p, PZoneLine->originPos, true));

                    denyZone(PChar);
                    return;
                }

                // Ensure the destination exists
                CZone* PDestination = zoneutils::GetZone(PZoneLine->destinationZoneId);
                if (PDestination && (PDestination->GetIP() == 0 || PDestination->GetPort() == 0))
                {
                    ShowDebug("GP_CLI_COMMAND_MAPRECT: Zone %u closed to chars", PZoneLine->destinationZoneId);

                    denyZone(PChar);
                    return;
                }

                if (!isMogHouseEntrance && zoneutils::IsZoneAtPlayerCap(PZoneLine->destinationZoneId, PChar->m_GMlevel > 0))
                {
                    denyZone(PChar);
                    return;
                }

                if (isMogHouseEntrance)
                {
                    // TODO: for entering another persons mog house, it must be set here
                    PChar->m_moghouseID    = PChar->id;
                    PChar->loc.p           = PZoneLine->destinationPos;
                    PChar->loc.destination = PChar->getZone();
                    PChar->loc.prevzone    = PChar->getZone();

                    charutils::SavePrevZoneLineID(PChar, PZoneLine->zoneLineId);
                }
                else
                {
                    PChar->loc.destination = PZoneLine->destinationZoneId;
                    PChar->loc.p           = PZoneLine->nextSpawnPosition();

                    // Snap to navmesh for elevation on uneven zonelines
                    if (PDestination)
                    {
                        PDestination->navMesh()->snapToValidPosition(PChar->loc.p);
                    }

                    charutils::SavePrevZoneLineID(PChar, PZoneLine->zoneLineId);
                }
            }
        }

        ShowInfo("Zoning from zone %u to zone %u: %s", PChar->getZone(), PChar->loc.destination, PChar->getName());
    }

    PChar->clearPacketList();

    if (PChar->loc.destination >= MAX_ZONEID)
    {
        ShowWarning("GP_CLI_COMMAND_MAPRECT: Invalid destination passed to packet %u by %s", PChar->loc.destination, PChar->getName());
        PChar->loc.destination = startingZone;
        return;
    }

    auto destination = PChar->loc.destination == 0 ? PChar->getZone() : PChar->loc.destination;
    if (uint64_t ipp = zoneutils::GetZoneIPP(destination); ipp == 0)
    {
        ShowWarning(fmt::format("Char {} requested zone ({}) returned IPP of 0", PChar->name, destination));
        PChar->loc.destination = startingZone;
        PChar->loc.p           = startingPos;

        denyZone(PChar);
        return;
    }

    PChar->requestedZoneChange = true;

    // Save pet if any
    if (PChar->shouldPetPersistThroughZoning())
    {
        PChar->setPetZoningInfo();
    }
}
