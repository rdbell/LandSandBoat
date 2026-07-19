#pragma once

namespace trustcontrollerrangedattackdispatch
{
struct Plan
{
    bool handled;
    bool updateCooldown;
};

// Resolve preserves ranged attack's handled and cooldown-update outcomes.
template <typename CanChangeState, typename DispatchAttack>
constexpr auto Resolve(const bool admitted, CanChangeState&& canChangeState, DispatchAttack&& dispatchAttack) -> Plan
{
    if (!admitted)
    {
        return { false, false };
    }
    return { true, canChangeState() && dispatchAttack() };
}
} // namespace trustcontrollerrangedattackdispatch
