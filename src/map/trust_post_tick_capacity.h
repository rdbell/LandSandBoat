#pragma once

#include <chrono>

namespace trustposttickhelpers
{

inline constexpr auto UpdateInterval = std::chrono::milliseconds(250);

template <typename BasePostTick, typename Now, typename HasZone, typename HasUpdateMask,
          typename Disappeared, typename NextUpdate, typename ScheduleNext,
          typename SendEntityUpdate, typename HasMaster, typename HasParty,
          typename HasHPUpdate, typename SendGroupAttrs, typename ClearUpdateMask>
inline void Apply(
    BasePostTick&& basePostTick,
    Now&& now,
    HasZone&& hasZone,
    HasUpdateMask&& hasUpdateMask,
    Disappeared&& disappeared,
    NextUpdate&& nextUpdate,
    ScheduleNext&& scheduleNext,
    SendEntityUpdate&& sendEntityUpdate,
    HasMaster&& hasMaster,
    HasParty&& hasParty,
    HasHPUpdate&& hasHPUpdate,
    SendGroupAttrs&& sendGroupAttrs,
    ClearUpdateMask&& clearUpdateMask)
{
    basePostTick();
    const auto current = now();
    if (!hasZone() || !hasUpdateMask() || disappeared() || current <= nextUpdate())
    {
        return;
    }

    scheduleNext(current + UpdateInterval);
    sendEntityUpdate();
    if (hasMaster() && hasParty() && hasHPUpdate())
    {
        sendGroupAttrs();
    }
    clearUpdateMask();
}

} // namespace trustposttickhelpers
