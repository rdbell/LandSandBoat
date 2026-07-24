#pragma once

namespace despawnrespawnregistration
{
inline auto shouldRegister(const bool isMob, const bool allowRespawn, const bool hasZone) -> bool
{
    return isMob && allowRespawn && hasZone;
}
} // namespace despawnrespawnregistration
