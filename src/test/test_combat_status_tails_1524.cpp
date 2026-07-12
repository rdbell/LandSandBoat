#include "test_combat_status_tails_1524.h"

#include "map/combat_status_tails_capacity.h"

#include <iostream>

namespace
{
using combatstatustailshelpers::AfflatusMiseryAccuracyBonus;
using combatstatustailshelpers::CalculateSpikeDamagePreMDT;
using combatstatustailshelpers::ShouldApplySpikeHealMessage;
using combatstatustailshelpers::ShouldSetAfflatusMiseryDamage;
using combatstatustailshelpers::SpikesParamFromMod;
using combatstatustailshelpers::TacticalTPBonus;

auto Check() -> bool
{
    if (AfflatusMiseryAccuracyBonus(false, true, 0).applied || AfflatusMiseryAccuracyBonus(true, false, 0).applied)
    {
        return false;
    }
    if (AfflatusMiseryAccuracyBonus(true, true, 30).applied)
    {
        return false;
    }
    {
        const auto r = AfflatusMiseryAccuracyBonus(true, true, 0);
        if (!r.applied || r.newSubPower != 10 || r.accDelta != 10)
        {
            return false;
        }
    }
    {
        const auto r = AfflatusMiseryAccuracyBonus(true, true, 20);
        if (!r.applied || r.newSubPower != 30 || r.accDelta != 10)
        {
            return false;
        }
    }
    if (!ShouldSetAfflatusMiseryDamage(true, 1) || ShouldSetAfflatusMiseryDamage(true, 0) ||
        ShouldSetAfflatusMiseryDamage(false, 10))
    {
        return false;
    }

    {
        std::int16_t tp = 0;
        if (TacticalTPBonus(false, true, 50, tp) || tp != 0)
        {
            return false;
        }
        if (TacticalTPBonus(true, false, 50, tp) || tp != 0)
        {
            return false;
        }
        if (!TacticalTPBonus(true, true, 75, tp) || tp != 75)
        {
            return false;
        }
    }

    {
        const auto r = CalculateSpikeDamagePreMDT(100, 0, false, 0, false);
        if (r.damage != 100 || r.clampElementToFire)
        {
            return false;
        }
    }
    {
        const auto r = CalculateSpikeDamagePreMDT(100, 50, false, 0, false);
        if (r.damage != 150)
        {
            return false;
        }
    }
    {
        const auto r = CalculateSpikeDamagePreMDT(100, 50, true, 40, false);
        if (r.damage != 40)
        {
            return false;
        }
    }
    {
        const auto r = CalculateSpikeDamagePreMDT(10, 0, false, 0, true);
        if (!r.clampElementToFire || r.damage != 10)
        {
            return false;
        }
    }

    if (SpikesParamFromMod(-5) != 0 || SpikesParamFromMod(12) != 12)
    {
        return false;
    }
    if (!ShouldApplySpikeHealMessage(-1) || ShouldApplySpikeHealMessage(0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCombatStatusTails1524SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "combat_status_tails_1524 self-tests failed\n";
        return false;
    }
    return true;
}
