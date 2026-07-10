#include "test_auction_transaction_runtime.h"

#include "map/utils/auctionutils.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "auction transaction runtime self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAuctionTransactionRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok = expect(auctionutils::detail::AuctionFee(1000, 1, 1.0F, 100) == 11, "percentage fee") && ok;
    ok = expect(auctionutils::detail::AuctionFee(100000, 1, 10.0F, 500) == 500, "maximum fee clamp") && ok;
    ok = expect(auctionutils::detail::TransactionQuantity(false, 12) == 1, "single quantity") && ok;
    ok = expect(auctionutils::detail::TransactionQuantity(true, 12) == 12, "stack quantity") && ok;
    return ok;
}
