#include "test_magical_spell_tp_1568.h"

#include "map/tp_from_damage_capacity.h"

#include <iostream>

namespace
{
using namespace tpfromdamagehelpers;

auto Check() -> bool
{
    // Magical TP: mob base 100, non-mob 50 (Lua dAGI ~1)
    {
        MagicalTPGainParams p{};
        p.targetIsMob = true;
        if (MagicalTPGain(p) != 100)
        {
            return false;
        }
        p.targetIsMob = false;
        if (MagicalTPGain(p) != 50)
        {
            return false;
        }
        // Subtle Blow 50 → half
        p.subtleBlow = 50;
        if (MagicalTPGain(p) != 25)
        {
            return false;
        }
    }

    if (!ShouldZeroMagicalTPGain(true, 5, false) || !ShouldZeroMagicalTPGain(false, 0, false) ||
        !ShouldZeroMagicalTPGain(false, 5, true) ||
        ShouldZeroMagicalTPGain(false, 5, false))
    {
        return false;
    }

    // Occult skill eligibility
    if (!OccultSkillEligible(SkillElementalMagic) || !OccultSkillEligible(SkillDarkMagic) ||
        OccultSkillEligible(0) || OccultSkillEligible(35))
    {
        return false;
    }

    // SpellTP
    if (SpellTP(false, false, SkillElementalMagic, 100, 50, 0) != 0)
    {
        return false;
    }
    if (SpellTP(true, true, SkillElementalMagic, 100, 50, 0) != 0)
    {
        return false;
    }
    if (SpellTP(true, false, 0, 100, 50, 0) != 0)
    {
        return false;
    }
    // floor(200 * 0.5 * 1) = 100
    if (SpellTP(true, false, SkillElementalMagic, 200, 50, 0) != 100)
    {
        return false;
    }
    // floor(100 * 1 * 1.5) = 150
    if (SpellTP(true, false, SkillDarkMagic, 100, 100, 50) != 150)
    {
        return false;
    }
    // floor(33 * 0.25) = 8
    if (SpellTP(true, false, SkillElementalMagic, 33, 25, 0) != 8)
    {
        return false;
    }

    return true;
}
} // namespace

auto runMagicalSpellTP1568SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "magical_spell_tp_1568 self-tests failed\n";
        return false;
    }
    return true;
}
