#include "test_combat_aoe_knockback_1581.h"

#include "map/ability_aoe_capacity.h"
#include "map/knockback_capacity.h"
#include "map/magic_aoe_capacity.h"

#include <array>
#include <iostream>

namespace
{
auto Check() -> bool
{
    // Ability: Liement + Epeolatry.
    {
        const auto r = abilityaoehelpers::TypeAndRadius(
            abilityaoehelpers::AbilityLiement, 0, abilityaoehelpers::TypeNone, 0, 1, false, 0);
        if (r.type != abilityaoehelpers::TypeRound || r.radius != abilityaoehelpers::OverrideRadius)
        {
            return false;
        }
    }
    // Contradance Healing Waltz.
    {
        const auto r = abilityaoehelpers::TypeAndRadius(
            abilityaoehelpers::AbilityHealingWaltz, 0, abilityaoehelpers::TypeNone, 0, 0, true, 0);
        if (r.type != abilityaoehelpers::TypeRound || r.radius != abilityaoehelpers::OverrideRadius)
        {
            return false;
        }
    }
    // Phantom roll + roll range.
    {
        const auto r = abilityaoehelpers::TypeAndRadius(
            1, abilityaoehelpers::RecastPhantomRoll, abilityaoehelpers::TypeNone, 8, 0, false, 8);
        if (r.type != abilityaoehelpers::TypeRound || r.radius != 16)
        {
            return false;
        }
    }
    // Passthrough.
    {
        const auto r = abilityaoehelpers::TypeAndRadius(1, 0, abilityaoehelpers::TypeRound, 12, 0, false, 0);
        if (r.type != abilityaoehelpers::TypeRound || r.radius != 12)
        {
            return false;
        }
    }

    // Mob collapse RADIAL_ACCE.
    {
        const auto r = magicaoehelpers::MobAoE(magicaoehelpers::AOERadialAcce, 10);
        if (r.type != magicaoehelpers::AOENone || r.radius != 0)
        {
            return false;
        }
    }
    // Majesty Cure.
    {
        magicaoehelpers::Params p{};
        p.isPC        = true;
        p.hasMajesty = true;
        p.spellFamily = magicaoehelpers::SpellFamilyCure;
        p.baseType    = magicaoehelpers::AOENone;
        p.baseRadius  = 0;
        const auto r  = magicaoehelpers::TypeAndRadius(p);
        if (r.type != magicaoehelpers::AOERadial || r.radius != magicaoehelpers::OverrideRadius)
        {
            return false;
        }
    }
    // Pianissimo consume.
    {
        magicaoehelpers::Params p{};
        p.isPC          = true;
        p.spellGroup    = magicaoehelpers::SpellGroupSong;
        p.baseType      = magicaoehelpers::AOEPianissimo;
        p.hasPianissimo = true;
        p.baseRadius    = 10;
        const auto r    = magicaoehelpers::TypeAndRadius(p);
        if (r.type != magicaoehelpers::AOENone || r.radius != 0 || !r.consumePianissimo)
        {
            return false;
        }
    }
    // Song string skill scale.
    {
        if (magicaoehelpers::SongRadius(
                10, magicaoehelpers::AOERadial, false, magicaoehelpers::JobBRD,
                magicaoehelpers::SkillStringInstrument, magicaoehelpers::SpellGroupSong, 200, 100) != 20)
        {
            return false;
        }
        if (magicaoehelpers::SongRadius(
                10, magicaoehelpers::AOERadial, false, magicaoehelpers::JobBRD,
                magicaoehelpers::SkillStringInstrument, magicaoehelpers::SpellGroupSong, 50, 100) != 10)
        {
            return false;
        }
    }
    // Divine Veil proc helper.
    if (!magicaoehelpers::DivineVeilProc(true, true, 0, 100) ||
        !magicaoehelpers::DivineVeilProc(true, false, 50, 50) ||
        magicaoehelpers::DivineVeilProc(true, false, 50, 51) ||
        magicaoehelpers::DivineVeilProc(false, true, 100, 1))
    {
        return false;
    }

    const std::array knockbackCases{
        std::array{ 5, 0, 5 },
        std::array{ 7, 0, knockbackhelpers::Level7 },
        std::array{ 5, 2, 3 },
        std::array{ 5, 5, knockbackhelpers::LevelNone },
        std::array{ 5, 8, knockbackhelpers::LevelNone },
        std::array{ 10, 3, knockbackhelpers::Level7 },
        std::array{ 3, -2, 5 },
        std::array{ 0, -8, knockbackhelpers::Level7 },
        std::array{ -1, 0, knockbackhelpers::LevelNone },
    };
    for (const auto& [skillKnockback, reductionMod, expected] : knockbackCases)
    {
        if (knockbackhelpers::Calculate(skillKnockback, reductionMod) != expected)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runCombatAoeKnockback1581SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "combat_aoe_knockback_1581 self-tests failed\n";
        return false;
    }
    return true;
}
