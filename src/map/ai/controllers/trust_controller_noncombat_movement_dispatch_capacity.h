#pragma once

#include <cstdint>

#include "trust_controller_noncombat_movement_capacity.h"

namespace trustcontrollernoncombatmovementdispatch
{

enum class Action : uint8_t
{
    Hold,
    Warp,
    Follow,
    Step,
};

// Resolve selects how a non-combat movement plan is dispatched.
template <typename HasSpeed>
inline auto Resolve(const trustcontrollernoncombatmovement::Action movementAction, const bool pathFound, HasSpeed&& hasSpeed) -> Action
{
    switch (movementAction)
    {
        case trustcontrollernoncombatmovement::Action::Warp:
            return Action::Warp;
        case trustcontrollernoncombatmovement::Action::Path:
            if (pathFound)
            {
                return Action::Follow;
            }
            return hasSpeed() ? Action::Step : Action::Hold;
        case trustcontrollernoncombatmovement::Action::Step:
            return hasSpeed() ? Action::Step : Action::Hold;
        case trustcontrollernoncombatmovement::Action::Hold:
            return Action::Hold;
    }

    return Action::Hold;
}

} // namespace trustcontrollernoncombatmovementdispatch
