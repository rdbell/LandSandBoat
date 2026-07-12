#include "test_spell_interrupt_1563.h"

#include "map/spell_interrupt_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace spellinterrupthelpers;

auto NearlyEqual(const float a, const float b, const float eps = 1e-5f) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    // Level ratio floor
    if (!NearlyEqual(LevelRatio(1, 99, false), LevelRatioMin))
    {
        return false;
    }
    // PC base 50: equal levels → 0.50
    if (!NearlyEqual(LevelRatio(75, 75, false), 0.50f))
    {
        return false;
    }
    // Mob base 5: equal levels → 0.05
    if (!NearlyEqual(LevelRatio(75, 75, true), 0.05f))
    {
        return false;
    }

    // Skill ratio
    if (!NearlyEqual(SkillRatio(100, 50, false), 1.0f))
    {
        return false;
    }
    if (!NearlyEqual(SkillRatio(100, 0, true), ZeroSkillRatio))
    {
        return false;
    }
    if (!NearlyEqual(SkillRatio(100, 50, true), 2.0f))
    {
        return false;
    }

    // SIRD
    if (!NearlyEqual(SIRDRatio(0, 0), 1.0f) || !NearlyEqual(SIRDRatio(0, 100), 0.0f))
    {
        return false;
    }
    // 102% SIRD → negative
    if (!(SIRDRatio(0, 102) < 0.0f))
    {
        return false;
    }

    // Evaluate: roll safe
    {
        Params p{};
        p.attackerLevel = 75;
        p.defenderLevel = 75;
        p.defenderIsPC  = true;
        p.skillCap      = 100;
        p.skillLevel    = 100;
        p.roll          = 0.99f;
        const auto r    = Evaluate(p);
        if (r.interrupted || r.rollInterrupted)
        {
            return false;
        }
    }
    // Evaluate: interrupt without aquaveil
    {
        Params p{};
        p.attackerLevel = 75;
        p.defenderLevel = 75;
        p.defenderIsPC  = true;
        p.skillCap      = 100;
        p.skillLevel    = 100;
        // final = 0.5 * 1 * 1 = 0.5
        p.roll = 0.0f;
        const auto r = Evaluate(p);
        if (!r.interrupted || !r.rollInterrupted || r.aquaveilConsumed)
        {
            return false;
        }
    }
    // Aquaveil prevents interrupt, power 1 → delete
    {
        Params p{};
        p.attackerLevel  = 75;
        p.defenderLevel  = 75;
        p.defenderIsPC   = true;
        p.skillCap       = 100;
        p.skillLevel     = 100;
        p.hasAquaveil    = true;
        p.aquaveilPower  = 1;
        p.roll           = 0.0f;
        const auto r     = Evaluate(p);
        if (r.interrupted || !r.aquaveilConsumed || !r.aquaveilDelete)
        {
            return false;
        }
    }
    // Aquaveil power 3 → new power 2
    {
        Params p{};
        p.attackerLevel  = 75;
        p.defenderLevel  = 75;
        p.defenderIsPC   = true;
        p.skillCap       = 100;
        p.skillLevel     = 100;
        p.hasAquaveil    = true;
        p.aquaveilPower  = 3;
        p.roll           = 0.0f;
        const auto r     = Evaluate(p);
        if (r.interrupted || !r.aquaveilConsumed || r.aquaveilDelete || r.aquaveilNewPower != 2)
        {
            return false;
        }
    }
    // Overcap SIRD never interrupts
    {
        Params p{};
        p.attackerLevel     = 99;
        p.defenderLevel     = 1;
        p.defenderIsPC      = true;
        p.skillCap          = 300;
        p.skillLevel        = 1;
        p.spellInterruptMod = 102;
        p.roll              = 0.0f;
        const auto r        = Evaluate(p);
        if (r.interrupted || r.rollInterrupted)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runSpellInterrupt1563SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spell_interrupt_1563 self-tests failed\n";
        return false;
    }
    return true;
}
