#include "test_time_server_tick_tail_6928.h"

#include "map/time_server_tick_tail.h"

#include <array>
#include <iostream>

auto runTimeServerTickTail6928SelfTests() -> bool
{
    using timeserverticktailhelpers::Action;

    constexpr auto expected = std::array{
        Action::TriggerTimer,
        Action::TransportTimer,
        Action::CheckInstance,
        Action::ProcessLoadQueue,
        Action::OnTimeServerTick,
        Action::ReloadFilewatchList,
        Action::OnModuleTimeServerTick,
    };
    if (timeserverticktailhelpers::MakePlan() != expected)
    {
        std::cerr << "time server tick tail 6928 self-test failed\n";
        return false;
    }
    return true;
}
