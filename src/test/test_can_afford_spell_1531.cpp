#include "test_can_afford_spell_1531.h"

#include "map/can_afford_spell_capacity.h"

#include <iostream>

namespace
{
using canaffordspellhelpers::CanAffordSpell;

auto Check() -> bool
{
    if (!CanAffordSpell(true, 0, false, 0, true, 999, 0))
    {
        return false;
    }
    if (!CanAffordSpell(false, 1, false, 0, true, 50, 0))
    {
        return false;
    }
    if (!CanAffordSpell(false, 0, true, 1, true, 50, 0))
    {
        return false;
    }
    if (CanAffordSpell(false, 0, true, 0, true, 50, 0))
    {
        return false;
    }
    if (!CanAffordSpell(false, 0, false, 0, true, 50, 50))
    {
        return false;
    }
    if (CanAffordSpell(false, 0, false, 0, true, 50, 49))
    {
        return false;
    }
    if (!CanAffordSpell(false, 0, false, 0, false, 999, 0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCanAffordSpell1531SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "can_afford_spell_1531 self-tests failed\n";
        return false;
    }
    return true;
}
