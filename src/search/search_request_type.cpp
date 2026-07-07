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

#include "search_request_type.h"

auto SearchRequestTypeToString(uint8 type) -> std::string
{
    switch (type)
    {
        case TCP_SEARCH:
            return "SEARCH";
        case TCP_SEARCH_ALL:
            return "SEARCH_ALL";
        case TCP_SEARCH_COMMENT:
            return "SEARCH_COMMENT";
        case TCP_GROUP_LIST:
            return "GROUP_LIST";
        case TCP_AH_REQUEST:
            return "AH_REQUEST";
        case TCP_AH_REQUEST_MORE:
            return "AH_REQUEST_MORE";
        case TCP_AH_HISTORY_SINGLE:
            return "AH_HISTORY_SINGLE";
        case TCP_AH_HISTORY_STACK:
            return "AH_HISTORY_STACK";
        default:
            return "UNKNOWN";
    }
}
