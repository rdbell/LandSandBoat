#pragma once

#include "common/cbasetypes.h"

// Pure Abyssea canGiveNMKI policy helpers (slice 2861).
//
// Lua production host: scripts/globals/abyssea.lua xi.abyssea.canGiveNMKI:
//
//   local redProcValue = mob:getLocalVar('[AbysseaRedProc]')
//   if math.random(1, 100) <= dropChance or redProcValue == 1 then
//     return true
//   end
//   return false
//
// giveNMDrops calls canGiveNMKI(mob, 20) for normal KIs and canGiveNMKI(mob, 10)
// for atma KIs.
//
// Host injects scalars only (no entity / mob pointers):
//   roll1to100 — math.random(1, 100) result
//   dropChance — percentage chance (20 normal / 10 atma)
//   redProc    — true when [AbysseaRedProc] local var == 1
//
// Key-item grant writeback remains host-owned.

namespace abysseahelpers
{

// Normal / atma drop-chance pins from giveNMDrops.
inline constexpr int32 NormalNMKIDropChance = 20;
inline constexpr int32 AtmaNMKIDropChance   = 10;

// CanGiveNMKI is the pure free-function form of xi.abyssea.canGiveNMKI:
//   roll1to100 <= dropChance || redProc
// Future Lua host injects scalars into this helper instead of re-inlining
// the comparison.
inline auto CanGiveNMKI(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

// CanGiveNormalNMKI dual-wires giveNMDrops normal-drop chance 20.
inline auto CanGiveNormalNMKI(const int32 roll1to100, const bool redProc) -> bool
{
    return CanGiveNMKI(roll1to100, NormalNMKIDropChance, redProc);
}

// CanGiveAtmaNMKI dual-wires giveNMDrops atma-drop chance 10.
inline auto CanGiveAtmaNMKI(const int32 roll1to100, const bool redProc) -> bool
{
    return CanGiveNMKI(roll1to100, AtmaNMKIDropChance, redProc);
}

} // namespace abysseahelpers
