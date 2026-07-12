#include "test_automaton_weapon_damage_1592.h"

#include "map/automaton_weapon_damage_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace automatonweapondamagehelpers;

auto Check() -> bool
{
    if (WeaponDamage(0) != 3)
    {
        return false;
    }
    if (WeaponDamage(87) != 23)
    {
        return false;
    }
    const auto want300 = static_cast<std::uint16_t>(std::floor((300.0f / 8.7f) * 2.0f + 3.0f));
    if (WeaponDamage(300) != want300)
    {
        return false;
    }
    if (WeaponDamageWithRating(87, 5) != 28 || WeaponDamageWithRating(87, -2) != 21)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAutomatonWeaponDamage1592SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "automaton_weapon_damage_1592 self-tests failed\n";
        return false;
    }
    return true;
}
