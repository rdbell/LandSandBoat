#pragma once

namespace automatonposttickhelpers
{

template <typename PetPostTick, typename Disappeared, typename HasPlayerMaster, typename SendExtendedJobs>
inline void Apply(
    const bool hadUpdateMask,
    PetPostTick&& petPostTick,
    Disappeared&& disappeared,
    HasPlayerMaster&& hasPlayerMaster,
    SendExtendedJobs&& sendExtendedJobs)
{
    petPostTick();
    if (hadUpdateMask && !disappeared() && hasPlayerMaster())
    {
        sendExtendedJobs();
    }
}

} // namespace automatonposttickhelpers
