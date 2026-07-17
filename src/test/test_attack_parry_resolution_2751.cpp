#include "test_attack_parry_resolution_2751.h"

#include "map/attack_capacity.h"

auto runAttackParryResolution2751SelfTests() -> bool
{
    using namespace attackhelpers;

    const auto normalProc = ResolveParryCheck(false, AttackTypeNormal, true);
    const auto normalMiss = ResolveParryCheck(false, AttackTypeNormal, false);
    const auto existing   = ResolveParryCheck(true, AttackTypeNormal, false);
    const auto daken      = ResolveParryCheck(false, AttackTypeDaken, true);
    const auto dakenPrior = ResolveParryCheck(true, AttackTypeDaken, false);

    return normalProc.parried && !normalMiss.parried && existing.parried && !daken.parried && dakenPrior.parried;
}
