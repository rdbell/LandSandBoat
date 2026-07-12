#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure battleutils::CalculateSpellCastTime after entity/spell/RNG injects.
// Durations are millisecond counts.
// Parity: internal/spell.CalculateSpellCastTime

namespace spellcasthelpers
{

constexpr std::uint8_t  AOEPianissimo = 5;
constexpr std::uint16_t SpellGroupSong      = 1;
constexpr std::uint16_t SpellGroupBlack     = 2;
constexpr std::uint16_t SpellGroupNinjutsu  = 4;
constexpr std::uint16_t SpellGroupSummoning = 5;
constexpr std::uint16_t SpellGroupWhite     = 6;

constexpr std::uint8_t SkillElementalMagic = 36;
constexpr std::uint8_t SkillDarkMagic      = 37;
constexpr std::uint8_t SkillGeomancy       = 44;

constexpr std::uint16_t SongSpellcastingCap         = 50;
constexpr std::uint16_t AlacrityCelerityBaseReduce  = 50;

inline auto FloorMulMs(const std::int64_t ms, const float factor) -> std::int64_t
{
    return static_cast<std::int64_t>(std::floor(static_cast<float>(ms) * factor));
}

inline auto ClampI16(const std::int16_t v, const std::int16_t lo, const std::int16_t hi) -> std::int16_t
{
    return std::clamp(v, lo, hi);
}

constexpr auto ShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

struct SpellCastParams
{
    std::int64_t  baseMs{};
    std::uint16_t spellGroup{};
    std::uint8_t  aoe{};
    std::uint8_t  skillType{};
    bool          isCure{};
    bool          isNa{};
    bool          isPC{};

    bool quickMagicProc{};

    bool hassoOrSeigan{};
    bool alacrity{};
    bool celerity{};
    bool darkArtsOrAddendumBlack{};
    bool lightArtsOrAddendumWhite{};
    bool pianissimo{};
    bool nightingale{};
    bool nightingaleInstant{};
    bool troubadour{};

    std::int16_t  blackMagicCast{};
    std::int16_t  whiteMagicCast{};
    std::int16_t  darkMagicCast{};
    std::int16_t  grimoireSpellcasting{};
    std::uint16_t alacrityCelerityEffect{};
    std::uint8_t  strategemEffectII{};
    bool          weatherMatchesElement{};

    std::int16_t summoningMagicCast{};
    std::int32_t summoningMagicCastTimeMerit{};

    std::uint16_t songSpellcastingTime{};
    std::uint8_t  ninjutsuCastTimeBonus{};

