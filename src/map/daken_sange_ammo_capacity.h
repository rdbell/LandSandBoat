#pragma once

#include "common/cbasetypes.h"

namespace dakensangeammohelpers
{

constexpr uint8 AttackTypeDaken = 9;

template <typename HasSange, typename IsCharacter, typename HasShurikenAmmo>
inline auto ShouldRemoveShuriken(
    const uint8 attackType,
    const bool hasStatusContainer,
    HasSange&& hasSange,
    IsCharacter&& isCharacter,
    HasShurikenAmmo&& hasShurikenAmmo) -> bool
{
    if (attackType != AttackTypeDaken || !hasStatusContainer)
    {
        return false;
    }
    if (!hasSange())
    {
        return false;
    }
    if (!isCharacter())
    {
        return false;
    }
    return hasShurikenAmmo();
}

} // namespace dakensangeammohelpers
