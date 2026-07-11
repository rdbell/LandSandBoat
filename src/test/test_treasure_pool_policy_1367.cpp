#include "test_treasure_pool_policy_1367.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure pool policy 1367 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "treasure pool policy 1367 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runTreasurePoolPolicy1367SelfTests() -> bool
{
    using namespace treasurepoolhelpers;
    bool ok = true;

    ok = expect(IsFreeSlotUnset(FreeSlotUnset) && IsFreeSlotUnset(11), "unset") && ok;
    ok = expect(!IsFreeSlotUnset(0) && !IsFreeSlotUnset(9), "set") && ok;
    ok = expect(IsValidFreeSlot(0) && !IsValidFreeSlot(10), "valid") && ok;
    ok = expect(ShouldSkipRareCheck(false, true) && !ShouldSkipRareCheck(true, true), "skip rare") && ok;
    ok = expect(ShouldApplyRareMemberCheck(true, false) && !ShouldApplyRareMemberCheck(true, true), "apply rare") && ok;
    ok = expect(ShouldRejectRareAllHave(true, false) && !ShouldRejectRareAllHave(true, true), "reject all have") && ok;
    ok = expect(CanEvictNonRareNonExclusive(false, false) && !CanEvictNonRareNonExclusive(true, false), "evict nrne") && ok;
    ok = expect(CanEvictNonExclusive(false) && !CanEvictNonExclusive(true), "evict ne") && ok;
    ok = expect(PreferOlderTimestamp(1, 2) && !PreferOlderTimestamp(2, 1), "older") && ok;
    ok = expectEq(DefaultFallbackSlot(), static_cast<uint8>(0), "fallback") && ok;
    ok = expect(ShouldForceCheckOnFullPoolInsert(10) && !ShouldForceCheckOnFullPoolInsert(3), "force full") && ok;
    ok = expect(ShouldAutoResolveSolo(1) && !ShouldAutoResolveSolo(2), "solo") && ok;
    ok = expect(IsSlotOutOfRange(10) && !IsSlotOutOfRange(9), "slot range") && ok;
    ok = expect(CanLotWithInventory(1) && !CanLotWithInventory(0), "inv") && ok;
    ok = expect(CanLotRareItem(true, false) && !CanLotRareItem(true, true), "lot rare") && ok;
    ok = expect(IsHigherLot(50, 10) && !IsHigherLot(10, 50), "higher lot") && ok;
    ok = expect(IsPassedLot(PassedLot) && !IsPassedLot(1), "pass lot") && ok;
    ok = expect(ShouldRejectNullMember(true, false) && ShouldRejectNullMember(false, true), "null member") && ok;
    ok = expect(ShouldRejectNullItem(true) && !ShouldRejectNullItem(false), "null item") && ok;
    ok = expect(ShouldUpdatePoolForChar(false) && !ShouldUpdatePoolForChar(true), "update") && ok;
    ok = expect(ShouldFlushPool(1) && !ShouldFlushPool(0), "flush") && ok;

    bool empty[3] = { false, true, false };
    ok = expectEq(FindFirstEmptySlot(empty, 3), static_cast<uint8>(1), "first empty") && ok;
    bool full[2] = { false, false };
    ok = expect(IsFreeSlotUnset(FindFirstEmptySlot(full, 2)), "no empty") && ok;

    EvictionSlotView slots[3] = {
        { false, true, true, false, 100 },  // rare, stamp 100
        { false, true, false, false, 50 },  // non-rare non-ex, oldest
        { false, true, false, true, 10 },   // exclusive, stamp 10
    };
    ok = expectEq(SelectFreeOrEvictionSlot(FreeSlotUnset, slots, 3), static_cast<uint8>(1), "evict pass1") && ok;
    ok = expectEq(SelectFreeOrEvictionSlot(2, slots, 3), static_cast<uint8>(2), "prefer free") && ok;

    // Pass 2: only exclusive/rare remain for non-ex filter — slot0 rare ok for pass2, slot2 exclusive blocked
    EvictionSlotView slots2[2] = {
        { false, true, true, false, 100 },
        { false, true, false, true, 10 },
    };
    ok = expectEq(SelectFreeOrEvictionSlot(FreeSlotUnset, slots2, 2), static_cast<uint8>(0), "evict pass2") && ok;

    return ok;
}
