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

#include <string_view>

#include "base.h"

namespace translatehelpers
{

struct ResponsePlan
{
    uint16           itemID;
    std::string_view translation;
};

[[nodiscard]] constexpr auto BuildResponsePlan(const bool found, const uint16 itemID, const std::string_view translation) -> ResponsePlan
{
    return found ? ResponsePlan{ itemID, translation } : ResponsePlan{};
}

} // namespace translatehelpers

enum class GP_CLI_COMMAND_TRANSLATE_INDEX : uint8_t
{
    Japanese = 0,
    English  = 1,
};

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x002B
// This packet is sent by the client when using the /translate command.
GP_CLI_PACKET_VLA(GP_CLI_COMMAND_TRANSLATE, Name,
                  GP_CLI_COMMAND_TRANSLATE_INDEX FromIndex;
                  GP_CLI_COMMAND_TRANSLATE_INDEX ToIndex;
                  uint16_t                       padding00;
                  uint8_t                        Name[64]; // Variable length
);
