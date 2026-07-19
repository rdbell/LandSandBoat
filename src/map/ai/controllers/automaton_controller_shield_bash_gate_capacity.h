#pragma once

#include <chrono>
#include <cstdint>

namespace automatoncontrollershieldbashgate
{
template <typename TimeValue, typename Cooldown>
inline auto CanUseShieldBash(TimeValue now, TimeValue lastShieldBash, Cooldown cooldown, bool targetInterruptible, int16_t delay) -> bool
{
    return cooldown > 0s && targetInterruptible && now > lastShieldBash + cooldown - std::chrono::seconds(delay);
}
} // namespace automatoncontrollershieldbashgate
