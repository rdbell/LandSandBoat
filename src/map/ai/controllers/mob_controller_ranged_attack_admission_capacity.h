#pragma once

namespace mobcontrollerrangedattackadmission
{
// CanAttempt reports whether a combat tick may dispatch a ranged attack.
constexpr auto CanAttempt(const bool enabled, const bool inRange, const bool cooldownReady, const bool canChangeState) -> bool
{
    return enabled && inRange && cooldownReady && canChangeState;
}
} // namespace mobcontrollerrangedattackadmission
