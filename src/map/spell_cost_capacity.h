#pragma once

#include <algorithm>
#include <cstdint>

// Pure battleutils::CalculateSpellCost after entity/spell injects.
// Parity: internal/spell.CalculateSpellCost
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 0813 / 1546 / 2104: CalculateSpellCost + zero gates residual suite
//   - 2964: ShouldReturnZeroNullSpell (spellNull identity)
//
// Production host: battleutils::CalculateSpellCost injects
// PSpell == nullptr into ShouldReturnZeroNullSpell before hasMPCost / cost.
// Go dual-wire: spell.ShouldReturnZeroNullSpell
// (internal/spell/null_spell_cost.go).
//
// Sibling residual: ShouldReturnZeroNoMPCost (!hasMPCost) remains 2104 and is
// not dual-wired in slice 2964.

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

// ShouldReturnZeroNullSpell mirrors CalculateSpellCost's PSpell == nullptr gate.
// Host returns 0 (and logs a warning) before hasMPCost / cost work when true.
//
// Formula (slice 2964 dual-wire):
//   spellNull
//
// spellNull — host-evaluated PSpell == nullptr
// true  → return 0 (null spell short-circuit)
// false → null gate passes; host continues to hasMPCost / pure cost body
//
// Dual-wire of Go spell.ShouldReturnZeroNullSpell.
// Host inject (battleutils::CalculateSpellCost):
//   if (ShouldReturnZeroNullSpell(PSpell == nullptr)) return 0;
//
// ShouldReturnZeroNoMPCost remains 2104 residual sibling (not dual-wired here).
constexpr auto ShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

// ShouldReturnZeroNoMPCost mirrors hasMPCost short-circuit after the null gate.
// Residual 2104; evaluated only when ShouldReturnZeroNullSpell passes (2964).
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
