#pragma once

#include "common/cbasetypes.h"

// Pure Dark Ixion helpers for dual-wire slices:
//   - 2885: CanBreakHorn residual dual-wire suite (checkHornBreak pure gate)
//   - 3154: CanBreakHorn prior dedicated dual-wire (retained)
//   - 3266: CanBreakHorn prior dedicated dual-wire expand residual 2885 (retained)
//   - 3295: CanBreakHorn prior dedicated dual-wire expand residual 2885 (retained)
//   - 3325: CanBreakHorn dedicated dual-wire (can_break_horn.go;
//           expand residual 2885)
//   - 2893: CanRestoreHorn residual dual-wire suite (Damsel Memento pure gate)
//   - 3187: CanRestoreHorn dedicated dual-wire (can_restore_horn.go)
//   - 2907: HornBreakRoll residual dual-wire suite (checkHornBreak 5% roll)
//   - 3206: HornBreakRoll dedicated dual-wire (horn_break_roll.go)
//   - 2911: HornRestoreRoll (Damsel Memento 25% roll after CanRestoreHorn)
//   - 2915: ShouldDoubleGlowSkill residual dual-wire suite (DI_GLOW double-up)
//   - 3389: ShouldDoubleGlowSkill dedicated dual-wire (should_double_glow_skill.go;
//           expand residual 2915)
//
// Dual-wire index:
//   - 2885: CanBreakHorn residual dual-wire suite
//   - 3154: CanBreakHorn prior dedicated dual-wire (retained)
//   - 3266: CanBreakHorn prior dedicated dual-wire expand residual 2885 (retained)
//   - 3295: CanBreakHorn prior dedicated dual-wire expand residual 2885 (retained)
//   - 3325: CanBreakHorn (!busy && (NORMAL||GLOWING) && attackerInFront)
//   - 2893: CanRestoreHorn residual dual-wire suite
//   - 3187: CanRestoreHorn (animSub == HORN_BROKEN)
//   - 2907: HornBreakRoll residual dual-wire suite
//   - 3206: HornBreakRoll (roll1to100 >= 1 && roll1to100 <= 5)
//   - 2911: HornRestoreRoll
//   - 2915: ShouldDoubleGlowSkill residual dual-wire suite
//   - 3389: ShouldDoubleGlowSkill (animSub == GLOWING)
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
// Residual dual-wire suite: slice 2885 / test_darkixion_break_horn_2885.
// Prior dedicated dual-wire: slice 3154 / test_darkixion_can_break_horn_3154.
// Prior dedicated dual-wire expand residual 2885: slice 3266 /
//   test_darkixion_can_break_horn_3266 (retained).
// Prior dedicated dual-wire expand residual 2885: slice 3295 /
//   test_darkixion_can_break_horn_3295 (retained).
// Dedicated dual-wire expand residual 2885: slice 3325 /
//   test_darkixion_can_break_horn_3325.
// Dual-wire of Go darkixion.CanBreakHorn (slice 3325 dedicated expand residual
// 2885; prior dedicated 3295 / 3266 / 3154 retained).
// Residual dual-wire suite: slice 2893 / test_darkixion_restore_horn_2893.
// Dedicated dual-wire suite: slice 3187 / test_darkixion_can_restore_horn_3187.
// Dual-wire of Go darkixion.CanRestoreHorn (slice 3187 dedicated; residual 2893).
// Residual dual-wire suite: slice 2907 / test_darkixion_horn_break_roll_2907.
// Dedicated dual-wire suite: slice 3206 / test_darkixion_horn_break_roll_3206.
// Dual-wire of Go darkixion.HornBreakRoll (slice 3206 dedicated; residual 2907).
// Dual-wire of Go darkixion.HornRestoreRoll (slice 2911).
// Residual dual-wire suite: slice 2915 / test_darkixion_double_glow_2915.
// Dedicated dual-wire expand residual 2915: slice 3389 /
//   test_darkixion_double_glow_3389.
// Dual-wire of Go darkixion.ShouldDoubleGlowSkill (slice 3389 dedicated expand
// residual 2915; residual 2915 retained).

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

// ---------------------------------------------------------------------------
// Slice 2885 residual / 3154 prior dedicated / 3266 prior dedicated expand /
// 3295 prior dedicated expand / 3325 dedicated — CanBreakHorn pure dual-wire
//
// Formula (slice 3325 dedicated dual-wire expand residual 2885; prior
// dedicated 3295 / 3266 / 3154 / pure inject 0985 — formula unchanged):
//   CanBreakHorn(busy, animSub, attackerInFront) =
//     !busy && (animSub == kAnimNormal || animSub == kAnimGlowing) && attackerInFront
//
// Dual-wire of Go darkixion.CanBreakHorn.
// Call site: future Lua checkHornBreak inject before 5% roll.
// Prior pure port: slice 0985. Residual dual-wire suite: 2885 /
// test_darkixion_break_horn_2885. Prior dedicated dual-wire suite: 3154 /
// test_darkixion_can_break_horn_3154 (retained). Prior dedicated expand residual
// suite: 3266 / test_darkixion_can_break_horn_3266 (retained). Prior dedicated
// expand residual suite: 3295 / test_darkixion_can_break_horn_3295 (retained).
// Dedicated dual-wire suite is test_darkixion_can_break_horn_3325. Host still
// owns isEntityBusy / getAnimationSub / isInfront inject, the 5% math.random
// roll, and changeHornState(mob, 2) writeback.
// ---------------------------------------------------------------------------

