#pragma once

#include <cstdint>
#include <functional>

// Pure early-gate policy from CCharEntity::OnAbility before claim/valid-target
// and ability execution. Host injects recast/amnesia presence, target-find, and
// battle-message packets.

namespace charabilitypreflighthelpers
{

enum class Result : std::uint8_t
{
    WaitLonger = 0,
    Amnesia,
    NoTarget,
    Proceed,
};

template <typename PushWaitLonger, typename PushAmnesia, typename ResetTargetFind, typename FindSingleTarget>
inline auto Evaluate(const bool hasRecast,
                     const bool hasAmnesia,
                     const std::uint16_t validTarget,
                     PushWaitLonger&& pushWaitLonger,
                     PushAmnesia&& pushAmnesia,
                     ResetTargetFind&& resetTargetFind,
                     FindSingleTarget&& findSingleTarget) -> Result
{
    if (hasRecast)
    {
        std::invoke(pushWaitLonger);
        return Result::WaitLonger;
    }
    if (hasAmnesia)
    {
        std::invoke(pushAmnesia);
        return Result::Amnesia;
    }

    // TARGET_PLAYER_DEAD == 0x0020; FINDFLAGS_DEAD == 1
    const std::uint8_t findFlags = (validTarget & 0x0020) != 0 ? 1 : 0;
    std::invoke(resetTargetFind);
    if (std::invoke(findSingleTarget, findFlags, validTarget) == 0)
    {
        return Result::NoTarget;
    }
    return Result::Proceed;
}

} // namespace charabilitypreflighthelpers
