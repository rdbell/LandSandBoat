#include "test_pathfind_finished_3930.h"

#include "map/ai/helpers/pathfind_finished_capacity.h"

#include <iostream>

auto runPathfindFinished3930SelfTests() -> bool
{
    using pathfindfinishedhelpers::Action;
    using pathfindfinishedhelpers::Resolve;

    const bool ok = Resolve(true, true, true) == Action::NextTurn &&
                    Resolve(false, true, true) == Action::RestartPatrol &&
                    Resolve(false, true, false) == Action::Clear &&
                    Resolve(false, false, true) == Action::Clear;
    if (!ok)
    {
        std::cerr << "pathfind FinishedPath 3930 self-test failed\n";
    }
    return ok;
}
