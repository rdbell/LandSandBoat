#pragma once

// Pure CDeathState construction side-effect gates (slice 6295).
//
// Production ctor order (death_state.cpp):
//   1. DelStatusEffectsByFlag(Death, Silent)   // host
//   2. animation = ANIMATION_DEATH             // host
//   3. updatemask |= UPDATE_HP                 // host
//   4. if PathFind present → PathFind->Clear() // gated by shouldClearPathFind
//
// Status wipe, animation, and updatemask remain host-owned applications.
// This header provides the PathFind clear predicate plus an ordered inject
// surface so LSB and Go share one construction checklist.

namespace deathentry
{
// PathFind clear predicate: if PAI->PathFind is present → Clear().
constexpr auto shouldClearPathFind(const bool hasPathFind) -> bool
{
    return hasPathFind;
}

// Apply CDeathState construction side-effects in production order via host injects.
// clearPathFind is invoked only when shouldClearPathFind(hasPathFind) is true.
template <typename DelDeathStatus, typename SetAnimationDeath, typename OrUpdateHP, typename ClearPathFind>
inline void ApplyConstructionEffects(DelDeathStatus&& delDeathStatusSilent,
                                     SetAnimationDeath&& setAnimationDeath,
                                     OrUpdateHP&& orUpdateHP,
                                     const bool hasPathFind,
                                     ClearPathFind&& clearPathFind)
{
    delDeathStatusSilent();
    setAnimationDeath();
    orUpdateHP();
    if (shouldClearPathFind(hasPathFind))
    {
        clearPathFind();
    }
}
} // namespace deathentry
