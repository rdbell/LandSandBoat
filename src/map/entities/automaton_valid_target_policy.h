#pragma once

namespace automatonvalidtargethelpers
{

template <typename PetValidTarget>
inline auto ValidTarget(const bool targetPlayer, const bool initiatorIsSelf, PetValidTarget&& petValidTarget) -> bool
{
    if (targetPlayer && initiatorIsSelf)
    {
        return true;
    }

    return petValidTarget();
}

} // namespace automatonvalidtargethelpers
