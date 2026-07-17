#include "test_treasure_lot_preflight_2772.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure lot preflight 2772 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTreasureLotPreflight2772SelfTests() -> bool
{
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;

    bool ok = true;

    // Happy path: all gates open.
    ok = expect(
            PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
            "proceed non-rare") &&
        ok;
    ok = expect(
            PlanLotItemPreflight(false, false, false, false, 5, true, false) == LotItemPreflight::Proceed,
            "proceed rare unowned") &&
        ok;

    // 1) null member / pool mismatch (warn return)
    ok = expect(
            PlanLotItemPreflight(true, false, false, false, 1, false, false) == LotItemPreflight::RejectMember,
            "null char") &&
        ok;
    ok = expect(
            PlanLotItemPreflight(false, true, false, false, 1, false, false) == LotItemPreflight::RejectMember,
            "pool mismatch") &&
        ok;

    // 2) slot out of range (silent return)
    ok = expect(
            PlanLotItemPreflight(false, false, true, false, 1, false, false) == LotItemPreflight::RejectSlot,
            "slot out of range") &&
        ok;

    // 3) null item (warn return)
    ok = expect(
            PlanLotItemPreflight(false, false, false, true, 1, false, false) == LotItemPreflight::RejectItem,
            "null item") &&
        ok;

    // 4) full inventory (error return)
    ok = expect(
            PlanLotItemPreflight(false, false, false, false, 0, false, false) == LotItemPreflight::RejectFullInventory,
            "full inventory") &&
        ok;

    // 5) rare already owned (error return)
    ok = expect(
            PlanLotItemPreflight(false, false, false, false, 1, true, true) == LotItemPreflight::RejectRareOwned,
            "rare owned") &&
        ok;

    // Short-circuit order: earlier gates win when multiple would reject.
    ok = expect(
            PlanLotItemPreflight(true, true, true, true, 0, true, true) == LotItemPreflight::RejectMember,
            "order: member before all") &&
        ok;
    ok = expect(
            PlanLotItemPreflight(false, false, true, true, 0, true, true) == LotItemPreflight::RejectSlot,
            "order: slot before item/inv/rare") &&
        ok;
    ok = expect(
            PlanLotItemPreflight(false, false, false, true, 0, true, true) == LotItemPreflight::RejectItem,
            "order: item before inv/rare") &&
        ok;
    ok = expect(
            PlanLotItemPreflight(false, false, false, false, 0, true, true) == LotItemPreflight::RejectFullInventory,
            "order: inv before rare") &&
        ok;

    // Compose existing helpers into host inputs for plan.
    ok = expect(treasurepoolhelpers::IsSlotOutOfRange(10), "helper slot 10") && ok;
    ok = expect(
            PlanLotItemPreflight(
                treasurepoolhelpers::ShouldRejectNullMember(false, false),
                false,
                treasurepoolhelpers::IsSlotOutOfRange(9),
                treasurepoolhelpers::ShouldRejectNullItem(false),
                2,
                false,
                false) == LotItemPreflight::Proceed,
            "compose helpers proceed") &&
        ok;

    return ok;
}
