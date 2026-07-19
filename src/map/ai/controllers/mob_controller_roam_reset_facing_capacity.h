#pragma once

namespace mobcontrollerroamresetfacing
{
// ShouldReset reports whether a completed roam path restores spawn rotation.
constexpr auto ShouldReset(const bool enabled, const float distanceFromSpawn, const float maxRoamDistance) -> bool
{
    return enabled && distanceFromSpawn <= maxRoamDistance;
}
} // namespace mobcontrollerroamresetfacing
