#include "test_weapon_rank_1645.h"

#include "map/weapon_rank_capacity.h"

#include <iostream>

namespace
{
using namespace weaponrankhelpers;

auto Check() -> bool
{
    // --- Main: empty weapon → dmg/9 only ---
    if (ResolveMainWeaponRank(0, false, 13, 5, false, false) != 0)
    {
        return false;
    }
    if (ResolveMainWeaponRank(45, false, 99, -10, true, true) != 5) // 45/9
    {
        return false;
    }
    if (ResolveMainWeaponRank(8, false, 0, 0, false, false) != 0) // 8/9
    {
        return false;
    }

    // --- Main: integer /9 cases ---
    if (ResolveMainWeaponRank(54, true, 0, 0, false, false) != 6)
    {
        return false;
    }
    if (ResolveMainWeaponRank(53, true, 0, 0, false, false) != 5)
    {
        return false;
    }
    if (ResolveMainWeaponRank(0, true, 0, 0, false, false) != 0)
    {
        return false;
    }
    if (ResolveMainWeaponRank(9, true, 0, 0, false, false) != 1)
    {
        return false;
    }
    if (ResolveMainWeaponRank(90, true, 0, 0, false, false) != 10)
    {
        return false;
    }

    // MAIN_DMG_RANK: 45 + 13 = 58 → 6
    if (ResolveMainWeaponRank(45, true, 13, 0, false, false) != 6)
    {
        return false;
    }
    // DMG_RATING subtract: 54 - 9 = 45 → 5
    if (ResolveMainWeaponRank(54, true, 0, 9, false, false) != 5)
    {
        return false;
    }
    // Combined: 40 + 14 - 5 = 49 → 5
    if (ResolveMainWeaponRank(40, true, 14, 5, false, false) != 5)
    {
        return false;
    }

    // --- Main: PC H2H +3 ---
    if (ResolveMainWeaponRank(42, true, 0, 0, true, true) != 5) // 42+3=45 → 5
    {
        return false;
    }
    if (ResolveMainWeaponRank(42, true, 0, 0, false, true) != 4) // non-PC H2H
    {
        return false;
    }
    if (ResolveMainWeaponRank(42, true, 0, 0, true, false) != 4) // PC non-H2H
    {
        return false;
    }
    // PC H2H with rank: 30 + 12 + 3 = 45 → 5
    if (ResolveMainWeaponRank(30, true, 12, 0, true, true) != 5)
    {
        return false;
    }

    // Negative dmgRating: 40 - (-5) = 45 → 5
    if (ResolveMainWeaponRank(40, true, 0, -5, false, false) != 5)
    {
        return false;
    }

    // --- Sub: empty / plain / MAIN_DMG_RANK (yes MAIN) ---
    if (ResolveSubWeaponRank(36, false, 99, 1) != 4)
    {
        return false;
    }
    if (ResolveSubWeaponRank(0, false, 0, 0) != 0)
    {
        return false;
    }
    if (ResolveSubWeaponRank(45, true, 0, 0) != 5)
    {
        return false;
    }
    // MAIN_DMG_RANK on sub: 30 + 15 = 45 → 5
    if (ResolveSubWeaponRank(30, true, 15, 0) != 5)
    {
        return false;
    }
    // DMG_RATING: 54 - 9 = 45 → 5
    if (ResolveSubWeaponRank(54, true, 0, 9) != 5)
    {
        return false;
    }
    // Combined: 20 + 10 - 3 = 27 → 3
    if (ResolveSubWeaponRank(20, true, 10, 3) != 3)
    {
        return false;
    }
    if (ResolveSubWeaponRank(26, true, 0, 0) != 2)
    {
        return false;
    }

    // --- Ranged base scale ---
    // Non-PC underleveled: no scale
    if (ResolveRangedBaseDamage(100, 75, 50, false) != 100)
    {
        return false;
    }
    // PC level-adequate
    if (ResolveRangedBaseDamage(100, 75, 75, true) != 100)
    {
        return false;
    }
    if (ResolveRangedBaseDamage(100, 50, 75, true) != 100)
    {
        return false;
    }
    // PC underleveled: 100 * 50 * 3 / 4 / 75 = 50
    if (ResolveRangedBaseDamage(100, 75, 50, true) != 50)
    {
        return false;
    }
    // 80 * 40 * 3 / 4 / 60 = 40
    if (ResolveRangedBaseDamage(80, 60, 40, true) != 40)
    {
        return false;
    }
    // Truncation: 99 * 10 * 3 / 4 / 50 = 14
    if (ResolveRangedBaseDamage(99, 50, 10, true) != 14)
    {
        return false;
    }
    // reqLvl 0 never > mLevel
    if (ResolveRangedBaseDamage(40, 0, 0, true) != 40)
    {
        return false;
    }
    if (ResolveRangedBaseDamage(0, 99, 1, true) != 0)
    {
        return false;
    }

    // --- Ranged rank ---
    if (ResolveRangedWeaponRank(false, 99, 1, 75, true, 13) != 0)
    {
        return false;
    }
    if (ResolveRangedWeaponRank(true, 45, 1, 75, true, 0) != 5)
    {
        return false;
    }
    // RANGED_DMG_RANK: 32 + 13 = 45 → 5
    if (ResolveRangedWeaponRank(true, 32, 1, 75, true, 13) != 5)
    {
        return false;
    }
    if (ResolveRangedWeaponRank(true, 44, 0, 0, false, 0) != 4)
    {
        return false;
    }
    // Underleveled PC + rank: base 50 + 4 = 54 → 6
    if (ResolveRangedWeaponRank(true, 100, 75, 50, true, 4) != 6)
    {
        return false;
    }
    // Non-PC underleveled uses raw: 100/9 = 11
    if (ResolveRangedWeaponRank(true, 100, 75, 50, false, 0) != 11)
    {
        return false;
    }
    // Rank-only on zero base: 27/9 = 3
    if (ResolveRangedWeaponRank(true, 0, 0, 0, false, 27) != 3)
    {
        return false;
    }

    return true;
}
} // namespace

auto runWeaponRank1645SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "weapon_rank_1645 self-tests failed\n";
        return false;
    }
    return true;
}
