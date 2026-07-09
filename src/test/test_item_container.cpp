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

#include "test_item_container.h"

#include "map/item_container.h"
#include "map/items/item.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectItem(const CItem* actual, const CItem* expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto makeStackItem(uint16 id, uint32 stackSize, uint32 quantity) -> std::unique_ptr<CItem>
{
    auto item = std::make_unique<CItem>(id);
    item->setStackSize(stackSize);
    item->setQuantity(quantity);
    return item;
}

struct DestructionProbeItem final : CItem
{
    DestructionProbeItem(bool& destroyed, uint8& slot, uint8& location)
    : CItem(1)
    , destroyed_(destroyed)
    , slot_(slot)
    , location_(location)
    {
    }

    ~DestructionProbeItem() override
    {
        slot_      = getSlotID();
        location_  = getLocationID();
        destroyed_ = true;
    }

    bool&  destroyed_;
    uint8& slot_;
    uint8& location_;
};

auto testDefaultsAndCapacityMutation() -> bool
{
    CItemContainer container(0x0123);

    bool ok = true;
    ok      = expectUInt(container.GetID(), 0x0123, "default id") && ok;
    ok      = expectUInt(container.GetBuff(), 0, "default buff") && ok;
    ok      = expectUInt(container.GetSize(), 0, "default size") && ok;
    ok      = expectUInt(container.GetFreeSlotsCount(), 0, "default free slots") && ok;
    ok      = expectUInt(container.SortingPacket, 0, "default sorting packet") && ok;
    ok      = expectBool(container.LastSortingTime == timer::time_point::min(), true, "default sorting time") && ok;

    ok = expectUInt(container.SetSize(3), 3, "set size") && ok;
    ok = expectUInt(container.AddSize(2), 5, "grow size") && ok;
    ok = expectUInt(container.AddSize(-2), 3, "shrink size") && ok;
    ok = expectUInt(container.SetSize(MAX_CONTAINER_SIZE + 1), ERROR_SLOTID, "reject oversized size") && ok;
    ok = expectUInt(container.GetSize(), 3, "oversized size unchanged") && ok;

    // Pin the uint8 intermediate in AddSize: 3 + -4 wraps to 255 and fails.
    ok = expectUInt(container.AddSize(-4), ERROR_SLOTID, "size underflow sentinel") && ok;
    ok = expectUInt(container.GetSize(), 3, "size underflow unchanged") && ok;

    CItemContainer buffed(7);
    ok = expectUInt(buffed.AddBuff(10), 10, "add buff") && ok;
    ok = expectUInt(buffed.GetBuff(), 10, "stored buff") && ok;
    ok = expectUInt(buffed.AddBuff(-3), 7, "subtract buff") && ok;
    ok = expectUInt(buffed.GetBuff(), 7, "stored reduced buff") && ok;

    // Pin the uint16 compound assignment: zero plus -1 wraps to 65535 before
    // the usable size is clamped to 80.
    CItemContainer wrapped(8);
    ok = expectUInt(wrapped.AddBuff(-1), 80, "buff underflow size") && ok;
    ok = expectUInt(wrapped.GetBuff(), 0xFFFF, "buff underflow value") && ok;
    ok = expectUInt(wrapped.GetSize(), 80, "buff underflow clamp") && ok;
    return ok;
}

auto testInsertionReplacementAndRemoval() -> bool
{
    CItemContainer container(0x0123);
    container.SetSize(3);

    auto first      = std::make_unique<CItem>(100);
    auto firstRaw   = first.get();
    auto second     = std::make_unique<CItem>(200);
    auto secondRaw  = second.get();

    bool ok = true;
    ok      = expectUInt(container.InsertItem(std::move(first)), 1, "first insert slot") && ok;
    ok      = expectUInt(firstRaw->getSlotID(), 1, "first item slot") && ok;
    ok      = expectUInt(firstRaw->getLocationID(), 0x23, "first item truncated location") && ok;
    ok      = expectUInt(container.InsertItem(std::move(second)), 2, "second insert slot") && ok;
    ok      = expectItem(container.GetItem(2), secondRaw, "second item lookup") && ok;

    auto replacement    = std::make_unique<CItem>(201);
    auto replacementRaw = replacement.get();
    ok = expectUInt(container.InsertItem(std::move(replacement), 2), 2, "replacement slot") && ok;
    ok = expectItem(container.GetItem(2), replacementRaw, "replacement item") && ok;
    ok = expectUInt(container.GetFreeSlotsCount(), 1, "replacement preserves count") && ok;

    auto zero    = std::make_unique<CItem>(300);
    auto zeroRaw = zero.get();
    ok = expectUInt(container.InsertItem(std::move(zero), 0), 0, "slot zero insert") && ok;
    ok = expectItem(container.GetItem(0), zeroRaw, "slot zero item") && ok;
    ok = expectUInt(zeroRaw->getSlotID(), 0, "slot zero metadata") && ok;
    ok = expectUInt(zeroRaw->getLocationID(), 0x23, "slot zero location") && ok;
    ok = expectUInt(container.GetFreeSlotsCount(), 1, "slot zero not counted") && ok;

    auto removed = container.RemoveItem(1);
    ok           = expectItem(removed.get(), firstRaw, "removed first item") && ok;
    ok           = expectItem(container.GetItem(1), nullptr, "removed slot empty") && ok;
    ok           = expectUInt(container.GetFreeSlotsCount(), 2, "remove increments free slots") && ok;

    auto third    = std::make_unique<CItem>(400);
    auto thirdRaw = third.get();
    ok = expectUInt(container.InsertItem(std::move(third)), 1, "first-free reinsertion") && ok;
    ok = expectItem(container.GetItem(1), thirdRaw, "first-free item") && ok;

    auto removedZero = container.RemoveItem(0);
    ok               = expectItem(removedZero.get(), zeroRaw, "removed slot zero") && ok;
    ok               = expectUInt(container.GetFreeSlotsCount(), 1, "slot zero removal preserves count") && ok;
    ok               = expectItem(container.RemoveItem(4).get(), nullptr, "out-of-range remove") && ok;
    ok               = expectItem(container.GetItem(4), nullptr, "out-of-range get") && ok;
    return ok;
}

