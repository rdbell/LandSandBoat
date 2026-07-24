#pragma once

// Pure CState::HasErrorMsg inject (slice 6324).
// Dual-wire of Go aistate.HasErrorMsg (internal/aistate/error_msg.go).

namespace statehelpers
{

// HasErrorMsg reports whether an error packet is present.
// Mirrors: return m_errorMsg != nullptr;
// Formula (slice 6324): hasErrorMsg
inline auto HasErrorMsg(const bool hasErrorMsg) -> bool
{
    return hasErrorMsg;
}

} // namespace statehelpers
