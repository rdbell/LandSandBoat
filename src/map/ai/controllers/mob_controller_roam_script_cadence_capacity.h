#pragma once

#include <chrono>

namespace mobcontrollerroamscriptcadence
{
constexpr auto Cadence = std::chrono::seconds(3);

constexpr auto ShouldRun(
    const std::chrono::steady_clock::time_point now,
    const std::chrono::steady_clock::time_point last) -> bool
{
    return now >= last + Cadence;
}
} // namespace mobcontrollerroamscriptcadence
