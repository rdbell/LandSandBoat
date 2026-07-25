#pragma once

#include <algorithm>
#include <cstdint>

// Pure AddPoints policy from charutils.

namespace charpointshelpers
{
// ClampPointTotal mirrors AddPoints' resolved balance update. The widened
// addition keeps the intended [0, max] result defined at integer extremes.
constexpr auto ClampPointTotal(const int32_t current, const int32_t amount, const int32_t max) -> int32_t
{
    if (max <= 0)
    {
        return 0;
    }

    const auto total = static_cast<int64_t>(current) + static_cast<int64_t>(amount);
    return static_cast<int32_t>(std::clamp<int64_t>(total, 0, max));
}

// ShouldAwardUnityAccolades identifies AddPoints' positive Unity-accolade
// branch, whose recursive balance award and notification are host-owned.
constexpr auto ShouldAwardUnityAccolades(const bool isUnityAccolades, const int32_t amount) -> bool
{
    return isUnityAccolades && amount > 0;
}
} // namespace charpointshelpers
