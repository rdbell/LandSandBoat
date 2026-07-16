#include "test_zone_char_update_2656.h"

#include "map/utils/zoneutils.h"

#include <iostream>

namespace
{
struct Candidate { int value; };
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition) { std::cerr << "zone char update 2656 self-test failed: " << label << '\n'; }
    return condition;
}
} // namespace

auto runZoneCharUpdate2656SelfTests() -> bool
{
    Candidate primary{ 1 }, partyA{ 2 }, partyB{ 3 }, tertiary{ 4 };
    bool ok = true;
    auto selection = zoneutils::detail::CharUpdateSelection<Candidate>{};
    zoneutils::detail::ConsiderCharToUpdate(selection, &partyA, 10, 99, 99, 77);
    zoneutils::detail::ConsiderCharToUpdate(selection, &tertiary, 77, 0, 99, 77);
    zoneutils::detail::ConsiderCharToUpdate(selection, &partyB, 11, 99, 99, 77);
    ok = expect(selection.selected() == &partyB, "latest party fallback wins over tertiary") && ok;
    zoneutils::detail::ConsiderCharToUpdate(selection, &primary, 99, 99, 99, 77);
    zoneutils::detail::ConsiderCharToUpdate(selection, &partyA, 12, 99, 99, 77);
    ok = expect(selection.selected() == &primary && selection.secondary == &partyB, "primary wins and stops later updates") && ok;
    selection = {};
    zoneutils::detail::ConsiderCharToUpdate(selection, &tertiary, 77, 99, 99, 77);
    return expect(selection.secondary == &tertiary && selection.tertiary == nullptr, "party match precedes tertiary match") && ok;
}
