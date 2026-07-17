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

// DailyDialPools describes the process-wide lists used by SelectItem. Keeping
// the pools explicit lets the rare-owned replacement policy be tested without
// character, item catalog, or RNG hosts.
struct DailyDialPools
{
    const std::vector<uint16>& materials;
    const std::vector<uint16>& food;
    const std::vector<uint16>& medicine;
    const std::vector<uint16>& sundries1;
    const std::vector<uint16>& sundries2;
    const std::vector<uint16>& special;
    const std::vector<uint16>& gobbieJunk;
};

// ClassifyDailyItem is the pure LoadDailyItems row classifier. Every input
// row joins the special pool because LoadDailyItems' query filters on flags &
// 4; numberedDial follows the original aH / item-ID policy.
auto ClassifyDailyItem(const DailyItemRow& row) -> DailyItemClassification;

uint16 SelectItem(CCharEntity* player, uint8 dial);
uint16 SelectItemFromPool(const std::vector<uint16>& items, size_t index);

inline auto DailyDialPool(const DailyDialPools& pools, const uint8 dial) -> const std::vector<uint16>&
{
    switch (dial)
    {
        case 1: return pools.materials;
        case 2: return pools.food;
        case 3: return pools.medicine;
        case 4: return pools.sundries1;
        case 5: return pools.sundries2;
        case 6: return pools.special;
        default: return pools.gobbieJunk;
    }
}

// SelectItemWithRareFallback is the deterministic core of SelectItem. A rare
// selected item that is already owned is replaced by a Gobbie-junk selection.
template <typename RandomIndex, typename IsRare, typename HasItem>
auto SelectItemWithRareFallback(
    const DailyDialPools& pools,
    const uint8          dial,
    RandomIndex&&        randomIndex,
    IsRare&&             isRare,
    HasItem&&            hasItem) -> uint16
{
    const auto& source    = DailyDialPool(pools, dial);
    uint16      selection = SelectItemFromPool(source, randomIndex(source.size()));

    if (isRare(selection) && hasItem(selection))
    {
        const auto& fallback = pools.gobbieJunk;
        selection            = SelectItemFromPool(fallback, randomIndex(fallback.size()));
    }

    return selection;
}

void   LoadDailyItems();

} // namespace daily
