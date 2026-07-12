#pragma once

#include "data/enums/ecosystem.h"
#include "modifier.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>

// Pure combat damage/status bonus tails after entity injects.
// Parity: internal/attackutils soul_eater, consume_mana, overwhelm, bind_break,
// tranquil_heart; internal/ecosystem KillerMod.

namespace combatbonustailshelpers
{

// --- Soul Eater ---

// SoulEaterBonusDamage mirrors float bonusDamage before the >= 1 gate.
// se / se2 are SOULEATER_EFFECT / SOULEATER_EFFECT_II percent mods.
inline auto SoulEaterBonusDamage(const std::int32_t hp,
                                 const std::int16_t souleaterEffect,
                                 const std::int16_t souleaterEffectII) -> float
{
    // Parity: int * 0.01 (double) → float
    const auto souleaterBonus   = static_cast<float>(static_cast<double>(souleaterEffect) * 0.01);
    const auto souleaterBonusII = static_cast<float>(static_cast<double>(souleaterEffectII) * 0.01);
    auto       extra            = souleaterBonus + souleaterBonusII;
    if (extra < 0.f)
    {
        extra = 0.f;
    }
    return static_cast<float>(hp) * (0.1f + extra);
}

// SoulEaterHPCostScale is 1 - max(stalwartSoul/100, 0).
inline auto SoulEaterHPCostScale(const std::int16_t stalwartSoul) -> float
{
    return 1.f - std::max(static_cast<float>(stalwartSoul) / 100.f, 0.f);
}

// ApplySoulEaterToDamage: DRK full bonus, others half (float add into uint32).
inline auto ApplySoulEaterToDamage(const std::uint32_t damage, const float bonusDamage, const bool isDRK)
    -> std::uint32_t
{
    if (isDRK)
    {
        return static_cast<std::uint32_t>(damage + bonusDamage);
    }
    return static_cast<std::uint32_t>(damage + bonusDamage / 2.f);
}

// --- Consume Mana ---

// ConsumeManaBonus: floor(mp/10) when effect active; else 0.
constexpr auto ConsumeManaBonus(const bool hasEffect, const std::int32_t mp) -> std::int32_t
{
    if (!hasEffect)
    {
        return 0;
    }
    return mp / 10;
}

// --- Overwhelm ---

// OverwhelmBonusFraction for merit levels 1..5.
constexpr auto OverwhelmBonusFraction(const std::uint8_t meritValue) -> float
{
    switch (meritValue)
    {
        case 1:
            return 0.05f;
        case 2:
            return 0.10f;
        case 3:
            return 0.15f;
        case 4:
            return 0.17f;
        case 5:
            return 0.19f;
        default:
            return 0.f;
    }
}

// OverwhelmDamageBonus after PC + infront gates; merit 0/default leaves damage.
inline auto OverwhelmDamageBonus(const std::int32_t damage, const std::uint8_t meritValue, const bool inFront)
    -> std::int32_t
{
    if (!inFront)
    {
        return damage;
    }
    const auto frac = OverwhelmBonusFraction(meritValue);
    if (frac == 0.f)
    {
        return damage;
    }
    auto tmp = static_cast<float>(damage);
    tmp += tmp * frac;
    return static_cast<std::int32_t>(std::floor(tmp));
}

// --- Bind break ---

constexpr std::uint16_t BindBreakChance = 950;

// BindBreaks: chance > roll (roll is GetRandomNumber(1000) → 0..999).
constexpr auto BindBreaks(const std::uint16_t roll) -> bool
{
    return BindBreakChance > roll;
}

// --- Tranquil Heart ---

// TranquilHeartReduction: enmity reduction fraction in [0, 0.25].
inline auto TranquilHeartReduction(const bool hasTrait, const std::int16_t healingSkill) -> float
{
    if (!hasTrait)
    {
        return 0.f;
    }
    auto reductionPercent = (static_cast<float>(healingSkill) / 10.0f) * 0.5f;
    if (reductionPercent > 25.f)
    {
        reductionPercent = 25.f;
    }
    return reductionPercent / 100.0f;
}

// --- Ecosystem killer grant map (addEcosystemKillerEffects) ---

constexpr std::int16_t KillerBonus = 5;

// KillerMod is the correlation map: Amorph→BIRD_KILLER, … Luminion↔Luminian swapped.
inline auto KillerMod(const xi::Ecosystem eco) -> std::optional<Mod>
{
    switch (eco)
    {
        case xi::Ecosystem::Amorph:
            return Mod::BIRD_KILLER;
        case xi::Ecosystem::Aquan:
            return Mod::AMORPH_KILLER;
        case xi::Ecosystem::Arcana:
            return Mod::UNDEAD_KILLER;
        case xi::Ecosystem::Beast:
            return Mod::LIZARD_KILLER;
        case xi::Ecosystem::Bird:
            return Mod::AQUAN_KILLER;
        case xi::Ecosystem::Demon:
            return Mod::DRAGON_KILLER;
        case xi::Ecosystem::Dragon:
            return Mod::DEMON_KILLER;
        case xi::Ecosystem::Lizard:
            return Mod::VERMIN_KILLER;
        case xi::Ecosystem::Luminion:
            return Mod::LUMINIAN_KILLER;
        case xi::Ecosystem::Luminian:
            return Mod::LUMINION_KILLER;
        case xi::Ecosystem::Plantoid:
            return Mod::BEAST_KILLER;
        case xi::Ecosystem::Undead:
            return Mod::ARCANA_KILLER;
        case xi::Ecosystem::Vermin:
            return Mod::PLANTOID_KILLER;
        default:
            return std::nullopt;
    }
}

} // namespace combatbonustailshelpers
