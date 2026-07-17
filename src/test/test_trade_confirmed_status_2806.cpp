#include "test_trade_confirmed_status_2806.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade confirmed status 2806 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTradeConfirmedStatus2806SelfTests() -> bool
{
    using tradecontainerhelpers::ConfirmedStatusAmount;
    using tradecontainerhelpers::ShouldAllowSetConfirmedStatus;

    bool ok = true;

    // ShouldAllowSetConfirmedStatus: slotInRange && itemNonNull && quantityGteAmount
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "allow all true") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, true), "reject out of range") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, true), "reject null item") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, false), "reject quantity low") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, false, false), "reject all false") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, false, true), "reject range+null") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, false), "reject null+qty") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, false), "reject range+qty") && ok;

    // ConfirmedStatusAmount: min(amount, itemQuantity)
    ok = expect(ConfirmedStatusAmount(0, 0) == 0, "min zero/zero") && ok;
    ok = expect(ConfirmedStatusAmount(1, 5) == 1, "amount below quantity") && ok;
    ok = expect(ConfirmedStatusAmount(5, 5) == 5, "amount equal quantity") && ok;
    ok = expect(ConfirmedStatusAmount(6, 5) == 5, "amount above quantity") && ok;
    ok = expect(ConfirmedStatusAmount(0, 100) == 0, "zero amount") && ok;
    ok = expect(ConfirmedStatusAmount(100, 0) == 0, "zero quantity") && ok;
    ok = expect(ConfirmedStatusAmount(0xFFFFFFFFu, 1) == 1, "large amount clamps") && ok;
    ok = expect(ConfirmedStatusAmount(1, 0xFFFFFFFFu) == 1, "large quantity passes amount") && ok;

    return ok;
}
