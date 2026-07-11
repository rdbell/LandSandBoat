#pragma once

namespace automatondeathhelpers
{

template <typename RemoveManeuvers, typename DieAsPet>
inline void Apply(const bool hasMaster, RemoveManeuvers&& removeManeuvers, DieAsPet&& dieAsPet)
{
    if (hasMaster)
    {
        removeManeuvers();
    }
    dieAsPet();
}

} // namespace automatondeathhelpers
