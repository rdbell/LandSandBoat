#include "test_char_trade_item_plan_6903.h"
#include "map/char_trade_item_plan.h"
#include <iostream>
auto runCharTradeItemPlan6903SelfTests() -> bool
{
    using tradeitemplanhelpers::Action; using tradeitemplanhelpers::Plan;
    const bool ok = tradeitemplanhelpers::BuildPlan(false, 0, 0) == Plan{} && tradeitemplanhelpers::BuildPlan(true, 1, 1) == Plan{ .actions = { Action::CloneToTarget, Action::ClearReserve, Action::RemoveFromSource, Action::ClearTradeSlot }, .count = 4 } && tradeitemplanhelpers::BuildPlan(true, 99, 4) == Plan{ .actions = { Action::AddToTarget, Action::ClearReserve, Action::RemoveFromSource, Action::ClearTradeSlot }, .count = 4 };
    if (!ok) std::cerr << "trade item plan 6903 self-test failed\n";
    return ok;
}
