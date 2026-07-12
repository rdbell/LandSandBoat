#pragma once

#include <functional>
#include <utility>

namespace chareventskiphelpers
{

template <typename IsInEvent, typename IsLocked, typename CanSkip,
          typename SendSkipped, typename SendCancel, typename ClearSubstate,
          typename InterruptText, typename SendInterrupt, typename EndCurrent>
inline bool Skip(
    IsInEvent&&      isInEvent,
    IsLocked&&       isLocked,
    CanSkip&&        canSkip,
    SendSkipped&&    sendSkipped,
    SendCancel&&     sendCancel,
    ClearSubstate&&  clearSubstate,
    InterruptText&&  interruptText,
    SendInterrupt&&  sendInterrupt,
    EndCurrent&&     endCurrent)
{
    if (!std::invoke(std::forward<IsInEvent>(isInEvent)) ||
        std::invoke(std::forward<IsLocked>(isLocked)) ||
        !std::invoke(std::forward<CanSkip>(canSkip)))
    {
        return false;
    }

    std::invoke(std::forward<SendSkipped>(sendSkipped));
    std::invoke(std::forward<SendCancel>(sendCancel));
    std::invoke(std::forward<ClearSubstate>(clearSubstate));

    const auto text = std::invoke(std::forward<InterruptText>(interruptText));
    if (text != 0)
    {
        std::invoke(std::forward<SendInterrupt>(sendInterrupt), text);
    }

    std::invoke(std::forward<EndCurrent>(endCurrent));
    return true;
}

} // namespace chareventskiphelpers
