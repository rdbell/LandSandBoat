#include "test_attack_critical_ratio_2745.h"
#include "map/attack_capacity.h"
#include <cmath>
#include <iostream>

auto runAttackCriticalRatio2745SelfTests() -> bool
{
    using namespace attackhelpers;
    bool       ok      = true;
    const auto daken   = ResolveCriticalRatioModifiers(true, false, false, 0, true, 20, 3);
    ok                 = daken.rangedAttackBonus == 60 && std::fabs(daken.meleeAttackBonus - 1.0f) < 0.0001f && ok;
    const auto wrapped = ResolveCriticalRatioModifiers(true, false, false, 0, true, -1, 1);
    ok                 = wrapped.rangedAttackBonus == 0xFFFF && ok;
    const auto kick    = ResolveCriticalRatioModifiers(false, true, true, 128, false, 0, 0);
    ok                 = std::fabs(kick.meleeAttackBonus - 1.5f) < 0.0001f && ok;
    const auto normal  = ResolveCriticalRatioModifiers(false, false, true, 128, true, 20, 3);
    ok                 = normal.rangedAttackBonus == 0 && std::fabs(normal.meleeAttackBonus - 1.0f) < 0.0001f && ok;
    if (!ok)
        std::cerr << "attack critical ratio 2745 self-test failed\n";
    return ok;
}
