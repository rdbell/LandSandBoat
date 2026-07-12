#include "test_is_intimidated_1553.h"

#include "map/intimidate_capacity.h"

#include <iostream>

namespace
{
using intimidatehelpers::IntimidateChance;
using intimidatehelpers::IntimidateKillerMod;
using intimidatehelpers::IsIntimidated;

auto Check() -> bool
{
    // Ecosystem → killer mod map samples
    {
        const auto m = IntimidateKillerMod(xi::Ecosystem::Beast);
        if (!m || *m != Mod::BEAST_KILLER)
        {
            return false;
        }
    }
    {
        const auto m = IntimidateKillerMod(xi::Ecosystem::Amorph);
        if (!m || *m != Mod::AMORPH_KILLER)
        {
            return false;
        }
    }
    {
        const auto m = IntimidateKillerMod(xi::Ecosystem::Empty);
        if (!m || *m != Mod::EMPTY_KILLER)
        {
            return false;
        }
    }
    {
        const auto m = IntimidateKillerMod(xi::Ecosystem::Humanoid);
        if (!m || *m != Mod::HUMANOID_KILLER)
        {
            return false;
        }
    }
    // Luminian/Luminion not swapped on intimidate path
    {
        const auto m = IntimidateKillerMod(xi::Ecosystem::Luminion);
        if (!m || *m != Mod::LUMINION_KILLER)
        {
            return false;
        }
    }
    {
        const auto m = IntimidateKillerMod(xi::Ecosystem::Luminian);
        if (!m || *m != Mod::LUMINIAN_KILLER)
        {
            return false;
        }
    }
    // Unmapped ecosystems
    if (IntimidateKillerMod(xi::Ecosystem::Beastmen).has_value() ||
        IntimidateKillerMod(xi::Ecosystem::Elemental).has_value() ||
        IntimidateKillerMod(xi::Ecosystem::Unclassified).has_value())
    {
        return false;
    }

    // Chance assembly
    if (IntimidateChance(true, 50, 10, 5) != 0)
    {
        return false;
    }
    if (IntimidateChance(false, 25, 0, 0) != 25)
    {
        return false;
    }
    if (IntimidateChance(false, 0, 15, 0) != 15)
    {
        return false;
    }
    if (IntimidateChance(false, 0, 0, 20) != 20)
    {
        return false;
    }
    if (IntimidateChance(false, 10, 5, 7) != 22)
    {
        return false;
    }
    if (IntimidateChance(false, -5, 0, 0) != -5)
    {
        return false;
    }
    if (IntimidateChance(false, 10, -3, -2) != 5)
    {
        return false;
    }

    // d100 compare: roll < chance
    if (IsIntimidated(0, 0) || IsIntimidated(0, 50))
    {
        return false;
    }
    if (!IsIntimidated(1, 0) || IsIntimidated(1, 1))
    {
        return false;
    }
    if (!IsIntimidated(50, 49) || IsIntimidated(50, 50))
    {
        return false;
    }
    if (!IsIntimidated(100, 99) || !IsIntimidated(100, 0))
    {
        return false;
    }
    if (IsIntimidated(-1, 0))
    {
        return false;
    }
    if (!IsIntimidated(200, 99))
    {
        return false;
    }

    // End-to-end: 30% beast killer boundary + self short-circuit
    if (!IsIntimidated(IntimidateChance(false, 30, 0, 0), 29) ||
        IsIntimidated(IntimidateChance(false, 30, 0, 0), 30))
    {
        return false;
    }
    if (IsIntimidated(IntimidateChance(true, 99, 99, 99), 0))
    {
        return false;
    }

    return true;
}
} // namespace

auto runIsIntimidated1553SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "is_intimidated_1553 self-tests failed\n";
        return false;
    }
    return true;
}
