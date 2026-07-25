#pragma once

#include <array>

// Fixed per-tick action order after time_server's earth and Vana'diel work.
namespace timeserverticktailhelpers
{

enum class Action
{
    TriggerTimer,
    TransportTimer,
    CheckInstance,
    ProcessLoadQueue,
    OnTimeServerTick,
    ReloadFilewatchList,
    OnModuleTimeServerTick,
};

constexpr auto MakePlan() -> std::array<Action, 7>
{
    return {
        Action::TriggerTimer,
        Action::TransportTimer,
        Action::CheckInstance,
        Action::ProcessLoadQueue,
        Action::OnTimeServerTick,
        Action::ReloadFilewatchList,
        Action::OnModuleTimeServerTick,
    };
}

} // namespace timeserverticktailhelpers
