#pragma once

#include <chrono>

namespace petposttickhelpers
{

inline constexpr auto UpdateInterval = std::chrono::milliseconds(250);

inline auto ShouldSendUpdate(
    const bool hasZone,
    const bool hasUpdateMask,
    const bool disappeared,
    const bool nowAfterNextUpdate) -> bool
{
    return hasZone && hasUpdateMask && !disappeared && nowAfterNextUpdate;
}

inline auto ShouldSyncMaster(const bool hasMaster, const bool masterPetIsSelf) -> bool
{
    return hasMaster && masterPetIsSelf;
}

template <typename BasePostTick, typename Now, typename HasZone, typename HasUpdateMask,
          typename Disappeared, typename NextUpdate, typename ScheduleNext,
          typename SendEntityUpdate, typename SyncMaster, typename SendMasterSync, typename ClearUpdateMask>
inline void Apply(
    BasePostTick&& basePostTick,
    Now&& now,
    HasZone&& hasZone,
    HasUpdateMask&& hasUpdateMask,
    Disappeared&& disappeared,
    NextUpdate&& nextUpdate,
    ScheduleNext&& scheduleNext,
    SendEntityUpdate&& sendEntityUpdate,
    SyncMaster&& syncMaster,
    SendMasterSync&& sendMasterSync,
    ClearUpdateMask&& clearUpdateMask)
{
    basePostTick();
    const auto current = now();
    if (!ShouldSendUpdate(hasZone(), hasUpdateMask(), disappeared(), current > nextUpdate()))
    {
        return;
    }
    scheduleNext(current + UpdateInterval);
    sendEntityUpdate();
    if (syncMaster())
    {
        sendMasterSync();
    }
    clearUpdateMask();
}

} // namespace petposttickhelpers
