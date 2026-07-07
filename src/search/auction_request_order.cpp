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

#include "auction_request_order.h"

#include "common/utils.h"

auto BuildAuctionHouseOrderByString(const uint8* packetData, uint8 paramCount) -> std::string
{
    std::string orderByString = "ORDER BY";

    for (uint8 i = 0; i < paramCount; ++i)
    {
        const uint8 param = ref<uint32>(packetData, 0x18 + 8 * i);
        ShowInfoFmt(" Param{}: {}", i, param);

        switch (param)
        {
            case 2:
                orderByString.append(" item_equipment.level DESC,");
                break;
            case 5:
                orderByString.append(" item_weapon.dmg DESC,");
                break;
            case 6:
                orderByString.append(" item_weapon.delay DESC,");
                break;
            case 9:
                orderByString.append(" item_basic.sortname,");
                break;
        }
    }

    orderByString.append(" item_basic.itemid");
    return orderByString;
}
