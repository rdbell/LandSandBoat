#pragma once

namespace automatoncontrollerelementalgate
{
template <typename Tick, typename LastElementalTick, typename Cooldown>
inline auto CanTryElemental(bool hasMaster, Tick tick, LastElementalTick lastElementalTick, Cooldown cooldown) -> bool
{
    return hasMaster && cooldown != Cooldown::zero() && tick > lastElementalTick + cooldown;
}
} // namespace automatoncontrollerelementalgate
