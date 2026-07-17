#pragma once

#include "common/cbasetypes.h"

#include <string>

// Pure Voidwalker helpers for dual-wire slices:
//   - 2884: ShouldUpgradeKI residual dual-wire expand
//   - 3173: ShouldUpgradeKI prior dedicated dual-wire
//           (roll == UpgradeRollSuccess (5);
//            residual expand 2884 / pure 0987)
//   - 3377: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3173 / residual expand 2884 / pure 0987 —
//            formula unchanged)
//   - 3433: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3377 / 3173 / residual expand 2884 / pure 0987 —
//            formula unchanged)
//   - 3482: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3433 / 3377 / 3173 / residual expand 2884 /
//            pure 0987 — formula unchanged)
//   - 3527: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3482 / 3433 / 3377 / 3173 / residual expand 2884 /
//            pure 0987 — formula unchanged)
//   - 3563: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3527 / 3482 / 3433 / 3377 / 3173 / residual expand
//            2884 / pure 0987 — formula unchanged)
//   - 3608: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3563 / 3527 / 3482 / 3433 / 3377 / 3173 /
//            residual expand 2884 / pure 0987 — formula unchanged)
//   - 3653: ShouldUpgradeKI prior dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3608 / 3563 / 3527 / 3482 / 3433 / 3377 / 3173 /
//            residual expand 2884 / pure 0987 — formula unchanged)
//   - 3698: ShouldUpgradeKI dedicated dual-wire expand residual 2884
//           (roll == UpgradeRollSuccess (5);
//            prior dedicated 3653 / 3608 / 3563 / 3527 / 3482 / 3433 / 3377 /
//            3173 / residual expand 2884 / pure 0987 — formula unchanged)
//   - 2903: ShouldRandomly residual dual-wire expand
//   - 3213: ShouldRandomly dedicated dual-wire
//           (roll <= chance && !hasEffect && now > (last + between);
//            residual expand 2903 / pure 0987)
//   - 2908: ShouldDoMobSkillEveryHPP residual dual-wire expand
//   - 3334: ShouldDoMobSkillEveryHPP dedicated dual-wire
//           (every/start/condition/mod/localVar gates;
//            residual expand 2908 / pure 0987)
//   - residual 0987: pure voidwalker helpers (prior pure port of ShouldUpgradeKI)
//
// Lua production host: scripts/globals/voidwalker.lua
//   local function checkUpgrade(player, mob, nextKeyItem)
//     ...
//     local rand = math.random(1, 10)
//     if rand == 5 then
//       -- delKeyItem / addKeyItem / messageSpecial
//     end
//   end
//
//   local function randomly(mob, chance, between, effect, skill)
//     if
//       math.random(0, 100) <= chance and
//       not mob:hasStatusEffect(effect) and
//       GetSystemTime() > (mob:getLocalVar('MOBSKILL_TIME') + between)
//     then
//       mob:setLocalVar('MOBSKILL_USE', 1)
//       mob:setLocalVar('MOBSKILL_TIME', GetSystemTime())
//       mob:useMobAbility(skill)
//     end
//   end
//
//   local function doMobSkillEveryHPP(mob, every, start, mobskill, condition)
//     local mobhpp = mob:getHPP()
//     if mobhpp <= start and condition then
//       local isSame = (start % every) == (mobhpp % every)
//       if isSame and mob:getLocalVar('MOB_SKILL_' .. mobhpp) == 0 then
//         mob:useMobAbility(mobskill)
//         mob:setLocalVar('MOB_SKILL_' .. mobhpp, 1)
//       end
//     end
//   end
//
// Host injects scalars only (no player / mob pointers):
//   roll         — math.random(1, 10) for upgrade / math.random(0, 100) for randomly
//   chance       — randomly chance percent
//   hasEffect    — mob:hasStatusEffect(effect)
//   now          — GetSystemTime()
//   lastSkillTime — mob:getLocalVar('MOBSKILL_TIME')
//   between      — cooldown seconds between skill uses
//   mobHPP       — mob:getHPP()
//   every / start — HPP step / ceiling for doMobSkillEveryHPP
//   condition    — host status / always-true gate
//   localVarSet  — getLocalVar('MOB_SKILL_' .. mobhpp) != 0
//
// Zone/player identity checks, delKeyItem / addKeyItem, messageSpecial,
// hasStatusEffect, setLocalVar, and useMobAbility writeback remain host-owned.
// Prior pure port: OmegaXI slice 0987 (internal/voidwalker).
// Go dual-wire: voidwalker.ShouldUpgradeKI / UpgradeRollSuccess
// (internal/voidwalker/upgrade_ki.go). Prior pure port: slice 0987.
// Residual dual-wire suite: 2884 / test_voidwalker_upgrade_ki_2884.
// Prior dedicated dual-wire suite: 3173 / test_voidwalker_should_upgrade_ki_3173.
// Prior dedicated dual-wire expand residual 2884 suite: 3377 /
// test_voidwalker_should_upgrade_ki_3377.
// Prior dedicated dual-wire expand residual 2884 suite: 3433 /
// test_voidwalker_should_upgrade_ki_3433.
// Prior dedicated dual-wire expand residual 2884 suite: 3482 /
// test_voidwalker_should_upgrade_ki_3482.
// Prior dedicated dual-wire expand residual 2884 suite: 3527 /
// test_voidwalker_should_upgrade_ki_3527.
// Prior dedicated dual-wire expand residual 2884 suite: 3563 /
// test_voidwalker_should_upgrade_ki_3563.
// Prior dedicated dual-wire expand residual 2884 suite: 3608 /
// test_voidwalker_should_upgrade_ki_3608.
// Prior dedicated dual-wire expand residual 2884 suite: 3653 /
// test_voidwalker_should_upgrade_ki_3653.
// Dedicated dual-wire expand residual 2884 suite: 3698 /
// test_voidwalker_should_upgrade_ki_3698.
// Dual-wire of Go voidwalker.ShouldRandomly / RandomlyRollMax
// (internal/voidwalker/should_randomly.go). Prior pure port: slice 0987.
// Residual dual-wire suite: 2903 / test_voidwalker_should_randomly_2903.
// Dedicated dual-wire suite: 3213 / test_voidwalker_should_randomly_3213.
// Dual-wire of Go voidwalker.ShouldDoMobSkillEveryHPP / MobSkillLocalVar
// (internal/voidwalker/should_do_mob_skill_every_hpp.go). Prior pure port: slice 0987.
// Residual dual-wire suite: 2908 / test_voidwalker_mob_skill_hpp_2908.
// Dedicated dual-wire suite: 3334 / test_voidwalker_mob_skill_hpp_3334.

