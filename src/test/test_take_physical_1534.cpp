#include "test_take_physical_1534.h"

#include "map/take_physical_capacity.h"

#include <iostream>

namespace
{
using takephysicalhelpers::ApplyBlockAbsorb;
using takephysicalhelpers::ApplyFormlessPower;
using takephysicalhelpers::ApplySDT;
using takephysicalhelpers::ApplyShieldDefBonus;
using takephysicalhelpers::FlipCounterAbsorb;
using takephysicalhelpers::FormlessAbsorbNull;
using takephysicalhelpers::FormlessAbsorbNullResult;
using takephysicalhelpers::FormlessStrikesMod;
using takephysicalhelpers::GiveTPToAttacker;
using takephysicalhelpers::GiveTPToVictim;
using takephysicalhelpers::ReprisalBlockedDamage;
using takephysicalhelpers::ReprisalEffectPower;
using takephysicalhelpers::ReprisalSpikesDamage;
using takephysicalhelpers::ShieldBlockAbsorb;
using takephysicalhelpers::ShouldAddShieldMasteryTP;
using takephysicalhelpers::ShouldApplyReprisalSpikes;
using takephysicalhelpers::ShouldClaimOnPhysicalDamage;
using takephysicalhelpers::ShouldTryHitInterruptPhysical;
using takephysicalhelpers::ShouldUseFormlessStrikesPath;
using takephysicalhelpers::UseTypeSpecificSDT;

auto Check() -> bool
{
    if (!GiveTPToAttacker(true, false) || GiveTPToAttacker(true, true))
    {
        return false;
    }
    if (!GiveTPToVictim(true, 0) || GiveTPToVictim(true, 9))
    {
        return false;
    }
    if (!ShouldUseFormlessStrikesPath(true, false) || ShouldUseFormlessStrikesPath(true, true))
    {
        return false;
    }
    if (FormlessStrikesMod(true, 10) != 65 || ApplyFormlessPower(100, 60) != 60)
    {
        return false;
    }
    {
        std::int32_t out = 0;
        if (FormlessAbsorbNull(40, true, false, false, out) != FormlessAbsorbNullResult::Absorbed || out != -40)
        {
            return false;
        }
        if (FormlessAbsorbNull(40, false, true, false, out) != FormlessAbsorbNullResult::Nulled || out != 0)
        {
            return false;
        }
        if (FormlessAbsorbNull(40, false, false, false, out) != FormlessAbsorbNullResult::PassThrough || out != 40)
        {
            return false;
        }
    }
    if (FlipCounterAbsorb(-30, true) != 30 || ApplySDT(100, 2500) != 125)
    {
        return false;
    }
    if (!UseTypeSpecificSDT(false, false) || UseTypeSpecificSDT(true, false))
    {
        return false;
    }
    if (ApplyShieldDefBonus(100, 30) != 70 || !ShouldAddShieldMasteryTP(50, 10, 20))
    {
        return false;
    }
    if (ShieldBlockAbsorb(false, false, 0) != 50 || ShieldBlockAbsorb(true, true, 40) != 60)
    {
        return false;
    }
    if (ApplyBlockAbsorb(200, 50) != 100)
    {
        return false;
    }
    if (!ShouldApplyReprisalSpikes(10, true) || ReprisalEffectPower(50, 10) != 55)
    {
        return false;
    }
    if (ReprisalBlockedDamage(100, 200, 50, false) != 50 || ReprisalBlockedDamage(100, 200, 50, true) != 100)
    {
        return false;
    }
    if (ReprisalSpikesDamage(100, 50) != 50)
    {
        return false;
    }
    if (!ShouldClaimOnPhysicalDamage(true, false) || ShouldClaimOnPhysicalDamage(true, true))
    {
        return false;
    }
    if (!ShouldTryHitInterruptPhysical(false, false, true, true) ||
        ShouldTryHitInterruptPhysical(true, false, true, true) ||
        ShouldTryHitInterruptPhysical(false, true, true, true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runTakePhysical1534SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "take_physical_1534 self-tests failed\n";
        return false;
    }
    return true;
}
