#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

// Pure CBattleEntity::UpdateHealth HP/MP maxima recalculation with fully
// injected inputs. Parity: internal/updatehealth (slice 1711).
//
// Reference:
//   src/map/entities/battle_entity.cpp
//     CBattleEntity::UpdateHealth (~271–330)
//
// Host retains entity storage, modifier map reads, battle-type gate, and
// updatemask. Resolve takes injected raw maxima, mods, PC flag, and current
// HP/MP only.

namespace updatehealthhelpers
{

// PCMaxHP is the player effective HP ceiling (LSB clamp 1..9999).
constexpr std::int32_t PCMaxHP = 9999;

// PCMaxMP is the player effective MP ceiling (LSB clamp 0..9999).
constexpr std::int32_t PCMaxMP = 9999;

// MaxInt32 is the non-PC defensive storage ceiling matching Go int32 max.
constexpr std::int32_t MaxInt32 = std::numeric_limits<std::int32_t>::max();

struct Params
{
    std::int32_t BaseHP   = 0;
    std::int32_t BaseMP   = 0;
    std::int16_t WeaknessPct = 0;
    std::int16_t CursePct    = 0;
    std::int16_t HPP         = 0;
    std::int16_t MPP         = 0;
    std::int16_t BaseHPMod   = 0;
    std::int16_t HPMod       = 0;
    std::int16_t FoodHP      = 0;
    std::int16_t BaseMPMod   = 0;
    std::int16_t MPMod       = 0;
    std::int16_t FoodMP      = 0;
    std::int16_t ConvMPToHP  = 0;
    std::int16_t ConvHPToMP  = 0;
    std::int16_t FoodHPP     = 0;
    std::int16_t FoodHPCap   = 0;
    std::int16_t FoodMPP     = 0;
    std::int16_t FoodMPCap   = 0;
    bool         IsPC        = false;
    std::int32_t CurrentHP   = 0;
    std::int32_t CurrentMP   = 0;
};

struct Result
{
    std::int32_t EffectiveMaxHP = 0;
    std::int32_t EffectiveMaxMP = 0;
    std::int32_t ClampedHP      = 0;
    std::int32_t ClampedMP      = 0;
};

// FloorFloat32 matches host floor via double: floor((double)float32_value).
// float32 intermediates pin large-int rounding (e.g. 16777217 → 16777216).
inline auto FloorFloat32(const float value) -> std::int32_t
{
    return static_cast<std::int32_t>(std::floor(static_cast<double>(value)));
}

inline auto Min(const std::int32_t left, const std::int32_t right) -> std::int32_t
{
    return left < right ? left : right;
}

inline auto Clamp(const std::int32_t value, const std::int32_t lower, const std::int32_t upper) -> std::int32_t
{
    if (value < lower)
    {
        return lower;
    }
    if (value > upper)
    {
        return upper;
    }
    return value;
}

// FoodBonus: min<int16>(base * percent / 100, cap) with int16 product narrow.
inline auto FoodBonus(const std::int32_t base, const std::int16_t percent, const std::int16_t cap) -> std::int32_t
{
    const auto bonus = static_cast<std::int32_t>(static_cast<std::int16_t>(static_cast<std::int64_t>(base) * static_cast<std::int64_t>(percent) / 100));
    return Min(bonus, static_cast<std::int32_t>(cap));
}

// Resolve is the pure form of CBattleEntity::UpdateHealth.
inline auto Resolve(const Params& p) -> Result
{
    const float weaknessPower = (100.0f + static_cast<float>(p.WeaknessPct)) / 100.0f;
    const float cursePower    = (100.0f + static_cast<float>(p.CursePct)) / 100.0f;
    const float hppPower      = (100.0f + static_cast<float>(p.HPP)) / 100.0f;
    const float mppPower      = (100.0f + static_cast<float>(p.MPP)) / 100.0f;

    std::int32_t baseHPBonus = FloorFloat32((static_cast<float>(p.BaseHP) + static_cast<float>(p.BaseHPMod)) * weaknessPower);
    baseHPBonus              = FloorFloat32((static_cast<float>(baseHPBonus) + static_cast<float>(p.HPMod)) * cursePower) + static_cast<std::int32_t>(p.FoodHP);
    std::int32_t baseMPBonus = FloorFloat32((static_cast<float>(p.BaseMP) + static_cast<float>(p.BaseMPMod)) * weaknessPower);
    baseMPBonus              = FloorFloat32((static_cast<float>(baseMPBonus) + static_cast<float>(p.MPMod)) * cursePower) + static_cast<std::int32_t>(p.FoodMP);

    const std::int32_t conversion = static_cast<std::int32_t>(p.ConvMPToHP) - static_cast<std::int32_t>(p.ConvHPToMP);
    std::int32_t       convertHP  = 0;
    std::int32_t       convertMP  = 0;
    if (conversion > 0)
    {
        convertHP = Min(baseMPBonus, conversion);
        convertMP = -convertHP;
    }
    else if (conversion < 0)
    {
        // -1 so we don't end up with zero HP...
        convertMP = Min(baseHPBonus - 1, -conversion);
        convertHP = -convertMP;
    }

    baseHPBonus = FloorFloat32((static_cast<float>(baseHPBonus) + static_cast<float>(convertHP)) * hppPower);
    baseMPBonus = FloorFloat32((static_cast<float>(baseMPBonus) + static_cast<float>(convertMP)) * mppPower);

    const std::int32_t foodHPBonus = FoodBonus(baseHPBonus, p.FoodHPP, p.FoodHPCap);
    const std::int32_t foodMPBonus = FoodBonus(baseMPBonus, p.FoodMPP, p.FoodMPCap);

    std::int32_t effectiveHP = baseHPBonus + foodHPBonus;
    std::int32_t effectiveMP = baseMPBonus + foodMPBonus;
    effectiveHP              = Clamp(effectiveHP, 0, MaxInt32);
    effectiveMP              = Clamp(effectiveMP, 0, MaxInt32);
    if (p.IsPC)
    {
        effectiveHP = Clamp(effectiveHP, 1, PCMaxHP);
        effectiveMP = Clamp(effectiveMP, 0, PCMaxMP);
    }

    return Result{
        .EffectiveMaxHP = effectiveHP,
        .EffectiveMaxMP = effectiveMP,
        .ClampedHP      = Clamp(p.CurrentHP, 0, effectiveHP),
        .ClampedMP      = Clamp(p.CurrentMP, 0, effectiveMP),
    };
}

} // namespace updatehealthhelpers
