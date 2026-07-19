#pragma once

#include <chrono>

namespace mobcontrollerdisengageroamschedule
{
// Schedule sets the last roam action time after disengagement.
constexpr auto Schedule(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::seconds roamCooldown) -> std::chrono::steady_clock::time_point
{
    return tick - roamCooldown + std::chrono::seconds(10);
}
} // namespace mobcontrollerdisengageroamschedule
