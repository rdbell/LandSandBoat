#pragma once

#include <cstdint>

namespace avatarsfavor
{

// Mirrors the shared gate in removeAvatarsFavorDebuffsFromPet and
// applyAvatarsFavorDebuffsToPet from scripts/globals/avatars_favor.lua.
inline auto ShouldApplyPreSOADebuffs(const uint16_t petId, const bool enableSoA) -> bool
{
    return !enableSoA && ((petId >= 8 && petId <= 16) || petId == 20 || petId == 76);
}

} // namespace avatarsfavor
