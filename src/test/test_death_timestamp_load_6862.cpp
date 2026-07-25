#include "test_death_timestamp_load_6862.h"

#include "map/char_death_timestamp_load.h"

#include <iostream>

auto runDeathTimestampLoad6862SelfTests() -> bool
{
    bool ok = true;

    const auto missing = deathtimestamploadhelpers::MakeDeathTimestampLoadPlan(false, 0, 600, 3600);
    ok = !missing.setDeathTime && !missing.callDie && missing.deathTimeSecondsAgo == 0 && missing.dieRemainingSeconds == 0 && ok;

    const auto alive = deathtimestamploadhelpers::MakeDeathTimestampLoadPlan(true, 1, 600, 3600);
    ok = !alive.setDeathTime && !alive.callDie && alive.deathTimeSecondsAgo == 0 && alive.dieRemainingSeconds == 0 && ok;

    const auto dead = deathtimestamploadhelpers::MakeDeathTimestampLoadPlan(true, 0, 600, 3600);
    ok = dead.setDeathTime && dead.callDie && dead.deathTimeSecondsAgo == 600 && dead.dieRemainingSeconds == 3000 && ok;

    const auto expired = deathtimestamploadhelpers::MakeDeathTimestampLoadPlan(true, 0, 4000, 3600);
    ok = expired.setDeathTime && expired.callDie && expired.deathTimeSecondsAgo == 4000 && expired.dieRemainingSeconds == -400 && ok;

    if (!ok)
    {
        std::cerr << "death timestamp load 6862 self-test failed\\n";
    }
    return ok;
}
