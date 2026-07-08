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

#include "test_item_currency.h"

#include "map/items/item_currency.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item currency self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item currency self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConstructorDefaults() -> bool
{
    CItemCurrency item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_CURRENCY), true, "currency type flag") && ok;
    ok      = expectBool(item.isType(ITEM_EQUIPMENT), false, "equipment type flag") && ok;
    ok      = expectUInt(item.getStackSize(), 999999999, "stack size") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    return ok;
}

auto testQuantityClampUsesCurrencyStackSize() -> bool
{
    CItemCurrency item(0x2000);

    bool ok = true;
    item.setQuantity(999999999);
    ok = expectUInt(item.getQuantity(), 999999999, "quantity at currency stack boundary") && ok;

    item.setQuantity(1000000000);

    ok      = expectUInt(item.getQuantity(), 999999999, "quantity clamped to currency stack") && ok;
    return ok;
}

auto testCopyConstructorCopiesBaseFields() -> bool
{
    CItemCurrency original(0x2222);
    original.setQuantity(123456);
    original.setBasePrice(777);
    original.setStackSize(42);

    CItemCurrency copy(original);
    original.setQuantity(1);
    original.setBasePrice(1);
    original.setStackSize(1);

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_CURRENCY), true, "copy currency type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 123456, "copy quantity") && ok;
    ok      = expectUInt(copy.getBasePrice(), 777, "copy base price") && ok;
    ok      = expectUInt(copy.getStackSize(), 42, "copy stack size") && ok;
    return ok;
}

} // namespace

auto runItemCurrencySelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testQuantityClampUsesCurrencyStackSize() && ok;
    ok      = testCopyConstructorCopiesBaseFields() && ok;
    return ok;
}
