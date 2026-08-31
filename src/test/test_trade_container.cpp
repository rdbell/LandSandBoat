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

#include "test_trade_container.h"
#include "omega_self_test_registry.h"

#include "map/trade_container.h"
#include "map/items/item.h"

#include <cstdint>
#include <iostream>
#include <variant>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "trade container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "trade container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testDefaultStateAndBounds() -> bool
{
    CTradeContainer container;

    bool ok = true;
    ok      = expectUInt(CONTAINER_SIZE, 17, "CONTAINER_SIZE") && ok;
    ok      = expectUInt(TRADE_CONTAINER_SIZE, 8, "TRADE_CONTAINER_SIZE") && ok;
    ok      = expectUInt(container.getSize(), CONTAINER_SIZE, "default size") && ok;
    ok      = expectUInt(container.getExSize(), 0, "default ex size") && ok;
    ok      = expectUInt(container.getType(), 0, "default type") && ok;
    ok      = expectUInt(container.getItemsCount(), 0, "default items count") && ok;
    ok      = expectUInt(container.getSlotCount(), 0, "default slot count") && ok;
    ok      = expectUInt(container.getTotalQuantity(), 0, "default total quantity") && ok;
    ok      = expectBool(container.getItem(0) == nullptr, true, "default item pointer") && ok;
    ok      = expectUInt(container.getItemID(0), 0, "default item id") && ok;
    ok      = expectUInt(container.getInvSlotID(0), 0xFF, "default inventory slot") && ok;
    ok      = expectUInt(container.getQuantity(0), 0, "default quantity") && ok;
    ok      = expectUInt(container.getConfirmedStatus(0), 0, "default confirmed status") && ok;
    ok      = expectBool(std::holds_alternative<std::monostate>(container.getRestriction(0)), true, "default restriction") && ok;
    ok      = expectBool(container.getItem(CONTAINER_SIZE) == nullptr, true, "out-of-range item pointer") && ok;
    ok      = expectUInt(container.getItemID(CONTAINER_SIZE), 0, "out-of-range item id") && ok;
    ok      = expectUInt(container.getInvSlotID(CONTAINER_SIZE), 0xFF, "out-of-range inventory slot") && ok;
    ok      = expectUInt(container.getQuantity(CONTAINER_SIZE), 0, "out-of-range quantity") && ok;
    ok      = expectUInt(container.getConfirmedStatus(CONTAINER_SIZE), 0, "out-of-range confirmed status") && ok;
    ok      = expectBool(std::holds_alternative<std::monostate>(container.getRestriction(CONTAINER_SIZE)), true, "out-of-range restriction") && ok;
    return ok;
}

auto testScalarSlotsAndTotals() -> bool
{
    CTradeContainer container;

    container.setType(7);
    container.setExSize(4);
    container.setItem(0, 0x1001, 3, 12);
    container.setItem(1, 0x1001, 4, 5);
    container.setItem(2, 0xFFFF, 5, 999);
    container.setItem(1, 0x1002, 9, 7);

    bool ok = true;
    ok      = expectUInt(container.getType(), 7, "set type") && ok;
    ok      = expectUInt(container.getExSize(), 4, "set ex size") && ok;
    ok      = expectUInt(container.getItemsCount(), 4, "setItem increments item count on every call") && ok;
    ok      = expectUInt(container.getSlotCount(), 3, "occupied slot count") && ok;
    ok      = expectUInt(container.getItemID(0), 0x1001, "slot 0 item id") && ok;
    ok      = expectUInt(container.getInvSlotID(1), 9, "overwritten inventory slot") && ok;
    ok      = expectUInt(container.getQuantity(1), 7, "overwritten quantity") && ok;
    ok      = expectUInt(container.getItemQuantity(0x1001), 12, "item quantity after overwrite") && ok;
    ok      = expectUInt(container.getItemQuantity(0x1002), 7, "new item quantity") && ok;
    ok      = expectUInt(container.getItemQuantity(0xFFFF), 999, "gil item quantity lookup") && ok;
    ok      = expectUInt(container.getTotalQuantity(), 20, "total quantity counts gil as one") && ok;

    container.setItemsCount(2);
    ok = expectUInt(container.getItemsCount(), 2, "manual items count setter") && ok;
    container.setItemID(1, 0x1003);
    container.setQuantity(1, 11);
    container.setInvSlotID(1, 10);
    ok = expectUInt(container.getItemID(1), 0x1003, "set item id") && ok;
    ok = expectUInt(container.getQuantity(1), 11, "set quantity") && ok;
    ok = expectUInt(container.getInvSlotID(1), 10, "set inventory slot") && ok;
    ok = expectUInt(container.getTotalQuantity(), 24, "total after scalar setters") && ok;
    return ok;
}

