#pragma once

#include <algorithm>
#include <cstdint>

namespace mobcontrollerroampathrandomness
{
// Calculate returns the maximum randomized delay after a completed roam path.
constexpr auto Calculate(const std::int16_t roamCooldown, const float roamRate) -> std::uint32_t
{
    return std::clamp<std::uint32_t>(static_cast<std::uint16_t>(roamCooldown * 1000 / roamRate), 0, 120 * 1000);
}
} // namespace mobcontrollerroampathrandomness
