#pragma once

#include "common/cbasetypes.h"

#include <functional>
#include <utility>

namespace charpersistencehelpers
{

inline void Request(uint8& pending, const uint8 requested)
{
    pending |= requested;
}

template <typename Changes, typename PersistVariables, typename SaveEquip, typename SaveLook,
          typename SavePosition, typename SaveEffects>
inline auto Flush(
    Changes&          changes,
    uint8&            pending,
    const uint8       equipFlag,
    const uint8       positionFlag,
    const uint8       effectsFlag,
    PersistVariables&& persistVariables,
    SaveEquip&&        saveEquip,
    SaveLook&&         saveLook,
    SavePosition&&     savePosition,
    SaveEffects&&      saveEffects) -> bool
{
    bool didPersist = false;

    if (!changes.empty())
    {
        for (const auto& name : changes)
        {
            std::invoke(persistVariables, name);
        }
        changes.clear();
        didPersist = true;
    }

    if (pending == 0)
    {
        return didPersist;
    }
    didPersist = true;

    if ((pending & equipFlag) != 0)
    {
        std::invoke(std::forward<SaveEquip>(saveEquip));
        std::invoke(std::forward<SaveLook>(saveLook));
    }
    if ((pending & positionFlag) != 0)
    {
        std::invoke(std::forward<SavePosition>(savePosition));
    }
    if ((pending & effectsFlag) != 0)
    {
        std::invoke(std::forward<SaveEffects>(saveEffects));
    }

    pending = 0;
    return didPersist;
}

template <typename TimePoint, typename Duration, typename FlushNow>
inline auto FlushAt(
    const TimePoint tick,
    TimePoint&      nextPersistTime,
    const Duration interval,
    FlushNow&&      flushNow) -> bool
{
    if (tick < nextPersistTime || !std::invoke(std::forward<FlushNow>(flushNow)))
    {
        return false;
    }

    nextPersistTime = tick + interval;
    return true;
}

} // namespace charpersistencehelpers
