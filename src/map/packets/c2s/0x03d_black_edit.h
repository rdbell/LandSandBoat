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

struct SAVE_BLACK
{
    uint32_t ID;       // PS2: ID
    uint8_t  Name[16]; // PS2: Name
};

enum class GP_CLI_COMMAND_BLACK_EDIT_MODE : int8_t
{
    Add    = 0, // Adding a player to the blacklist.
    Remove = 1, // Removing a player from the blacklist.
};

namespace blackedithelpers
{

// Operation is the blacklist persistence operation selected after packet
// validation. A None operation can only result from bypassing validation.
enum class Operation : uint8
{
    None,
    Add,
    Remove,
};

// Response is the S2C BLACK_EDIT mode selected after name lookup and the
// selected blacklist persistence operation have completed.
enum class Response : uint8
{
    None,
    Error,
    Add,
    Delete,
};

[[nodiscard]] constexpr auto OperationFor(const GP_CLI_COMMAND_BLACK_EDIT_MODE mode) -> Operation
{
    switch (mode)
    {
        case GP_CLI_COMMAND_BLACK_EDIT_MODE::Add:
            return Operation::Add;
        case GP_CLI_COMMAND_BLACK_EDIT_MODE::Remove:
            return Operation::Remove;
    }

    return Operation::None;
}

[[nodiscard]] constexpr auto ResponseFor(const bool targetFound, const Operation operation, const bool mutationSucceeded) -> Response
{
    if (!targetFound)
    {
        return Response::Error;
    }

    switch (operation)
    {
        case Operation::Add:
            return mutationSucceeded ? Response::Add : Response::Error;
        case Operation::Remove:
            return mutationSucceeded ? Response::Delete : Response::Error;
        case Operation::None:
            return Response::None;
    }

    return Response::None;
}

} // namespace blackedithelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x003D
// This packet is sent by the client when interacting with the blacklist system.
GP_CLI_PACKET(GP_CLI_COMMAND_BLACK_EDIT,
              SAVE_BLACK Data;         // PS2: Data
              int8_t     Mode;         // PS2: Mode
              uint8_t    padding00[3]; // PS2: (New; did not exist.)
);
