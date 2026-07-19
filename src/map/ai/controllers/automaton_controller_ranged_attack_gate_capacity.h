#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>

namespace automatoncontrollerrangedattackgate
{
template <typename TimeValue, typename Cooldown>
inline auto CanRangedAttack(TimeValue now, TimeValue lastRanged, Cooldown cooldown, bool sharpshotFrame, bool sharpshotHead, int16_t delayModifier) -> bool
{
    if (!sharpshotFrame || cooldown <= 0s)
    {
        return false;
    }
    const auto minDelay = std::chrono::duration_cast<Cooldown>(sharpshotHead ? std::chrono::seconds(5) : std::chrono::seconds(10));
    return now > lastRanged + std::max(cooldown - std::chrono::seconds(delayModifier), minDelay);
}
} // namespace automatoncontrollerrangedattackgate
