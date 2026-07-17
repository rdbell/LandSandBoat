#pragma once

// Pure Dynamis helpers shared by dual-wire slices:
//   - 2857 residual: AtOrigin (zoneOnZoneIn origin equality half)
//   - 3197: AtOrigin dedicated dual-wire (at_origin.go)
//   - 3078: ShouldSnapToEntryPos residual dual-wire suite
//   - 3257: ShouldSnapToEntryPos dedicated dual-wire expand residual 3197
//   - 2921 residual: CanUnlockSJ (somnial threshold startEvent param)
//   - 3151: CanUnlockSJ dedicated dual-wire (unlock_sj.go)
//   - 3422: CanUnlockSJ dedicated dual-wire expand residual 2921
//     (prior 3151 retained)
//   - 3476: CanUnlockSJ dedicated dual-wire expand residual 2921
//     (prior 3422 / 3151 retained)
//   - 3349: ShouldUnlockSJRestriction dedicated dual-wire expand residual
//     2921/2857 (zone.go)
//
// Dual-wire index:
//   - 2857: AtOrigin residual dual-wire notes
//   - 3197: AtOrigin = x == 0 && y == 0 && z == 0
//   - 3078: ShouldSnapToEntryPos residual dual-wire suite
//   - 3257: ShouldSnapToEntryPos = hasDynamisEffect && AtOrigin(x, y, z)
//     dedicated dual-wire expand residual 3197
//   - 2921: CanUnlockSJ residual dual-wire suite
//   - 3151: CanUnlockSJ = hasSJRestriction ? 1 : 0
//   - 3422: CanUnlockSJ = hasSJRestriction ? 1 : 0
//     dedicated dual-wire expand residual 2921 (prior 3151 retained)
//   - 3476: CanUnlockSJ = hasSJRestriction ? 1 : 0
//     dedicated dual-wire expand residual 2921 (prior 3422 / 3151 retained)
//   - 3349: ShouldUnlockSJRestriction =
//     hasSJRestriction && IsSubjobUnlockOption(option)
//     dedicated dual-wire expand residual 2921/2857
//
// Lua production hosts: scripts/globals/dynamis.lua
//   - xi.dynamis.zoneOnZoneIn (origin snap elseif after DYNAMIS check)
//   - xi.dynamis.somnialThresholdOnTrigger (canUnlockSJ event param)
//   - xi.dynamis.somnialThresholdOnEventFinish (option==2 unlock SJ)
// Go dual-wire: dynamis.AtOrigin (internal/dynamis/at_origin.go);
// dynamis.ShouldSnapToEntryPos (internal/dynamis/snap_entry.go);
// dynamis.CanUnlockSJ (internal/dynamis/unlock_sj.go);
// dynamis.ShouldUnlockSJRestriction (internal/dynamis/zone.go). Future Lua
// host injects free functions then setPos / startEvent /
// delStatusEffectSilent.
//
// Host injects scalars only (no entity pointers). Entity writeback
// (setPos / startEvent / delStatusEffectSilent) remains host-owned.
//
// Prior pure ports: OmegaXI slices 1119 (zone), 1077 (somnial).
// Residual dual-wire suite: 2857 / 2921 / 3078.
// Dedicated dual-wire suite: 3078 / 3151 / 3197 / 3257 / 3349 / 3422 / 3476.
//
// Index 3078: dynamis.ShouldSnapToEntryPos residual dual-wire suite.
// Index 3151: dynamis.CanUnlockSJ pure dual-wire.
// Index 3422: dynamis.CanUnlockSJ dedicated dual-wire expand residual 2921
// (prior 3151 retained; formula unchanged).
// Index 3476: dynamis.CanUnlockSJ dedicated dual-wire expand residual 2921
// (prior 3422 / 3151 retained; formula unchanged).
// Index 3197: dynamis.AtOrigin pure dual-wire.
// Index 3257: dynamis.ShouldSnapToEntryPos dedicated dual-wire expand
// residual 3197.
// Index 3349: dynamis.ShouldUnlockSJRestriction dedicated dual-wire expand
// residual 2921/2857.
// Go dual-wire: dynamis.AtOrigin (internal/dynamis/at_origin.go);
// dynamis.ShouldSnapToEntryPos (internal/dynamis/snap_entry.go);
// dynamis.CanUnlockSJ (internal/dynamis/unlock_sj.go);
// dynamis.ShouldUnlockSJRestriction (internal/dynamis/zone.go).

