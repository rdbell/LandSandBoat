#pragma once

#include <algorithm>
#include <cstdint>

// Pure battleutils::CalculateSpellCost after entity/spell injects.
// Parity: internal/spell.CalculateSpellCost
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 0813 / 1546 / 2104: CalculateSpellCost + zero gates residual suite
//   - 2964: ShouldReturnZeroNullSpell residual dual-wire suite (spellNull)
//   - 2969: ShouldReturnZeroNoMPCost residual dual-wire suite (!hasMPCost)
//   - 3183: ShouldReturnZeroNoMPCost prior dedicated dual-wire
//           (no_mp_cost.go; expand residual 2969 / pure 2104; suite retained)
//   - 3210: ShouldReturnZeroNullSpell dedicated dual-wire
//           (null_spell_cost.go; expand residual 2964 / pure 2104)
//   - 3402: ShouldReturnZeroNoMPCost prior dedicated dual-wire
//           (no_mp_cost.go; expand residual 2969 / prior 3183 / pure 2104;
//           suite retained)
//   - 3454: ShouldReturnZeroNoMPCost dedicated dual-wire
//           (no_mp_cost.go; expand residual 2969 / prior 3402 / prior 3183 /
//           pure 2104)
//
// Production host: battleutils::CalculateSpellCost injects
// PSpell == nullptr into ShouldReturnZeroNullSpell before hasMPCost / cost,
// then PSpell->hasMPCost() into ShouldReturnZeroNoMPCost before pure cost body.
// Go dual-wire: spell.ShouldReturnZeroNullSpell
// (internal/spell/null_spell_cost.go) and spell.ShouldReturnZeroNoMPCost
// (internal/spell/no_mp_cost.go).

