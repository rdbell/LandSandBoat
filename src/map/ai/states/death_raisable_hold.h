#pragma once

// Pure non-PC CDeathState::Update death-timer RAISABLE hold helpers (slice 6290).
//
// When the death-timer deadline has passed for a non-PC:
//   if isMob && raisable (BEHAVIOR_RAISABLE) → stay in death (no Complete/OnDeathTimer)
//   else → Complete + OnDeathTimer
//
// Outer early exit: completed || !isDead → leave state.

namespace deathraisablehold
{
// Early non-PC gate: IsCompleted() || !isDead() → Update returns true (exit).
constexpr auto shouldExitEarly(const bool completed, const bool isDead) -> bool
{
    return completed || !isDead;
}

// After death-timer deadline: raisable mobs hold death indefinitely until raised.
// Mirrors: if (PMob && (PMob->m_Behavior & BEHAVIOR_RAISABLE)) return false;
constexpr auto shouldHold(const bool isMob, const bool raisable) -> bool
{
    return isMob && raisable;
}

// After death-timer deadline: non-raisable non-PC should Complete + OnDeathTimer.
constexpr auto shouldComplete(const bool isMob, const bool raisable) -> bool
{
    return !shouldHold(isMob, raisable);
}
} // namespace deathraisablehold