namespace dynamishelpers
{

// ---------------------------------------------------------------------------
// Origin equality half (AtOrigin residual 2857; dedicated dual-wire 3197)
// ---------------------------------------------------------------------------

// AtOrigin reports exact float equality of position to (0, 0, 0).
// Matches LSB Lua `getXPos()==0 and getYPos()==0 and getZPos()==0`.
//
// Formula (slice 3197 dedicated dual-wire; residual expand 2857 / pure 1119 —
// formula unchanged):
//   AtOrigin(x, y, z) = x == 0.0f && y == 0.0f && z == 0.0f
//
// Exact float equality (no epsilon). IEEE −0.0f equals +0.0f.
// Dual-wire of OmegaXI internal/dynamis AtOrigin (at_origin.go).
// Used by ShouldSnapToEntryPos (3257 expand residual 3197; residual 3078).
// Call site: future Lua host inject of zoneOnZoneIn origin equality half;
// hosts share one pure surface.
// Prior pure port: slice 1119. Residual dual-wire suite: 2857 /
// test_dynamis_snap_entry_2857. Dedicated dual-wire suite is
// test_dynamis_at_origin_3197.
// Sibling left alone: CanUnlockSJ (3151).
// Index 3197: dynamis.AtOrigin pure dual-wire.
inline auto AtOrigin(const float x, const float y, const float z) -> bool
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

// ---------------------------------------------------------------------------
// Origin-snap gate (ShouldSnapToEntryPos slice 3257 expand residual 3197;
// residual suite 3078; composes AtOrigin)
// ---------------------------------------------------------------------------

// ShouldSnapToEntryPos is the pure free-function form of the origin-snap gate.
//
// Formula (slice 3257 dedicated dual-wire expand residual 3197; prior
// dedicated 3078 / residual 2857 / pure 1119 — formula unchanged):
//   hasDynamisEffect && AtOrigin(x, y, z)
//
// hasDynamisEffect — host-evaluated effective DYNAMIS status
//                    (post entry-setup or pre-existing)
// x,y,z            — player position (exact float equality to origin)
// true  → host setPos(unpack(info.entryPos))
// false → leave position alone (or eject CS when no DYNAMIS)
//
// Dual-wire of Go dynamis.ShouldSnapToEntryPos (snap_entry.go / slice 3257).
// Call site: future Lua host inject of zoneOnZoneIn origin-snap branch;
// OmegaXI ResolveZoneIn.SnapToEntryPos dual-wires this free function.
// Prior pure port: slice 1119; residual dual-wire notes: slice 2857;
// prior dedicated dual-wire suite: slice 3078 / test_dynamis_snap_entry_3078.
// Dedicated dual-wire suite is test_dynamis_snap_entry_3257.
// Index 3257: dynamis.ShouldSnapToEntryPos dedicated dual-wire expand
// residual 3197.
// Sibling left alone this slice: CanUnlockSJ (3151 / residual 2921).
// Composes AtOrigin (3197 dedicated dual-wire; residual 2857).
// Coverage: free == inline == pin; residual poles (effect on/off, origin vs
// non-origin); dense hasDynamis × positions. Residual 3078 suite retained.
inline auto ShouldSnapToEntryPos(const bool hasDynamisEffect, const float x, const float y, const float z) -> bool
{
    return hasDynamisEffect && AtOrigin(x, y, z);
}

// ---------------------------------------------------------------------------
// Somnial CanUnlockSJ (slice 2921 residual / 3151 dedicated / 3422 expand
// residual 2921 / 3476 expand residual 2921)
// ---------------------------------------------------------------------------

// CanUnlockSJ is the pure free-function form of the somnial threshold
// startEvent param from xi.dynamis.somnialThresholdOnTrigger:
//
//   local canUnlockSJ = player:hasStatusEffect(xi.effect.SJ_RESTRICTION) and 1 or 0
//   player:startEvent(101, 0x27, canUnlockSJ, menuBits)
//
// Formula (slice 3476 dedicated dual-wire expand residual 2921; prior dedicated
// 3422 / 3151 / residual expand 2921 / pure 1077 — formula unchanged):
//   CanUnlockSJ(hasSJRestriction) = hasSJRestriction ? 1 : 0
//
// Host injects hasStatusEffect(SJ_RESTRICTION) as a bool. Returns 1 when the
// player has SJ restriction (can unlock support jobs), else 0.
// Dual-wires OmegaXI internal/dynamis CanUnlockSJ (unlock_sj.go).
// Call site: future Lua somnialThresholdOnTrigger inject.
// Prior pure port: slice 1077. Residual dual-wire suite: 2921 /
// test_dynamis_unlock_sj_2921. Prior dedicated dual-wire suite: 3151 /
// test_dynamis_can_unlock_sj_3151 (retained). Prior dedicated dual-wire expand
// residual 2921 suite: 3422 / test_dynamis_can_unlock_sj_3422 (retained).
// Dedicated dual-wire expand residual 2921 suite is
// test_dynamis_can_unlock_sj_3476.
// startEvent / menuBits / finish delStatusEffectSilent remain host-owned.
// Sibling left alone: ShouldSnapToEntryPos (3257 / residual 3078);
// ShouldUnlockSJRestriction (3349) pairs with unlock option (not thrashed).
// Index 3476: dynamis.CanUnlockSJ dedicated dual-wire expand residual 2921.
inline auto CanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Somnial unlock gate (ShouldUnlockSJRestriction slice 3349 expand residual
// 2921/2857; pure 1119; composes IsSubjobUnlockOption)
// ---------------------------------------------------------------------------

// SomnialOptionUnlockSJ matches OmegaXI dynamis.SomnialOptionUnlockSJ (2).
// Finish option from xi.dynamis.somnialThresholdOnEventFinish unlock branch.
inline constexpr int SomnialOptionUnlockSJ = 2;

// IsSubjobUnlockOption reports somnial finish option == 2 (unlock support
// jobs). Dual-wire of OmegaXI internal/dynamis IsSubjobUnlockOption (zone.go).
// Composed by ShouldUnlockSJRestriction (slice 3349).
inline auto IsSubjobUnlockOption(const int option) -> bool
{
    return option == SomnialOptionUnlockSJ;
}

// ShouldUnlockSJRestriction is the pure free-function form of the semantic
// unlock gate for hosts that already resolved hasStatusEffect(SJ_RESTRICTION)
// and the somnial finish option:
//
//   elseif option == 2 then
//     player:messageSpecial(ID.text.DYNAMIS_SUB_UNLOCKED)
//     player:delStatusEffectSilent(xi.effect.SJ_RESTRICTION)
//
// Formula (slice 3349 dedicated dual-wire expand residual 2921/2857; prior
// pure 1119 — formula unchanged):
//   hasSJRestriction && IsSubjobUnlockOption(option)
//
// hasSJRestriction — host-evaluated hasStatusEffect(SJ_RESTRICTION)
// option           — somnial finish option (2 = unlock support jobs)
// true  → host messageSpecial + delStatusEffectSilent(SJ_RESTRICTION)
// false → semantic no-op (LSB still may run delStatusEffectSilent harmlessly)
//
// Dual-wire of Go dynamis.ShouldUnlockSJRestriction (zone.go / slice 3349).
// Call site: future Lua host inject of somnialThresholdOnEventFinish unlock
// branch; pairs with CanUnlockSJ != 0 when option is unlock.
// Prior pure port: slice 1119; residual dual-wire lineage: slice 2921
// (CanUnlockSJ pairing / capacity) / slice 2857 (capacity surface).
// Dedicated dual-wire suite is test_dynamis_unlock_sj_3349.
// Residual dual-wire suite retained: test_dynamis_unlock_sj_2921.
// Index 3349: dynamis.ShouldUnlockSJRestriction dedicated dual-wire expand
// residual 2921/2857.
// Sibling left alone this slice: CanUnlockSJ (3151 / residual 2921),
// ShouldSnapToEntryPos / AtOrigin (3257 / 3197 / residual 2857), eject.
// Coverage: free == inline == pin; residual poles (hasSJ on/off × unlock vs
// non-unlock option); dense hasSJ × options. Residual 2921 suite retained.
inline auto ShouldUnlockSJRestriction(const bool hasSJRestriction, const int option) -> bool
{
    return hasSJRestriction && IsSubjobUnlockOption(option);
}

} // namespace dynamishelpers
