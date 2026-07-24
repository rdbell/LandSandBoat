#pragma once

// Pure CState::WasExitDelayed inject (slice 6326).
// Dual-wire of Go aistate.WasExitDelayedFlag
// (internal/aistate/was_exit_delayed.go).

namespace statehelpers
{

// WasExitDelayed reports whether DelayExitTime has marked the state delayed.
// Mirrors: return m_wasDelayed;
// Formula (slice 6326): wasDelayed
inline auto WasExitDelayed(const bool wasDelayed) -> bool
{
    return wasDelayed;
}

} // namespace statehelpers
