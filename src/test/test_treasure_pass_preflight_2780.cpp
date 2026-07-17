#include "test_treasure_pass_preflight_2780.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure pass preflight 2780 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTreasurePassPreflight2780SelfTests() -> bool
{
    using treasurepoolhelpers::PassItemPreflight;
    using treasurepoolhelpers::PlanPassItemPreflight;

    bool ok = true;

    // Happy path: all gates open.
    ok = expect(
            PlanPassItemPreflight(false, false, false) == PassItemPreflight::Proceed,
            "proceed") &&
        ok;

    // 1) null member / pool mismatch (warn return)
    ok = expect(
            PlanPassItemPreflight(true, false, false) == PassItemPreflight::RejectMember,
            "null char") &&
        ok;
    ok = expect(
            PlanPassItemPreflight(false, true, false) == PassItemPreflight::RejectMember,
            "pool mismatch") &&
        ok;

    // 2) slot out of range (silent return)
    ok = expect(
            PlanPassItemPreflight(false, false, true) == PassItemPreflight::RejectSlot,
            "slot out of range") &&
        ok;

    // Short-circuit order: member reject wins when multiple would reject.
    ok = expect(
            PlanPassItemPreflight(true, true, true) == PassItemPreflight::RejectMember,
            "order: member before slot") &&
        ok;
    ok = expect(
            PlanPassItemPreflight(false, true, true) == PassItemPreflight::RejectMember,
            "order: mismatch before slot") &&
        ok;
    ok = expect(
            PlanPassItemPreflight(true, false, true) == PassItemPreflight::RejectMember,
            "order: null before slot") &&
        ok;

    // Compose existing helpers into host inputs for plan.
    ok = expect(treasurepoolhelpers::IsSlotOutOfRange(10), "helper slot 10") && ok;
    ok = expect(!treasurepoolhelpers::IsSlotOutOfRange(9), "helper slot 9") && ok;
    ok = expect(
            PlanPassItemPreflight(
                treasurepoolhelpers::ShouldRejectNullMember(false, false),
                false,
                treasurepoolhelpers::IsSlotOutOfRange(9)) == PassItemPreflight::Proceed,
            "compose helpers proceed") &&
        ok;
    ok = expect(
            PlanPassItemPreflight(false, false, treasurepoolhelpers::IsSlotOutOfRange(10)) ==
                PassItemPreflight::RejectSlot,
            "compose IsSlotOutOfRange") &&
        ok;
    ok = expect(
            PlanPassItemPreflight(
                treasurepoolhelpers::ShouldRejectNullMember(true, false),
                false,
                false) == PassItemPreflight::RejectMember,
            "compose ShouldRejectNullMember") &&
        ok;

    return ok;
}
