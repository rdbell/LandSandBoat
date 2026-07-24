#include "test_mob_controller_deaggro_respawn_6285.h"

#include "map/ai/controllers/mob_controller_deaggro_respawn.h"

#include <chrono>
#include <iostream>

auto runMobControllerDeaggroRespawn6285SelfTests() -> bool
{
    using namespace std::chrono_literals;

    const auto ok = mobcontrollerdeaggrorespawn::delay() == 60s;
    if (!ok)
    {
        std::cerr << "mob controller deaggro respawn 6285 self-test failed: deaggro delay is 60 seconds\n";
    }
    return ok;
}
