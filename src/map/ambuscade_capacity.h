#pragma once

#include "common/cbasetypes.h"

// Pure Ambuscade helpers shared by dual-wire slices:
//   - 2875: onInstanceTimeUpdate complete-when-no-mobs-alive gate
//
// Production host is Lua under
// scripts/zones/Maquette_Abdhaljs-Legion_B/instances/ambuscade.lua
// (onInstanceTimeUpdate) and scripts/globals/ambuscade.lua. Capacity is for
// future Lua/C++ inject so hosts dual-wire pure free functions instead of
// re-inlining comparisons. Helpers take host-injected scalars only (no
// entity / instance / mob pointers). Side effects (instance:complete,
// currency/KI writeback, setPos) remain host-owned.
//
// Parity: internal/ambuscade complete_instance.go

namespace ambuscadehelpers
{

// ---------------------------------------------------------------------------
// Slice 2875 — onInstanceTimeUpdate complete gate
// ---------------------------------------------------------------------------

// ShouldCompleteInstance mirrors ambuscade.lua onInstanceTimeUpdate:
//   if not mobsStillAlive then instance:complete() end
// anyMobAlive is the host inject for the per-mob isAlive loop result
// (mobsStillAlive). Host still calls instance:complete() after a true gate.
inline auto ShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

} // namespace ambuscadehelpers
