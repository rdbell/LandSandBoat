#include "test_s2c_bazaar_list_runtime.h"
#include "map/packets/s2c/0x105_bazaar_list.h"
#include <iostream>

auto runS2CBazaarListRuntimeSelfTests() -> bool
{
    bool ok  = true;
    auto nil = bazaarlisthelpers::PlanFor(7, 9, {});
    ok       = nil.slot == 7 && nil.tax == 0 && nil.price == 0 && ok;
    bazaarlisthelpers::ItemFacts normal{ .present = true, .price = 1, .quantity = 2, .itemId = 3 };
    normal.extra[2] = 4;
    auto plain      = bazaarlisthelpers::PlanFor(5, 6, normal);
    ok              = plain.price == 1 && plain.quantity == 2 && plain.tax == 6 && plain.itemId == 3 && plain.attr[2] == 4 && ok;
    for (const auto future : { true, false })
    {
        auto charged = bazaarlisthelpers::PlanFor(1, 2, { .present = true, .chargedUsable = true, .charges = 3, .nextUseFuture = future, .nextUseTimestamp = 0x11223344, .delayTimestamp = 0x55667788 });
        ok           = charged.attr[0] == 1 && charged.attr[1] == 3 && charged.attr[3] == (future ? 0x90 : 0xD0) && charged.attr[4] == 0x44 && charged.attr[7] == 0x11 && charged.attr[8] == 0x88 && charged.attr[11] == 0x55 && ok;
    }
    if (!ok)
        std::cerr << "s2c BAZAAR_LIST runtime self-test failed\n";
    return ok;
}
