#include "test_c2s_item_transfer_post_trade.h"

#include "map/packets/c2s/0x036_item_transfer.h"

#include <iostream>

auto runC2SItemTransferPostTradeSelfTests() -> bool
{
    using itemtransferhelpers::ShouldForceSynthCriticalFail;

    const bool ok =
        !ShouldForceSynthCriticalFail(false, false) &&
        !ShouldForceSynthCriticalFail(false, true) &&
        !ShouldForceSynthCriticalFail(true, false) &&
        ShouldForceSynthCriticalFail(true, true);
    if (!ok)
    {
        std::cerr << "c2s ITEM_TRANSFER post-trade self-test failed\n";
    }
    return ok;
}
