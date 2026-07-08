/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_item_shop.h"

#include "map/items/item_shop.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item shop self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item shop self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testDefaultSettersAndMenuFlags() -> bool
{
    CItemShop item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectUInt(item.getMinPrice(), 0, "default min price") && ok;
    ok      = expectUInt(item.getMaxPrice(), 0, "default max price") && ok;
    ok      = expectUInt(item.getDailyIncrease(), 0, "default daily increase") && ok;
    ok      = expectUInt(item.getInitialQuantity(), 0, "default initial quantity") && ok;
    ok      = expectBool(item.IsDailyIncrease(), false, "default daily increase flag") && ok;
    ok      = expectBool(item.IsInMenu(), false, "zero quantity menu flag") && ok;

    item.setMinPrice(1200);
    item.setMaxPrice(3456);
    item.setDailyIncrease(7);
    item.setInitialQuantity(45);
    item.setStackSize(1);
    item.setQuantity(1);

    ok = expectUInt(item.getMinPrice(), 1200, "set min price") && ok;
    ok = expectUInt(item.getMaxPrice(), 3456, "set max price") && ok;
    ok = expectUInt(item.getDailyIncrease(), 7, "set daily increase") && ok;
    ok = expectUInt(item.getInitialQuantity(), 45, "set initial quantity") && ok;
    ok = expectBool(item.IsDailyIncrease(), true, "daily increase flag") && ok;
    ok = expectBool(item.IsInMenu(), true, "nonzero quantity menu flag") && ok;
    return ok;
}

auto testSellPriceFormula() -> bool
{
    CItemShop shopItem(0x2800);
    shopItem.setMinPrice(1200);
    shopItem.setStackSize(25);
    shopItem.setQuantity(25);
    shopItem.setStackSize(12);

    CItemShop fractionalStockItem(0x2801);
    fractionalStockItem.setMinPrice(10000);
    fractionalStockItem.setStackSize(25);
    fractionalStockItem.setQuantity(25);
    fractionalStockItem.setStackSize(12);

    CItemShop shopBoundary(0x6FFF);
    shopBoundary.setMinPrice(999);
    shopBoundary.setStackSize(99);
    shopBoundary.setQuantity(99);

    CItemShop normalItem(0x27FF);
    normalItem.setBasePrice(1000);

    CItemShop upperNonShopItem(0x7000);
    upperNonShopItem.setMinPrice(10000);
    upperNonShopItem.setStackSize(12);
    upperNonShopItem.setQuantity(12);
    upperNonShopItem.setBasePrice(900);

    bool ok = true;
    ok      = expectUInt(shopItem.getSellPrice(), 120, "shop item sell price with stock multiplier") && ok;
    ok      = expectUInt(fractionalStockItem.getSellPrice(), 1000, "shop item sell price integer stock multiplier") && ok;
    ok      = expectUInt(shopBoundary.getSellPrice(), 91, "shop boundary sell price") && ok;
    ok      = expectUInt(normalItem.getSellPrice(), 333, "normal item sell price") && ok;
    ok      = expectUInt(upperNonShopItem.getSellPrice(), 300, "upper non-shop item sell price") && ok;
    return ok;
}

auto testCopyConstructorCopiesShopAndBaseFields() -> bool
{
    CItemShop original(0x3000);
    original.setMinPrice(1800);
    original.setMaxPrice(2200);
    original.setDailyIncrease(6);
    original.setInitialQuantity(30);
    original.setStackSize(24);
    original.setQuantity(24);
    original.setStackSize(12);
    original.setBasePrice(9000);

    CItemShop copy(original);
    original.setMinPrice(1);
    original.setQuantity(0);

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x3000, "copy id") && ok;
    ok      = expectUInt(copy.getMinPrice(), 1800, "copy min price") && ok;
    ok      = expectUInt(copy.getMaxPrice(), 2200, "copy max price") && ok;
    ok      = expectUInt(copy.getDailyIncrease(), 6, "copy daily increase") && ok;
    ok      = expectUInt(copy.getInitialQuantity(), 30, "copy initial quantity") && ok;
    ok      = expectUInt(copy.getQuantity(), 24, "copy quantity") && ok;
    ok      = expectUInt(copy.getStackSize(), 12, "copy stack size") && ok;
    ok      = expectUInt(copy.getBasePrice(), 9000, "copy base price") && ok;
    ok      = expectBool(copy.IsInMenu(), true, "copy menu flag") && ok;
    ok      = expectUInt(copy.getSellPrice(), 180, "copy sell price") && ok;
    return ok;
}

auto testSetQuantityClampsToCurrentStackSize() -> bool
{
    CItemShop item(0x3000);

    bool ok = true;
    item.setQuantity(5);
    ok = expectUInt(item.getQuantity(), 0, "quantity before stack size") && ok;

    item.setStackSize(3);
    item.setQuantity(5);
    ok = expectUInt(item.getQuantity(), 3, "quantity after clamp") && ok;

    item.setStackSize(1);
    ok = expectUInt(item.getQuantity(), 3, "quantity after shrinking stack size") && ok;
    return ok;
}

} // namespace

auto runItemShopSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultSettersAndMenuFlags() && ok;
    ok      = testSellPriceFormula() && ok;
    ok      = testCopyConstructorCopiesShopAndBaseFields() && ok;
    ok      = testSetQuantityClampsToCurrentStackSize() && ok;
    return ok;
}
