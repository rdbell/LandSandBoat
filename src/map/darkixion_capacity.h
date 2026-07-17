#pragma once

#include "common/cbasetypes.h"

// Pure Dark Ixion helpers for dual-wire slices:
//   - 2885: CanBreakHorn (checkHornBreak pure gate before 5% roll)
//   - 2893: CanRestoreHorn (Damsel Memento pure gate before 25% roll)
//   - 2907: HornBreakRoll (checkHornBreak 5% roll after CanBreakHorn)
//   - 2911: HornRestoreRoll (Damsel Memento 25% roll after CanRestoreHorn)
//   - 2915: ShouldDoubleGlowSkill (DI_GLOW double-up pure gate)
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
//   xi.darkixion.onMobWeaponSkill (Damsel Memento restore):
//     if skillID == xi.mobSkill.DAMSEL_MEMENTO then
//       if
//           mob:getAnimationSub() == animationSubs.HORN_BROKEN and
//           math.random(1, 100) <= 25
//       then
//           skill:setFinalAnimationSub(3)
//           mob:queue(0, function(mobArg)
//               mobArg:stun(500)
//               changeHornState(mobArg, 1)
//           end)
//       end
//     end
//
//   xi.darkixion.onMobWeaponSkill (DI_GLOW double-up):
//     elseif skillID == xi.mobSkill.DI_GLOW then
//       local chosenSkill = utils.randomEntry(skillList)
//       mob:setBehavior(...)
//       mob:setAutoAttackEnabled(false)
//       mob:useMobAbility(chosenSkill)
//       if mob:getAnimationSub() == animationSubs.GLOWING then
//           mob:useMobAbility(chosenSkill)
//       end
//     end
//
// Host injects scalars only (no mob / attacker pointers):
//   busy            — xi.combat.behavior.isEntityBusy(mob)   [CanBreakHorn]
//   animSub         — mob:getAnimationSub()
//   attackerInFront — attacker ~= nil and attacker:isInfront(mob)  [CanBreakHorn]
//   roll1to100      — math.random(1, 100)  [HornBreakRoll / HornRestoreRoll]
//
// RNG generation (math.random), useMobAbility / setBehavior writeback, and
// changeHornState writeback remain host-owned.
// Prior pure port: OmegaXI slice 0985 (internal/darkixion).
// Dual-wire of Go darkixion.CanBreakHorn (slice 2885).
// Dual-wire of Go darkixion.CanRestoreHorn (slice 2893).
// Dual-wire of Go darkixion.HornBreakRoll (slice 2907).
// Dual-wire of Go darkixion.HornRestoreRoll (slice 2911).
// Dual-wire of Go darkixion.ShouldDoubleGlowSkill (slice 2915).

namespace darkixionhelpers
{

// Animation-sub catalog pins (local animationSubs / Go Anim* constants):
//   NORMAL      = 0 — can trample
//   TRAMPLE     = 1 — animation during trample event
//   HORN_BROKEN = 2 — broken horn; cannot trample or double-up
//   GLOWING     = 3 — doubles up mobskills; brief horn regrowth
// CanBreakHorn only accepts NORMAL or GLOWING.
// CanRestoreHorn only accepts HORN_BROKEN.
// ShouldDoubleGlowSkill only accepts GLOWING.
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

// CanRestoreHorn is the pure gate half of Damsel Memento horn restore before
// the 25% roll:
//
//   animSub == AnimHornBroken
//
// Host still owns getAnimationSub inject, the 25% math.random roll,
// setFinalAnimationSub(3), stun(500), and changeHornState(mob, 1) writeback.
// Dual-wire of Go darkixion.CanRestoreHorn.
inline auto CanRestoreHorn(const int32 animSub) -> bool
{
    return animSub == kAnimHornBroken;
}

// HornBreakChancePercent is the 5% roll ceiling on critical / weaponskill hit
// (math.random(1, 100) <= 5 → ~5%).
// Dual-wire of Go darkixion.HornBreakChancePercent.
inline constexpr int32 HornBreakChancePercent = 5;

// HornBreakRoll is the pure roll half of checkHornBreak after CanBreakHorn:
//
//   math.random(1, 100) <= 5
//
// Implemented as roll >= 1 && roll <= HornBreakChancePercent so out-of-range
// rolls do not spuriously succeed. roll1to100 is the host-injected
// math.random(1, 100) result. Host still owns CanBreakHorn gate inject, RNG
// generation, and changeHornState(mob, 2) writeback.
// Dual-wire of Go darkixion.HornBreakRoll.
inline auto HornBreakRoll(const int32 roll1to100) -> bool
{
    return roll1to100 >= 1 && roll1to100 <= HornBreakChancePercent;
}

// HornRestoreChancePercent is the 25% roll ceiling after DAMSEL_MEMENTO
// (math.random(1, 100) <= 25 → ~25%).
// Dual-wire of Go darkixion.HornRestoreChancePercent.
inline constexpr int32 HornRestoreChancePercent = 25;

// HornRestoreRoll is the pure roll half of Damsel Memento horn restore after
// CanRestoreHorn:
//
//   math.random(1, 100) <= 25
//
// Implemented as roll >= 1 && roll <= HornRestoreChancePercent so out-of-range
// rolls do not spuriously succeed. roll1to100 is the host-injected
// math.random(1, 100) result. Host still owns CanRestoreHorn gate inject, RNG
// generation, setFinalAnimationSub(3), stun(500), and changeHornState(mob, 1)
// writeback.
// Dual-wire of Go darkixion.HornRestoreRoll.
inline auto HornRestoreRoll(const int32 roll1to100) -> bool
{
    return roll1to100 >= 1 && roll1to100 <= HornRestoreChancePercent;
}

// ShouldDoubleGlowSkill is the pure gate for DI_GLOW double-up of the
// follow-up TP move:
//
//   animSub == AnimGlowing
//
// Host still owns getAnimationSub inject, skill-list pick (randomEntry),
// setBehavior / setAutoAttackEnabled, and useMobAbility writeback (first and
// optional second queue of the chosen skill).
// Dual-wire of Go darkixion.ShouldDoubleGlowSkill.
inline auto ShouldDoubleGlowSkill(const int32 animSub) -> bool
{
    return animSub == kAnimGlowing;
}

} // namespace darkixionhelpers
