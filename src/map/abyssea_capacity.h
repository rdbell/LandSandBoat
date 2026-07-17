#pragma once

#include "common/cbasetypes.h"

// Pure Abyssea helpers shared by dual-wire slices:
//   - 2861: canGiveNMKI residual dual-wire suite / normal+atma compose
//   - 2866: BuffPower residual dual-wire suite (visions cruor enhance)
//   - 2868: DecodeSurveyorOption (Conflux Surveyor finish option parse)
//   - 3089: CanGiveNMKI dedicated dual-wire (can_give_nmki.go)
//   - 3148: BuffPower dedicated dual-wire (prospector.go / buff_power_test.go)
//   - 3238: CanGiveAtmaNMKI prior dedicated dual-wire expand residual 2861
//           (prior dedicated 3089; formula unchanged)
//   - 3284: CanGiveAtmaNMKI prior dedicated dual-wire expand residual 2861
//           (prior ~3238/3089; formula unchanged)
//   - 3314: CanGiveAtmaNMKI prior dedicated dual-wire expand residual 2861
//           (prior ~3284; formula unchanged)
//   - 3434: CanGiveNMKI dedicated dual-wire expand residual 2861
//           (prior 3314 / 3284 / 3238 / 3089; formula unchanged)
//
// Dual-wire index:
//   - 2861: CanGiveNMKI residual dual-wire suite
//   - 2866: BuffPower residual dual-wire suite
//   - 2868: DecodeSurveyorOption
//   - 3089: CanGiveNMKI prior dedicated (roll1to100 <= dropChance || redProc)
//   - 3148: BuffPower (base + abyssiteTotal * mult)
//   - 3238: CanGiveAtmaNMKI prior expand (CanGiveNMKI(roll, AtmaNMKIDropChance/*10*/, redProc))
//   - 3284: CanGiveAtmaNMKI prior expand (CanGiveNMKI(roll, AtmaNMKIDropChance/*10*/, redProc))
//   - 3314: CanGiveAtmaNMKI prior expand (CanGiveNMKI(roll, AtmaNMKIDropChance/*10*/, redProc))
//   - 3434: CanGiveNMKI (roll1to100 <= dropChance || redProc)
//
// Lua production host (2861 / 3089 / 3238 / 3284 / 3314 / 3434): scripts/globals/abyssea.lua
// xi.abyssea.canGiveNMKI:
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
// Go dual-wire: abyssea.CanGiveNMKI / CanGiveAtmaNMKI
// (internal/abyssea/can_give_nmki.go).
// Future Lua host injects CanGiveNMKI / CanGiveAtmaNMKI then npcUtil.giveKeyItem.

