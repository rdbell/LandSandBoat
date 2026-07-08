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

#include "test_item_access.h"

#include "map/items/item.h"
#include "map/items/item_access.h"
#include "map/items/transaction.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item access self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item access self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

struct TxAccess final : Transaction
{
    ~TxAccess() override
    {
        rollbackIfOpen();
    }

    static auto enter(CItem* item) -> bool
    {
        return enterTx(item);
    }

    static void exit(CItem* item)
    {
        exitTx(item);
    }

    auto holds(const CItem*) const -> bool override
    {
        return false;
    }

protected:
    auto doCommit() -> bool override
    {
        return true;
    }

    void doRollback() override
    {
    }
};

auto testDefaultItemState() -> bool
{
    CItem item(0x1234);

    bool ok = true;
    ok      = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Free), "default state") && ok;
    ok      = expectBool(item.isBusy(), false, "default busy") && ok;
    return ok;
}

auto testMarkStateTransitions() -> bool
{
    CItem item(0x2000);

    bool ok = true;
    ok      = expectBool(xi::items::mark(nullptr, ItemState::Equipped), false, "mark null") && ok;
    ok      = expectBool(xi::items::mark(&item, ItemState::Free), true, "mark free while free") && ok;

    ok = expectBool(xi::items::mark(&item, ItemState::Equipped), true, "mark equipped") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Equipped), "state equipped") && ok;
    ok = expectBool(item.isBusy(), true, "equipped busy") && ok;

    ok = expectBool(xi::items::mark(&item, ItemState::Bazaar), false, "busy mark rejected") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Equipped), "busy mark keeps state") && ok;

    ok = expectBool(xi::items::mark(&item, ItemState::Free), true, "release to free") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Free), "state free") && ok;
    ok = expectBool(item.isBusy(), false, "free not busy") && ok;

    ok = expectBool(xi::items::mark(&item, ItemState::InTransaction), false, "public mark in transaction rejected") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Free), "rejected tx mark keeps free") && ok;

    ok = expectBool(xi::items::mark(&item, ItemState::PlacedFurniture), true, "mark placed furniture") && ok;
    ok = expectBool(xi::items::mark(&item, ItemState::Free), true, "release placed furniture") && ok;
    return ok;
}

auto testTransactionOnlyTransitions() -> bool
{
    CItem item(0x3000);

    bool ok = true;
    ok      = expectBool(TxAccess::enter(nullptr), false, "enter tx null") && ok;
    TxAccess::exit(nullptr);

    ok = expectBool(TxAccess::enter(&item), true, "enter tx") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::InTransaction), "state in transaction") && ok;
    ok = expectBool(item.isBusy(), true, "tx busy") && ok;
    ok = expectBool(xi::items::mark(&item, ItemState::Equipped), false, "mark from tx rejected") && ok;
    ok = expectBool(xi::items::mark(&item, ItemState::Free), false, "public free from tx rejected") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::InTransaction), "tx rejection keeps state") && ok;

    TxAccess::exit(&item);
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Free), "exit tx frees") && ok;
    ok = expectBool(item.isBusy(), false, "exit tx not busy") && ok;

    ok = expectBool(xi::items::mark(&item, ItemState::Bazaar), true, "mark bazaar") && ok;
    ok = expectBool(TxAccess::enter(&item), false, "enter tx while busy rejected") && ok;
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Bazaar), "busy tx enter keeps state") && ok;
    TxAccess::exit(&item);
    ok = expectUInt(static_cast<uint8>(item.state()), static_cast<uint8>(ItemState::Free), "exit tx from busy frees") && ok;
    ok = expectBool(item.isBusy(), false, "exit tx from busy not busy") && ok;
    return ok;
}

} // namespace

auto runItemAccessSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultItemState() && ok;
    ok      = testMarkStateTransitions() && ok;
    ok      = testTransactionOnlyTransitions() && ok;
    return ok;
}
