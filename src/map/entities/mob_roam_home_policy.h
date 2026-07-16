#pragma once

namespace mobroamhomehelpers
{

inline auto CanRoamHome(const bool hasSpeed, const bool worm, const bool noMove, const bool noDespawn, const bool globalNoDespawn, const bool withinHomeDistance) -> bool
{
    if ((!hasSpeed && !worm) || noMove)
    {
        return false;
    }
    return noDespawn || globalNoDespawn || withinHomeDistance;
}

} // namespace mobroamhomehelpers
