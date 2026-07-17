#pragma once

#include "common/cbasetypes.h"

// Pure Assault helpers shared by dual-wire slices:
//   - 2860: InstanceAssault progress auto-complete (residual dual-wire suite)
//   - 2863: onAssaultUpdate party/alliance proceed (residual dual-wire suite)
//   - 2867: onRytaalEventFinish obtain new Imperial Army ID tag (residual dual-wire)
//   - 3057: ShouldAutoComplete prior dedicated dual-wire (auto_complete.go)
//   - 3145: ShouldIssueNewTag prior dedicated dual-wire (issue_tag.go)
//   - 3199: ShouldProceedAssaultUpdate dedicated dual-wire (proceed_update.go)
//   - 3258: ShouldIssueNewTag dedicated dual-wire expand residual 2867
//           / prior 3145 (issue_tag.go)
//   - 3345: ShouldAutoComplete dedicated dual-wire expand residual 2860
//           / prior 3057 (auto_complete.go)
//   - 3388: CanIssueTagFromStock dedicated dual-wire expand residual 2867
//           (npc_handler.go stock gate after ShouldIssueNewTag)
//
// Dual-wire index:
//   - 2860: ProgressMeetsRequired / ShouldAutoComplete residual dual-wire
//   - 2863: ShouldProceedAssaultUpdate residual dual-wire (party/alliance proceed)
//   - 2867: ShouldIssueNewTag residual dual-wire (Rytaal obtain new tag)
//   - 3057: ShouldAutoComplete prior dedicated dual-wire
//           (requiredProgress==0 || alreadyCompleted → false;
//           else ProgressMeetsRequired on onInstanceProgressUpdate)
//   - 3145: ShouldIssueNewTag (option == kRytaalOptionObtainTag &&
//           !hasImperialArmyIDTag; prior dedicated dual-wire suite)
//   - 3199: ShouldProceedAssaultUpdate (!PartyTooSmallForAssault &&
//           !IsAllianceBlocked; dedicated dual-wire suite)
//   - 3258: ShouldIssueNewTag (option == kRytaalOptionObtainTag &&
//           !hasImperialArmyIDTag; dedicated expand residual 2867 / prior 3145)
//   - 3345: ShouldAutoComplete (requiredProgress==0 || alreadyCompleted → false;
//           else ProgressMeetsRequired; dedicated expand residual 2860 / prior 3057)
//   - 3388: CanIssueTagFromStock (tagStock > 0; dedicated expand residual 2867
//           stock gate after ShouldIssueNewTag)
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
// Slice 2860 / 3057 / 3345 — onInstanceProgressUpdate auto-complete
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
// Formula (slice 3345 dedicated dual-wire expand residual 2860 / prior 3057 /
// pure 1078 — formula unchanged):
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
// test_assault_auto_complete_2860. Prior dedicated dual-wire suite: 3057 /
// test_assault_auto_complete_3057. Dedicated dual-wire expand residual suite
// is test_assault_auto_complete_3345. Residual ProgressMeetsRequired remains
// the pure threshold (no truthy/completed gates).
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
// Slice 2863 / 3199 — onAssaultUpdate party / alliance proceed
// ---------------------------------------------------------------------------

// AllianceBlocked matches checkSoloPartyAlliance() == 2 (full alliance).
// 0 = solo, 1 = party, 2 = alliance.
inline constexpr int32 kAllianceBlocked = 2;

// PartyTooSmallForAssault mirrors the first early-return gate:
//   gmLevel == 0 && partySize < assaultMinimum
// GM (gmLevel != 0) bypasses the minimum. assaultMinimum is
// settings.main.ASSAULT_MINIMUM (default 1; TOAU-era 3).
// Residual component (slice 1100 / 2863); composed by ShouldProceedAssaultUpdate.
inline auto PartyTooSmallForAssault(const int32 gmLevel, const int32 partySize, const int32 assaultMinimum) -> bool
{
    return gmLevel == 0 && partySize < assaultMinimum;
}

// IsAllianceBlocked mirrors the second early-return gate:
//   checkSoloPartyAlliance() == 2
// Residual component (slice 1100 / 2863); composed by ShouldProceedAssaultUpdate.
inline auto IsAllianceBlocked(const int32 checkSoloPartyAlliance) -> bool
{
    return checkSoloPartyAlliance == kAllianceBlocked;
}

