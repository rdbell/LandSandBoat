#include "test_pet_weapon_damage_1593.h"

#include "map/pet_weapon_damage_capacity.h"

#include <iostream>

namespace
{
using namespace petweapondamagehelpers;

auto Check() -> bool
{
    // Wyvern L75 → 40
    if (WyvernWeaponDamage(0) != 3 || WyvernWeaponDamage(1) != 3 || WyvernWeaponDamage(2) != 4 ||
        WyvernWeaponDamage(3) != 4 || WyvernWeaponDamage(75) != 40 || WyvernWeaponDamage(255) != 130)
    {
        return false;
    }
    if (WyvernWeaponDamageWithRating(75, 2) != 42 || WyvernWeaponDamageWithRating(1, -5) != -2)
    {
        return false;
    }
    // Jug L30: float path truncates 32.999… → 32 (matches Go JugWeaponDamage)
    if (JugWeaponDamage(30.0f) != 32)
    {
        return false;
    }
    // Jug L40: identity
    if (JugWeaponDamage(40.0f) != 40 || JugWeaponDamage(50.0f) != 50)
    {
        return false;
    }
    // Jug L10: 10 * (1.4 - 0.1) truncates to 12 (same as Go)
    if (JugWeaponDamage(10.0f) != 12)
    {
        return false;
    }
    if (JugWeaponDamage(10.5f) != 13 || JugWeaponDamageWithRating(10.5f, -2) != 11)
    {
        return false;
    }
    return true;
}
} // namespace

auto runPetWeaponDamage1593SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "pet_weapon_damage_1593 self-tests failed\n";
        return false;
    }
    return true;
}
