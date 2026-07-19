#pragma once

namespace mobcontrollerdeadmasterdespawn
{
inline auto ShouldDespawn(const bool canDespawn, const bool hasMaster, const bool masterAlive) -> bool
{
    return canDespawn && hasMaster && !masterAlive;
}
} // namespace mobcontrollerdeadmasterdespawn
