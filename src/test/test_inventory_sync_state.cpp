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

#include "test_inventory_sync_state.h"

#include "entities/battle_entity.h"
#include "inventory_sync_state.h"
#include "items/item.h"

#include <iostream>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "inventory sync state self-test failed: " << label << " got "
                  << static_cast<int>(actual) << " expected " << static_cast<int>(expected) << '\n';
        return false;
    }
    return true;
}

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "inventory sync state self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testSyncedContainers() -> bool
{
    InventorySyncState state;

    bool ok = true;
    ok      = expectBool(state.isSynced(LOC_INVENTORY), false, "default inventory synced") && ok;
    ok      = expectEqual(state.getSyncedFlags(), static_cast<uint32_t>(0), "default synced flags") && ok;

    state.markSynced(LOC_INVENTORY);
    state.markSynced(LOC_WARDROBE);
    state.markSynced(LOC_RECYCLEBIN);
    state.markSynced(MAX_CONTAINER_ID);

    ok = expectBool(state.isSynced(LOC_INVENTORY), true, "inventory synced") && ok;
    ok = expectBool(state.isSynced(LOC_WARDROBE), true, "wardrobe synced") && ok;
    ok = expectBool(state.isSynced(LOC_RECYCLEBIN), true, "recycle bin synced") && ok;
    ok = expectBool(state.isSynced(MAX_CONTAINER_ID), false, "max sentinel synced") && ok;

    const auto expectedFlags = static_cast<uint32_t>((1u << LOC_INVENTORY) | (1u << LOC_WARDROBE) | (1u << LOC_RECYCLEBIN));
    ok                       = expectEqual(state.getSyncedFlags(), expectedFlags, "synced flags") && ok;
    return ok;
}

auto testEquipChangeQueue() -> bool
{
    InventorySyncState state;
    CItem              item(100);
    item.setLocationID(LOC_MOGSAFE);

    state.queueEquipChange(LOC_WARDROBE3, 7, SLOT_MAIN, nullptr, Equipping::Yes);
    state.queueEquipChange(LOC_INVENTORY, 0, SLOT_SUB, &item, Equipping::No);

    bool ok = true;
    ok      = expectBool(state.hasPendingEquipChanges(), true, "has pending changes") && ok;

    const auto& changes = state.pendingEquipChanges();
    ok                  = expectEqual(changes.size(), static_cast<std::size_t>(2), "pending change count") && ok;
    ok                  = expectEqual(changes[0].container, LOC_WARDROBE3, "first container") && ok;
    ok                  = expectEqual(changes[0].containerSlotId, static_cast<uint8>(7), "first container slot") && ok;
    ok                  = expectEqual(changes[0].equipSlot, SLOT_MAIN, "first equip slot") && ok;
    ok                  = expectBool(changes[0].item == nullptr, true, "first item") && ok;
    ok                  = expectBool(changes[0].equipping == Equipping::Yes, true, "first equipping") && ok;
    ok                  = expectEqual(changes[1].container, LOC_INVENTORY, "second container") && ok;
    ok                  = expectEqual(changes[1].containerSlotId, static_cast<uint8>(0), "second container slot") && ok;
    ok                  = expectEqual(changes[1].equipSlot, SLOT_SUB, "second equip slot") && ok;
    ok                  = expectBool(changes[1].item == &item, true, "second item") && ok;
    ok                  = expectBool(changes[1].equipping == Equipping::No, true, "second equipping") && ok;

    const auto& dirty = state.dirtyContainers();
    ok                = expectEqual(dirty.size(), static_cast<std::size_t>(2), "dirty container count") && ok;
    ok                = expectBool(dirty.contains(LOC_WARDROBE3), true, "dirty equip container") && ok;
    ok                = expectBool(dirty.contains(LOC_MOGSAFE), true, "dirty unequip item location") && ok;

    state.removeEquipChange(&item);
    ok = expectEqual(state.pendingEquipChanges().size(), static_cast<std::size_t>(1), "pending count after remove") && ok;
    ok = expectBool(state.pendingEquipChanges()[0].item == nullptr, true, "remaining change after remove") && ok;

    state.clearEquipChanges();
    ok = expectBool(state.hasPendingEquipChanges(), false, "has pending after clear") && ok;
    ok = expectEqual(state.pendingEquipChanges().size(), static_cast<std::size_t>(0), "pending count after clear") && ok;
    ok = expectEqual(state.dirtyContainers().size(), static_cast<std::size_t>(0), "dirty count after clear") && ok;
    return ok;
}

} // namespace

auto runInventorySyncStateSelfTests() -> bool
{
    bool ok = true;
    ok      = testSyncedContainers() && ok;
    ok      = testEquipChangeQueue() && ok;
    return ok;
}
