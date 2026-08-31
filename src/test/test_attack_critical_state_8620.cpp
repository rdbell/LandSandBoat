#include "test_attack_critical_state_8620.h"

#include "map/attack_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{
using namespace attackhelpers;

auto Check() -> bool
{
    bool ok = true;
    const auto expect = [&](const bool value, const char* label)
    {
        if (!value)
        {
            std::cerr << "attack critical state 8620: " << label << '\n';
        }
        ok = value && ok;
    };

    const auto daken = ResolveCriticalState(true, true, 2.25f, 1.75f);
    expect(daken.isCritical, "Daken sets critical flag");
    expect(daken.damageRatio == 2.25f, "Daken selects ranged ratio");

    const auto melee = ResolveCriticalState(true, false, 2.25f, 1.75f);
    expect(melee.isCritical, "melee sets critical flag");
    expect(melee.damageRatio == 1.75f, "melee selects melee ratio");

    const auto cleared = ResolveCriticalState(false, true, 0.5f, 1.5f);
    expect(!cleared.isCritical, "clearing critical flag is preserved");
    expect(cleared.damageRatio == 0.5f, "clearing still selects Daken ratio");

    const auto kick = ResolveCriticalState(false, false, 0.5f, 0.0f);
    expect(!kick.isCritical && kick.damageRatio == 0.0f,
           "non-Daken branch selects melee ratio for every other type");
    return ok;
}
} // namespace

auto runAttackCriticalState8620SelfTests() -> bool
{
    return Check();
}

OMEGA_REGISTER_SELF_TEST("attack-critical-state-8620", runAttackCriticalState8620SelfTests);
