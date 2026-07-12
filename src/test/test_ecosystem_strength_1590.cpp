#include "test_ecosystem_strength_1590.h"

#include "map/ecosystem_strength_capacity.h"

#include <iostream>

namespace
{
using namespace ecosystemstrengthhelpers;

auto Check() -> bool
{
    // Beast > Lizard, Beast < Plantoid
    if (StrengthBonus(EcoBeast, EcoLizard) != 1 || StrengthBonus(EcoBeast, EcoPlantoid) != -1)
    {
        return false;
    }
    // Bird > Aquan, Bird < Amorph
    if (StrengthBonus(EcoBird, EcoAquan) != 1 || StrengthBonus(EcoBird, EcoAmorph) != -1)
    {
        return false;
    }
    // Undead > Arcana mutual
    if (StrengthBonus(EcoUndead, EcoArcana) != 1 || StrengthBonus(EcoArcana, EcoUndead) != 1)
    {
        return false;
    }
    // Unlisted pairs
    if (StrengthBonus(EcoBeast, EcoBeast) != 0 || StrengthBonus(0, EcoBeast) != 0)
    {
        return false;
    }
    // Correlation advantage is strict > 0
    if (!HasMonsterCorrelationAdvantage(EcoBeast, EcoLizard) ||
        HasMonsterCorrelationAdvantage(EcoBeast, EcoPlantoid) ||
        HasMonsterCorrelationAdvantage(EcoBeast, EcoBeast))
    {
        return false;
    }
    return true;
}
} // namespace

auto runEcosystemStrength1590SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "ecosystem_strength_1590 self-tests failed\n";
        return false;
    }
    return true;
}
