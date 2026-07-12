#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure combat status mitigation tails after entity injects.
// Parity: internal/attackutils one_for_all_stoneskin, damage_cap, fan_dance,
// severe_damage, scarlet_delirium, store_tp_merit.

namespace combatstatusmitigationhelpers
{

// --- One For All ---
constexpr auto HandleOneForAll(const std::int32_t damage, const std::int32_t power, const bool hasEffect) -> std::int32_t
{
    if (damage > 0 && hasEffect)
    {
        auto d = damage - power;
        if (d < 0)
        {
            d = 0;
        }
        return d;
    }
    return damage;
}

// --- Stoneskin pure product ---
struct StoneskinResult
{
    std::int32_t remainingDamage{};
    std::int32_t skinLeft{};
    bool         removeEffect{};
};

constexpr auto HandleStoneskin(const std::int32_t damage, const std::int32_t skin) -> StoneskinResult
{
    if (damage > 0 && skin > 0)
    {
        if (skin > damage)
        {
            return { 0, skin - damage, false };
        }
        return { damage - skin, 0, true };
    }
    return { damage, skin, false };
}

// --- Damage cap ---
constexpr auto ClampI32(const std::int32_t v, const std::int32_t lo, const std::int32_t hi) -> std::int32_t
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

// subtract is GetRandomNumber(0, damageVariant+1) inject when cap path taken.
inline auto CheckAndApplyDamageCap(std::int32_t damage,
                                   const std::int32_t damageCap,
                                   std::int32_t       damageVariant,
                                   const std::int32_t subtract) -> std::int32_t
{
    if (damageCap == 0 || damage < damageCap)
    {
        return damage;
    }
    damage = ClampI32(damage, 0, damageCap);
    if (damageVariant > damageCap)
    {
        damageVariant = 0;
    }
    damage -= subtract;
    return ClampI32(damage, damageCap - damageVariant, damageCap);
}

// --- Store TP merit ---
constexpr auto StoreTPBonusFromMerit(const bool isPC, const bool isSAMMain, const std::int16_t meritValue) -> std::uint8_t
{
    if (isPC && isSAMMain)
    {
        return static_cast<std::uint8_t>(meritValue);
    }
    return 0;
}

// --- Fan Dance ---
struct FanDanceResult
{
    std::int32_t newDamage{};
    std::int32_t newPower{};
};

inline auto HandleFanDance(const std::int32_t damage, const std::int32_t power, const bool hasEffect) -> FanDanceResult
{
    if (!hasEffect)
    {
        return { damage, power };
    }
    const auto resist    = 1.0f - static_cast<float>(power) / 10000.0f;
    auto       newDamage = static_cast<std::int32_t>(static_cast<float>(damage) * resist);
    auto       newPower  = power;
    if (power > 2000)
    {
        newPower = power - 1000;
    }
    return { newDamage, newPower };
}

// --- Scarlet Delirium ---
inline auto ScarletDeliriumPower(const std::int32_t damage, const std::int32_t maxHP) -> std::uint16_t
{
    auto hppRatio = static_cast<float>(damage) / static_cast<float>(maxHP) / 2.0f;
    if (hppRatio < 0.f)
    {
        hppRatio = 0.f;
    }
    if (hppRatio > 0.5f)
    {
        hppRatio = 0.5f;
    }
    return static_cast<std::uint16_t>(std::floor(hppRatio * 1000.f));
}

// Duration seconds: 90 + jpValue
constexpr auto ScarletDeliriumDurationSec(const std::uint16_t jpValue) -> std::int32_t
{
    return 90 + static_cast<std::int32_t>(jpValue);
}

// --- Severe damage effect ---
inline auto SevereDamageThreshold(const std::int32_t maxHP, const std::uint16_t powerPercent) -> double
{
    return static_cast<double>(maxHP) * (static_cast<double>(powerPercent) / 100.0);
}

inline auto ExceedsSevereThreshold(const std::int32_t damage, const std::int32_t maxHP, const std::uint16_t power) -> bool
{
    return static_cast<double>(damage) > SevereDamageThreshold(maxHP, power);
}

constexpr auto SevereDamageRemainingFactor(const std::uint16_t subPower) -> std::int32_t
{
    auto rem = 100 - static_cast<std::int32_t>(subPower);
    if (rem < 0)
    {
        rem = 0;
    }
    if (rem > 100)
    {
        rem = 100;
    }
    return rem / 100;
}

struct SevereDamageResult
{
    std::int32_t newDamage{};
    bool         triggered{};
};

inline auto HandleSevereDamageEffect(const std::int32_t  damage,
                                     const std::int32_t  maxHP,
                                     const std::uint16_t power,
                                     const std::uint16_t subPower,
                                     const bool          hasEffect) -> SevereDamageResult
{
    if (!hasEffect)
    {
        return { damage, false };
    }
    if (!ExceedsSevereThreshold(damage, maxHP, power))
    {
        return { damage, false };
    }
    return { damage * SevereDamageRemainingFactor(subPower), true };
}

// Schurzen: damage >= hp and AUTO_SCHURZEN and EarthManeuver count >= 1 → hp-1
constexpr auto ApplySchurzenCap(const std::int32_t damage,
                                const std::int32_t hp,
                                const bool         isPhysicalPet,
                                const bool         hasSchurzen,
                                const bool         hasEarthManeuver) -> std::int32_t
{
    if (isPhysicalPet && hasSchurzen && damage >= hp && hasEarthManeuver)
    {
        return hp - 1;
    }
    return damage;
}

} // namespace combatstatusmitigationhelpers
