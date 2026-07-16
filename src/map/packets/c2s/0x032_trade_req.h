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

#pragma once

#include "base.h"

// The handler's entity lookup, container access, and packet delivery remain
// host-owned.  This small plan captures the ordered, portable policy gates.
namespace tradereqhelpers
{
enum class Plan
{
    Ignore,
    Reject,
    RejectAidBlocked,
    CancelPreviousTrade,
    SendRequest,
};

struct Facts
{
    bool targetMatches{};
    bool eitherInPrison{};
    bool eitherCrafting{};
    bool aidBlocked{};
    bool targetAlreadyPendingSource{};
    bool targetContainerEmpty{};
    bool targetHasRecentPendingTrade{};
    bool targetTradeContainer{};
    bool sourceHasPendingTrade{};
    bool previousTargetMatches{};
};

constexpr auto makePlan(const Facts& facts) -> Plan
{
    if (!facts.targetMatches)
    {
        return Plan::Ignore;
    }

    if (facts.eitherInPrison || facts.eitherCrafting)
    {
        return Plan::Reject;
    }

    if (facts.aidBlocked)
    {
        return Plan::RejectAidBlocked;
    }

    if (facts.targetAlreadyPendingSource)
    {
        return Plan::Ignore;
    }

    if (!facts.targetContainerEmpty || facts.targetHasRecentPendingTrade || facts.targetTradeContainer)
    {
        return Plan::Reject;
    }

    if (facts.sourceHasPendingTrade && facts.previousTargetMatches)
    {
        return Plan::CancelPreviousTrade;
    }

    return Plan::SendRequest;
}
} // namespace tradereqhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0032
// This packet is sent by the client when it is requesting to trade with another player.
GP_CLI_PACKET(GP_CLI_COMMAND_TRADE_REQ,
              uint32_t UniqueNo;  // PS2: UniqueNo
              uint16_t ActIndex;  // PS2: ActIndex
              uint16_t padding00; // PS2: dammy2
);
