#include "test_playtime_save_plan_6867.h"
#include "map/char_playtime_save_plan.h"
#include <iostream>
auto runPlayTimeSavePlan6867SelfTests() -> bool
{
    bool ok = true;
    ok = playtimesavehelpers::MakePlayTimeSavePlan(900, false) == playtimesavehelpers::PlayTimeSavePlan{ .persistPlayTime = 900 } && ok;
    ok = playtimesavehelpers::MakePlayTimeSavePlan(863999, true) == playtimesavehelpers::PlayTimeSavePlan{ .persistPlayTime = 863999 } && ok;
    ok = playtimesavehelpers::MakePlayTimeSavePlan(864000, true) == playtimesavehelpers::PlayTimeSavePlan{ .persistPlayTime = 864000, .clearNewAdventurer = true, .setUpdateHP = true, .savePlayerSettings = true } && ok;
    ok = playtimesavehelpers::MakePlayTimeSavePlan((std::int64_t{ 1 } << 32) + 9, false).persistPlayTime == 9 && ok;
    if (!ok) std::cerr << "playtime save plan 6867 self-test failed\n";
    return ok;
}
