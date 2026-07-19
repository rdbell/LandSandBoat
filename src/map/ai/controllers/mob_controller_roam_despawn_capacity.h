#pragma once

namespace mobcontrollerroamdespawn
{
inline auto ShouldDespawn(bool hasDespawnTime, bool elapsed) -> bool
{
    return hasDespawnTime && elapsed;
}
} // namespace mobcontrollerroamdespawn