namespace abysseahelpers
{

// ---------------------------------------------------------------------------
// Slice 2861 / 3089 / 3238 / 3284 / 3314 / 3434 — canGiveNMKI roll / red-proc gate
// ---------------------------------------------------------------------------

// Normal / atma drop-chance pins from giveNMDrops.
// Parity: Go NormalNMKIDropChance / AtmaNMKIDropChance.
inline constexpr int32 NormalNMKIDropChance = 20;
inline constexpr int32 AtmaNMKIDropChance   = 10;

// CanGiveNMKI mirrors xi.abyssea.canGiveNMKI:
//   if math.random(1, 100) <= dropChance or redProcValue == 1 then return true end
//
// Formula (slice 3434 dedicated dual-wire expand residual 2861; prior
// dedicated 3089; residual expand 2861; pure inject 1041; CanGiveAtmaNMKI
// prior expand residual 3314 / 3284 / 3238 leave this free function body
// unchanged — positive OR form, QF1001-safe):
//   CanGiveNMKI(roll1to100, dropChance, redProc)
//     = roll1to100 <= dropChance || redProc
//
// roll1to100 — host-injected math.random(1, 100) result
// dropChance — percentage chance (20 normal / 10 atma)
// redProc    — true when [AbysseaRedProc] local var == 1 (forces drop)
// true  → host grants KI (npcUtil.giveKeyItem)
// false → no KI from this roll
//
// Dual-wire of Go abyssea.CanGiveNMKI.
// Call site: future Lua canGiveNMKI / giveNMDrops inject.
// Prior pure port: slice 1041. Residual dual-wire suite: 2861 /
// test_abyssea_can_give_nmki_2861. Prior dedicated dual-wire suite:
// test_abyssea_can_give_nmki_3089. Prior CanGiveAtmaNMKI expand suites:
// test_abyssea_can_give_nmki_3238 / test_abyssea_can_give_nmki_3284 /
// test_abyssea_can_give_nmki_3314. Dedicated dual-wire expand residual
// suite is test_abyssea_can_give_nmki_3434. Host still owns math.random,
// getLocalVar, and KI grant writeback. Red proc is pre-normalized to bool
// before the pure gate.
inline auto CanGiveNMKI(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

// CanGiveNormalNMKI dual-wires giveNMDrops normal-drop chance 20 through
// CanGiveNMKI (production compose for deferred giveNMDrops host; residual
// sibling under 3434 — leave alone if already dual-wired).
inline auto CanGiveNormalNMKI(const int32 roll1to100, const bool redProc) -> bool
{
    return CanGiveNMKI(roll1to100, NormalNMKIDropChance, redProc);
}

// CanGiveAtmaNMKI dual-wires giveNMDrops atma-drop chance 10 through
// CanGiveNMKI (production compose for deferred giveNMDrops host).
//
// Formula (prior slice 3314 dedicated dual-wire expand residual 2861; prior
// ~3284 — formula unchanged). Residual sibling under 3434 (CanGiveNMKI
// primary expand leaves this compose helper alone):
//   CanGiveAtmaNMKI(roll1to100, redProc)
//     = CanGiveNMKI(roll1to100, AtmaNMKIDropChance /*10*/, redProc)
//     = roll1to100 <= 10 || redProc
//
// Dual-wire of Go abyssea.CanGiveAtmaNMKI.
// Call site: future Lua giveNMDrops atma path inject.
// Residual dual-wire suite: 2861 / test_abyssea_can_give_nmki_2861.
// Prior dedicated: 3089 / test_abyssea_can_give_nmki_3089.
// Prior CanGiveAtmaNMKI expand: 3238 / test_abyssea_can_give_nmki_3238.
// Prior CanGiveAtmaNMKI expand: 3284 / test_abyssea_can_give_nmki_3284.
// Prior CanGiveAtmaNMKI expand: 3314 / test_abyssea_can_give_nmki_3314.
// CanGiveNMKI dedicated dual-wire expand residual suite is
// test_abyssea_can_give_nmki_3434.
inline auto CanGiveAtmaNMKI(const int32 roll1to100, const bool redProc) -> bool
{
    return CanGiveNMKI(roll1to100, AtmaNMKIDropChance, redProc);
}

// ---------------------------------------------------------------------------
// Slice 2866 / 3148 — Visions cruor prospector BuffPower
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
// Go dual-wire: abyssea.BuffPower (internal/abyssea/prospector.go).
// Future Lua host injects BuffPower then applies status / HP / MP / cruor.
// ---------------------------------------------------------------------------

// BuffPower is the pure free-function form of the ENHANCEMENT power formula:
//   base + abyssiteTotal * mult
//
// Formula (slice 3148 dedicated dual-wire; residual expand 2866; pure inject
// 1046 — formula unchanged):
//   BuffPower(base, abyssiteTotal, mult) = base + abyssiteTotal * mult
//
// Dual-wire of Go abyssea.BuffPower.
// Call site: future Lua visionsCruorProspectorOnEventFinish inject.
// Prior pure port: slice 1046. Residual dual-wire suite: 2866 /
// test_abyssea_buff_power_2866. Dedicated dual-wire suite is
// test_abyssea_buff_power_3148. Host still owns getAbyssiteTotal,
// addStatusEffect, addHP / addMP, and delCurrency writeback.
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
