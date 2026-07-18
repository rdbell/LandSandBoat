#pragma once

#include <chrono>
#include <cstdint>

namespace playercontrollerabilityrecast
{
constexpr auto RemainingSeconds(
    const std::chrono::steady_clock::time_point timestamp, const std::chrono::steady_clock::time_point now,
    const std::chrono::steady_clock::duration recastTime, const std::chrono::steady_clock::duration chargeTime,
    const uint8_t maxCharges) -> uint32_t
{
    auto remaining = timestamp - now + recastTime;
    if (maxCharges > 1)
    {
        remaining -= chargeTime * (maxCharges - 1);
    }
    if (remaining <= std::chrono::steady_clock::duration::zero())
    {
        return 0;
    }
    return static_cast<uint32_t>(std::chrono::ceil<std::chrono::seconds>(remaining).count());
}
} // namespace playercontrollerabilityrecast
