#include "test_trade_setitem_entry_2812.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade setitem entry 2812 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTradeSetItemEntry2812SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry;
    using tradecontainerhelpers::ShouldSetTradeItemEntry;

    bool ok = true;

    // ShouldSetTradeItemEntry: identity of slotInRange
    ok = expect(ShouldSetTradeItemEntry(true), "admit in range") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "reject out of range") && ok;

    // ShouldBumpItemsCountOnSetEntry: always bump when admitted (parity quirk)
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "bump when in range") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "no bump out of range") && ok;

    // Admitted path always bumps (replace and clear included) — both gates true
    // when slotInRange is true; neither when false.
    ok = expect(ShouldSetTradeItemEntry(true) && ShouldBumpItemsCountOnSetEntry(true),
                "admit implies bump") &&
         ok;
    ok = expect(!ShouldSetTradeItemEntry(false) && !ShouldBumpItemsCountOnSetEntry(false),
                "reject implies no bump") &&
         ok;

    return ok;
}
