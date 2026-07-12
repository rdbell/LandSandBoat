#include "test_take_damage_1533.h"

#include "map/take_damage_capacity.h"

#include <iostream>

namespace
{
using takedamagehelpers::ApplyPhalanx;
using takedamagehelpers::ApplyWSDefenseAndNull;
using takedamagehelpers::ClampWSDamage;
using takedamagehelpers::CorrectedDamageAfterTake;
using takedamagehelpers::IsWSRangedSlot;
using takedamagehelpers::ShouldApplyOverwhelm;
using takedamagehelpers::ShouldApplySpellDamageEffects;
using takedamagehelpers::ShouldApplySwipeLungeHitEffects;
using takedamagehelpers::ShouldApplyWSPhalanxStoneskin;
using takedamagehelpers::ShouldClaimOnWSDamage;
using takedamagehelpers::ShouldProcessWSHitEffects;
using takedamagehelpers::ShouldRemoveHagakure;
using takedamagehelpers::ShouldUpdateEnmityFromZeroWS;
using takedamagehelpers::ShouldUpdateMobHiPCLvl;
using takedamagehelpers::WSAttackerAddTP;
using takedamagehelpers::WSDefenseBoostNullifies;
using takedamagehelpers::WSDefenderTP;
using takedamagehelpers::WSStandbyTP;

auto Check() -> bool
{
    if (!IsWSRangedSlot(2) || !IsWSRangedSlot(3) || IsWSRangedSlot(0))
    {
        return false;
    }
    if (!WSDefenseBoostNullifies(1, true, 64, true) || WSDefenseBoostNullifies(1, true, 0, true))
    {
        return false;
    }
    if (ApplyWSDefenseAndNull(100, true, 1, false, false) != 0)
    {
        return false;
    }
    if (ApplyWSDefenseAndNull(100, false, 3, true, false) != 0)
    {
        return false;
    }
    if (ApplyWSDefenseAndNull(100, false, 2, true, true) != 100)
    {
        return false;
    }
    if (!ShouldApplyWSPhalanxStoneskin(1) || ShouldApplyWSPhalanxStoneskin(0))
    {
        return false;
    }
    if (ApplyPhalanx(50, 20) != 30 || ApplyPhalanx(10, 20) != 0)
    {
        return false;
    }
    if (!ShouldApplyOverwhelm(false) || ShouldApplyOverwhelm(true))
    {
        return false;
    }
    if (ClampWSDamage(100000) != 99999 || ClampWSDamage(-100000) != -99999)
    {
        return false;
    }
    if (CorrectedDamageAfterTake(-50, 40) != -40 || CorrectedDamageAfterTake(50, 40) != 50)
    {
        return false;
    }
    if (!ShouldClaimOnWSDamage(true) || ShouldClaimOnWSDamage(false))
    {
        return false;
    }
    if (!ShouldProcessWSHitEffects(1) || ShouldUpdateEnmityFromZeroWS(1, true) || !ShouldUpdateEnmityFromZeroWS(0, true))
    {
        return false;
    }
    if (!ShouldUpdateMobHiPCLvl(40, 50) || ShouldUpdateMobHiPCLvl(50, 50))
    {
        return false;
    }
    if (WSStandbyTP(true, 100, 1.5f, 40) != 160 || WSStandbyTP(false, 100, 1.5f, 40) != 0)
    {
        return false;
    }
    if (WSDefenderTP(1.0f, 0.5f, 80) != 40)
    {
        return false;
    }
    if (WSAttackerAddTP(200, 160) != 200 || WSAttackerAddTP(100, 160) != 160)
    {
        return false;
    }
    if (!ShouldRemoveHagakure(true) || ShouldRemoveHagakure(false))
    {
        return false;
    }
    if (!ShouldApplySpellDamageEffects(true, 1) || ShouldApplySpellDamageEffects(false, 10))
    {
        return false;
    }
    if (!ShouldApplySwipeLungeHitEffects(1) || ShouldApplySwipeLungeHitEffects(0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runTakeDamage1533SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "take_damage_1533 self-tests failed\n";
        return false;
    }
    return true;
}
