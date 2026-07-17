#pragma once

#include "common/cbasetypes.h"

#include <string>

// Pure Voidwalker helpers for dual-wire slices:
//   - 2884: ShouldUpgradeKI residual dual-wire expand
//   - 3173: ShouldUpgradeKI dedicated dual-wire
//           (roll == UpgradeRollSuccess (5);
//            residual expand 2884 / pure 0987)
//   - 2903: ShouldRandomly (local randomly chance / effect / cooldown)
//   - 2908: ShouldDoMobSkillEveryHPP (local doMobSkillEveryHPP HPP-modulo gate)
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
// Dedicated dual-wire suite: 3173 / test_voidwalker_should_upgrade_ki_3173.
// Dual-wire of Go voidwalker.ShouldRandomly / RandomlyRollMax (slice 2903).
// Dual-wire of Go voidwalker.ShouldDoMobSkillEveryHPP / MobSkillLocalVar (slice 2908).

namespace voidwalkerhelpers
{

// Upgrade-roll range for checkUpgrade: math.random(1, 10); success when
// roll == UpgradeRollSuccess (5).
// Dual-wire constants (dedicated 3173; residual 2884 / pure 0987).
inline constexpr int32 UpgradeRollMin     = 1;
inline constexpr int32 UpgradeRollMax     = 10;
inline constexpr int32 UpgradeRollSuccess = 5;

// ShouldUpgradeKI is the pure upgrade roll half of checkUpgrade once the
// host injects the RNG scalar:
//
// Formula (slice 3173 dedicated dual-wire; residual expand 2884 / pure 0987 —
// formula unchanged):
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
// Dedicated dual-wire suite is test_voidwalker_should_upgrade_ki_3173. Formula
// is unchanged; this slice only expands dual-wire docs + index + dedicated suite.
// Call site (deferred): Lua checkUpgrade host inject after math.random(1, 10).
inline auto ShouldUpgradeKI(const int32 roll) -> bool
{
    return roll == UpgradeRollSuccess;
}

// Randomly roll range for local randomly: math.random(0, 100).
// Host injects roll in [0, RandomlyRollMax].
inline constexpr int32 RandomlyRollMax = 100;

// ShouldRandomly is the pure inject of local randomly once the host supplies
// chance / status / cooldown scalars:
//
//   roll <= chance
//     and not hasEffect
//     and now > (lastSkillTime + between)
//
// When true, the host should set MOBSKILL_USE=1, MOBSKILL_TIME=now, and
// useMobAbility(skill).
//
// Dual-wire of Go voidwalker.ShouldRandomly.
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
//   if every <= 0 || mobHPP > start || !condition: false
//   if (start % every) != (mobHPP % every): false
//   return !localVarSet
//
// every <= 0 returns false (Lua % 0 errors; hosts never pass 0).
// When true, the host should useMobAbility and setLocalVar('MOB_SKILL_'..hpp, 1).
//
// Dual-wire of Go voidwalker.ShouldDoMobSkillEveryHPP.
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
// Dual-wire of Go voidwalker.MobSkillLocalVar.
inline auto MobSkillLocalVar(const int32 mobHPP) -> std::string
{
    return "MOB_SKILL_" + std::to_string(mobHPP);
}

} // namespace voidwalkerhelpers