// CanBreakHorn is the pure gate half of checkHornBreak before the 5% roll:
//
//   !busy && (animSub == AnimNormal || animSub == AnimGlowing) && attackerInFront
//
// Formula (slice 3325 dedicated dual-wire expand residual 2885; prior
// dedicated 3295 / 3266 / 3154 / pure 0985 — formula unchanged):
//
//   CanBreakHorn(busy, animSub, attackerInFront) =
//     !busy && (animSub == kAnimNormal || animSub == kAnimGlowing) && attackerInFront
//
// Host still owns isEntityBusy / getAnimationSub / isInfront inject, the 5%
// math.random roll, and changeHornState(mob, 2) writeback.
// Dual-wire of Go darkixion.CanBreakHorn (slice 3325 dedicated expand residual
// 2885; prior dedicated 3295 / 3266 / 3154 retained).
inline auto CanBreakHorn(const bool busy, const int32 animSub, const bool attackerInFront) -> bool
{
    return !busy && (animSub == kAnimNormal || animSub == kAnimGlowing) && attackerInFront;
}

// ---------------------------------------------------------------------------
// Slice 2893 / 3187 — CanRestoreHorn pure dual-wire
//
// Formula (slice 3187 dedicated dual-wire; residual expand 2893; pure inject
// 0985 — formula unchanged):
//   CanRestoreHorn(animSub) =
//     animSub == kAnimHornBroken
//
// Dual-wire of Go darkixion.CanRestoreHorn.
// Call site: future Lua Damsel Memento inject before 25% roll.
// Prior pure port: slice 0985. Residual dual-wire suite: 2893 /
// test_darkixion_restore_horn_2893. Dedicated dual-wire suite is
// test_darkixion_can_restore_horn_3187. Host still owns getAnimationSub inject,
// the 25% math.random roll, setFinalAnimationSub(3), stun(500), and
// changeHornState(mob, 1) writeback.
// ---------------------------------------------------------------------------

// CanRestoreHorn is the pure gate half of Damsel Memento horn restore before
// the 25% roll:
//
//   animSub == AnimHornBroken
//
// Host still owns getAnimationSub inject, the 25% math.random roll,
// setFinalAnimationSub(3), stun(500), and changeHornState(mob, 1) writeback.
// Dual-wire of Go darkixion.CanRestoreHorn (slice 3187 dedicated; residual 2893).
inline auto CanRestoreHorn(const int32 animSub) -> bool
{
    return animSub == kAnimHornBroken;
}

// ---------------------------------------------------------------------------
// Slice 2907 / 3206 — HornBreakRoll pure dual-wire
//
// Formula (slice 3206 dedicated dual-wire; residual expand 2907; pure inject
// 0985 — formula unchanged):
//   HornBreakRoll(roll1to100) =
//     roll1to100 >= 1 && roll1to100 <= HornBreakChancePercent
//
// Dual-wire of Go darkixion.HornBreakRoll.
// Call site: future Lua checkHornBreak inject after CanBreakHorn gate.
// Prior pure port: slice 0985. Residual dual-wire suite: 2907 /
// test_darkixion_horn_break_roll_2907. Dedicated dual-wire suite is
// test_darkixion_horn_break_roll_3206. Host still owns CanBreakHorn gate
// inject, math.random(1, 100), and changeHornState(mob, 2) writeback.
// ---------------------------------------------------------------------------

// HornBreakChancePercent is the 5% roll ceiling on critical / weaponskill hit
// (math.random(1, 100) <= 5 → ~5%).
// Dual-wire of Go darkixion.HornBreakChancePercent.
inline constexpr int32 HornBreakChancePercent = 5;

// HornBreakRoll is the pure roll half of checkHornBreak after CanBreakHorn:
//
//   roll1to100 >= 1 && roll1to100 <= HornBreakChancePercent
//
// Formula (slice 3206 dedicated dual-wire; residual expand 2907 / pure 0985 —
// formula unchanged). Implemented with a defensive lower bound so out-of-range
// rolls do not spuriously succeed. roll1to100 is the host-injected
// math.random(1, 100) result. Host still owns CanBreakHorn gate inject, RNG
// generation, and changeHornState(mob, 2) writeback.
// Dual-wire of Go darkixion.HornBreakRoll (slice 3206 dedicated; residual 2907).
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

// ---------------------------------------------------------------------------
// Slice 2915 residual / 3389 dedicated — ShouldDoubleGlowSkill pure dual-wire
//
// Formula (slice 3389 dedicated dual-wire expand residual 2915; pure inject
// 0985 — formula unchanged):
//   ShouldDoubleGlowSkill(animSub) =
//     animSub == AnimGlowing
//
// Dual-wire of Go darkixion.ShouldDoubleGlowSkill.
// Call site: future Lua DI_GLOW inject after first useMobAbility(chosenSkill).
// Prior pure port: slice 0985. Residual dual-wire suite: 2915 /
// test_darkixion_double_glow_2915. Dedicated dual-wire suite is
// test_darkixion_double_glow_3389. Host still owns getAnimationSub inject,
// skill-list pick (randomEntry), setBehavior / setAutoAttackEnabled, and
// useMobAbility writeback (first and optional second queue of the chosen skill).
// ---------------------------------------------------------------------------

// ShouldDoubleGlowSkill is the pure gate for DI_GLOW double-up of the
// follow-up TP move:
//
//   animSub == AnimGlowing
//
// Formula (slice 3389 dedicated dual-wire expand residual 2915 / pure 0985 —
// formula unchanged). Host still owns getAnimationSub inject, skill-list pick
// (randomEntry), setBehavior / setAutoAttackEnabled, and useMobAbility
// writeback (first and optional second queue of the chosen skill).
// Dual-wire of Go darkixion.ShouldDoubleGlowSkill (slice 3389 dedicated expand
// residual 2915; residual 2915 retained).
inline auto ShouldDoubleGlowSkill(const int32 animSub) -> bool
{
    return animSub == kAnimGlowing;
}

} // namespace darkixionhelpers
