#include "test_item_free_slots_2826.h"

#include "map/item_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "item free slots 2826 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runItemFreeSlots2826SelfTests() -> bool
{
    using itemcontainerhelpers::FreeSlotsCount;

    bool ok = true;

    ok = expect(FreeSlotsCount(0, 0) == 0, "empty zero") && ok;
    ok = expect(FreeSlotsCount(10, 0) == 10, "full free") && ok;
    ok = expect(FreeSlotsCount(10, 3) == 7, "partial") && ok;
    ok = expect(FreeSlotsCount(10, 10) == 0, "full occupied") && ok;
    ok = expect(FreeSlotsCount(80, 80) == 0, "max inventory full") && ok;
    ok = expect(FreeSlotsCount(120, 1) == 119, "max size nearly empty") && ok;

    // Wrap: count > size underflows uint8 subtraction.
    ok = expect(FreeSlotsCount(0, 1) == 255, "wrap count one over zero") && ok;
    ok = expect(FreeSlotsCount(5, 6) == 255, "wrap count one over size") && ok;
    ok = expect(FreeSlotsCount(0, 255) == 1, "wrap max count") && ok;
    ok = expect(FreeSlotsCount(1, 255) == 2, "wrap large count over one") && ok;
    ok = expect(FreeSlotsCount(255, 0) == 255, "size max free") && ok;
    ok = expect(FreeSlotsCount(255, 255) == 0, "size max full") && ok;

    return ok;
}
