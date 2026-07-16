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

// Captures ROE_CLAIM's post-validation ordering without coupling tests to the
// RoE script runtime or packet transport. onRecordClaim returns whether its
// claim trigger ran; CURRENCIES_1 is sent regardless of that result.
namespace roeclaimhelpers
{
enum class ClaimAction : uint8
{
    NoRecord,
    TriggerClaim,
};

struct Result
{
    ClaimAction action;
    bool        sendCurrencies1;
};

constexpr auto SelectResult(const bool recordClaimed) -> Result
{
    return { recordClaimed ? ClaimAction::TriggerClaim : ClaimAction::NoRecord, true };
}
} // namespace roeclaimhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x010E
// This packet is sent by the client when requesting to claim a completed Records of Eminence objectives reward.
GP_CLI_PACKET(GP_CLI_COMMAND_ROE_CLAIM,
              uint16_t ObjectiveId; // The Records of Eminence object id.
              uint16_t padding00;   // Padding; unused.
);
