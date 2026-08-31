#include "test_player_controller_can_act_8560.h"

#include "map/ai/controllers/player_controller_can_act_capacity.h"
#include "omega_self_test_registry.h"

#include <chrono>
#include <iostream>

auto runPlayerControllerCanAct8560SelfTests() -> bool
{
    using namespace std::chrono_literals;
    using playercontrollercanact::CanAct;

    const auto last = 10000ms;
    const auto at = last + 2500ms;
    const auto after = at + 1ms;
    const auto before = at - 1ms;
    const bool ok = !CanAct(before, last) && !CanAct(at, last) && CanAct(after, last) &&
                    CanAct(100000ms, 0ms);
    if (!ok)
    {
        std::cerr << "player controller can-act 8560 self-test failed\n";
    }
    return ok;
}

OMEGA_REGISTER_SELF_TEST("player-controller-can-act-8560", runPlayerControllerCanAct8560SelfTests);
