#include "test_damage_affinity_1557.h"

#include "map/damage_affinity_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace damageaffinityhelpers;

auto NearlyEqual(const float a, const float b, const float eps = 1e-5f) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    // Spikes
    if (GetSpikesDamageType(ActionReactKind::BlazeSpikes) != xi::DamageType::Fire)
    {
        return false;
    }
    if (GetSpikesDamageType(ActionReactKind::CurseSpikes) != xi::DamageType::None)
    {
        return false;
    }
    if (GetSpikesDamageType(ActionReactKind::DeathSpikes) != xi::DamageType::Dark)
    {
        return false;
    }
    if (GetSpikesDamageType(ActionReactKind::ReprisalSpikes) != xi::DamageType::Light)
    {
        return false;
    }

    // Enspell I/II
    if (GetEnspellDamageType(EnspellIFire) != xi::DamageType::Fire ||
        GetEnspellDamageType(EnspellIIFire) != xi::DamageType::Fire)
    {
        return false;
    }
    if (GetEnspellDamageType(EnspellIDark) != xi::DamageType::Dark ||
        GetEnspellDamageType(EnspellIIDark) != xi::DamageType::Dark)
    {
        return false;
    }
    if (GetEnspellDamageType(EnspellNone) != xi::DamageType::None)
    {
        return false;
    }

    // Rune damage type + element aligned
    if (GetRuneEnhancementDamageType(xi::StatusEffect::Ignis) != xi::DamageType::Fire ||
        GetRuneEnhancementElement(xi::StatusEffect::Ignis) != ElementFire)
    {
        return false;
    }
    if (GetRuneEnhancementDamageType(xi::StatusEffect::Tenebrae) != xi::DamageType::Dark ||
        GetRuneEnhancementElement(xi::StatusEffect::Tenebrae) != ElementDark)
    {
        return false;
    }
    if (GetRuneEnhancementElement(xi::StatusEffect::None) != ElementNone)
    {
        return false;
    }

    // Liement: inactive
    {
        const auto r = CheckLiementAbsorb(false, 15, 0x1111, 1);
        if (r.consume || !NearlyEqual(r.multiplier, 1.0f))
        {
            return false;
        }
    }
    // one matching fire (1) nibble, power 15 → -(85+15)/100 = -1.0
    {
        const auto r = CheckLiementAbsorb(true, 15, 0x0001, 1);
        if (!r.consume || !NearlyEqual(r.multiplier, -1.0f))
        {
            return false;
        }
    }
    // two matching fire nibbles, power 15 → -(85+30)/100 = -1.15
    {
        const auto r = CheckLiementAbsorb(true, 15, 0x0011, 1);
        if (!r.consume || !NearlyEqual(r.multiplier, -1.15f))
        {
            return false;
        }
    }
    // no match
    {
        const auto r = CheckLiementAbsorb(true, 15, 0x0002, 1);
        if (r.consume || !NearlyEqual(r.multiplier, 1.0f))
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runDamageAffinity1557SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "damage_affinity_1557 self-tests failed\n";
        return false;
    }
    return true;
}
