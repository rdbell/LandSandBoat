#pragma once

#include "common/cbasetypes.h"

// Pure Assault helpers shared by dual-wire slices:
//   - 2860: InstanceAssault progress auto-complete (residual dual-wire suite)
//   - 2863: onAssaultUpdate party/alliance proceed
//   - 2867: onRytaalEventFinish obtain new Imperial Army ID tag
//   - 3057: ShouldAutoComplete dedicated dual-wire (auto_complete.go)
//
// Dual-wire index:
//   - 2860: ProgressMeetsRequired / ShouldAutoComplete residual dual-wire
//   - 2863: ShouldProceedAssaultUpdate (party/alliance proceed)
//   - 2867: ShouldIssueNewTag (Rytaal obtain new tag)
//   - 3057: ShouldAutoComplete (requiredProgress==0 || alreadyCompleted → false;
//           else ProgressMeetsRequired on onInstanceProgressUpdate)
//
// Production hosts are Lua under scripts/globals/assault/ (container.lua,
// npc_handler.lua). Capacity is for future Lua/C++ inject so hosts dual-wire
// pure free functions instead of re-inlining comparisons. Helpers take
// host-injected scalars only (no entity / instance / party / settings
// pointers). Side effects (instance:complete, messages, instanceEntry,
// onEventUpdate, giveKeyItem, setCurrency) remain host-owned.
// Go dual-wire: assault.ShouldAutoComplete (internal/assault/auto_complete.go);
// residual ProgressMeetsRequired / Content.ShouldAutoComplete in instance.go.
// Future Lua host injects ShouldAutoComplete then instance:complete().

namespace assaulthelpers
{

// ---------------------------------------------------------------------------
// Slice 2860 / 3057 — onInstanceProgressUpdate auto-complete
// ---------------------------------------------------------------------------

// ProgressMeetsRequired mirrors progress >= requiredProgress.
// Residual threshold helper (slice 1078 / 2860); composed by ShouldAutoComplete.
inline auto ProgressMeetsRequired(const int32 progress, const int32 requiredProgress) -> bool
{
    return progress >= requiredProgress;
}

// ShouldAutoComplete mirrors InstanceAssault:onInstanceProgressUpdate:
//   if requiredProgress and progress >= requiredProgress and not completed then
//     instance:complete()
//   end
//
// Formula (slice 3057 dual-wire; residual expand 2860):
//   if requiredProgress == 0 || alreadyCompleted: false
//   else: ProgressMeetsRequired(progress, requiredProgress)
//
// requiredProgress — host-injected content requiredProgress (Lua truthy; 0 skip)
// progress         — host-injected instance progress after update
// alreadyCompleted — host-injected instance:completed()
// true  → host calls instance:complete()
// false → leave instance running
//
// Dual-wire of Go assault.ShouldAutoComplete.
// Call site: future Lua InstanceAssault:onInstanceProgressUpdate inject.
// Prior pure port: slice 1078. Residual dual-wire suite: 2860 /
// test_assault_auto_complete_2860. Dedicated dual-wire suite is
// test_assault_auto_complete_3057. Residual ProgressMeetsRequired remains the
// pure threshold (no truthy/completed gates).
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

// ---------------------------------------------------------------------------
// Slice 2867 — onRytaalEventFinish obtain new tag
// ---------------------------------------------------------------------------

// RytaalOptionObtainTag matches option == 1 (obtain Imperial Army ID tag).
// option == 2 is end-assault / reclaim (separate pure gate).
inline constexpr int32 kRytaalOptionObtainTag = 1;

// IMPERIAL_ARMY_ID_TAG key-item id pin (scripts/enum/key_item.lua).
// Pure gate takes has-KI bool only; pin is for host inject documentation.
inline constexpr uint16 kKeyItemImperialArmyIDTag = 787;

// ShouldIssueNewTag is the pure free-function form of the option==1 gate:
//   option == 1 and not hasKeyItem(IMPERIAL_ARMY_ID_TAG)
// Host still checks tagStock > 0 and currentAssault == 0, then giveKeyItem
// and stock/timer currency writeback.
//
// Lua host (onRytaalEventFinish after csid == 268):
//   if option == 1 and not hasKeyItem(IMPERIAL_ARMY_ID_TAG) then
//     if tagStock == 0 then return end
//     if getCurrentAssault() ~= 0 then messageSpecial(...); return end
//     giveKeyItem(...); setCharVar/setCurrency writeback
//   end
inline auto ShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == kRytaalOptionObtainTag && !hasImperialArmyIDTag;
}

} // namespace assaulthelpers
