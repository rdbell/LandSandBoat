#include "test_c2s_trade_res_runtime.h"

#include <initializer_list>
#include <iostream>

#include "map/packets/c2s/0x033_trade_res.h"

namespace
{

auto hasActions(const tradereshelpers::ActionPlan& plan, std::initializer_list<tradereshelpers::Action> expected) -> bool
{
    if (plan.count != expected.size())
    {
        return false;
    }
    uint8 i = 0;
    for (const auto action : expected)
    {
        if (plan.actions[i++] != action)
        {
            return false;
        }
    }
    return true;
}

} // namespace

auto runC2STradeResRuntimeSelfTests() -> bool
{
    using Action = tradereshelpers::Action;
    auto ready   = tradereshelpers::RuntimeFacts{ true, true, true, true, true, false, false, false, true, true };

    if (!hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Start, ready), { Action::SetInitiatorTradeContainer, Action::NotifyTargetStart, Action::SetTargetTradeContainer, Action::NotifyInitiatorStart }) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Start, { true, true, true, false, true, false, false, false, true, true }), { Action::ClearPendingTargets }) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Cancell, { true, false, false, false, false, false, true, false, false, false }), { Action::CleanTargetTradeContainer, Action::ClearPendingTargets, Action::NotifyTargetCancell }) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Make, ready), { Action::LockInitiatorTradeContainer, Action::NotifyTargetMake }) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Make, { true, true, true, true, true, false, false, true, true, true }), { Action::LockInitiatorTradeContainer, Action::NotifyTargetMake, Action::TradeInitiatorToTarget, Action::NotifyTargetEnd, Action::TradeTargetToInitiator, Action::NotifyInitiatorEnd, Action::CleanInitiatorTradeContainer, Action::CleanTargetTradeContainer, Action::ClearPendingTargets }) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Make, { true, true, true, true, true, false, false, true, true, false }), { Action::LockInitiatorTradeContainer, Action::NotifyTargetMake, Action::NotifyTargetCancell, Action::NotifyInitiatorCancell, Action::CleanInitiatorTradeContainer, Action::CleanTargetTradeContainer, Action::ClearPendingTargets }) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::MakeCancell, ready), {}) ||
        !hasActions(tradereshelpers::BuildActionPlan(GP_CLI_COMMAND_TRADE_RES_KIND::Start, {}), {}))
    {
        std::cerr << "c2s TRADE_RES runtime self-test failed\n";
        return false;
    }
    return true;
}