    std::int16_t fastCast{};
    std::int16_t uFastCast{};
    std::int16_t inspirationFastCast{};
    std::int16_t elementalCelerity{};
    std::int16_t cureCastTime{};
    std::int16_t cureCastTimeMerit{};
    std::int16_t widenedCompassJP{};
    std::int16_t divineBenison{};
};

inline auto CalculateSpellCastMs(const SpellCastParams& p) -> std::int64_t
{
    if (p.quickMagicProc)
    {
        return 0;
    }

    const auto baseMs = p.baseMs;
    auto       cast   = baseMs;

    if (p.hassoOrSeigan)
    {
        cast = FloorMulMs(cast, 1.5f);
    }

    if (p.spellGroup == SpellGroupBlack)
    {
        if (p.alacrity)
        {
            auto bonus = static_cast<std::uint16_t>(0);
            if (p.weatherMatchesElement)
            {
                bonus += p.alacrityCelerityEffect;
            }
            if (p.isPC)
            {
                bonus += p.strategemEffectII;
            }
            const auto reduction = (100.0f - static_cast<float>(AlacrityCelerityBaseReduce + bonus)) / 100.0f;
            cast -= FloorMulMs(baseMs, reduction);
        }
        else if (p.skillType == SkillDarkMagic)
        {
            cast = FloorMulMs(cast, 1.0f + (static_cast<float>(p.blackMagicCast) + static_cast<float>(p.darkMagicCast)) / 100.0f);
        }
        else if (p.darkArtsOrAddendumBlack)
        {
            cast = FloorMulMs(cast, 1.0f + (static_cast<float>(p.blackMagicCast) + static_cast<float>(p.grimoireSpellcasting)) / 100.0f);
        }
        else
        {
            cast = FloorMulMs(cast, 1.0f + static_cast<float>(p.blackMagicCast) / 100.0f);
        }
    }
    else if (p.spellGroup == SpellGroupWhite)
    {
        if (p.celerity)
        {
            auto bonus = static_cast<std::uint16_t>(0);
            if (p.weatherMatchesElement)
            {
                bonus += p.alacrityCelerityEffect;
            }
            if (p.isPC)
            {
                bonus += p.strategemEffectII;
            }
            const auto reduction = (100.0f - static_cast<float>(AlacrityCelerityBaseReduce + bonus)) / 100.0f;
            cast -= FloorMulMs(baseMs, reduction);
        }
        else if (p.lightArtsOrAddendumWhite)
        {
            cast = FloorMulMs(cast, 1.0f + (static_cast<float>(p.whiteMagicCast) + static_cast<float>(p.grimoireSpellcasting)) / 100.0f);
        }
        else
        {
            cast = FloorMulMs(cast, 1.0f + static_cast<float>(p.whiteMagicCast) / 100.0f);
        }
    }
    else if (p.spellGroup == SpellGroupSummoning)
    {
        auto amount = static_cast<std::int64_t>(p.summoningMagicCast) * 1000;
        if (p.isPC)
        {
            amount += FloorMulMs(baseMs, 0.01f * static_cast<float>(p.summoningMagicCastTimeMerit));
        }
        if (amount < 0)
        {
            amount = 0;
        }
        if (cast > amount)
        {
            cast -= amount;
        }
        else
        {
            cast = 0;
        }
    }
    else if (p.spellGroup == SpellGroupSong)
    {
        if (p.pianissimo && p.aoe == AOEPianissimo)
        {
            cast = baseMs / 2;
        }
        if (p.nightingale)
        {
            if (p.isPC && p.nightingaleInstant)
            {
                return 0;
            }
            cast = FloorMulMs(cast, 0.5f);
        }
        if (p.troubadour)
        {
            cast = FloorMulMs(cast, 1.5f);
        }
        auto songcasting = p.songSpellcastingTime;
        if (songcasting > SongSpellcastingCap)
        {
            songcasting = SongSpellcastingCap;
        }
        cast = FloorMulMs(cast, 1.0f - (static_cast<float>(songcasting) / 100.0f));
    }
    else if (p.spellGroup == SpellGroupNinjutsu)
    {
        if (p.isPC)
        {
            cast = FloorMulMs(cast, 1.0f - (0.03f * static_cast<float>(p.ninjutsuCastTimeBonus)));
        }
    }

    auto fastCast = ClampI16(p.fastCast, -100, 50);
    if (p.skillType == SkillElementalMagic)
    {
        fastCast = static_cast<std::int16_t>(fastCast + p.elementalCelerity);
    }
    else if (p.isCure)
    {
        fastCast = static_cast<std::int16_t>(fastCast + p.cureCastTime);
        if (p.isPC)
        {
            fastCast = static_cast<std::int16_t>(fastCast + p.cureCastTimeMerit);
        }
    }
    else if (p.skillType == SkillGeomancy && p.isPC)
    {
        fastCast = static_cast<std::int16_t>(fastCast + p.widenedCompassJP);
    }
    fastCast = ClampI16(fastCast, -100, 80);

    auto uncappedFastCast    = ClampI16(p.uFastCast, -100, 100);
    auto inspirationFastCast = ClampI16(p.inspirationFastCast, -100, 100);
    if (p.isNa)
    {
        uncappedFastCast = ClampI16(static_cast<std::int16_t>(uncappedFastCast + p.divineBenison), -100, 100);
    }

    auto sumFastCast = static_cast<float>(fastCast + uncappedFastCast + inspirationFastCast);
    sumFastCast      = std::clamp(sumFastCast, -100.0f, 100.0f);

    cast = FloorMulMs(cast, (100.0f - sumFastCast) / 100.0f);
    if (cast < 0)
    {
        return 0;
    }
    return cast;
}

} // namespace spellcasthelpers
