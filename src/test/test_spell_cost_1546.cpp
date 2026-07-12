#include "test_spell_cost_1546.h"

#include "map/spell_cost_capacity.h"

#include <iostream>

namespace
{
using spellcosthelpers::AOERadialAcce;
using spellcosthelpers::AOERadialMani;
using spellcosthelpers::CalculateSpellCost;
using spellcosthelpers::IDEmbrava;
using spellcosthelpers::ShouldReturnZeroNoMPCost;
using spellcosthelpers::ShouldReturnZeroNullSpell;
using spellcosthelpers::SpellGroupBlack;
using spellcosthelpers::SpellGroupWhite;

auto Check() -> bool
{
    if (!ShouldReturnZeroNullSpell(true) || ShouldReturnZeroNullSpell(false))
    {
        return false;
    }
    if (!ShouldReturnZeroNoMPCost(false) || ShouldReturnZeroNoMPCost(true))
    {
        return false;
    }
    // Base passthrough
    if (CalculateSpellCost(1, 0, 0, 100, 0, false, false, false, false, 0, 0, 0, false) != 100)
    {
        return false;
    }
    // Embrava 20% of 500 = 100
    if (CalculateSpellCost(IDEmbrava, SpellGroupWhite, 0, 0, 500, false, false, false, false, 0, 0, 0, false) != 100)
    {
        return false;
    }
    // Black Manifestation doubles
    if (CalculateSpellCost(1, SpellGroupBlack, AOERadialMani, 50, 0, true, false, false, false, 0, 0, 0, false) != 100)
    {
        return false;
    }
    // Black Parsimony halves (no arts)
    if (CalculateSpellCost(1, SpellGroupBlack, 0, 100, 0, false, true, false, false, 20, 0, 0, false) != 50)
    {
        return false;
    }
    // Black arts +20% of base 100 → 120
    if (CalculateSpellCost(1, SpellGroupBlack, 0, 100, 0, false, false, false, false, 20, 0, 0, false) != 120)
    {
        return false;
    }
    // White Accession doubles
    if (CalculateSpellCost(1, SpellGroupWhite, AOERadialAcce, 40, 0, false, false, true, false, 0, 0, 0, false) != 80)
    {
        return false;
    }
    // MP cost reduction 50% of 100 → 50
    if (CalculateSpellCost(1, 0, 0, 100, 0, false, false, false, false, 0, 0, 50, false) != 50)
    {
        return false;
    }
    // No MP depletion roll hit
    if (CalculateSpellCost(1, 0, 0, 100, 0, false, false, false, false, 0, 0, 0, true) != 0)
    {
        return false;
    }
    // Clamp max
    if (CalculateSpellCost(1, 0, 0, 20000, 0, false, false, false, false, 0, 0, 0, false) != 9999)
    {
        return false;
    }
    return true;
}
} // namespace

auto runSpellCost1546SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spell_cost_1546 self-tests failed\n";
        return false;
    }
    return true;
}
