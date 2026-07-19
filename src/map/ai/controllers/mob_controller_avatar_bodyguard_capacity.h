#pragma once

namespace mobcontrolleravatarbodyguard
{
// ShouldDefend reports whether an idle active avatar defends its master.
// Pet classification and player ownership are evaluated only as needed.
template <typename PetIsPet, typename PetIsAvatar, typename PetIsActiveAvatar, typename TargetOwnsAttacker>
constexpr auto ShouldDefend(
    const bool petHasNoTarget,
    PetIsPet&& petIsPet,
    PetIsAvatar&& petIsAvatar,
    PetIsActiveAvatar&& petIsActiveAvatar,
    const bool targetIsPlayer,
    TargetOwnsAttacker&& targetOwnsAttacker) -> bool
{
    if (!petHasNoTarget || !petIsPet() || !petIsAvatar() || !petIsActiveAvatar())
    {
        return false;
    }
    return !targetIsPlayer || targetOwnsAttacker();
}
} // namespace mobcontrolleravatarbodyguard
