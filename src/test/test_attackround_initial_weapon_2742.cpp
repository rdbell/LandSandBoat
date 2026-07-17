#include "test_attackround_initial_weapon_2742.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround initial weapon 2742 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAttackRoundInitialWeapon2742SelfTests() -> bool
{
    using namespace attackroundhelpers;

    bool ok = true;

    const auto noWeapons = PlanInitialWeaponAttacks(false, false, false, false, false);
    ok                   = expect(noWeapons.mainAttackCalls == 0 && !noWeapons.mainUsesLeftDirection && !noWeapons.createSubAttack, "empty") && ok;

    const auto normal = PlanInitialWeaponAttacks(true, false, false, true, true);
    ok                = expect(normal.mainAttackCalls == 1 && !normal.mainUsesLeftDirection && normal.createSubAttack, "normal dual wield") && ok;

    const auto h2h = PlanInitialWeaponAttacks(true, true, false, true, true);
    ok             = expect(h2h.mainAttackCalls == 2 && h2h.mainUsesLeftDirection && h2h.createSubAttack, "h2h two calls plus sub") && ok;

    const auto h2hSingle = PlanInitialWeaponAttacks(true, true, true, false, false);
    ok                   = expect(h2hSingle.mainAttackCalls == 1 && h2hSingle.mainUsesLeftDirection && !h2hSingle.createSubAttack, "h2h single swing") && ok;

    const auto subOnly = PlanInitialWeaponAttacks(false, true, true, true, true);
    ok                 = expect(subOnly.mainAttackCalls == 0 && !subOnly.mainUsesLeftDirection && subOnly.createSubAttack, "sub independent of main") && ok;

    return ok;
}
