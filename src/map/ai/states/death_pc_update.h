#pragma once

// Pure PC CDeathState::Update ordered gates (slice 6293).
//
// Production TYPE_PC branch (death_state.cpp):
//   if shouldExitAfterRaise(...):  animation/updatemask; return true
//   else if shouldAutoHomepoint(...): Complete(); OnDeathTimer(); return true
//   if shouldOfferRaiseMenu(...): push Raise packet; raiseSent = true
//   return false
//
// Host keeps animation/updatemask, GP_SERV_COMMAND_RES Raise push, and
// OnDeathTimer side-effects. Time comparisons are precomputed by the host
// (strict tick > deadline) so this header stays free of timer includes.
// Non-PC RAISABLE path remains death_raisable_hold.h (slice 6290).

namespace deathpcupdate
{
// Exit after raise: raiseAccepted && completed && tick > accepted+2s.
constexpr auto shouldExitAfterRaise(const bool raiseAccepted, const bool completed, const bool tickAfterAcceptedPlus2s) -> bool
{
    return raiseAccepted && completed && tickAfterAcceptedPlus2s;
}

// Auto-homepoint death timer: tick > entry + deathTime - DESPAWN_TIME_REDUCTION.
// Host precomputes tickAfterDeadline; production then Complete + OnDeathTimer.
constexpr auto shouldAutoHomepoint(const bool tickAfterDeadline) -> bool
{
    return tickAfterDeadline;
}

// Raise-menu offer: tick > raiseTime && !raiseSent && isDead && hasRaise.
// Host pushes GP_SERV_COMMAND_RES Raise and sets raiseSent when true.
constexpr auto shouldOfferRaiseMenu(const bool tickAfterRaiseTime, const bool raiseSent, const bool isDead, const bool hasRaise) -> bool
{
    return tickAfterRaiseTime && !raiseSent && isDead && hasRaise;
}
} // namespace deathpcupdate
