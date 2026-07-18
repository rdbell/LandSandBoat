#include "test_targetfind_master_3923.h"

#include "map/ai/helpers/targetfind_master_capacity.h"

#include <iostream>

auto runTargetfindMaster3923SelfTests() -> bool
{
    using targetfindmasterhelpers::ClassifyMasteredTarget;
    using targetfindmasterhelpers::MasteredTargetDecision;

    const bool ok = ClassifyMasteredTarget(false, FIND_TYPE::MONSTER_PLAYER, true, true, false, false) == MasteredTargetDecision::CONTINUE &&
                    ClassifyMasteredTarget(true, FIND_TYPE::MONSTER_PLAYER, true, false, false, false) == MasteredTargetDecision::REJECT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::MONSTER_PLAYER, false, true, false, false) == MasteredTargetDecision::CONTINUE &&
                    ClassifyMasteredTarget(true, FIND_TYPE::PLAYER_MONSTER, false, true, false, false) == MasteredTargetDecision::REJECT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::PLAYER_MONSTER, true, false, false, false) == MasteredTargetDecision::CONTINUE &&
                    ClassifyMasteredTarget(true, FIND_TYPE::MONSTER_MONSTER, false, false, false, false) == MasteredTargetDecision::REJECT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::MONSTER_MONSTER, false, false, true, false) == MasteredTargetDecision::ACCEPT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::MONSTER_MONSTER, false, false, false, true) == MasteredTargetDecision::ACCEPT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::PLAYER_PLAYER, false, false, false, false) == MasteredTargetDecision::REJECT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::PLAYER_PLAYER, false, false, true, false) == MasteredTargetDecision::ACCEPT &&
                    ClassifyMasteredTarget(true, FIND_TYPE::NONE, true, true, false, false) == MasteredTargetDecision::CONTINUE;
    if (!ok)
    {
        std::cerr << "targetfind master 3923 self-test failed\n";
    }
    return ok;
}
