#pragma once

#include "common/cbasetypes.h"

#include <functional>
#include <utility>

namespace chartickhelpers
{

template <typename TimePoint>
constexpr auto HasDeathTimestamp(const TimePoint deathTimestamp, const TimePoint minimum) -> bool
{
    return deathTimestamp > minimum;
}

template <typename TimePoint>
constexpr auto DeathSyncDue(
    const bool hasDeathTimestamp,
    const TimePoint tick,
    const TimePoint nextDeathSync) -> bool
{
    return hasDeathTimestamp && tick >= nextDeathSync;
}

template <typename TimePoint, typename Duration, typename InMogHouse, typename UpdateGardening>
inline void AfterBase(
    const TimePoint tick,
    const bool      hasDeathTimestamp,
    TimePoint&      nextDeathSync,
    uint8&          updateMask,
    const uint8     statusUpdateBit,
    const Duration  deathSyncFrequency,
    InMogHouse&&    inMogHouse,
    UpdateGardening&& updateGardening)
{
    if (DeathSyncDue(hasDeathTimestamp, tick, nextDeathSync))
    {
        updateMask |= statusUpdateBit;
        nextDeathSync = tick + deathSyncFrequency;
    }

    if (std::invoke(std::forward<InMogHouse>(inMogHouse)))
    {
        std::invoke(std::forward<UpdateGardening>(updateGardening));
    }
}

template <typename TimePoint, typename Duration, typename BaseTick, typename HasDeathTimestamp,
          typename InMogHouse, typename UpdateGardening>
inline void Apply(
    const TimePoint tick,
    TimePoint&      nextDeathSync,
    uint8&          updateMask,
    const uint8     statusUpdateBit,
    const Duration  deathSyncFrequency,
    BaseTick&&      baseTick,
    HasDeathTimestamp&& hasDeathTimestamp,
    InMogHouse&&    inMogHouse,
    UpdateGardening&& updateGardening)
{
    std::invoke(std::forward<BaseTick>(baseTick));
    AfterBase(
        tick,
        std::invoke(std::forward<HasDeathTimestamp>(hasDeathTimestamp)),
        nextDeathSync,
        updateMask,
        statusUpdateBit,
        deathSyncFrequency,
        std::forward<InMogHouse>(inMogHouse),
        std::forward<UpdateGardening>(updateGardening));
}

} // namespace chartickhelpers
