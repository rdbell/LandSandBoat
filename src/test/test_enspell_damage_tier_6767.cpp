#include "test_enspell_damage_tier_6767.h"

#include "map/enspell_damage_tier_capacity.h"

#include <iostream>

namespace
{
using enspelldamagetierhelpers::CalculateEnspellTier1Damage;
using enspelldamagetierhelpers::CalculateEnspellTier2Damage;
using enspelldamagetierhelpers::EnspellTier2Cap;

auto Check() -> bool
{
    // Cap table goldens match internal/attackutils TestEnspellTier2Cap.
    if (EnspellTier2Cap(0) != 6 || EnspellTier2Cap(100) != 18 || EnspellTier2Cap(200) != 30)
    {
        return false;
    }
    if (EnspellTier2Cap(201) != 30 || EnspellTier2Cap(300) != 40)
    {
        return false;
    }

    // Tier 1: enspellDMG + bonus + merit
    if (CalculateEnspellTier1Damage(10, 5, 3) != 18)
    {
        return false;
    }
    if (CalculateEnspellTier1Damage(0, 0, 0) != 0)
    {
        return false;
    }

    // Tier 2 under cap (skill 100 → cap 18): damage = old potency, new = old+1
    {
        const auto res = CalculateEnspellTier2Damage(5, 100, 2, 1);
        // damage = 5 + 2 + 1*2 = 9; new = 6
        if (res.damage != 9 || res.newEnspellDMG != 6)
        {
            return false;
        }
    }
    // At cap: damage = cap + bonus + merit*2; new unchanged
    {
        const auto res = CalculateEnspellTier2Damage(18, 100, 3, 2);
        // 18 + 3 + 4 = 25; new = 18
        if (res.damage != 25 || res.newEnspellDMG != 18)
        {
            return false;
        }
    }
    // Over cap: clamp mod to cap
    {
        const auto res = CalculateEnspellTier2Damage(50, 100, 0, 0);
        if (res.damage != 18 || res.newEnspellDMG != 18)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runEnspellDamageTier6767SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "enspell_damage_tier_6767 self-tests failed\n";
        return false;
    }
    return true;
}
