#include "test_trade_total_quantity_2821.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade total quantity 2821 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTradeTotalQuantity2821SelfTests() -> bool
{
    using tradecontainerhelpers::TradeCurrencyItemID;
    using tradecontainerhelpers::TradeSlotTotalContribution;

    bool ok = true;

    ok = expect(TradeCurrencyItemID == 0xFFFF, "TradeCurrencyItemID is 0xFFFF") && ok;

    // Currency sentinel: always 1 regardless of stored quantity.
    ok = expect(TradeSlotTotalContribution(TradeCurrencyItemID, 0) == 1, "currency qty 0") && ok;
    ok = expect(TradeSlotTotalContribution(TradeCurrencyItemID, 1) == 1, "currency qty 1") && ok;
    ok = expect(TradeSlotTotalContribution(TradeCurrencyItemID, 999) == 1, "currency qty 999") && ok;
    ok = expect(TradeSlotTotalContribution(TradeCurrencyItemID, 0xFFFFFFFFu) == 1, "currency qty max") && ok;
    ok = expect(TradeSlotTotalContribution(0xFFFF, 42) == 1, "raw 0xFFFF") && ok;

    // Non-currency: pass through quantity.
    ok = expect(TradeSlotTotalContribution(0, 0) == 0, "empty zero") && ok;
    ok = expect(TradeSlotTotalContribution(0, 6) == 6, "cleared id retains qty") && ok;
    ok = expect(TradeSlotTotalContribution(0x1001, 0) == 0, "normal zero qty") && ok;
    ok = expect(TradeSlotTotalContribution(0x1001, 12) == 12, "normal qty") && ok;
    ok = expect(TradeSlotTotalContribution(0x1002, 0xFFFFFFFFu) == 0xFFFFFFFFu, "normal max qty") && ok;
    ok = expect(TradeSlotTotalContribution(0xFFFE, 3) == 3, "near-sentinel id") && ok;

    return ok;
}
