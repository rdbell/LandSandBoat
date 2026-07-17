#include "test_item_search_space_2808.h"

#include "map/item_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "item search space 2808 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runItemSearchSpace2808SelfTests() -> bool
{
    using itemcontainerhelpers::HasSpaceForQuantity;
    using itemcontainerhelpers::MatchesSearchItemWithSpace;

    bool ok = true;

    // Exact fit: quantity 5, stack 12, request 7 → 5 <= 12-7 → true
    ok = expect(HasSpaceForQuantity(5, 12, 7), "space exact fit") && ok;
    // Over capacity: 5 <= 12-8 → 5 <= 4 → false
    ok = expect(!HasSpaceForQuantity(5, 12, 8), "space over capacity") && ok;
    // Full stack: 12 <= 12-0 → true
    ok = expect(HasSpaceForQuantity(12, 12, 0), "space zero request full stack") && ok;
    // Empty-ish stack accepting full request: 0 <= 12-12 → true
    ok = expect(HasSpaceForQuantity(0, 12, 12), "space empty accepts full stack") && ok;
    ok = expect(!HasSpaceForQuantity(1, 12, 12), "space one over full stack request") && ok;

    // Wrap edge: requestQuantity > stackSize → uint32 underflow of stackSize - request
    // makes the right-hand side huge, so any quantity matches.
    ok = expect(HasSpaceForQuantity(5, 12, 13), "space wrap request > stack") && ok;
    ok = expect(HasSpaceForQuantity(0, 0, 1), "space wrap zero stack request one") && ok;
    ok = expect(HasSpaceForQuantity(0xFFFFFFFF, 1, 2), "space wrap large quantity") && ok;
    // request == stackSize + quantity when quantity is zero is exact, not wrap:
    // 0 <= 0-0 stays true without wrap
    ok = expect(HasSpaceForQuantity(0, 0, 0), "space zero all") && ok;

    // MatchesSearchItemWithSpace: occupancy + id + space composition
    ok = expect(MatchesSearchItemWithSpace(true, true, 5, 12, 7), "match exact fit") && ok;
    ok = expect(!MatchesSearchItemWithSpace(true, true, 5, 12, 8), "match over capacity") && ok;
    ok = expect(MatchesSearchItemWithSpace(true, true, 5, 12, 13), "match wrap edge") && ok;
    ok = expect(!MatchesSearchItemWithSpace(false, true, 5, 12, 7), "match empty slot") && ok;
    ok = expect(!MatchesSearchItemWithSpace(true, false, 5, 12, 7), "match id mismatch") && ok;
    ok = expect(!MatchesSearchItemWithSpace(false, false, 5, 12, 7), "match empty and id fail") && ok;
    // Empty/id fail even when wrap would pass the space check alone
    ok = expect(!MatchesSearchItemWithSpace(false, true, 5, 12, 13), "match empty ignores wrap") && ok;
    ok = expect(!MatchesSearchItemWithSpace(true, false, 5, 12, 13), "match id fail ignores wrap") && ok;

    return ok;
}
