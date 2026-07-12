#pragma once

#include <functional>
#include <utility>

namespace chareventqueuehelpers
{

template <typename ResetCurrent, typename ResetPreparation, typename Unlock,
          typename ClearZoneCutscene, typename ClearSubstate, typename TryStartNext>
inline void EndCurrent(
    ResetCurrent&&      resetCurrent,
    ResetPreparation&&  resetPreparation,
    Unlock&&            unlock,
    ClearZoneCutscene&& clearZoneCutscene,
    ClearSubstate&&     clearSubstate,
    TryStartNext&&      tryStartNext)
{
    std::invoke(std::forward<ResetCurrent>(resetCurrent));
    std::invoke(std::forward<ResetPreparation>(resetPreparation));
    std::invoke(std::forward<Unlock>(unlock));
    std::invoke(std::forward<ClearZoneCutscene>(clearZoneCutscene));
    std::invoke(std::forward<ClearSubstate>(clearSubstate));
    std::invoke(std::forward<TryStartNext>(tryStartNext));
}

template <typename Queue, typename Event, typename EventID, typename Duplicate,
          typename Enqueue, typename TryStartNext>
inline bool QueueEvent(
    const Queue&   queue,
    Event          event,
    EventID&&      eventID,
    Duplicate&&    duplicate,
    Enqueue&&      enqueue,
    TryStartNext&& tryStartNext)
{
    for (const auto& queuedEvent : queue)
    {
        if (std::invoke(eventID, queuedEvent) == std::invoke(eventID, event))
        {
            std::invoke(std::forward<Duplicate>(duplicate));
            return false;
        }
    }

    std::invoke(std::forward<Enqueue>(enqueue), event);
    std::invoke(std::forward<TryStartNext>(tryStartNext));
    return true;
}

} // namespace chareventqueuehelpers
