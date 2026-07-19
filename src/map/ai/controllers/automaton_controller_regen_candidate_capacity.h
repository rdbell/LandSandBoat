#pragma once

namespace automatoncontrollerregencandidate
{
inline auto CanCastRegen(bool hasTarget, bool hasRegen, bool hasGeoRegen) -> bool
{
    return hasTarget && !hasRegen && !hasGeoRegen;
}
} // namespace automatoncontrollerregencandidate
