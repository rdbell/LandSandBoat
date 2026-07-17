#include "test_linkshell_rank_change_plan_2774.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell rank change plan 2774 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const linkshellhelpers::ChangeMemberRankPlan& plan,
    const linkshellhelpers::ChangeMemberRankPreflight disposition,
    const uint16                                  newItemID,
    const char* const                             label) -> bool
{
    return expect(plan.disposition == disposition && plan.newItemID == newItemID, label);
}

} // namespace

auto runLinkshellRankChangePlan2774SelfTests() -> bool
{
    using namespace linkshellhelpers;

    bool ok = true;

    // Happy path: pearl→sack and sack→pearl with shell-holder requester.
    ok = expectPlan(
             PlanChangeMemberRank(RankChangePearlToSack, LSTYPE_LINKSHELL),
             ChangeMemberRankPreflight::Proceed,
             ITEMID::PEARLSACK,
             "proceed pearl to sack") &&
         ok;
    ok = expectPlan(
             PlanChangeMemberRank(RankChangeSackToPearl, LSTYPE_LINKSHELL),
             ChangeMemberRankPreflight::Proceed,
             ITEMID::LINKPEARL,
             "proceed sack to pearl") &&
         ok;

    // 1) invalid newRank (error return)
    ok = expectPlan(
             PlanChangeMemberRank(1, LSTYPE_LINKSHELL),
             ChangeMemberRankPreflight::RejectInvalidNewRank,
             0,
             "reject new rank 1") &&
         ok;
    ok = expectPlan(
             PlanChangeMemberRank(4, LSTYPE_LINKSHELL),
             ChangeMemberRankPreflight::RejectInvalidNewRank,
             0,
             "reject new rank 4") &&
         ok;
    ok = expectPlan(
             PlanChangeMemberRank(0, LSTYPE_LINKSHELL),
             ChangeMemberRankPreflight::RejectInvalidNewRank,
             0,
             "reject new rank 0") &&
         ok;

    // 2) invalid requester (error return)
    ok = expectPlan(
             PlanChangeMemberRank(RankChangePearlToSack, LSTYPE_PEARLSACK),
             ChangeMemberRankPreflight::RejectInvalidRequester,
             0,
             "reject requester sack") &&
         ok;
    ok = expectPlan(
             PlanChangeMemberRank(RankChangeSackToPearl, LSTYPE_LINKPEARL),
             ChangeMemberRankPreflight::RejectInvalidRequester,
             0,
             "reject requester pearl") &&
         ok;

    // Short-circuit order: invalid newRank wins over invalid requester.
    ok = expectPlan(
             PlanChangeMemberRank(1, LSTYPE_PEARLSACK),
             ChangeMemberRankPreflight::RejectInvalidNewRank,
             0,
             "order: new rank before requester") &&
         ok;

    // Compose existing helpers into the same gates the plan uses.
    ok = expect(IsValidRankChangeNewRank(2) && IsValidRankChangeRequester(LSTYPE_LINKSHELL), "component gates proceed") && ok;
    ok = expect(ResolveRankChangeItemID(2) == ITEMID::PEARLSACK && IsValidRankChangeItemID(ITEMID::PEARLSACK), "component item sack") && ok;
    ok = expect(ResolveRankChangeItemID(3) == ITEMID::LINKPEARL && IsValidRankChangeItemID(ITEMID::LINKPEARL), "component item pearl") && ok;
    ok = expect(!IsValidRankChangeItemID(ITEMID::LINKSHELL), "shell item id invalid for rank change") && ok;

    // Host error text remains available for reject dispositions.
    ok = expect(
             FormatChangeMemberRankError("Bob", 7) ==
                 "CLinkshell::ChangeMemberRank: Invalid rank change request for member 'Bob' in linkshell 7.",
             "host error format") &&
         ok;

    return ok;
}
