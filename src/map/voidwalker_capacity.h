#pragma once

#include "common/cbasetypes.h"

// Pure Voidwalker helpers for dual-wire slices:
//   - 2884: ShouldUpgradeKI (checkUpgrade math.random(1,10)==5)
//   - 2903: ShouldRandomly (local randomly chance / effect / cooldown)
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
// Host injects scalars only (no player / mob pointers):
//   roll         — math.random(1, 10) for upgrade / math.random(0, 100) for randomly
//   chance       — randomly chance percent
//   hasEffect    — mob:hasStatusEffect(effect)
//   now          — GetSystemTime()
//   lastSkillTime — mob:getLocalVar('MOBSKILL_TIME')
//   between      — cooldown seconds between skill uses
//
// Zone/player identity checks, delKeyItem / addKeyItem, messageSpecial,
// hasStatusEffect, setLocalVar, and useMobAbility writeback remain host-owned.
// Prior pure port: OmegaXI slice 0987 (internal/voidwalker).
// Dual-wire of Go voidwalker.ShouldUpgradeKI / UpgradeRollSuccess (slice 2884).
// Dual-wire of Go voidwalker.ShouldRandomly / RandomlyRollMax (slice 2903).

namespace voidwalkerhelpers
{

// Upgrade-roll range for checkUpgrade: math.random(1, 10); success when
// roll == UpgradeRollSuccess (5).
inline constexpr int32 UpgradeRollMin     = 1;
inline constexpr int32 UpgradeRollMax     = 10;
inline constexpr int32 UpgradeRollSuccess = 5;

// ShouldUpgradeKI is the pure upgrade roll half of checkUpgrade once the
// host injects the RNG scalar:
//
//   roll == UpgradeRollSuccess  // math.random(1, 10) == 5
//
// Dual-wire of Go voidwalker.ShouldUpgradeKI.
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

} // namespace voidwalkerhelpers
