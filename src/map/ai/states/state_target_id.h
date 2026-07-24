#pragma once

#include <cstdint>

// Pure CState::GetTargetID inject (slice 6329).
// Dual-wire of Go aistate.TargetIDValue (internal/aistate/target_id.go).

namespace statehelpers
{

// TargetID returns the stored target id.
// Mirrors: return m_targid;
// Formula (slice 6329): targid
inline auto TargetID(const uint16_t targid) -> uint16_t
{
    return targid;
}

} // namespace statehelpers
