#pragma once

#include <cstdint>
#include <functional>

namespace charitemfinishpreflighthelpers
{

template <typename WarnInvalidItem, typename PushFailure, typename ResetTargetFind, typename FindSingleTarget, typename InitializeAction>
inline bool Apply(const bool equipment,
                  const std::uint32_t quantity,
                  const std::uint32_t reserve,
                  const std::uint16_t validTarget,
                  const std::uint32_t actorID,
                  const std::uint16_t itemID,
                  WarnInvalidItem&&   warnInvalidItem,
                  PushFailure&&       pushFailure,
                  ResetTargetFind&&   resetTargetFind,
                  FindSingleTarget&&  findSingleTarget,
                  InitializeAction&&  initializeAction)
{
    if (!equipment && (quantity < 1 || reserve > 0))
    {
        std::invoke(warnInvalidItem);
        std::invoke(pushFailure);
        return false;
    }

    const std::uint8_t findFlags = (validTarget & 0x0020) != 0 ? 1 : 0;
    std::invoke(resetTargetFind);
    if (std::invoke(findSingleTarget, findFlags, validTarget) == 0)
    {
        return false;
    }
    std::invoke(initializeAction, actorID, itemID);
    return true;
}

} // namespace charitemfinishpreflighthelpers
