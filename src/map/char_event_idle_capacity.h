#pragma once

#include <functional>
#include <utility>

namespace chareventidlehelpers
{

inline bool ShouldDeferStart(const bool isInEvent)
{
    return isInEvent;
}

template <typename MarkPosition, typename MountedPower, typename SetMountedAnimation,
          typename IsDead, typename SetIdleAnimation, typename SendServerStatus>
inline void RestoreIdle(
    MarkPosition&&        markPosition,
    MountedPower&&        mountedPower,
    SetMountedAnimation&& setMountedAnimation,
    IsDead&&              isDead,
    SetIdleAnimation&&    setIdleAnimation,
    SendServerStatus&&    sendServerStatus)
{
    std::invoke(std::forward<MarkPosition>(markPosition));
    auto power = std::invoke(std::forward<MountedPower>(mountedPower));
    if (power)
    {
        std::invoke(std::forward<SetMountedAnimation>(setMountedAnimation), *power);
    }
    else
    {
        std::invoke(std::forward<SetIdleAnimation>(setIdleAnimation), std::invoke(std::forward<IsDead>(isDead)));
    }
    std::invoke(std::forward<SendServerStatus>(sendServerStatus));
}

} // namespace chareventidlehelpers
