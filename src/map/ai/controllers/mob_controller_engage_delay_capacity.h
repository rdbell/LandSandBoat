#pragma once

#include <chrono>

namespace mobcontrollerengagedelay
{
// ScheduleMagic returns the first magic-action time after a successful engage.
constexpr auto ScheduleMagic(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::seconds cooldown,
    const std::chrono::seconds randomDelay) -> std::chrono::steady_clock::time_point
{
    return tick + cooldown + randomDelay;
}

// ScheduleSpecial returns the last special-action time after a successful engage.
constexpr auto ScheduleSpecial(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::seconds cooldown,
    const std::chrono::seconds randomDelay) -> std::chrono::steady_clock::time_point
{
    return tick - cooldown - randomDelay;
}
} // namespace mobcontrollerengagedelay
