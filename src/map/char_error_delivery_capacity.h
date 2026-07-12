#pragma once

#include <functional>
#include <utility>

namespace charerrordeliveryhelpers
{

template <typename Message, typename IsCharmed, typename PushError>
inline void DeliverError(Message& message, IsCharmed&& isCharmed, PushError&& pushError)
{
    if (message && !std::invoke(std::forward<IsCharmed>(isCharmed)))
    {
        std::invoke(std::forward<PushError>(pushError), std::move(message));
    }
}

template <typename LastErrorTime, typename WeaponDelay, typename Tick, typename SetLastErrorTime>
inline bool AttackError(
    LastErrorTime&&    lastErrorTime,
    WeaponDelay&&      weaponDelay,
    Tick&&             tick,
    SetLastErrorTime&& setLastErrorTime)
{
    const auto deadline = std::invoke(std::forward<LastErrorTime>(lastErrorTime)) +
                          std::invoke(std::forward<WeaponDelay>(weaponDelay));
    if (deadline < std::invoke(tick))
    {
        std::invoke(std::forward<SetLastErrorTime>(setLastErrorTime), std::invoke(std::forward<Tick>(tick)));
        return true;
    }
    return false;
}

} // namespace charerrordeliveryhelpers