auto testRestrictionsResizeAndClean() -> bool
{
    CTradeContainer container;

    container.setItem(0, 0x2001, 1, 3);
    container.setItem(1, 0x2002, 2, 4);
    container.setRestriction(0, JobRestriction{ 8, 55 });
    container.setRestriction(1, GuildRestriction{ 4, 1234 });
    container.setRestriction(CONTAINER_SIZE, JobRestriction{ 1, 1 });

    const auto jobRestriction   = container.getRestriction(0);
    const auto guildRestriction = container.getRestriction(1);

    bool ok = true;
    ok      = expectBool(std::holds_alternative<JobRestriction>(jobRestriction), true, "job restriction type") && ok;
    ok      = expectUInt(std::get<JobRestriction>(jobRestriction).jobId, 8, "job restriction job id") && ok;
    ok      = expectUInt(std::get<JobRestriction>(jobRestriction).level, 55, "job restriction level") && ok;
    ok      = expectBool(std::holds_alternative<GuildRestriction>(guildRestriction), true, "guild restriction type") && ok;
    ok      = expectUInt(std::get<GuildRestriction>(guildRestriction).guildId, 4, "guild restriction guild id") && ok;
    ok      = expectUInt(std::get<GuildRestriction>(guildRestriction).rank, 1234, "guild restriction rank") && ok;
    ok      = expectBool(std::holds_alternative<std::monostate>(container.getRestriction(CONTAINER_SIZE)), true, "out-of-range restriction setter ignored") && ok;

    container.setSize(2);
    ok = expectUInt(container.getSize(), 2, "shrink size") && ok;
    ok = expectUInt(container.getItemID(1), 0x2002, "shrink preserves retained slot") && ok;
    ok = expectUInt(container.getItemID(2), 0, "shrink makes removed slot out of range") && ok;

    container.setSize(4);
    ok = expectUInt(container.getSize(), 4, "grow size") && ok;
    ok = expectUInt(container.getInvSlotID(2), 0xFF, "grown slot default inventory slot") && ok;
    ok = expectBool(std::holds_alternative<std::monostate>(container.getRestriction(2)), true, "grown slot default restriction") && ok;

    container.Clean();
    ok = expectUInt(container.getSize(), CONTAINER_SIZE, "clean resets size") && ok;
    ok = expectUInt(container.getItemsCount(), 0, "clean resets items count") && ok;
    ok = expectUInt(container.getItemID(0), 0, "clean resets item id") && ok;
    ok = expectUInt(container.getInvSlotID(0), 0xFF, "clean resets inventory slot") && ok;
    ok = expectBool(std::holds_alternative<std::monostate>(container.getRestriction(0)), true, "clean resets restriction") && ok;
    return ok;
}

