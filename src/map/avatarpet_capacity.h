#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Light Spirit / avatar pet dual-wire helpers (scripts/globals/pets/avatar.lua).
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1043: residual pure port (internal/avatarpet catalogs + injects)
//   - 2968: CanApplyBuff residual dual-wire suite (tryBuffSpell canApplyBuff)
//   - 3152: CanApplyBuff prior dedicated dual-wire (can_apply_buff.go)
//   - 3230: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3370: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3425: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3479: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3543: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3587: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3632: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3677: CanApplyBuff prior dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//   - 3722: CanApplyBuff dedicated dual-wire expand residual 2968
//           (can_apply_buff.go)
//
// Dual-wire index:
//   - 2968: CanApplyBuff residual dual-wire suite
//   - 3152: CanApplyBuff prior dedicated dual-wire
//   - 3230: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3370: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3425: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3479: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3543: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3587: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3632: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3677: CanApplyBuff prior dedicated dual-wire expand residual 2968
//   - 3722: CanApplyBuff =
//       !hasStatusEffect || (spellHasTier && statusTier < spellTier)
//     (positive form: no status → true; status + !spellHasTier → false;
//      else statusTier < spellTier)
//     dedicated dual-wire expand residual 2968
//
// Lua production host: scripts/globals/pets/avatar.lua tryBuffSpell (~121–122):
//
//   local statusEffect = member:getStatusEffect(spellData.effect)
//   local canApplyBuff = not statusEffect
//       or spellData.tier and statusEffect:getTier() < spellData.tier
//
//   if canApplyBuff then
//       return spellData.spell, member
//   end
//
// Host injects scalars only (no pet / party / status pointers):
//   hasStatusEffect — member:getStatusEffect(spellData.effect) ~= nil
//   statusTier      — statusEffect:getTier() (ignored when !hasStatusEffect)
//   spellHasTier    — spellData.tier ~= nil (Haste/Regen omit tier → false)
//   spellTier       — spellData.tier (Protect/Shell 1..5)
//
// Party shuffle, distance, isAlive, and cast selection remain host-owned.
// Prior pure port: OmegaXI slice 1043 (internal/avatarpet).
// Residual dual-wire suite: 2968 / test_avatarpet_apply_buff_2968.
// Prior dedicated dual-wire: 3152 / test_avatarpet_can_apply_buff_3152.
// Prior dedicated dual-wire expand residual 2968: 3230 /
// test_avatarpet_can_apply_buff_3230.
// Prior dedicated dual-wire expand residual 2968: 3370 /
// test_avatarpet_can_apply_buff_3370.
// Prior dedicated dual-wire expand residual 2968: 3425 /
// test_avatarpet_can_apply_buff_3425.
// Prior dedicated dual-wire expand residual 2968: 3479 /
// test_avatarpet_can_apply_buff_3479.
// Prior dedicated dual-wire expand residual 2968: 3543 /
// test_avatarpet_can_apply_buff_3543.
// Prior dedicated dual-wire expand residual 2968: 3587 /
// test_avatarpet_can_apply_buff_3587.
// Prior dedicated dual-wire expand residual 2968: 3632 /
// test_avatarpet_can_apply_buff_3632.
// Prior dedicated dual-wire expand residual 2968: 3677 /
// test_avatarpet_can_apply_buff_3677.
// Dedicated dual-wire expand residual 2968: 3722 /
// test_avatarpet_can_apply_buff_3722.
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/avatarpet (can_apply_buff.go) so hosts call CanApplyBuff instead
// of re-inlining:
//
//   !hasStatusEffect || (spellHasTier && statusTier < spellTier)
//
// Go dual-wire: avatarpet.CanApplyBuff / avatarpet.CanApplyBuffSpell
// (internal/avatarpet/can_apply_buff.go).

namespace avatarpethelpers
{

// ---------------------------------------------------------------------------
// 2968 residual / 3152 prior dedicated / 3230 prior dedicated expand residual
// 2968 / 3370 prior dedicated expand residual 2968 / 3425 prior dedicated
// expand residual 2968 / 3479 prior dedicated expand residual 2968 /
// 3543 prior dedicated expand residual 2968 /
// 3587 prior dedicated expand residual 2968 /
// 3632 prior dedicated expand residual 2968 /
// 3677 prior dedicated expand residual 2968 /
// 3722 dedicated expand residual 2968
// — tryBuffSpell canApplyBuff tier overwrite
// ---------------------------------------------------------------------------

// CanApplyBuff mirrors tryBuffSpell's canApplyBuff pure half:
//
// Formula (slice 3722 dedicated dual-wire; residual expand 2968 / pure 1043 /
// prior dedicated 3152 / prior dedicated expand residual 2968 3230 /
// prior dedicated expand residual 2968 3370 / prior dedicated expand residual
// 2968 3425 / prior dedicated expand residual 2968 3479 / prior dedicated
// expand residual 2968 3543 / prior dedicated expand residual 2968 3587 /
// prior dedicated expand residual 2968 3632 / prior dedicated expand residual
// 2968 3677 —
// formula unchanged):
//   if !hasStatusEffect → true
//   if !spellHasTier    → false  (Haste/Regen never reapply while active)
//   else                → statusTier < spellTier
//
// Equivalent one-liner (Lua `and` binds tighter than `or`):
//
//   !hasStatusEffect || (spellHasTier && statusTier < spellTier)
//
// Host injects status presence + tiers only. Host still owns status lookup,
// party shuffle, distance, isAlive, and cast selection.
// Dual-wire of Go avatarpet.CanApplyBuff (can_apply_buff.go).
// Call site: future Lua tryBuffSpell inject.
// Prior pure port: slice 1043. Residual dual-wire suite: 2968 /
// test_avatarpet_apply_buff_2968. Prior dedicated dual-wire suite:
// test_avatarpet_can_apply_buff_3152. Prior dedicated dual-wire expand residual
// 2968 suite: test_avatarpet_can_apply_buff_3230. Prior dedicated dual-wire
// expand residual 2968 suite: test_avatarpet_can_apply_buff_3370. Prior
// dedicated dual-wire expand residual 2968 suite:
// test_avatarpet_can_apply_buff_3425. Prior dedicated dual-wire expand residual
// 2968 suite: test_avatarpet_can_apply_buff_3479. Prior dedicated dual-wire
// expand residual 2968 suite: test_avatarpet_can_apply_buff_3543. Prior
// dedicated dual-wire expand residual 2968 suite:
// test_avatarpet_can_apply_buff_3587. Prior dedicated dual-wire expand residual
// 2968 suite: test_avatarpet_can_apply_buff_3632. Prior dedicated dual-wire
// expand residual 2968 suite: test_avatarpet_can_apply_buff_3677. Dedicated
// dual-wire expand residual 2968 suite is test_avatarpet_can_apply_buff_3722.
// Matches Go avatarpet.CanApplyBuff (1043 residual / 2968 / 3152 / 3230 /
// 3370 / 3425 / 3479 / 3543 / 3587 / 3632 / 3677 / 3722 dual-wire).
inline auto CanApplyBuff(const bool hasStatusEffect, const uint8 statusTier, const bool spellHasTier, const uint8 spellTier) -> bool
{
    if (!hasStatusEffect)
    {
        return true;
    }
    if (!spellHasTier)
    {
        return false;
    }
    return statusTier < spellTier;
}

} // namespace avatarpethelpers
