#include "test_trade_item_quantity_2830.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade item quantity 2830 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTradeItemQuantity2830SelfTests() -> bool
{
    using tradecontainerhelpers::TradeCurrencyItemID;
    using tradecontainerhelpers::TradeSlotMatchesItemID;

    bool ok = true;

    ok = expect(TradeSlotMatchesItemID(0, 0), "both zero") && ok;
    ok = expect(!TradeSlotMatchesItemID(0, 1), "empty vs nonzero") && ok;
    ok = expect(!TradeSlotMatchesItemID(1, 0), "nonzero vs empty") && ok;
    ok = expect(TradeSlotMatchesItemID(0x1001, 0x1001), "normal match") && ok;
    ok = expect(!TradeSlotMatchesItemID(0x1001, 0x1002), "normal mismatch") && ok;
    ok = expect(TradeSlotMatchesItemID(TradeCurrencyItemID, TradeCurrencyItemID), "currency match") && ok;
    ok = expect(TradeSlotMatchesItemID(TradeCurrencyItemID, 0xFFFF), "currency raw 0xFFFF") && ok;
    ok = expect(!TradeSlotMatchesItemID(TradeCurrencyItemID, 0xFFFE), "currency near-miss") && ok;
    ok = expect(TradeSlotMatchesItemID(0xFFFF, 0xFFFF), "raw 0xFFFF match") && ok;
    ok = expect(!TradeSlotMatchesItemID(0xFFFE, 0xFFFF), "near-sentinel mismatch") && ok;

    return ok;
}
