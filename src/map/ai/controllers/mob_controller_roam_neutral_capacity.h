#pragma once

#include <chrono>

namespace mobcontrollerroamneutral
{
// IsNeutral reports whether the post-disengage neutral grace period is active.
constexpr auto IsNeutral(
    const bool canBeNeutral,
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point neutralTime) -> bool
{
    return canBeNeutral && tick <= neutralTime + std::chrono::seconds(10);
}
} // namespace mobcontrollerroamneutral
