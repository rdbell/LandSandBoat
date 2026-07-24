#pragma once

#include "common/cbasetypes.h"

// Pure CState::SetTarget update admission (slice 6317).
// Dual-wire of Go aistate.ShouldUpdateTarget (internal/aistate/set_target.go).

namespace statehelpers
{

// ShouldUpdateTarget reports whether SetTarget should assign m_targid and
// call UpdateTarget.
// Mirrors:
//   if (!m_PTarget || _targid != m_targid || (m_PTarget && m_PTarget->targid != _targid))
// Formula:
//   !hasTarget || newTargid != storedTargid || (hasTarget && entityTargid != newTargid)
// hasTarget — m_PTarget != nullptr
// storedTargid — m_targid
// newTargid — SetTarget argument
// entityTargid — m_PTarget->targid when hasTarget; host injects 0 when null
inline auto ShouldUpdateTarget(const bool hasTarget, const uint16 storedTargid, const uint16 newTargid, const uint16 entityTargid) -> bool
{
    return !hasTarget || newTargid != storedTargid || (hasTarget && entityTargid != newTargid);
}

} // namespace statehelpers
