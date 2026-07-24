#pragma once

// Pure CState::IsCompleted inject (slice 6325).
// Dual-wire of Go aistate.IsCompletedFlag (internal/aistate/is_completed.go).

namespace statehelpers
{

// IsCompleted reports whether the state completion flag is set.
// Mirrors: return m_completed;
// Formula (slice 6325): completed
inline auto IsCompleted(const bool completed) -> bool
{
    return completed;
}

} // namespace statehelpers
