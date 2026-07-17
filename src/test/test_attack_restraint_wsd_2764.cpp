#include "test_attack_restraint_wsd_2764.h"

#include "map/attack_capacity.h"

#include <iostream>

auto runAttackRestraintWsd2764SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    ok = ShouldApplyRestraintBoost(true, true, true) && ok;
    ok = !ShouldApplyRestraintBoost(false, true, true) && ok;
    ok = !ShouldApplyRestraintBoost(true, false, true) && ok;
    ok = !ShouldApplyRestraintBoost(true, true, false) && ok;

    // Base delay 4000ms ≈ 0.623 → floor 0, sub ≈ 62.
    const auto base = ComputeRestraintWSDBoost(4000, 0, 0, 0, 0);
    ok              = base.applies && base.boostAmount == 0 && base.newSubPower == 62 && ok;

    // subPower 50 carries into floor 1, rem ≈ 12.
    const auto carry = ComputeRestraintWSDBoost(4000, 0, 50, 0, 0);
    ok               = carry.applies && carry.boostAmount == 1 && carry.newSubPower == 12 && ok;

    // JP and Enhances only move remainder at this delay.
    const auto jp = ComputeRestraintWSDBoost(4000, 0, 0, 0, 20);
    ok            = jp.applies && jp.boostAmount == 0 && jp.newSubPower == 74 && ok;

    const auto enh = ComputeRestraintWSDBoost(4000, 0, 0, 10, 0);
    ok             = enh.applies && enh.boostAmount == 0 && enh.newSubPower == 68 && ok;

    // Cap trim: power 28 with large floor → boost = 2.
    const auto capped = ComputeRestraintWSDBoost(20000, 28, 95, 0, 0);
    ok                = capped.applies && capped.boostAmount == 2 && ok;

    // Integral boost stores remainder 100 (ceil quirk).
    const auto integral = ComputeRestraintWSDBoost(0, 0, 200, 0, 0);
    ok                  = integral.applies && integral.boostAmount == 2 && integral.newSubPower == 100 && ok;

    // Resolve gate rejects non-first / no effect / at-cap; preserves subPower.
    const auto gated = ResolveRestraintWSDBoost(false, true, 0, 7, 4000, 0, 0);
    ok               = !gated.applies && gated.newSubPower == 7 && ok;

    const auto atCap = ResolveRestraintWSDBoost(true, true, 30, 40, 4000, 0, 0);
    ok               = !atCap.applies && atCap.newSubPower == 40 && ok;

    const auto resolved = ResolveRestraintWSDBoost(true, true, 0, 50, 4000, 0, 0);
    ok                  = resolved.applies && resolved.boostAmount == 1 && resolved.newSubPower == 12 && ok;

    if (!ok)
    {
        std::cerr << "attack restraint wsd 2764 self-test failed\n";
    }
    return ok;
}
