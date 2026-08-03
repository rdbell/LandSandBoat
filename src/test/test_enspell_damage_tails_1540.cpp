#include "test_enspell_damage_tails_1540.h"

#include "map/enspell_damage_tails_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using enspelldamagetailshelpers::ApplyEnspellDamageMultiplier;
using enspelldamagetailshelpers::ApplyEnspellResistAndDayBonus;
using enspelldamagetailshelpers::CalculateEnspellTier3Damage;
using enspelldamagetailshelpers::ClampEnspellFinalDamage;
using enspelldamagetailshelpers::EnspellBonusFromExclude;
using enspelldamagetailshelpers::EnspellDamageMultiplier;
using enspelldamagetailshelpers::EnspellDayWeatherBonus;
using enspelldamagetailshelpers::EnspellNonWeaponPct;
using enspelldamagetailshelpers::EnspellPctApplicable;
using enspelldamagetailshelpers::EnspellResistLadder;
using enspelldamagetailshelpers::EnspellRuneDPS;
using enspelldamagetailshelpers::EnspellRuneMinMax;
using enspelldamagetailshelpers::ShouldApplyComposureEnspellBonus;

auto Check() -> bool
{
    if (EnspellBonusFromExclude(10, 3) != 7)
    {
        return false;
    }
    {
        bool decay = false;
        bool rem   = false;
        if (CalculateEnspellTier3Damage(5, decay, rem) != 5 || !decay || rem)
        {
            return false;
        }
        if (CalculateEnspellTier3Damage(1, decay, rem) != 1 || decay || !rem)
        {
            return false;
        }
    }
    if (std::fabs(EnspellRuneDPS(0.0, false, false) - (3.0 / 240.0)) > 1e-9)
    {
        return false;
    }
    if (EnspellRuneDPS(20.0, true, true) != 10.0 || EnspellRuneDPS(30.0, true, false) != 21.0)
    {
        return false;
    }
    {
        double min = 0;
        double max = 0;
        EnspellRuneMinMax(10.0, 1, min, max);
        if (min != std::floor(10.0 * 0.97) || max != std::floor(10.0 * 1.30))
        {
            return false;
        }
    }
    if (EnspellNonWeaponPct(15, 20) != 0 || EnspellNonWeaponPct(30, 10) != 20)
    {
        return false;
    }
    if (!ShouldApplyComposureEnspellBonus(true, true, true, 1, 3) ||
        ShouldApplyComposureEnspellBonus(true, true, true, 1, 7))
    {
        return false;
    }
    if (std::fabs(EnspellDamageMultiplier(20, 10, true) - 3.3f) > 1e-5f)
    {
        return false;
    }
    if (ApplyEnspellDamageMultiplier(1, EnspellDamageMultiplier(18, 82, true)) != 4)
    {
        return false;
    }
    if (ApplyEnspellDamageMultiplier(10, 3.3f) != 33)
    {
        return false;
    }
    if (EnspellPctApplicable(30, 12) != 18)
    {
        return false;
    }
    if (EnspellResistLadder(50, 0.01) != 0.0625f || EnspellResistLadder(50, 0.6) != 1.0f)
    {
        return false;
    }
    if (EnspellDayWeatherBonus(true, 0.0f, 1, 0, true, false) != 1.1f)
    {
        return false;
    }
    if (ApplyEnspellResistAndDayBonus(100, 0.5f, 1.1f) != 55)
    {
        return false;
    }
    if (ClampEnspellFinalDamage(100000) != 99999 || ClampEnspellFinalDamage(-100000) != -99999)
    {
        return false;
    }
    return true;
}
} // namespace

auto runEnspellDamageTails1540SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "enspell_damage_tails_1540 self-tests failed\n";
        return false;
    }
    return true;
}
