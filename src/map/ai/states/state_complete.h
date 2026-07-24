#pragma once

// Pure CState::Complete inject (slice 6328).
// Dual-wire of Go aistate.MarkCompleted (internal/aistate/complete.go).

namespace statehelpers
{

// MarkCompleted returns the sticky completion flag written by Complete.
// Mirrors: m_completed = true;
// Formula (slice 6328): true
inline auto MarkCompleted() -> bool
{
    return true;
}

} // namespace statehelpers
