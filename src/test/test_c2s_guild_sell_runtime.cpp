#include "test_c2s_guild_sell_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0ac_guild_sell.h"

auto runC2SGuildSellRuntimeSelfTests() -> bool
{
    using guildsellhelpers::Action;
    using guildsellhelpers::MakePlan;
    using guildsellhelpers::RuntimeFacts;

    const auto valid = RuntimeFacts{ true, 12, true, true, true, 0x4455, 7, -1, 3, 900 };
    const auto plan  = MakePlan(3, valid);
    const auto ok    =
        MakePlan(3, {}).action == Action::None &&
        MakePlan(13, RuntimeFacts{ true, 12 }).action == Action::RejectOverStack &&
        MakePlan(13, RuntimeFacts{ true, 12 }).trade == static_cast<uint8>(-4) &&
        plan.action == Action::SendScriptResult && plan.stock == 7 && plan.itemNo == 0x4455 && plan.trade == 0xFF &&
        plan.auditSale && plan.auditItemNo == 0x4455 && plan.auditBasePrice == 900 && plan.auditQuantity == 3 &&
        !MakePlan(3, RuntimeFacts{ true, 12, true, true, true, 0x4455, 7, 1, 0, 900 }).auditSale &&
        !MakePlan(3, RuntimeFacts{ true, 12, true, true, false, 0x4455, 7, 1, 3, 900 }).auditSale;
    if (!ok)
    {
        std::cerr << "c2s GUILD_SELL runtime self-test failed\n";
    }
    return ok;
}
