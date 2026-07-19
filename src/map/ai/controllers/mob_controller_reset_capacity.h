#pragma once

#include <chrono>

namespace mobcontrollerreset
{
struct State
{
    std::chrono::steady_clock::time_point lastAction;
    bool                                  neutral;
    std::chrono::steady_clock::time_point neutralTime;
    bool                                  clearTarget;
    bool                                  clearFollowTarget;
};

// Resolve returns the controller state established when a mob resets.
constexpr auto Resolve(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::seconds randomRoamCooldown) -> State
{
    return { tick - randomRoamCooldown, true, tick, true, true };
}
} // namespace mobcontrollerreset
