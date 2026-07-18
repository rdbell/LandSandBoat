#pragma once

namespace playercontrollerabilitygate
{
enum class Error { None, Unable, Recast };
struct Decision { bool dispatch; Error error; };

constexpr auto Evaluate(
    const bool canAct, const bool canChangeState, const bool abilityExists, const bool hasRecast,
    const bool targetUntargetable) -> Decision
{
    if (!canAct || !canChangeState || !abilityExists) return { false, Error::Unable };
    if (hasRecast) return { false, Error::Recast };
    if (targetUntargetable) return { false, Error::None };
    return { true, Error::None };
}
} // namespace playercontrollerabilitygate
