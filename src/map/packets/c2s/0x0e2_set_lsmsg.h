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

#include <algorithm>
#include <span>
#include <string>

#include "base.h"

enum class GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL : uint8_t
{
    Linkshell = 0, // Linkshell owner can set the message.
    Pearlsack = 1, // Pearlsack owners can set the message.
    Linkpearl = 2, // Linkpearl owners can set the message.
};

// SET_LSMSG's host-independent post-validation routing. Item lookup,
// linkshell mutation, database execution, and packet queue ownership remain
// in the map server.
namespace setlsmsgpackethelpers
{

// ItemType mirrors the LSTYPE values stored in CItemLinkshell. They are
// intentionally distinct from WriteLevel: process compensates for the known
// native off-by-one mismatch by subtracting one from postRights.
enum class ItemType : uint8_t
{
    NewLinkshell = 0,
    Linkshell    = 1,
    Pearlsack    = 2,
    Linkpearl    = 3,
    Broken       = 4,
};

struct Facts
{
    bool     hasEquippedLS1;
    bool     equippedItemIsLinkshell;
    ItemType equippedItemType;
    uint8_t  postRights;
};

struct Plan
{
    bool                                  setPostRights;
    GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL   postRights;
    bool                                  setMessage;
    std::string                           message;
    bool                                  sendLinkshellNoAccess;
};

[[nodiscard]] inline auto BoundedMessage(const std::span<const uint8_t> source) -> std::string
{
    const auto end = std::find(source.begin(), source.end(), uint8_t{});
    return { reinterpret_cast<const char*>(source.data()), static_cast<std::size_t>(end - source.begin()) };
}

[[nodiscard]] inline auto CanEditMessage(const ItemType itemType, const uint8_t postRights) -> bool
{
    // Preserve GP_CLI_COMMAND_SET_LSMSG::process's temporary compensation for
    // the mismatched LSTYPE definition, including uint8_t underflow for 0.
    switch (static_cast<GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL>(postRights - 1U))
    {
        case GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell:
            return itemType == ItemType::Linkshell;
        case GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Pearlsack:
            return itemType == ItemType::Linkshell || itemType == ItemType::Pearlsack;
        case GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkpearl:
            return true;
    }

    return false;
}

[[nodiscard]] inline auto PlanFor(const Facts& facts,
                                  const bool changeAccess,
                                  const bool changeMessage,
                                  const GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL requestedWriteLevel,
                                  const std::span<const uint8_t> message) -> Plan
{
    if (!facts.hasEquippedLS1 || !facts.equippedItemIsLinkshell)
    {
        return {};
    }

    // Native makes this an if/else-if: changing access wins when both flags
    // are supplied, even if message editing would otherwise be permitted.
    if (changeAccess)
    {
        if (facts.equippedItemType == ItemType::Linkshell)
        {
            return { .setPostRights = true, .postRights = requestedWriteLevel };
        }
        return { .sendLinkshellNoAccess = true };
    }

    if (changeMessage)
    {
        if (CanEditMessage(facts.equippedItemType, facts.postRights))
        {
            return { .setMessage = true, .message = BoundedMessage(message) };
        }
        return { .sendLinkshellNoAccess = true };
    }

    return {};
}

} // namespace setlsmsgpackethelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00E2
// This packet is sent by the client when altering the linkshell message or linkshell message access level.
GP_CLI_PACKET(GP_CLI_COMMAND_SET_LSMSG,
              uint8_t  unknown00 : 4;    // PS2: stat
              uint8_t  unknown01 : 1;    // PS2: attr (Was originally a single 4 bit value.)
              uint8_t  unknown02 : 1;    // PS2: attr (Was originally a single 4 bit value.)
              uint8_t  unknown03 : 1;    // PS2: attr (Was originally a single 4 bit value.)
              uint8_t  unknown04 : 1;    // PS2: attr (Was originally a single 4 bit value.)
              uint8_t  readLevel : 2;    // PS2: readLevel
              uint8_t  writeLevel : 2;   // PS2: writeLevel
              uint8_t  pubEditLevel : 2; // PS2: pubEditLevel
              uint8_t  LinkshellId : 2;  // PS2: dummyBits
              uint8_t  Category;         // PS2: (New; did not exist.)
              uint8_t  ItemIndex;        // PS2: (New; did not exist.)
              uint8_t  padding00[2];     // PS2: (New; did not exist.)
              uint16_t seqId;            // PS2: seqId
              uint32_t uniqNo;           // PS2: uniqNo
              uint8_t  sMessage[128];    // PS2: sMessage
);