namespace voidwalkerhelpers
{

// Upgrade-roll range for checkUpgrade: math.random(1, 10); success when
// roll == UpgradeRollSuccess (5).
// Dual-wire constants (dedicated expand residual 3698; prior dedicated 3653 /
// 3608 / 3563 / 3527 / 3482 / 3433 / 3377 / 3173; residual 2884 / pure 0987 —
// formula unchanged).
inline constexpr int32 UpgradeRollMin     = 1;
inline constexpr int32 UpgradeRollMax     = 10;
inline constexpr int32 UpgradeRollSuccess = 5;

// ShouldUpgradeKI is the pure upgrade roll half of checkUpgrade once the
// host injects the RNG scalar:
//
// Formula (slice 3698 dedicated dual-wire expand residual 2884; prior
// dedicated 3653 / 3608 / 3563 / 3527 / 3482 / 3433 / 3377 / 3173 / residual
// expand 2884 / pure 0987 — formula unchanged):
//   roll == UpgradeRollSuccess  // math.random(1, 10) == 5
//
// Host-injected scalars (no player / mob pointers):
//   roll — math.random(1, 10)
// true  → checkUpgrade may del/add KI + messageSpecial
// false → no upgrade this kill
//
// Dual-wire of Go voidwalker.ShouldUpgradeKI
// (internal/voidwalker/upgrade_ki.go). Prior pure port: slice 0987.
// Residual dual-wire suite: 2884 / test_voidwalker_upgrade_ki_2884.
// Prior dedicated dual-wire suite: 3173 / test_voidwalker_should_upgrade_ki_3173.
// Prior dedicated dual-wire expand residual 2884 suites:
// test_voidwalker_should_upgrade_ki_3377,
// test_voidwalker_should_upgrade_ki_3433,
// test_voidwalker_should_upgrade_ki_3482,
// test_voidwalker_should_upgrade_ki_3527,
// test_voidwalker_should_upgrade_ki_3563,
// test_voidwalker_should_upgrade_ki_3608,
// test_voidwalker_should_upgrade_ki_3653.
// Dedicated dual-wire expand residual 2884 suite is
// test_voidwalker_should_upgrade_ki_3698. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite.
// Call site (deferred): Lua checkUpgrade host inject after math.random(1, 10).
inline auto ShouldUpgradeKI(const int32 roll) -> bool
{
    return roll == UpgradeRollSuccess;
}

// Randomly roll range for local randomly: math.random(0, 100).
// Host injects roll in [0, RandomlyRollMax].
// Dual-wire constants (dedicated 3213; residual 2903 / pure 0987).
inline constexpr int32 RandomlyRollMax = 100;

// ShouldRandomly is the pure inject of local randomly once the host supplies
// chance / status / cooldown scalars:
//
// Formula (slice 3213 dedicated dual-wire; residual expand 2903 / pure 0987 —
// formula unchanged):
//   roll <= chance
//     and not hasEffect
//     and now > (lastSkillTime + between)
//
// Host-injected scalars (no player / mob pointers):
//   roll          — math.random(0, 100)
//   chance        — percent threshold (production: 10 or 30)
//   hasEffect     — mob:hasStatusEffect(effect)
//   now           — GetSystemTime()
//   lastSkillTime — mob:getLocalVar('MOBSKILL_TIME')
//   between       — cooldown seconds (production: 60)
// true  → host sets MOBSKILL_USE=1, MOBSKILL_TIME=now, useMobAbility(skill)
// false → no skill this tick
//
// Dual-wire of Go voidwalker.ShouldRandomly
// (internal/voidwalker/should_randomly.go). Prior pure port: slice 0987.
// Residual dual-wire suite: 2903 / test_voidwalker_should_randomly_2903.
// Dedicated dual-wire suite is test_voidwalker_should_randomly_3213. Formula
// is unchanged; this slice only expands dual-wire docs + index + dedicated suite.
// Call site (deferred): Lua randomly host inject after math.random(0, 100).
// Production keeps the compound conjunction (including identity-not of
// hasEffect). Dual-wire pin may use positive if/else form for lint-stable
// cross-checks (avoid QF1001 De Morgan rewrites of !hasEffect).
inline auto ShouldRandomly(const int32 roll,
                           const int32 chance,
                           const bool  hasEffect,
                           const int64 now,
                           const int64 lastSkillTime,
                           const int64 between) -> bool
{
    return roll <= chance && !hasEffect && now > (lastSkillTime + between);
}

// ShouldDoMobSkillEveryHPP is the pure half of local doMobSkillEveryHPP once
// the host supplies HPP / step / condition / local-var scalars:
//
// Formula (slice 3334 dedicated dual-wire; residual expand 2908 / pure 0987 —
// formula unchanged):
//   if every <= 0 || mobHPP > start || !condition: false
//   if (start % every) != (mobHPP % every): false
//   return !localVarSet
//
// Host-injected scalars (no player / mob pointers):
//   mobHPP      — mob:getHPP()
//   every       — HPP step (production: 20)
//   start       — HPP ceiling (production: 80 or 82)
//   condition   — status / always-true host gate
//   localVarSet — getLocalVar('MOB_SKILL_' .. mobhpp) != 0
// every <= 0 returns false (Lua % 0 errors; hosts never pass 0).
// true  → host useMobAbility + setLocalVar('MOB_SKILL_'..hpp, 1)
// false → no skill this tick
//
// Dual-wire of Go voidwalker.ShouldDoMobSkillEveryHPP
// (internal/voidwalker/should_do_mob_skill_every_hpp.go). Prior pure port: slice 0987.
// Residual dual-wire suite: 2908 / test_voidwalker_mob_skill_hpp_2908.
// Dedicated dual-wire suite is test_voidwalker_mob_skill_hpp_3334. Formula
// is unchanged; this slice only expands dual-wire docs + index + dedicated suite.
// Call site (deferred): Lua doMobSkillEveryHPP host inject after getHPP /
// getLocalVar. Production keeps early-return guards and terminal !localVarSet.
// Dual-wire pin may use positive if/else form for lint-stable cross-checks
// (avoid QF1001 De Morgan rewrites of !localVarSet / !condition):
//   if every <= 0 { return false }
//   if mobHPP > start { return false }
//   if !condition { return false }
//   if (start % every) != (mobHPP % every) { return false }
//   if localVarSet { return false }
//   return true
inline auto ShouldDoMobSkillEveryHPP(const int32 mobHPP,
                                     const int32 every,
                                     const int32 start,
                                     const bool  condition,
                                     const bool  localVarSet) -> bool
{
    if (every <= 0 || mobHPP > start || !condition)
    {
        return false;
    }
    if ((start % every) != (mobHPP % every))
    {
        return false;
    }
    return !localVarSet;
}

// MobSkillLocalVar is the local-var name 'MOB_SKILL_' .. mobhpp used by
// doMobSkillEveryHPP to fire once per HPP threshold.
//
// Dual-wire of Go voidwalker.MobSkillLocalVar
// (dedicated 3334; residual 2908 / pure 0987 — formula unchanged).
inline auto MobSkillLocalVar(const int32 mobHPP) -> std::string
{
    return "MOB_SKILL_" + std::to_string(mobHPP);
}

} // namespace voidwalkerhelpers
