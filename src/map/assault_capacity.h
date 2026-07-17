#pragma once

#include "common/cbasetypes.h"

// Pure Assault helpers shared by dual-wire slices:
//   - 2860: InstanceAssault progress auto-complete
//   - 2863: onAssaultUpdate party/alliance proceed
//
// Production hosts are Lua under scripts/globals/assault/container.lua.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / instance / party / settings pointers).
// Side effects (instance:complete, messages, instanceEntry, onEventUpdate)
// remain host-owned.

namespace assaulthelpers
{

// ---------------------------------------------------------------------------
// Slice 2860 — onInstanceProgressUpdate auto-complete
// ---------------------------------------------------------------------------

// ProgressMeetsRequired mirrors progress >= requiredProgress.
inline auto ProgressMeetsRequired(const int32 progress, const int32 requiredProgress) -> bool
{
    return progress >= requiredProgress;
}

// ShouldAutoComplete mirrors InstanceAssault:onInstanceProgressUpdate:
//   if requiredProgress and progress >= requiredProgress and not completed then
//     instance:complete()
//   end
// requiredProgress uses Lua truthy semantics (0 → skip). Host still calls complete().
inline auto ShouldAutoComplete(const int32 requiredProgress, const int32 progress, const bool alreadyCompleted) -> bool
{
    if (requiredProgress == 0 || alreadyCompleted)
    {
        return false;
    }
    return ProgressMeetsRequired(progress, requiredProgress);
}

// ---------------------------------------------------------------------------
// Slice 2863 — onAssaultUpdate party / alliance proceed
// ---------------------------------------------------------------------------

// AllianceBlocked matches checkSoloPartyAlliance() == 2 (full alliance).
// 0 = solo, 1 = party, 2 = alliance.
inline constexpr int32 kAllianceBlocked = 2;

// PartyTooSmallForAssault mirrors the first early-return gate:
//   gmLevel == 0 && partySize < assaultMinimum
// GM (gmLevel != 0) bypasses the minimum. assaultMinimum is
// settings.main.ASSAULT_MINIMUM (default 1; TOAU-era 3).
inline auto PartyTooSmallForAssault(const int32 gmLevel, const int32 partySize, const int32 assaultMinimum) -> bool
{
    return gmLevel == 0 && partySize < assaultMinimum;
}

// IsAllianceBlocked mirrors the second early-return gate:
//   checkSoloPartyAlliance() == 2
inline auto IsAllianceBlocked(const int32 checkSoloPartyAlliance) -> bool
{
    return checkSoloPartyAlliance == kAllianceBlocked;
}

// ShouldProceedAssaultUpdate is the pure free-function form of the post-cap
// proceed decision (inverse of either early-return):
//   !PartyTooSmallForAssault(...) && !IsAllianceBlocked(...)
// When true the host continues to xi.instance.onEventUpdate.
//
// Lua host (onAssaultUpdate after setLocalVar('AssaultCap')):
//   if getGMLevel() == 0 and getPartySize() < ASSAULT_MINIMUM then
//     messageSpecial(...); instanceEntry(npc, 1); return
//   elseif checkSoloPartyAlliance() == 2 then
//     messageText(...); instanceEntry(npc, 1); return
//   end
//   xi.instance.onEventUpdate(...)
inline auto ShouldProceedAssaultUpdate(const int32 gmLevel,
                                       const int32 partySize,
                                       const int32 assaultMinimum,
                                       const int32 checkSoloPartyAlliance) -> bool
{
    if (PartyTooSmallForAssault(gmLevel, partySize, assaultMinimum))
    {
        return false;
    }
    if (IsAllianceBlocked(checkSoloPartyAlliance))
    {
        return false;
    }
    return true;
}

} // namespace assaulthelpers
