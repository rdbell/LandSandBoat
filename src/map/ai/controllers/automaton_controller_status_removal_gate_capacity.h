#pragma once

namespace automatoncontrollerstatusremovalgate
{
template <typename TimeValue, typename Cooldown>
inline auto CanRemoveStatus(TimeValue now, TimeValue lastStatus, Cooldown cooldown, bool hasMaster) -> bool
{
    return hasMaster && cooldown > 0s && now > lastStatus + cooldown;
}
} // namespace automatoncontrollerstatusremovalgate
