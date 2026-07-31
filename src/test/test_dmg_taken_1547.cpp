#include "test_dmg_taken_1547.h"

#include "map/dmg_taken_capacity.h"

#include <iostream>

namespace
{
using dmgtakenhelpers::AbsorbNullOutcome;
using dmgtakenhelpers::ApplyAbsorbNull;
using dmgtakenhelpers::ApplyLiement;
using dmgtakenhelpers::MagicResist;
using dmgtakenhelpers::MagicalOutcome;
using dmgtakenhelpers::PhysicalOutcome;
using dmgtakenhelpers::PhysicalResist;
using dmgtakenhelpers::RangedOutcome;
using dmgtakenhelpers::RangedResist;
using dmgtakenhelpers::ShouldLiementShortCircuit;

auto Check() -> bool
{
    // Magic: no mods → full damage
    if (MagicResist(100, 0, 0, 0, 0) != 100)
    {
        return false;
    }
    // UDMG -50% → 50
    if (MagicResist(100, -5000, 0, 0, 0) != 50)
    {
        return false;
    }
    // DMG floor 0.5: -100% DMGMAGIC still 50
    if (MagicResist(100, 0, -10000, 0, 0) != 50)
    {
        return false;
    }
    // MDT II can go to 0.125: -50% + -50% II → need check: base 0.5 + (-0.5) = 0 then floor 0.125 → 12
    if (MagicResist(100, 0, -5000, 0, -5000) != 12)
    {
        return false;
    }

    // Physical equalizer: 100 dmg, maxHP 100, equalizer 50 → rate floor(50)/100=0.5 → 50
    if (PhysicalResist(100, 0, 0, 0, 0, 50, 100) != 50)
    {
        return false;
    }
    // Ranged no II
    if (RangedResist(100, 0, 0, 0, 0, 100) != 100)
    {
        return false;
    }

    if (MagicalOutcome(true, false, false, true, true, true, true) != AbsorbNullOutcome::Absorb)
    {
        return false;
    }
    if (MagicalOutcome(false, false, false, true, true, false, false) != AbsorbNullOutcome::Null)
    {
        return false;
    }
    if (MagicalOutcome(false, false, true, false, false, false, true) != AbsorbNullOutcome::Pass)
    {
        // element false → element absorb/null ignored
        return false;
    }
    if (PhysicalOutcome(false, true, false, true) != AbsorbNullOutcome::Absorb)
    {
        return false;
    }
    if (RangedOutcome(false, false, false, true) != AbsorbNullOutcome::Null)
    {
        return false;
    }
    if (ApplyAbsorbNull(40, AbsorbNullOutcome::Absorb) != -40 || ApplyAbsorbNull(40, AbsorbNullOutcome::Null) != 0)
    {
        return false;
    }
    if (!ShouldLiementShortCircuit(-1.0f) || ShouldLiementShortCircuit(0.0f) || ShouldLiementShortCircuit(1.0f))
    {
        return false;
    }
    if (ApplyLiement(100, -0.5f) != -50)
    {
        return false;
    }
    return true;
}
} // namespace

auto runDmgTaken1547SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "dmg_taken_1547 self-tests failed\n";
        return false;
    }
    return true;
}
