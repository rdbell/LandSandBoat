#pragma once

#include <chrono>

namespace automatoncontrollerhealinggate
{
template <typename Tick, typename LastHealTick, typename Cooldown, typename HealingDelay>
inline auto CanTryHeal(bool hasMaster, Tick tick, LastHealTick lastHealTick, Cooldown cooldown, HealingDelay healingDelay) -> bool
{
    return hasMaster && cooldown != Cooldown::zero() && tick > lastHealTick + cooldown - healingDelay;
}
} // namespace automatoncontrollerhealinggate