auto testInvalidAndNilItems() -> bool
{
    CItemContainer container(5);
    container.SetSize(2);

    bool  invalidDestroyed = false;
    uint8 invalidSlot      = 0;
    uint8 invalidLocation  = 0;

    bool ok = true;
    ok      = expectUInt(container.InsertItem(std::make_unique<DestructionProbeItem>(invalidDestroyed, invalidSlot, invalidLocation), 3), ERROR_SLOTID, "out-of-range explicit insert") && ok;
    ok      = expectBool(invalidDestroyed, true, "invalid insert consumes item") && ok;
    ok      = expectUInt(invalidSlot, ERROR_SLOTID, "invalid insert keeps slot before destruction") && ok;
    ok      = expectUInt(invalidLocation, ERROR_SLOTID, "invalid insert keeps location before destruction") && ok;
    ok      = expectUInt(container.InsertItem(nullptr), ERROR_SLOTID, "null first-free insert") && ok;

    ok = expectUInt(container.InsertItem(std::make_unique<CItem>(2)), 1, "fill slot one") && ok;
    ok = expectUInt(container.InsertItem(std::make_unique<CItem>(3)), 2, "fill slot two") && ok;
    ok = expectUInt(container.InsertItem(std::make_unique<CItem>(4)), ERROR_SLOTID, "reject full insert") && ok;
    ok = expectUInt(container.SetSize(1), ERROR_SLOTID, "reject shrink below count") && ok;
    ok = expectUInt(container.GetSize(), 2, "rejected shrink keeps size") && ok;
    return ok;
}

auto testCrossContainerMoves() -> bool
{
    CItemContainer source(1);
    CItemContainer destination(9);
    source.SetSize(3);
    destination.SetSize(2);

    auto first     = std::make_unique<CItem>(100);
    auto firstRaw  = first.get();
    auto second    = std::make_unique<CItem>(200);
    auto secondRaw = second.get();
    source.InsertItem(std::move(first));
    source.InsertItem(std::move(second));

    bool ok = true;
    ok      = expectUInt(source.MoveItemTo(1, destination), 1, "move to first free") && ok;
    ok      = expectItem(source.GetItem(1), nullptr, "first move clears source") && ok;
    ok      = expectItem(destination.GetItem(1), firstRaw, "first move destination") && ok;
    ok      = expectUInt(firstRaw->getLocationID(), 9, "first move location") && ok;
    ok      = expectUInt(firstRaw->getSlotID(), 1, "first move slot") && ok;

    ok = expectUInt(source.MoveItemTo(2, destination, 2), 2, "move to explicit slot") && ok;
    ok = expectItem(source.GetItem(2), nullptr, "explicit move clears source") && ok;
    ok = expectItem(destination.GetItem(2), secondRaw, "explicit move destination") && ok;
    ok = expectUInt(secondRaw->getLocationID(), 9, "explicit move location") && ok;
    ok = expectUInt(secondRaw->getSlotID(), 2, "explicit move slot") && ok;

    auto blocked    = std::make_unique<CItem>(300);
    auto blockedRaw = blocked.get();
    source.InsertItem(std::move(blocked), 3);

    ok = expectUInt(source.MoveItemTo(3, destination, 2), ERROR_SLOTID, "occupied move rejected") && ok;
    ok = expectItem(source.GetItem(3), blockedRaw, "occupied move keeps source") && ok;
    ok = expectUInt(blockedRaw->getLocationID(), 1, "occupied move keeps location") && ok;
    ok = expectUInt(blockedRaw->getSlotID(), 3, "occupied move keeps slot") && ok;
    ok = expectUInt(source.MoveItemTo(3, destination), ERROR_SLOTID, "full move rejected") && ok;
    ok = expectItem(source.GetItem(3), blockedRaw, "full move keeps source") && ok;
    ok = expectUInt(source.MoveItemTo(3, destination, 3), ERROR_SLOTID, "out-of-range destination rejected") && ok;
    ok = expectItem(source.GetItem(3), blockedRaw, "range failure keeps source") && ok;
    ok = expectUInt(source.MoveItemTo(0, destination), ERROR_SLOTID, "empty source rejected") && ok;
    return ok;
}

