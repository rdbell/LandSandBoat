#include "test_delivery_box_transaction_runtime.h"

#include "map/utils/dboxutils.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "delivery box transaction runtime self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDeliveryBoxTransactionRuntimeSelfTests() -> bool
{
    const auto success = dboxutils::detail::SendConfirmationResults(false);
    const auto full    = dboxutils::detail::SendConfirmationResults(true);
    return expect(success.first == 0x02 && success.second == 0x01, "successful send sequence") &&
           expect(full.first == 0x02 && full.second == 0xFE, "capacity send sequence");
}
