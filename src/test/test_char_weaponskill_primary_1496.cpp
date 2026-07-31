#include "test_char_weaponskill_primary_1496.h"

#include "map/char_weaponskill_primary_capacity.h"

#include <iostream>

namespace
{
using charweaponskillprimaryhelpers::IsNegatedResolution;
using charweaponskillprimaryhelpers::ShouldAwardWSPoints;
using charweaponskillprimaryhelpers::ShouldNotifyHit;
using charweaponskillprimaryhelpers::SkillchainWSPointBonus;
using charweaponskillprimaryhelpers::SkillchainWSPointLevel;
using charweaponskillprimaryhelpers::TotalWSPoints;

auto Check() -> bool
{
    if (!ShouldNotifyHit(false, true, true) ||
        ShouldNotifyHit(true, true, true) ||
        ShouldNotifyHit(false, false, true) ||
        ShouldNotifyHit(false, true, false))
    {
        return false;
    }

    if (!IsNegatedResolution(true, false) || !IsNegatedResolution(false, true) || IsNegatedResolution(false, false))
    {
        return false;
    }

    // Compression..Impaction → 1; Gravitation..Fusion → 2; Light/Darkness → 3
    if (SkillchainWSPointLevel(7) != 1 || SkillchainWSPointLevel(14) != 1 ||
        SkillchainWSPointLevel(3) != 2 || SkillchainWSPointLevel(6) != 2 ||
        SkillchainWSPointLevel(1) != 3 || SkillchainWSPointLevel(2) != 3 ||
        SkillchainWSPointLevel(15) != 2)
    {
        return false;
    }

    if (SkillchainWSPointBonus(7, 2) != 2 || SkillchainWSPointBonus(3, 2) != 4 ||
        SkillchainWSPointBonus(1, 2) != 6 || SkillchainWSPointBonus(1, 255) != 253)
    {
        return false;
    }

    if (TotalWSPoints(5, false, 7, 2) != 5 || TotalWSPoints(5, true, 7, 2) != 7 ||
        TotalWSPoints(5, true, 1, 255) != 2)
    {
        return false;
    }

    if (!ShouldAwardWSPoints(true, false, true) ||
        ShouldAwardWSPoints(false, false, true) ||
        ShouldAwardWSPoints(true, true, true) ||
        ShouldAwardWSPoints(true, false, false))
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharWeaponSkillPrimary1496SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char weaponskill primary 1496 self-test failed\n";
    }
    return ok;
}
