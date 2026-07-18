#include "test_targetfind_player_3926.h"
#include "map/ai/helpers/targetfind_player_capacity.h"
#include <iostream>
auto runTargetfindPlayer3926SelfTests() -> bool
{
    using targetfindplayerhelpers::IsPlayerTarget;
    const bool ok = !IsPlayerTarget(false, true, true) && IsPlayerTarget(true, true, false) && IsPlayerTarget(true, false, true) && !IsPlayerTarget(true, false, false);
    if (!ok) std::cerr << "targetfind player 3926 self-test failed\n";
    return ok;
}
