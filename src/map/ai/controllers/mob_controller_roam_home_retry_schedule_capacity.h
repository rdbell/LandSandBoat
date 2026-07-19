#pragma once

#include <chrono>

namespace mobcontrollerroamhomeretryschedule
{
// Schedule returns the last action time that permits a home-path retry after ten seconds.
constexpr auto Schedule(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::seconds roamCooldown) -> std::chrono::steady_clock::time_point
{
    return tick - (roamCooldown + std::chrono::seconds(10));
}
} // namespace mobcontrollerroamhomeretryschedule
