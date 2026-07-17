#include "test_trade_slot_count_2824.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade slot count 2824 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTradeSlotCount2824SelfTests() -> bool
{
    using tradecontainerhelpers::TradeCurrencyItemID;
    using tradecontainerhelpers::TradeSlotCountsTowardSlotCount;

    bool ok = true;
    ok = expect(!TradeSlotCountsTowardSlotCount(0), "zero empty") && ok;
    ok = expect(TradeSlotCountsTowardSlotCount(1), "nonzero occupied") && ok;
    ok = expect(TradeSlotCountsTowardSlotCount(TradeCurrencyItemID), "currency occupied") && ok;
    ok = expect(TradeSlotCountsTowardSlotCount(0xFFFF), "0xFFFF occupied") && ok;
    return ok;
}
