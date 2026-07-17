#pragma once

#include "common/cbasetypes.h"

// Pure Voidwalker helpers for dual-wire slices:
//   - 2884: ShouldUpgradeKI (checkUpgrade math.random(1,10)==5)
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
// Host injects scalars only (no player / mob pointers):
//   roll — math.random(1, 10)
//
// Zone/player identity checks, delKeyItem / addKeyItem, and messageSpecial
// writeback remain host-owned.
// Prior pure port: OmegaXI slice 0987 (internal/voidwalker).
// Dual-wire of Go voidwalker.ShouldUpgradeKI / UpgradeRollSuccess (slice 2884).

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

} // namespace voidwalkerhelpers
