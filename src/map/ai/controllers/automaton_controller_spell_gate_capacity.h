#pragma once

#include <chrono>
#include <cstdint>

namespace automatoncontrollerspellgate
{
template <typename TimeValue, typename Cooldown>
inline auto CanCastSpell(TimeValue now, TimeValue lastSpell, Cooldown cooldown, bool hasMaster, bool canCast, int16_t cooldownModifier) -> bool
{
    return hasMaster && cooldown > 0s && canCast && now > lastSpell + cooldown + std::chrono::seconds(cooldownModifier);
}
} // namespace automatoncontrollerspellgate
