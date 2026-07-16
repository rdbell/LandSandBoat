#include "test_c2s_trade_req_runtime.h"

#include "map/packets/c2s/0x032_trade_req.h"

namespace
{
using tradereqhelpers::Facts;
using tradereqhelpers::Plan;

constexpr auto plan(Facts facts) -> Plan
{
    return tradereqhelpers::makePlan(facts);
}
} // namespace

auto runC2STradeReqRuntimeSelfTests() -> bool
{
    Facts ready{};
    ready.targetMatches       = true;
    ready.targetContainerEmpty = true;

    return plan({}) == Plan::Ignore &&
           plan(Facts{ .targetMatches = true, .eitherInPrison = true, .targetContainerEmpty = true }) == Plan::Reject &&
           plan(Facts{ .targetMatches = true, .eitherCrafting = true, .targetContainerEmpty = true }) == Plan::Reject &&
           plan(Facts{ .targetMatches = true, .aidBlocked = true, .targetContainerEmpty = true }) == Plan::RejectAidBlocked &&
           plan(Facts{ .targetMatches = true, .aidBlocked = true, .targetAlreadyPendingSource = true, .targetContainerEmpty = true }) == Plan::RejectAidBlocked &&
           plan(Facts{ .targetMatches = true, .targetAlreadyPendingSource = true, .targetContainerEmpty = true }) == Plan::Ignore &&
           plan(Facts{ .targetMatches = true, .targetContainerEmpty = false }) == Plan::Reject &&
           plan(Facts{ .targetMatches = true, .targetContainerEmpty = true, .targetHasRecentPendingTrade = true }) == Plan::Reject &&
           plan(Facts{ .targetMatches = true, .targetContainerEmpty = true, .targetTradeContainer = true }) == Plan::Reject &&
           plan(Facts{ .targetMatches = true, .targetContainerEmpty = true, .sourceHasPendingTrade = true, .previousTargetMatches = true }) == Plan::CancelPreviousTrade &&
           plan(Facts{ .targetMatches = true, .targetContainerEmpty = true, .sourceHasPendingTrade = true }) == Plan::SendRequest &&
           plan(ready) == Plan::SendRequest;
}
