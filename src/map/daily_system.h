/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#include "entities/char_entity.h"

#include <cstddef>
#include <vector>

namespace daily
{

// DailyItemRow is the subset of item_basic consumed while building the daily
// Gobbie Mystery Box pools. LoadDailyItems has already limited its query to
// special-dial-eligible rows before this classification runs.
struct DailyItemRow
{
    uint16   itemID;
    uint16   auctionHouseCategory;
    ItemFlag flags;
};

// DailyItemClassification records the special dial and optional numbered dial
// to which one item_basic row belongs. A numbered dial of zero means no
// additional pool.
struct DailyItemClassification
{
    bool  special;
    uint8 numberedDial;
};

// ClassifyDailyItem is the pure LoadDailyItems row classifier. Every input
// row joins the special pool because LoadDailyItems' query filters on flags &
// 4; numberedDial follows the original aH / item-ID policy.
auto ClassifyDailyItem(const DailyItemRow& row) -> DailyItemClassification;

uint16 SelectItem(CCharEntity* player, uint8 dial);
uint16 SelectItemFromPool(const std::vector<uint16>& items, size_t index);
void   LoadDailyItems();

} // namespace daily
