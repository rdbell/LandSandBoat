#include "test_pathfind_finish_next_turn_6357.h"

#include "map/ai/helpers/pathfind_finished_capacity.h"
#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind finish next turn 6357 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for FinishedPath HasNextRoamTurn + Resolve compose (6357).
auto runPathfindFinishNextTurn6357SelfTests() -> bool
{
    using pathfindfinishedhelpers::Action;
    using pathfindfinishedhelpers::Resolve;
    using pathfindstatushelpers::AdvancedCurrentTurn;
    using pathfindstatushelpers::HasNextRoamTurn;
    using pathfindstatushelpers::ShouldClearAfterFailedPath;
    using pathfindstatushelpers::ShouldIteratePathPoint;

    bool ok = true;

    ok = expect(!HasNextRoamTurn(0, 0), "next 0/0") && ok;
    ok = expect(HasNextRoamTurn(0, 1), "next 0/1") && ok;
    ok = expect(HasNextRoamTurn(1, 2), "next 1/2") && ok;
    ok = expect(!HasNextRoamTurn(2, 2), "next 2/2") && ok;

    // Compose: after AdvancedCurrentTurn(0)=1 with two turns → NextTurn.
    const int turnAfter = AdvancedCurrentTurn(0);
    ok = expect(turnAfter == 1, "advance turn") && ok;
    ok = expect(HasNextRoamTurn(turnAfter, 2), "has next after advance") && ok;
    ok = expect(Resolve(HasNextRoamTurn(turnAfter, 2), false, true) == Action::NextTurn, "resolve next") && ok;
    ok = expect(Resolve(HasNextRoamTurn(2, 2), true, true) == Action::RestartPatrol, "resolve restart") && ok;
    ok = expect(Resolve(HasNextRoamTurn(2, 2), false, false) == Action::Clear, "resolve clear") && ok;
    ok = expect(ShouldClearAfterFailedPath(false) && !ShouldClearAfterFailedPath(true), "clear after fail") && ok;
    ok = expect(ShouldIteratePathPoint(0, 1), "6356 residual") && ok;

    return ok;
}
