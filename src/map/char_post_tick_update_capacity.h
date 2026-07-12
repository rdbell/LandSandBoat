#pragma once

#include "common/cbasetypes.h"

#include <functional>
#include <utility>

namespace charposttickupdatehelpers
{

template <typename TimePoint>
constexpr auto UpdateDue(const uint8 updateMask, const TimePoint now, const TimePoint nextUpdate) -> bool
{
    return updateMask != 0 && now > nextUpdate;
}

template <typename TimePoint, typename Duration, typename HasZone, typename UpdateZone, typename IsCharmed, typename UpdateCharmed, typename PushAllianceHP, typename SendStatus, typename FlushDirtyItems>
inline void Apply(
    const TimePoint   now,
    TimePoint&        nextUpdate,
    uint8&            updateMask,
    bool&             sendServerStatus,
    const uint8       positionUpdateBit,
    const uint8       hpUpdateBit,
    const Duration    updateFrequency,
    const bool        gmHidden,
    HasZone&&         hasZone,
    UpdateZone&&      updateZone,
    IsCharmed&&       isCharmed,
    UpdateCharmed&&   updateCharmed,
    PushAllianceHP&&  pushAllianceHP,
    SendStatus&&      sendStatus,
    FlushDirtyItems&& flushDirtyItems)
{
    if (UpdateDue(updateMask, now, nextUpdate))
    {
        nextUpdate = now + updateFrequency;

        if (std::invoke(std::forward<HasZone>(hasZone)) && !gmHidden)
        {
            std::invoke(std::forward<UpdateZone>(updateZone), updateMask);
        }

        if (std::invoke(std::forward<IsCharmed>(isCharmed)))
        {
            std::invoke(std::forward<UpdateCharmed>(updateCharmed), updateMask);
        }

        if (updateMask & hpUpdateBit)
        {
            std::invoke(std::forward<PushAllianceHP>(pushAllianceHP));
        }

        if ((updateMask ^ positionUpdateBit) || sendServerStatus)
        {
            std::invoke(std::forward<SendStatus>(sendStatus));
        }

        sendServerStatus = false;
        updateMask       = 0;
    }

    std::invoke(std::forward<FlushDirtyItems>(flushDirtyItems));
}

} // namespace charposttickupdatehelpers
