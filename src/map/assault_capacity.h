#pragma once

#include "common/cbasetypes.h"

// Pure Assault helpers shared by dual-wire slices:
//   - 2860: InstanceAssault progress auto-complete
//   - 2863: onAssaultUpdate party/alliance proceed
//   - 2867: onRytaalEventFinish obtain new Imperial Army ID tag
//
// Production hosts are Lua under scripts/globals/assault/ (container.lua,
// npc_handler.lua). Capacity is for future Lua/C++ inject so hosts dual-wire
// pure free functions instead of re-inlining comparisons. Helpers take
// host-injected scalars only (no entity / instance / party / settings
// pointers). Side effects (instance:complete, messages, instanceEntry,
// onEventUpdate, giveKeyItem, setCurrency) remain host-owned.

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
