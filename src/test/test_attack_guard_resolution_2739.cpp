#include "test_attack_guard_resolution_2739.h"

#include "map/attack_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "attack guard resolution 2739 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackGuardResolution2739SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    const auto unguarded = ResolveGuardCheck(false, 1.75f);
    ok = (!unguarded.guarded) && ok;
    ok = expectNear(unguarded.damageRatio, 1.75f, "unguarded preserves ratio") && ok;

    const auto guarded = ResolveGuardCheck(true, 1.75f);
    ok = guarded.guarded && ok;
    ok = expectNear(guarded.damageRatio, 0.75f, "guarded subtracts one") && ok;

    const auto floored = ResolveGuardCheck(true, 0.25f);
    ok = floored.guarded && ok;
    ok = expectNear(floored.damageRatio, 0.0f, "guarded ratio floors at zero") && ok;

    return ok;
}
