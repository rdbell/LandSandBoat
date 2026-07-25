#include "test_char_can_trade_plan_6904.h"
#include "map/char_can_trade_plan.h"
#include <iostream>
auto runCharCanTradePlan6904SelfTests() -> bool
{
    using cantradeplanhelpers::Decision; using cantradeplanhelpers::Facts;
    const bool ok = cantradeplanhelpers::BuildPlan({ .charMonstro = true }) == Decision::RejectMonstrosity && cantradeplanhelpers::BuildPlan({ .targetFreeSlots = 1, .tradeItemCount = 2 }) == Decision::RejectSpace && cantradeplanhelpers::BuildPlan({ .targetFreeSlots = 2, .tradeItemCount = 1, .rareDuplicate = true }) == Decision::RejectRareDuplicate && cantradeplanhelpers::BuildPlan({ .targetFreeSlots = 2, .tradeItemCount = 1 }) == Decision::Allow;
    if (!ok) std::cerr << "can trade plan 6904 self-test failed\n";
    return ok;
}
