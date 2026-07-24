#pragma once

// Pure CMobEntity::OnDeathTimer admission (slice 6297).
//
// Production (mob_entity.cpp):
//   raisable := (m_Behavior & BEHAVIOR_RAISABLE) != 0   // host inject
//   if shouldDespawn(raisable): PAI->Despawn()
//
// Completes the non-PC death-timer chain after death_raisable_hold (6290):
// non-raisable mobs Complete then OnDeathTimer → Despawn; raisable mobs hold
// in death and do not despawn from this gate.
//
// Despawn/FadeOut body and full despawn-state attachment remain host-owned.

namespace mobdeathtimer
{
// True when OnDeathTimer should call PAI->Despawn().
// Mirrors: if (!(m_Behavior & BEHAVIOR_RAISABLE)) PAI->Despawn();
constexpr auto shouldDespawn(const bool raisable) -> bool
{
    return !raisable;
}
} // namespace mobdeathtimer
