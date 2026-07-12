#pragma once

#include <cmath>
#include <cstdint>

// Pure IsParalyzed, IsAbsorbByShadow decision tree, and GetHitRateEx SA/TA
// short-circuit from battleutils.

namespace paralyzeshadowhelpers
{

// Blink fail: GetRandomNumber(100) < 20 → no absorb (80% proc).
constexpr int BlinkFailThreshold = 20;

// CE delta applied to mob enmity when PC Utsusemi absorb leaves remaining < 4.
constexpr std::int16_t UtsusemiAbsorbCEDelta = -25;

// Status effect IDs for icon/del path.
constexpr std::uint16_t EffectCopyImage  = 66;
constexpr std::uint16_t EffectBlink      = 36;
constexpr std::uint16_t EffectCopyImage2 = 444;
constexpr std::uint16_t EffectCopyImage3 = 445;

// IsParalyzed: roll < paralyzeMod (roll is GetRandomNumber(100) in [0,100)).
constexpr auto IsParalyzed(const std::int16_t paralyzeMod, const int roll) -> bool
{
    return roll < static_cast<int>(paralyzeMod);
}

enum class ShadowModKind : std::uint8_t
{
    None = 0,
    Utsusemi,
    Blink,
};

struct AbsorbByShadowResult
{
    bool            absorbed{ false };
    std::uint16_t   remaining{ 0 };
    ShadowModKind   usedMod{ ShadowModKind::None };
    bool            delCopyImage{ false };
    bool            delBlink{ false };
    bool            setIcon{ false };
    std::uint16_t   icon{ 0 };
    bool            applyCEEnmity{ false };
};

// IsAbsorbByShadow pure form after mod/type/RNG injects.
constexpr auto IsAbsorbByShadow(const std::uint16_t utsusemi,
                                const std::uint16_t blink,
                                const int           blinkFailRoll,
                                const bool          defenderIsPC,
                                const bool          hasCopyImageEffect,
                                const bool          attackerIsMob) -> AbsorbByShadowResult
{
    std::uint16_t shadow = utsusemi;
    auto          mod    = ShadowModKind::Utsusemi;
    if (shadow == 0)
    {
        shadow = blink;
        mod    = ShadowModKind::Blink;
        if (blinkFailRoll < BlinkFailThreshold)
        {
            return {};
        }
    }

    if (shadow == 0)
    {
        return {};
    }

    const std::uint16_t remaining = static_cast<std::uint16_t>(shadow - 1);
    AbsorbByShadowResult res{};
    res.absorbed  = true;
    res.remaining = remaining;
    res.usedMod   = mod;

    if (remaining == 0)
    {
        if (mod == ShadowModKind::Utsusemi)
        {
            res.delCopyImage = true;
        }
        else if (mod == ShadowModKind::Blink)
        {
            res.delBlink = true;
        }
        return res;
    }

    if (remaining < 4 && mod == ShadowModKind::Utsusemi && defenderIsPC && hasCopyImageEffect)
    {
        std::uint16_t icon = EffectCopyImage3;
        switch (remaining)
        {
            case 1:
                icon = EffectCopyImage;
                break;
            case 2:
                icon = EffectCopyImage2;
                break;
            default:
                break;
        }
        res.setIcon = true;
        res.icon    = icon;
        if (attackerIsMob)
        {
            res.applyCEEnmity = true;
        }
    }
    return res;
}

// GetHitRateEx SA/TA short-circuit + floor(luaHitRate * 100).
// hasValidTrickAttackWithChar is the combined TA+Assassin+valid-TA-char gate.
inline auto GetHitRateEx(const bool   hasValidSneakAttack,
                         const bool   hasValidTrickAttackWithChar,
                         const double luaHitRate) -> std::uint8_t
{
    if (hasValidSneakAttack || hasValidTrickAttackWithChar)
    {
        return 100;
    }
    // Production: hitrate = std::floor(luaHitRate * 100); return (uint8)hitrate
    const auto hitrate = static_cast<std::int32_t>(std::floor(luaHitRate * 100.0));
    return static_cast<std::uint8_t>(hitrate);
}

} // namespace paralyzeshadowhelpers