auto testAccountingMutationBoundaries() -> bool
{
    CTradeContainer container;

    // setItem is both the add and replacement operation. Every in-range call
    // advances the caller-maintained count, even when the slot is occupied or
    // the replacement clears it.
    container.setItem(0, 0x3001, 1, 4);
    container.setItem(0, 0x3002, 2, 9);
    container.setItem(0, 0, 0xFF, 0);

    bool ok = true;
    ok      = expectUInt(container.getItemsCount(), 3, "add replace clear item count") && ok;
    ok      = expectUInt(container.getSlotCount(), 0, "clear removes occupied slot") && ok;
    ok      = expectUInt(container.getTotalQuantity(), 0, "clear removes quantity") && ok;

    // Scalar updates do not maintain m_ItemsCount and clearing only an item ID
    // leaves the independent quantity vector contributing to totals.
    container.setItemsCount(17);
    container.setItem(1, 0x4001, 3, 6);
    container.setItemID(1, 0);
    ok = expectUInt(container.getItemsCount(), 18, "scalar update leaves item count") && ok;
    ok = expectUInt(container.getSlotCount(), 0, "item id clear changes occupancy") && ok;
    ok = expectUInt(container.getItemQuantity(0), 6, "zero item id quantity accounting") && ok;
    ok = expectUInt(container.getTotalQuantity(), 6, "cleared item id retains quantity accounting") && ok;

    // Native unsigned arithmetic wraps rather than saturating.
    container.Clean();
    container.setItemsCount(0xFF);
    container.setItem(0, 0x5001, 4, 1);
    ok = expectUInt(container.getItemsCount(), 0, "item count uint8 wrap") && ok;

    container.setItem(0, 0x5002, 4, UINT32_MAX);
    container.setItem(1, 0x5002, 5, 2);
    ok = expectUInt(container.getItemQuantity(0x5002), 1, "item quantity uint32 wrap") && ok;
    ok = expectUInt(container.getTotalQuantity(), 1, "total quantity uint32 wrap") && ok;

    // Gil contributes one to the total regardless of its stored quantity, but
    // per-item lookup still returns and wraps the stored quantities.
    container.setItem(2, 0xFFFF, 6, UINT32_MAX);
    container.setItem(3, 0xFFFF, 7, 2);
    ok = expectUInt(container.getItemQuantity(0xFFFF), 1, "gil quantity lookup uint32 wrap") && ok;
    ok = expectUInt(container.getTotalQuantity(), 3, "duplicate gil slots count independently") && ok;

    // Resizing storage never reconciles the separately maintained count.
    container.setItemsCount(91);
    container.setSize(1);
    ok = expectUInt(container.getItemsCount(), 91, "shrink leaves item count") && ok;
    ok = expectUInt(container.getSlotCount(), 1, "shrink retains occupied slot") && ok;

    container.Clean();
    container.setSize(UINT8_MAX);
    for (std::uint16_t slotID = 0; slotID < UINT8_MAX; ++slotID)
    {
        container.setItemID(static_cast<std::uint8_t>(slotID), 1);
    }
    ok = expectUInt(container.getSlotCount(), UINT8_MAX, "maximum slot count") && ok;
    return ok;
}

auto testUnreserveUnconfirmed() -> bool
{
    CTradeContainer container;
    CItem confirmedItem(0x6001);
    CItem unconfirmedItem(0x6002);

    confirmedItem.setStackSize(99);
    confirmedItem.setQuantity(10);
    unconfirmedItem.setStackSize(99);
    unconfirmedItem.setQuantity(7);

    // The pointer overload is independent from the scalar vectors used by
    // the trade packet. Confirmation reads the pointed item's quantity.
    container.setItem(0, &confirmedItem);
    container.setItemID(0, 0);
    container.setQuantity(0, 1);
    if (!container.setConfirmedStatus(0, 4))
    {
        std::cerr << "trade container self-test failed: confirm item" << '\n';
        return false;
    }
    confirmedItem.setReserve(1);

    container.setItem(1, &unconfirmedItem);
    container.setItemID(1, 0x6002);
    unconfirmedItem.setReserve(7);

    container.unreserveUnconfirmed();

    bool ok = true;
    ok      = expectUInt(confirmedItem.getReserve(), 4, "confirmed reserve restored") && ok;
    ok      = expectUInt(unconfirmedItem.getReserve(), 0, "unconfirmed reserve cleared") && ok;

    CItem outsideFixedRange(0x6003);
    outsideFixedRange.setStackSize(99);
    outsideFixedRange.setQuantity(6);
    outsideFixedRange.setReserve(6);
    container.setSize(CONTAINER_SIZE + 1);
    container.setItem(CONTAINER_SIZE, &outsideFixedRange);
    container.unreserveUnconfirmed();
    ok = expectUInt(outsideFixedRange.getReserve(), 6, "slot outside fixed scan unchanged") && ok;
    return ok;
}

} // namespace

auto runTradeContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultStateAndBounds() && ok;
    ok      = testScalarSlotsAndTotals() && ok;
    ok      = testRestrictionsResizeAndClean() && ok;
    ok      = testAccountingMutationBoundaries() && ok;
    ok      = testUnreserveUnconfirmed() && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("trade-container", runTradeContainerSelfTests);
