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

#include "test_universal_container.h"

#include "map/items/item.h"
#include "map/universal_container.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "universal container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "universal container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testDefaultStateAndTypeGuard() -> bool
{
    CUContainer container;

    bool ok = true;
    ok      = expectUInt(UCONTAINER_SIZE, 16, "UCONTAINER_SIZE") && ok;
    ok      = expectUInt(UCONTAINER_EMPTY, 0, "UCONTAINER_EMPTY") && ok;
    ok      = expectUInt(UCONTAINER_SYNTES, 1, "UCONTAINER_SYNTES") && ok;
    ok      = expectUInt(UCONTAINER_FISHING, 2, "UCONTAINER_FISHING") && ok;
    ok      = expectUInt(UCONTAINER_SHOP, 3, "UCONTAINER_SHOP") && ok;
    ok      = expectUInt(UCONTAINER_TRADE, 4, "UCONTAINER_TRADE") && ok;
    ok      = expectUInt(UCONTAINER_USEITEM, 5, "UCONTAINER_USEITEM") && ok;
    ok      = expectUInt(UCONTAINER_AUCTION, 6, "UCONTAINER_AUCTION") && ok;
    ok      = expectUInt(UCONTAINER_SEND_DELIVERYBOX, 7, "UCONTAINER_SEND_DELIVERYBOX") && ok;
    ok      = expectUInt(UCONTAINER_RECV_DELIVERYBOX, 8, "UCONTAINER_RECV_DELIVERYBOX") && ok;
    ok      = expectUInt(container.GetType(), UCONTAINER_EMPTY, "default type") && ok;
    ok      = expectBool(container.IsContainerEmpty(), true, "default empty type") && ok;
    ok      = expectBool(container.IsLocked(), false, "default lock") && ok;
    ok      = expectUInt(container.GetTarget(), 0, "default target") && ok;
    ok      = expectUInt(container.GetItemsCount(), 0, "default item count") && ok;
    ok      = expectBool(container.IsSlotEmpty(0), true, "default slot empty") && ok;
    ok      = expectBool(container.IsSlotEmpty(UCONTAINER_SIZE), true, "out-of-range slot empty") && ok;
    ok      = expectBool(container.GetItem(0) == nullptr, true, "default item pointer") && ok;
    ok      = expectBool(container.GetItem(UCONTAINER_SIZE) == nullptr, true, "out-of-range item pointer") && ok;

    container.SetType(UCONTAINER_SHOP);
    container.SetType(UCONTAINER_TRADE);
    ok = expectUInt(container.GetType(), UCONTAINER_SHOP, "second SetType ignored") && ok;
    ok = expectBool(container.IsContainerEmpty(), false, "typed container not empty") && ok;

    container.Clean();
    ok = expectUInt(container.GetType(), UCONTAINER_EMPTY, "clean resets type") && ok;
    ok = expectBool(container.IsContainerEmpty(), true, "clean resets empty type") && ok;
    return ok;
}

auto testLockTargetAndSlotMutation() -> bool
{
    CUContainer container;
    CItem       first(0x1001);
    CItem       second(0x1002);

    container.SetTarget(0x3456);
    container.SetType(UCONTAINER_TRADE);
    bool ok = true;
    ok      = expectUInt(container.GetTarget(), 0x3456, "set target") && ok;
    ok      = expectBool(container.SetItem(0, &first), true, "set first item") && ok;
    ok      = expectUInt(container.GetItemsCount(), 1, "count after first item") && ok;
    ok      = expectBool(container.IsSlotEmpty(0), false, "slot occupied") && ok;
    ok      = expectBool(container.GetItem(0) == &first, true, "stored first item pointer") && ok;

    ok = expectBool(container.SetItem(0, &second), true, "replace item") && ok;
    ok = expectUInt(container.GetItemsCount(), 1, "replace keeps count") && ok;
    ok = expectBool(container.GetItem(0) == &second, true, "stored second item pointer") && ok;

    ok = expectBool(container.SetItem(1, nullptr), true, "set empty slot to null") && ok;
    ok = expectUInt(container.GetItemsCount(), 1, "null into empty keeps count") && ok;
    ok = expectBool(container.SetItem(0, nullptr), true, "clear via SetItem") && ok;
    ok = expectUInt(container.GetItemsCount(), 0, "clear via SetItem decrements count") && ok;

    ok = expectBool(container.SetItem(UCONTAINER_SIZE, &first), false, "out-of-range SetItem") && ok;
    ok = expectUInt(container.GetItemsCount(), 0, "out-of-range SetItem keeps count") && ok;

    container.SetItem(0, &first);
    container.SetLock();
    ok = expectBool(container.IsLocked(), true, "set lock") && ok;
    ok = expectBool(container.SetItem(1, &second), false, "locked SetItem rejected") && ok;
    ok = expectBool(container.IsSlotEmpty(1), true, "locked SetItem leaves slot empty") && ok;
    container.UnLock();
    ok = expectBool(container.IsLocked(), false, "unlock") && ok;
    ok = expectBool(container.SetItem(1, &second), true, "unlocked SetItem accepted") && ok;
    ok = expectUInt(container.GetItemsCount(), 2, "unlocked SetItem increments count") && ok;
    return ok;
}

auto testResizeClearSlotAndClean() -> bool
{
    CUContainer container;
    CItem       first(0x1001);
    CItem       second(0x1002);

    container.SetType(UCONTAINER_TRADE);
    container.SetItem(0, &first);
    container.SetItem(1, &second);
    container.ClearSlot(0);

    bool ok = true;
    ok      = expectBool(container.IsSlotEmpty(0), true, "ClearSlot clears pointer") && ok;
    ok      = expectUInt(container.GetItemsCount(), 2, "ClearSlot keeps count") && ok;

    container.SetSize(1);
    ok = expectBool(container.GetItem(1) == nullptr, true, "shrink makes removed slot out of range") && ok;
    ok = expectUInt(container.GetItemsCount(), 2, "SetSize shrink keeps count") && ok;

    container.SetSize(3);
    ok = expectBool(container.IsSlotEmpty(2), true, "grow adds empty slot") && ok;
    ok = expectUInt(container.GetItemsCount(), 2, "SetSize grow keeps count") && ok;

    container.SetItem(1, &second);
    second.setReserve(123);
    container.Clean();
    ok = expectUInt(second.getReserve(), 0, "trade Clean clears item reserve") && ok;
    ok = expectUInt(container.GetItemsCount(), 0, "Clean resets count") && ok;
    ok = expectUInt(container.GetTarget(), 0, "Clean resets target") && ok;
    ok = expectBool(container.IsLocked(), false, "Clean resets lock") && ok;
    ok = expectBool(container.IsSlotEmpty(0), true, "Clean resets slots") && ok;
    ok = expectBool(container.IsSlotEmpty(UCONTAINER_SIZE - 1), true, "Clean restores default slot count") && ok;
    return ok;
}

} // namespace

auto runUniversalContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultStateAndTypeGuard() && ok;
    ok      = testLockTargetAndSlotMutation() && ok;
    ok      = testResizeClearSlotAndClean() && ok;
    return ok;
}