namespace spellcosthelpers
{

constexpr std::uint8_t AOENone       = 0;
constexpr std::uint8_t AOERadialMani = 3;
constexpr std::uint8_t AOERadialAcce = 4;

constexpr std::uint16_t SpellGroupNone  = 0;
constexpr std::uint16_t SpellGroupBlack = 2;
constexpr std::uint16_t SpellGroupWhite = 6;

constexpr std::uint16_t IDEmbrava = 478;
constexpr std::uint16_t IDKaustra = 502;

constexpr std::int16_t SpellCostMax = 9999;

// ---------------------------------------------------------------------------
// Slice 3210 — CalculateSpellCost null-spell gate
// (dedicated expand residual 2964 / pure 2104)
// ---------------------------------------------------------------------------

// ShouldReturnZeroNullSpell mirrors CalculateSpellCost's PSpell == nullptr gate.
// Host returns 0 (and logs a warning) before hasMPCost / cost work when true.
//
// Formula (slice 3210 dedicated dual-wire; residual expand 2964 / pure 2104 —
// formula unchanged):
//   spellNull
//
// spellNull — host-evaluated PSpell == nullptr
// true  → return 0 (null spell short-circuit)
// false → null gate passes; host continues to hasMPCost / pure cost body
//
// Dual-wire of Go spell.ShouldReturnZeroNullSpell
// (residual 2104 / residual dual-wire 2964 / dedicated dual-wire 3210).
// Host inject (battleutils::CalculateSpellCost):
//   if (ShouldReturnZeroNullSpell(PSpell == nullptr)) return 0;
//
// Residual dual-wire suite: 2964 (test_spell_null_cost_2964).
// Dedicated dual-wire suite: 3210 (test_spell_return_zero_null_spell_3210).
// Sibling residual only (not re-expanded under 3210):
// ShouldReturnZeroNoMPCost (3454 dedicated; residual 2969; prior 3402 / 3183).
constexpr auto ShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

// ---------------------------------------------------------------------------
// Slice 3454 — CalculateSpellCost no-MP-cost gate
// (dedicated expand residual 2969 / prior dedicated 3402 / prior dedicated
// 3183 / pure 2104)
// ---------------------------------------------------------------------------

// ShouldReturnZeroNoMPCost mirrors CalculateSpellCost's hasMPCost short-circuit
// after the null-spell gate. Host returns 0 (ninja tools / bard songs / trusts)
// before stratagem / mod injects and the pure cost body when true.
//
// Formula (slice 3454 dedicated dual-wire; residual expand 2969 / prior
// dedicated 3402 / prior dedicated 3183 / pure 2104 — formula unchanged):
//   !hasMPCost
//
// hasMPCost — host-evaluated PSpell->hasMPCost() (only after non-null)
// true  → has MP cost; gate passes; host continues to pure cost body
// false → return 0 (no-MP-cost short-circuit)
//
// Dual-wire of Go spell.ShouldReturnZeroNoMPCost
// (residual 2104 / residual dual-wire 2969 / prior dedicated 3183 /
// prior dedicated 3402 / dedicated dual-wire 3454).
// Host inject (battleutils::CalculateSpellCost):
//   if (ShouldReturnZeroNoMPCost(PSpell->hasMPCost())) return 0;
//
// Evaluated only when ShouldReturnZeroNullSpell passes (2964).
// Residual dual-wire suite: 2969 (test_spell_no_mp_cost_2969).
// Prior dedicated dual-wire suite: 3183
// (test_spell_return_zero_no_mp_cost_3183; retained).
// Prior dedicated dual-wire suite: 3402
// (test_spell_return_zero_no_mp_cost_3402; retained).
// Dedicated dual-wire suite: 3454 (test_spell_return_zero_no_mp_cost_3454).
// Sibling residual only (not re-expanded under 3454):
// ShouldReturnZeroNullSpell (2964 dual-wire), CanUseSpellWith (3159).
constexpr auto ShouldReturnZeroNoMPCost(const bool hasMPCost) -> bool
{
    return !hasMPCost;
}

// Full pure cost once fields and status/mods are injected.
// noMPDepletion is the result of: roll < NO_SPELL_MP_DEPLETION mod (host rolls).
inline auto CalculateSpellCost(const std::uint16_t spellID,
                               const std::uint16_t spellGroup,
                               const std::uint8_t  aoe,
                               const std::uint16_t baseMPCost,
                               const std::uint16_t maxMP,
                               const bool          manifestation,
                               const bool          parsimony,
                               const bool          accession,
                               const bool          penury,
                               const std::int16_t  blackMagicCost,
                               const std::int16_t  whiteMagicCost,
                               const std::int16_t  mpCostReduction,
                               const bool          noMPDepletion) -> std::uint16_t
{
    auto base = baseMPCost;
    if (spellID == IDEmbrava || spellID == IDKaustra)
    {
        base = static_cast<std::uint16_t>(static_cast<float>(maxMP) * 0.2f);
    }

    auto cost      = static_cast<std::int16_t>(base);
    auto applyArts = true;

    if (spellGroup == SpellGroupBlack)
    {
        if (aoe == AOERadialMani && manifestation)
        {
            cost *= 2;
            applyArts = false;
        }
        if (parsimony)
        {
            cost /= 2;
            applyArts = false;
        }
        else if (applyArts)
        {
            cost += static_cast<std::int16_t>(static_cast<float>(base) * (static_cast<float>(blackMagicCost) / 100.0f));
        }
    }
    else if (spellGroup == SpellGroupWhite)
    {
        if (aoe == AOERadialAcce && accession)
        {
            cost *= 2;
            applyArts = false;
        }
        if (penury)
        {
            cost /= 2;
            applyArts = false;
        }
        else if (applyArts)
        {
            cost += static_cast<std::int16_t>(static_cast<float>(base) * (static_cast<float>(whiteMagicCost) / 100.0f));
        }
    }

    if (mpCostReduction > 0)
    {
        cost = static_cast<std::int16_t>(static_cast<float>(cost) * (1.f - static_cast<float>(mpCostReduction) / 100.f));
    }

    if (noMPDepletion)
    {
        cost = 0;
    }

    return static_cast<std::uint16_t>(std::clamp<std::int16_t>(cost, 0, SpellCostMax));
}

} // namespace spellcosthelpers
