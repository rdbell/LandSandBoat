#pragma once

#include "trust_controller_roam_formation_capacity.h"

namespace trustcontrollerroammovementdispatch
{

enum class Action : uint8_t
{
    Hold,
    Warp,
    Follow,
    Step,
    Clear,
};

// Resolve selects how a roaming formation plan is dispatched.
template <typename HasSpeed>
inline auto Resolve(const trustcontrollerroamformation::Action formationAction, const bool pathFound, HasSpeed&& hasSpeed) -> Action
{
    switch (formationAction)
    {
        case trustcontrollerroamformation::Action::Declump:
            return pathFound ? Action::Follow : Action::Hold;
        case trustcontrollerroamformation::Action::Path:
            if (pathFound)
            {
                return Action::Follow;
            }
            return hasSpeed() ? Action::Step : Action::Hold;
        case trustcontrollerroamformation::Action::Step:
            return hasSpeed() ? Action::Step : Action::Hold;
        case trustcontrollerroamformation::Action::Warp:
            return Action::Warp;
        case trustcontrollerroamformation::Action::Clear:
            return Action::Clear;
        case trustcontrollerroamformation::Action::None:
            return Action::Hold;
    }

    return Action::Hold;
}

} // namespace trustcontrollerroammovementdispatch
