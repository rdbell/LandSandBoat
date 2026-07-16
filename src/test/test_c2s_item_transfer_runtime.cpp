#include "test_c2s_item_transfer_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x036_item_transfer.h"

auto runC2SItemTransferRuntimeSelfTests() -> bool
{
    using itemtransferhelpers::Action;
    using itemtransferhelpers::ItemFacts;
    using itemtransferhelpers::MakePlan;
    using itemtransferhelpers::RuntimeFacts;

    auto valid = RuntimeFacts{ false, true, true, 6.0f, false, true, {}, 1 };
    valid.items[0] = { true, false, false };

    auto partialReservation = valid;
    partialReservation.itemCount = 2;
    partialReservation.items[1] = { false, false, false };

    const auto invisible = MakePlan({ true, true, true, 0.0f, false, true, {}, 0 });
    const auto complete  = MakePlan(valid);
    const auto partial   = MakePlan(partialReservation);

    const auto ok =
        invisible.action == Action::CannotWhileInvisible && invisible.sendCannotWhileInvisible && !invisible.cleanTrade &&
        MakePlan({ false, false, true, 0.0f, false, true, {}, 0 }).action == Action::NoOp &&
        MakePlan({ false, true, false, 0.0f, false, true, {}, 0 }).action == Action::NoOp &&
        MakePlan({ false, true, true, 6.01f, false, true, {}, 0 }).action == Action::NoOp &&
        MakePlan({ false, true, true, 6.0f, true, false, {}, 0 }).action == Action::NoOp &&
        complete.action == Action::BeginTrade && complete.cleanTrade && complete.reservedItemCount == 1 &&
        complete.invokeOnTrade && complete.unreserveUnconfirmed &&
        MakePlan([] { auto facts = RuntimeFacts{ false, true, true, 0.0f, false, true, {}, 1 }; facts.items[0] = { false, false, false }; return facts; }()).action == Action::InvalidItem &&
        MakePlan([] { auto facts = RuntimeFacts{ false, true, true, 0.0f, false, true, {}, 1 }; facts.items[0] = { true, true, false }; return facts; }()).action == Action::ReservedItem &&
        MakePlan([] { auto facts = RuntimeFacts{ false, true, true, 0.0f, false, true, {}, 1 }; facts.items[0] = { true, false, true }; return facts; }()).action == Action::LockedItem &&
        partial.action == Action::InvalidItem && partial.cleanTrade && partial.reservedItemCount == 1 &&
        !partial.invokeOnTrade && !partial.unreserveUnconfirmed &&
        MakePlan([] { auto facts = RuntimeFacts{ false, true, true, 0.0f, false, true, {}, 10 }; for (auto& item : facts.items) { item = { true, false, false }; } return facts; }()).action == Action::InvalidItem;
    if (!ok)
    {
        std::cerr << "c2s ITEM_TRANSFER runtime self-test failed\n";
    }
    return ok;
}
