#include "test_c2s_shop_sell_set_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x085_shop_sell_set.h"

auto runC2SShopSellSetRuntimeSelfTests() -> bool
{
    using shopsellsethelpers::Action;
    using shopsellsethelpers::MakePlan;
    using shopsellsethelpers::RuntimeFacts;

    const auto valid = RuntimeFacts{ true, true, 2, 12, 5, 100, 100, false, 0, true, 900 };
    const auto ok    =
        MakePlan({ false, true, 2, 12, 5, 100, 100, false, 0, true, 900 }).action == Action::InvalidGil &&
        MakePlan({ true, false, 2, 12, 5, 100, 100, false, 0, true, 900 }).action == Action::InvalidItem &&
        MakePlan({ true, true, 0, 12, 5, 100, 100, false, 0, true, 900 }).action == Action::InvalidQuantity &&
        MakePlan({ true, true, 13, 12, 13, 100, 100, false, 0, true, 900 }).action == Action::InvalidQuantity &&
        MakePlan({ true, true, 6, 12, 5, 100, 100, false, 0, true, 900 }).action == Action::InsufficientQuantity &&
        MakePlan({ true, true, 2, 12, 5, 100, 101, false, 0, true, 900 }).action == Action::ItemMismatch &&
        MakePlan({ true, true, 2, 12, 5, 100, 100, true, 0, true, 900 }).action == Action::LockedItem &&
        MakePlan({ true, true, 2, 12, 5, 100, 100, false, 1, true, 900 }).action == Action::ReservedItem &&
        MakePlan({ true, true, 2, 12, 5, 100, 100, false, 0, false, 900 }).action == Action::RemoveFailed &&
        MakePlan(valid).action == Action::CompleteSale &&
        MakePlan(valid).saleCost == 1800;
    if (!ok)
    {
        std::cerr << "c2s SHOP_SELL_SET runtime self-test failed\n";
    }
    return ok;
}
