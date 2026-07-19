#pragma once

namespace mobcontrollerroamownerengage
{
inline auto ShouldAttempt(bool hasOwner, bool ignoreRoam) -> bool
{
    return hasOwner && !ignoreRoam;
}
} // namespace mobcontrollerroamownerengage
