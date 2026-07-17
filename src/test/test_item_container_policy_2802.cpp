#include "test_item_container_policy_2802.h"

#include "map/item_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "item container policy 2802 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runItemContainerPolicy2802SelfTests() -> bool
{
    using itemcontainerhelpers::CanInsertAtSlot;
    using itemcontainerhelpers::CanRemoveSlot;
    using itemcontainerhelpers::CanSetSize;
    using itemcontainerhelpers::MoveItemToDisposition;
    using itemcontainerhelpers::PlanMoveItemTo;
    using itemcontainerhelpers::ShouldDecrementCountOnRemove;
    using itemcontainerhelpers::ShouldIncrementCountOnInsertAt;

    bool ok = true;

    // CanSetSize: newSize <= max && newSize >= itemCount
    ok = expect(CanSetSize(10, 120, 0), "set size empty") && ok;
    ok = expect(CanSetSize(10, 120, 10), "set size equal count") && ok;
    ok = expect(CanSetSize(120, 120, 50), "set size at max") && ok;
    ok = expect(!CanSetSize(121, 120, 0), "set size above max") && ok;
    ok = expect(!CanSetSize(5, 120, 6), "set size below count") && ok;
    ok = expect(CanSetSize(0, 120, 0), "set size zero empty") && ok;
    ok = expect(!CanSetSize(0, 120, 1), "set size zero with items") && ok;
    // AddSize host wrap: candidate 255 (underflow) fails max gate
    ok = expect(!CanSetSize(255, 120, 0), "set size wrapped 255") && ok;

    // Count increment on InsertAt: empty && slotID != 0
    ok = expect(ShouldIncrementCountOnInsertAt(true, 1), "inc empty nonzero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 1), "no inc occupied") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(true, 0), "no inc empty zero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 0), "no inc occupied zero") && ok;

    // Count decrement on Remove: occupied && slotID != 0
    ok = expect(ShouldDecrementCountOnRemove(true, 1), "dec occupied nonzero") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 1), "no dec empty") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(true, 0), "no dec occupied zero") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 0), "no dec empty zero") && ok;

    // CanInsertAtSlot / CanRemoveSlot: slotID <= size
    ok = expect(CanInsertAtSlot(0, 0), "insert slot 0 size 0") && ok;
    ok = expect(CanInsertAtSlot(5, 5), "insert at size boundary") && ok;
    ok = expect(!CanInsertAtSlot(6, 5), "insert past size") && ok;
    ok = expect(CanRemoveSlot(0, 3), "remove slot 0") && ok;
    ok = expect(CanRemoveSlot(3, 3), "remove at size boundary") && ok;
    ok = expect(!CanRemoveSlot(4, 3), "remove past size") && ok;
    ok = expect(CanInsertAtSlot(0, 3) == CanRemoveSlot(0, 3), "insert/remove same range") && ok;

    // PlanMoveItemTo: explicit dest
    ok = expect(PlanMoveItemTo(true, 2, 5, false, 0) == MoveItemToDisposition::Allow,
                "move explicit free") &&
        ok;
    ok = expect(PlanMoveItemTo(true, 6, 5, false, 3) == MoveItemToDisposition::RejectOutOfRangeOrOccupied,
                "move explicit out of range") &&
        ok;
    ok = expect(PlanMoveItemTo(true, 2, 5, true, 3) == MoveItemToDisposition::RejectOutOfRangeOrOccupied,
                "move explicit occupied") &&
        ok;
    // out of range wins over occupancy (host may inject false occupancy)
    ok = expect(PlanMoveItemTo(true, 9, 5, true, 0) == MoveItemToDisposition::RejectOutOfRangeOrOccupied,
                "move explicit range before free-slot check") &&
        ok;
    // explicit path ignores free-slot count
    ok = expect(PlanMoveItemTo(true, 1, 5, false, 0) == MoveItemToDisposition::Allow,
                "move explicit free slots ignored") &&
        ok;

    // PlanMoveItemTo: auto dest
    ok = expect(PlanMoveItemTo(false, 0, 5, false, 3) == MoveItemToDisposition::Allow,
                "move auto has free") &&
        ok;
    ok = expect(PlanMoveItemTo(false, 0, 5, false, 0) == MoveItemToDisposition::RejectNoFreeSlots,
                "move auto no free") &&
        ok;
    // auto path ignores dstSlot/occupied
    ok = expect(PlanMoveItemTo(false, 99, 0, true, 1) == MoveItemToDisposition::Allow,
                "move auto ignores explicit fields") &&
        ok;

    return ok;
}
