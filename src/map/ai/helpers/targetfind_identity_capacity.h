#pragma once

namespace targetfindidentityhelpers
{

// ShouldRejectIdentityOrVisibility mirrors validEntity's post-context gate.
inline auto ShouldRejectIdentityOrVisibility(const bool isSelf,
                                             const bool wrongZone,
                                             const bool untargetable,
                                             const bool invisible) -> bool
{
    return isSelf || wrongZone || untargetable || invisible;
}

} // namespace targetfindidentityhelpers
