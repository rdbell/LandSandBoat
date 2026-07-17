#include "test_item_get_item_range_2831.h"

#include "map/item_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "item get item range 2831 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runItemGetItemRange2831SelfTests() -> bool
{
    using itemcontainerhelpers::CanInsertAtSlot;
    using itemcontainerhelpers::CanRemoveSlot;
    using itemcontainerhelpers::CanSearchSlotID;

    bool ok = true;

    // CanSearchSlotID: GetItem / search inclusive range slotID <= size
    ok = expect(CanSearchSlotID(0, 0), "slot 0 size 0") && ok;
    ok = expect(CanSearchSlotID(0, 5), "slot 0 included") && ok;
    ok = expect(CanSearchSlotID(5, 5), "at size boundary") && ok;
    ok = expect(!CanSearchSlotID(6, 5), "past size") && ok;
    ok = expect(CanSearchSlotID(120, 120), "max container boundary") && ok;
    ok = expect(!CanSearchSlotID(121, 120), "past max container") && ok;

    // Same predicate as insert/remove range gates (shared slotID <= size).
    ok = expect(CanSearchSlotID(0, 3) == CanInsertAtSlot(0, 3), "search/insert slot 0") && ok;
    ok = expect(CanSearchSlotID(3, 3) == CanRemoveSlot(3, 3), "search/remove boundary") && ok;
    ok = expect(CanSearchSlotID(4, 3) == CanInsertAtSlot(4, 3), "search/insert past size") && ok;

    return ok;
}
