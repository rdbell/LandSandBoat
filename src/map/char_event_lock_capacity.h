#pragma once

#include <functional>
#include <utility>

namespace chareventlockhelpers
{

template <typename EventID>
inline bool IsInEvent(const EventID eventID)
{
    return eventID != static_cast<EventID>(-1);
}

inline bool IsNpcLocked(const bool isInEvent, const bool inSequence)
{
    return isInEvent || inSequence;
}

template <typename Disengage, typename HasPet, typename DisengagePet, typename RelinquishClaim>
inline void SetLocked(
    bool&            currentLocked,
    const bool       locked,
    Disengage&&      disengage,
    HasPet&&         hasPet,
    DisengagePet&&   disengagePet,
    RelinquishClaim&& relinquishClaim)
{
    currentLocked = locked;
    if (!locked)
    {
        return;
    }

    std::invoke(std::forward<Disengage>(disengage));
    if (std::invoke(std::forward<HasPet>(hasPet)))
    {
        std::invoke(std::forward<DisengagePet>(disengagePet));
    }
    std::invoke(std::forward<RelinquishClaim>(relinquishClaim));
}

} // namespace chareventlockhelpers
