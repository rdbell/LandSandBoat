#include "test_item_search_match_2823.h"

#include "map/item_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "item search match 2823 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runItemSearchMatch2823SelfTests() -> bool
{
    using itemcontainerhelpers::MatchesSearchItem;

    bool ok = true;

    // MatchesSearchItem: occupancy + id composition (SearchItem / SearchItems)
    ok = expect(MatchesSearchItem(true, true), "match occupied id hit") && ok;
    ok = expect(!MatchesSearchItem(false, true), "match empty slot") && ok;
    ok = expect(!MatchesSearchItem(true, false), "match id mismatch") && ok;
    ok = expect(!MatchesSearchItem(false, false), "match empty and id fail") && ok;

    return ok;
}
