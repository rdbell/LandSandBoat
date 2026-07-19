#pragma once

namespace automatoncontrollerenfeeblegate
{
template <typename TimeValue, typename Cooldown>
inline auto CanEnfeeble(TimeValue now, TimeValue lastEnfeeble, Cooldown cooldown, bool hasMaster) -> bool
{
    return hasMaster && cooldown > 0s && now > lastEnfeeble + cooldown;
}
} // namespace automatoncontrollerenfeeblegate
