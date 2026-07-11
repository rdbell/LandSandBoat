#pragma once

#include "common/cbasetypes.h"

namespace attackentryhelpers
{

enum class AttackEntryInterrupt : uint8
{
    None,
    Paralyzed,
    Intimidated,
};

// ApplyInitialEffects preserves the mandatory claim-before-LastAttacked order.
template <typename ClaimTarget, typename UpdateLastAttacked>
inline void ApplyInitialEffects(ClaimTarget&& claimTarget, UpdateLastAttacked&& updateLastAttacked)
{
    claimTarget();
    updateLastAttacked();
}

// ResolveInterrupt checks paralysis first and invokes the intimidation check
// only when paralysis did not interrupt the attack.
template <typename IntimidationCheck>
inline auto ResolveInterrupt(const bool isParalyzed, IntimidationCheck&& intimidationCheck) -> AttackEntryInterrupt
{
    if (isParalyzed)
    {
        return AttackEntryInterrupt::Paralyzed;
    }
    if (intimidationCheck())
    {
        return AttackEntryInterrupt::Intimidated;
    }
    return AttackEntryInterrupt::None;
}

} // namespace attackentryhelpers
