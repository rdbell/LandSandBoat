#pragma once

#include "common/cbasetypes.h"

// Pure Abyssea helpers shared by dual-wire slices:
//   - 2861: canGiveNMKI / normal+atma compose
//   - 2866: BuffPower (visions cruor prospector enhance)
//   - 2868: DecodeSurveyorOption (Conflux Surveyor finish option parse)
//
// Lua production host (2861): scripts/globals/abyssea.lua xi.abyssea.canGiveNMKI:
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

// ---------------------------------------------------------------------------
// Visions cruor prospector BuffPower (slice 2866)
//
// Lua production host: scripts/globals/abyssea.lua
//   visionsCruorProspectorOnEventFinish ENHANCEMENT path:
//
//   power = v[3] + xi.abyssea.getAbyssiteTotal(player, v[4]) * v[5]
//
// Host injects scalars only (no player / entity pointers):
//   base          — row base power (v[3]; HP 20 / others 10)
//   abyssiteTotal — getAbyssiteTotal for the row abyssite type (v[4])
//   mult          — row multiplier (v[5]; HP×10, MP×5, stats×10)
//
// Entity addStatusEffect / addHP / addMP / delCurrency remains host-owned.
// ---------------------------------------------------------------------------

// BuffPower is the pure free-function form of the ENHANCEMENT power formula:
//   base + abyssiteTotal * mult
// Future Lua host injects scalars into this helper instead of re-inlining
// the arithmetic in visionsCruorProspectorOnEventFinish.
inline auto BuffPower(const int32 base, const int32 abyssiteTotal, const int32 mult) -> int32
{
    return base + abyssiteTotal * mult;
}

// ---------------------------------------------------------------------------
// Slice 2868 — Conflux Surveyor finish option field parse
// ---------------------------------------------------------------------------
//
// Lua production host: scripts/globals/abyssea/conflux_surveyor.lua
// xi.abyssea.surveyorOnEventFinish:
//
//   local optionSelected = bit.band(option, 0xF)
//   local additionalStones = math.min(bit.rshift(option, 16),
//                                     xi.abyssea.getHeldTraverserStones(player))
//
// DecodeSurveyorOption dual-wires only the pure bit splits (choice nibble +
// high-word requested stones). Host still owns held-stone clamp, CSID 2001 /
// choice 2|3 gate, Visitant duration mutation, spendTravStones, and lights.

// SurveyorOption mirrors Go abyssea.SurveyorOption:
//   Choice          = option & 0xF
//   RequestedStones = option >> 16   (before held-stone clamp)
struct SurveyorOption
{
    uint8 choice{};
    int32 requestedStones{};
};

// DecodeSurveyorOption is the pure free-function form of the Lua option bit
// splits. Future Lua host injects the event option into this helper instead
// of re-inlining bit.band / bit.rshift.
inline auto DecodeSurveyorOption(const uint32 option) -> SurveyorOption
{
    return SurveyorOption{
        .choice          = static_cast<uint8>(option & 0x0FU),
        .requestedStones = static_cast<int32>(option >> 16),
    };
}

} // namespace abysseahelpers
