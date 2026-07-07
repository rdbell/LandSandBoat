/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "common/cbasetypes.h"

#include <string>

enum TCPREQUESTTYPE
{
    TCP_SEARCH_ALL        = 0x00,
    TCP_GROUP_LIST        = 0x02,
    TCP_SEARCH            = 0x03,
    TCP_AH_HISTORY_SINGLE = 0x05,
    TCP_AH_HISTORY_STACK  = 0x06,
    TCP_SEARCH_COMMENT    = 0x08,
    TCP_AH_REQUEST_MORE   = 0x10,
    TCP_AH_REQUEST        = 0x15,
};

auto SearchRequestTypeToString(uint8 type) -> std::string;
