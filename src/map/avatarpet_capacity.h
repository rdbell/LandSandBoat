#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Light Spirit / avatar pet dual-wire helpers (scripts/globals/pets/avatar.lua).
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1043: residual pure port (internal/avatarpet catalogs + injects)
//   - 2968: CanApplyBuff (tryBuffSpell canApplyBuff tier overwrite inject)
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

// CanApplyBuff mirrors tryBuffSpell's canApplyBuff pure half (slice 2968):
//
//   if !hasStatusEffect → true
//   if !spellHasTier    → false  (Haste/Regen never reapply while active)
//   else                → statusTier < spellTier
//
// Equivalent one-liner (Lua `and` binds tighter than `or`):
//
//   !hasStatusEffect || (spellHasTier && statusTier < spellTier)
//
// Matches Go avatarpet.CanApplyBuff (1043 residual / 2968 dual-wire).
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
