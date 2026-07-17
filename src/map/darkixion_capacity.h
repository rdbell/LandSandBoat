#pragma once

#include "common/cbasetypes.h"

// Pure Dark Ixion helpers for dual-wire slices:
//   - 2885: CanBreakHorn (checkHornBreak pure gate before 5% roll)
//
// Lua production host: scripts/globals/dark_ixion.lua
//   local checkHornBreak = function(mob, attacker)
//     local animationSub = mob:getAnimationSub()
//     if
//         not xi.combat.behavior.isEntityBusy(mob) and
//         (animationSub == animationSubs.NORMAL or animationSub == animationSubs.GLOWING) and
//         (attacker ~= nil and attacker:isInfront(mob)) and
//         math.random(1, 100) <= 5
//     then
//         changeHornState(mob, 2)
//     end
//   end
//
// Host injects scalars only (no mob / attacker pointers):
//   busy            — xi.combat.behavior.isEntityBusy(mob)
//   animSub         — mob:getAnimationSub()
//   attackerInFront — attacker ~= nil and attacker:isInfront(mob)
//
// RNG 5% roll (HornBreakRoll) and changeHornState writeback remain host-owned.
// Prior pure port: OmegaXI slice 0985 (internal/darkixion).
// Dual-wire of Go darkixion.CanBreakHorn (slice 2885).

namespace darkixionhelpers
{

// Animation-sub catalog pins (local animationSubs / Go Anim* constants):
//   NORMAL      = 0 — can trample
//   TRAMPLE     = 1 — animation during trample event
//   HORN_BROKEN = 2 — broken horn; cannot trample or double-up
//   GLOWING     = 3 — doubles up mobskills; brief horn regrowth
// CanBreakHorn only accepts NORMAL or GLOWING.
inline constexpr int32 kAnimNormal     = 0;
inline constexpr int32 kAnimTrample    = 1;
inline constexpr int32 kAnimHornBroken = 2;
inline constexpr int32 kAnimGlowing    = 3;

// CanBreakHorn is the pure gate half of checkHornBreak before the 5% roll:
//
//   !busy && (animSub == AnimNormal || animSub == AnimGlowing) && attackerInFront
//
// Host still owns isEntityBusy / getAnimationSub / isInfront inject, the 5%
// math.random roll, and changeHornState(mob, 2) writeback.
// Dual-wire of Go darkixion.CanBreakHorn.
inline auto CanBreakHorn(const bool busy, const int32 animSub, const bool attackerInFront) -> bool
{
    return !busy && (animSub == kAnimNormal || animSub == kAnimGlowing) && attackerInFront;
}

} // namespace darkixionhelpers