// ShouldProceedAssaultUpdate is the pure free-function form of the post-cap
// proceed decision (inverse of either early-return):
//   !PartyTooSmallForAssault(...) && !IsAllianceBlocked(...)
// When true the host continues to xi.instance.onEventUpdate.
//
// Formula (slice 3199 dedicated dual-wire; residual expand 2863 / pure 1100
// — formula unchanged):
//   if PartyTooSmallForAssault(...): return false
//   if IsAllianceBlocked(...): return false
//   return true
//   // ≡ !PartyTooSmallForAssault(...) && !IsAllianceBlocked(...)
//
// Lua host (onAssaultUpdate after setLocalVar('AssaultCap')):
//   if getGMLevel() == 0 and getPartySize() < ASSAULT_MINIMUM then
//     messageSpecial(...); instanceEntry(npc, 1); return
//   elseif checkSoloPartyAlliance() == 2 then
//     messageText(...); instanceEntry(npc, 1); return
//   end
//   xi.instance.onEventUpdate(...)
//
// Dual-wire of Go assault.ShouldProceedAssaultUpdate
// (internal/assault/proceed_update.go).
// Call site: future Lua onAssaultUpdate inject.
// Prior pure port: slice 1100. Residual dual-wire suite: 2863 /
// test_assault_proceed_update_2863. Dedicated dual-wire suite is
// test_assault_proceed_update_3199. Residual PartyTooSmallForAssault /
// IsAllianceBlocked remain the pure component gates.
// Host still owns messages / instanceEntry / onEventUpdate after this gate.
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
// Slice 2867 / 3145 / 3258 — onRytaalEventFinish obtain new tag
// Slice 3388 — tagStock gate after ShouldIssueNewTag
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
// Formula (slice 3258 dedicated dual-wire expand residual 2867 / prior 3145 /
// pure 1100 — formula unchanged):
//   ShouldIssueNewTag(option, hasImperialArmyIDTag) =
//     option == kRytaalOptionObtainTag && !hasImperialArmyIDTag
//
// Lua host (onRytaalEventFinish after csid == 268):
//   if option == 1 and not hasKeyItem(IMPERIAL_ARMY_ID_TAG) then
//     if tagStock == 0 then return end
//     if getCurrentAssault() ~= 0 then messageSpecial(...); return end
//     giveKeyItem(...); setCharVar/setCurrency writeback
//   end
//
// Dual-wire of Go assault.ShouldIssueNewTag (internal/assault/issue_tag.go).
// Call site: future Lua onRytaalEventFinish inject.
// Prior pure port: slice 1100. Residual dual-wire suite: 2867 /
// test_assault_issue_tag_2867. Prior dedicated dual-wire suite: 3145 /
// test_assault_issue_new_tag_3145. Dedicated dual-wire expand residual suite
// is test_assault_issue_tag_3258. Host still owns stock / currentAssault /
// giveKeyItem / currency writeback after this gate.
// Stock gate dual-wires as CanIssueTagFromStock (slice 3388).
inline auto ShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == kRytaalOptionObtainTag && !hasImperialArmyIDTag;
}

// CanIssueTagFromStock is the pure free-function form of the tagStock gate
// after ShouldIssueNewTag is true:
//   tagStock > 0
//   // Lua early return: if tagStock == 0 then return end
// Host still checks currentAssault == 0, then giveKeyItem and stock/timer
// currency writeback.
//
// Formula (slice 3388 dedicated dual-wire expand residual 2867 / pure 1100 —
// formula unchanged):
//   CanIssueTagFromStock(tagStock) = tagStock > 0
//
// Lua host (onRytaalEventFinish after ShouldIssueNewTag path):
//   local tagStock = player:getCurrency('id_tags')
//   if tagStock == 0 then
//     return
//   end
//   -- host: currentAssault / giveKeyItem / setCurrency writeback
//
// Dual-wire of Go assault.CanIssueTagFromStock
// (internal/assault/npc_handler.go).
// Call site: future Lua onRytaalEventFinish inject after ShouldIssueNewTag.
// Prior pure port: slice 1100. Residual dual-wire expand (stock poles under
// ShouldIssueNewTag suites): 2867 / 3145 / 3258. Dedicated dual-wire expand
// residual suite is test_assault_issue_tag_3388. Host still owns
// currentAssault / giveKeyItem / currency writeback after this gate.
inline auto CanIssueTagFromStock(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

} // namespace assaulthelpers
