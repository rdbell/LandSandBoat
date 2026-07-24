#pragma once

// Pure CInactiveState::Update positive-duration exit (slice 6301).
//
// Production (inactive_state.cpp) after the zero-duration branch (6289):
//   return shouldExit(m_duration > 0ms, tick > GetEntryTime() + m_duration);
//
// Mirrors OmegaXI internal/aistate DurationExpired / InactiveDurationExit:
//   duration > 0 && tick > entry + duration (strict >).
// Host precomputes both sides so this header stays free of timer includes.
//
// Zero-duration status exit (6289), construction InterruptStates (6288),
// Cleanup, and full PAI ChangeState remain out of scope.

namespace inactivedurationexit
{
// Positive-duration tail: durationPositive && tickAfterEntryPlusDuration.
// Equality at the exact deadline does not exit (strict tick >).
constexpr auto shouldExit(const bool durationPositive, const bool tickAfterEntryPlusDuration) -> bool
{
    return durationPositive && tickAfterEntryPlusDuration;
}
} // namespace inactivedurationexit
