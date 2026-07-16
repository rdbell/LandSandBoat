#pragma once

namespace mobroamhelpers
{

inline auto CanRoam(const bool scripted, const bool hasMaster, const bool hasSpeed, const bool worm, const bool noMove) -> bool
{
    return !scripted && !hasMaster && (hasSpeed || worm) && !noMove;
}

} // namespace mobroamhelpers
