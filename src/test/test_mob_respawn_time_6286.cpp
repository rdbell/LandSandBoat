#include "test_mob_respawn_time_6286.h"

#include "map/lua/mob_respawn_time.h"

#include <iostream>

auto runMobRespawnTime6286SelfTests() -> bool
{
    const auto disable = mobrespawntime::plan(0, true, true);
    const auto pending = mobrespawntime::plan(60, false, true);
    const auto spawned = mobrespawntime::plan(60, true, true);
    const auto noZone  = mobrespawntime::plan(60, false, false);
    const auto ok      = disable.disable && !disable.registerPending && !pending.disable && pending.registerPending &&
                    !spawned.registerPending && !noZone.registerPending;
    if (!ok)
    {
        std::cerr << "mob respawn time 6286 self-test failed\n";
    }
    return ok;
}