auto testSearchAndUnsignedStackSpace() -> bool
{
    CItemContainer container(4);
    container.SetSize(4);
    container.InsertItem(makeStackItem(77, 12, 5), 0);
    container.InsertItem(makeStackItem(88, 12, 5), 1);
    container.InsertItem(makeStackItem(77, 12, 5), 2);

    bool ok = true;
    ok      = expectUInt(container.SearchItem(77), 0, "search includes slot zero") && ok;
    ok      = expectUInt(container.SearchItem(999), ERROR_SLOTID, "missing search sentinel") && ok;

    const auto slots = container.SearchItems(77);
    ok               = expectUInt(slots.size(), 2, "search all count") && ok;
    if (slots.size() == 2)
    {
        ok = expectUInt(slots[0], 0, "search all first slot") && ok;
        ok = expectUInt(slots[1], 2, "search all second slot") && ok;
    }
    ok = expectUInt(container.SearchItems(999).size(), 0, "missing search all") && ok;

    ok = expectUInt(container.SearchItemWithSpace(77, 7), 0, "exact-fit stack") && ok;
    ok = expectUInt(container.SearchItemWithSpace(77, 8), ERROR_SLOTID, "over-capacity stack") && ok;

    // Pin uint32 subtraction in stackSize - quantity. A quantity larger than
    // stack size wraps and therefore reports the stack as having room.
    ok = expectUInt(container.SearchItemWithSpace(77, 13), 0, "stack subtraction underflow") && ok;
    return ok;
}

auto testForEachAndClearCount() -> bool
{
    CItemContainer container(6);
    container.SetSize(3);
    container.InsertItem(std::make_unique<CItem>(10), 0);
    container.InsertItem(std::make_unique<CItem>(11), 1);
    container.InsertItem(std::make_unique<CItem>(13), 3);

    std::vector<uint16> ids;
    container.ForEachItem([&](CItem* item)
                          {
                              ids.push_back(item->getID());
                          });

    bool ok = true;
    ok      = expectUInt(ids.size(), 3, "foreach count") && ok;
    if (ids.size() == 3)
    {
        ok = expectUInt(ids[0], 10, "foreach slot zero") && ok;
        ok = expectUInt(ids[1], 11, "foreach slot one") && ok;
        ok = expectUInt(ids[2], 13, "foreach slot three") && ok;
    }
    ok = expectUInt(container.GetFreeSlotsCount(), 1, "free slots before clear") && ok;

    container.Clear();
    for (uint8 slot = 0; slot <= container.GetSize(); ++slot)
    {
        ok = expectItem(container.GetItem(slot), nullptr, "cleared item") && ok;
    }

    // CItemContainer::Clear clears ownership but intentionally does not reset
    // m_count. Pin that live behavior for the Go parity port.
    ok = expectUInt(container.GetFreeSlotsCount(), 1, "clear preserves tracked count") && ok;

    std::size_t calls = 0;
    container.ForEachItem([&](CItem*)
                          {
                              ++calls;
                          });
    ok = expectUInt(calls, 0, "foreach after clear") && ok;
    return ok;
}

auto testShrinkAndClearIgnoreHiddenItems() -> bool
{
    CItemContainer container(6);
    container.SetSize(3);
    auto high    = std::make_unique<CItem>(99);
    auto highRaw = high.get();
    container.InsertItem(std::move(high), 3);

    bool ok = true;
    // SetSize validates count rather than the highest occupied slot, so this
    // item becomes temporarily unreachable.
    ok = expectUInt(container.SetSize(1), 1, "shrink with high-slot item") && ok;
    ok = expectItem(container.GetItem(3), nullptr, "hidden item lookup") && ok;
    ok = expectUInt(container.SearchItem(99), ERROR_SLOTID, "hidden item search") && ok;

    // Clear only visits 0..size. Regrowing reveals the untouched high slot,
    // and the original tracked count still accounts for it.
    container.Clear();
    ok = expectUInt(container.SetSize(3), 3, "regrow after hidden clear") && ok;
    ok = expectItem(container.GetItem(3), highRaw, "hidden item survives clear") && ok;
    ok = expectUInt(container.SearchItem(99), 3, "hidden item search after regrow") && ok;
    ok = expectUInt(container.GetFreeSlotsCount(), 2, "hidden item count after regrow") && ok;
    return ok;
}

} // namespace

auto runItemContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultsAndCapacityMutation() && ok;
    ok      = testInsertionReplacementAndRemoval() && ok;
    ok      = testInvalidAndNilItems() && ok;
    ok      = testCrossContainerMoves() && ok;
    ok      = testSearchAndUnsignedStackSpace() && ok;
    ok      = testForEachAndClearCount() && ok;
    ok      = testShrinkAndClearIgnoreHiddenItems() && ok;
    return ok;
}
