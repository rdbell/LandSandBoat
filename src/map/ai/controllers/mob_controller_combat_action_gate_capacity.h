#pragma once

namespace mobcontrollercombatactiongate
{
// CanAct reports whether a combat tick may proceed to movement and actions.
constexpr auto CanAct(const bool inactiveState, const bool canChangeState) -> bool
{
    return !inactiveState && canChangeState;
}
} // namespace mobcontrollercombatactiongate
