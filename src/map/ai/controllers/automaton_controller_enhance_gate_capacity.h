#pragma once

namespace automatoncontrollerenhancegate
{
template <typename TimeValue, typename Cooldown>
inline auto CanEnhance(TimeValue now, TimeValue lastEnhance, Cooldown cooldown, bool hasMaster) -> bool
{
    return hasMaster && cooldown > 0s && now > lastEnhance + cooldown;
}
} // namespace automatoncontrollerenhancegate
