#include "test_combat_bonus_tails_1556.h"

#include "map/combat_bonus_tails_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace combatbonustailshelpers;

auto NearlyEqual(const float a, const float b, const float eps = 1e-5f) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    // Soul Eater: base 10% of HP
    if (!NearlyEqual(SoulEaterBonusDamage(1000, 0, 0), 100.f))
    {
        return false;
    }
    // gear additive: 10% + 5% → 0.25 * hp
    if (!NearlyEqual(SoulEaterBonusDamage(1000, 10, 5), 250.f))
    {
        return false;
    }
    // negative gear clamped
    if (!NearlyEqual(SoulEaterBonusDamage(1000, -50, 0), 100.f))
    {
        return false;
    }
    if (!NearlyEqual(SoulEaterHPCostScale(0), 1.f) || !NearlyEqual(SoulEaterHPCostScale(25), 0.75f))
    {
        return false;
    }
    if (ApplySoulEaterToDamage(100, 50.f, true) != 150 || ApplySoulEaterToDamage(100, 50.f, false) != 125)
    {
        return false;
    }

    // Consume Mana
    if (ConsumeManaBonus(false, 999) != 0 || ConsumeManaBonus(true, 99) != 9 || ConsumeManaBonus(true, 100) != 10)
    {
        return false;
    }

    // Overwhelm
    if (!NearlyEqual(OverwhelmBonusFraction(1), 0.05f) || !NearlyEqual(OverwhelmBonusFraction(5), 0.19f) ||
        OverwhelmBonusFraction(0) != 0.f)
    {
        return false;
    }
    if (OverwhelmDamageBonus(1000, 1, true) != 1050 || OverwhelmDamageBonus(1000, 1, false) != 1000)
    {
        return false;
    }
    if (OverwhelmDamageBonus(1000, 0, true) != 1000)
    {
        return false;
    }

    // Bind break: 950 > roll
    if (!BindBreaks(0) || !BindBreaks(949) || BindBreaks(950) || BindBreaks(999))
    {
        return false;
    }

    // Tranquil Heart: (skill/10)*0.5 capped at 25 → /100
    if (TranquilHeartReduction(false, 500) != 0.f)
    {
        return false;
    }
    // skill 100 → (10)*0.5 = 5 → 0.05
    if (!NearlyEqual(TranquilHeartReduction(true, 100), 0.05f))
    {
        return false;
    }
    // skill 1000 → 50 → cap 25 → 0.25
    if (!NearlyEqual(TranquilHeartReduction(true, 1000), 0.25f))
    {
        return false;
    }

    // Killer mod map samples
    {
        const auto m = KillerMod(xi::Ecosystem::Amorph);
        if (!m || *m != Mod::BIRD_KILLER)
        {
            return false;
        }
    }
    {
        const auto m = KillerMod(xi::Ecosystem::Luminion);
        if (!m || *m != Mod::LUMINIAN_KILLER)
        {
            return false;
        }
    }
    {
        const auto m = KillerMod(xi::Ecosystem::Luminian);
        if (!m || *m != Mod::LUMINION_KILLER)
        {
            return false;
        }
    }
    if (KillerMod(xi::Ecosystem::Empty).has_value() || KillerMod(xi::Ecosystem::Humanoid).has_value() ||
        KillerMod(xi::Ecosystem::Beastmen).has_value())
    {
        return false;
    }
    if (KillerBonus != 5)
    {
        return false;
    }

    return true;
}
} // namespace

auto runCombatBonusTails1556SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "combat_bonus_tails_1556 self-tests failed\n";
        return false;
    }
    return true;
}
