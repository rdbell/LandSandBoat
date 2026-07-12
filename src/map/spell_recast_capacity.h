#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure battleutils::CalculateSpellRecastTime after entity/spell injects.
// Durations are millisecond counts (SQL recast is ms).
// Parity: internal/spell.CalculateSpellRecastTime

namespace spellrecasthelpers
{

constexpr std::uint8_t  AOERadialMani = 3;
constexpr std::uint8_t  AOERadialAcce = 4;
constexpr std::uint16_t SpellGroupSong  = 1;
constexpr std::uint16_t SpellGroupBlack = 2;
constexpr std::uint16_t SpellGroupWhite = 6;

constexpr std::uint8_t SkillElementalMagic  = 36;
constexpr std::uint8_t SkillBlueMagic       = 43;
constexpr std::uint8_t SkillHealingMagic    = 33;
constexpr std::uint8_t SkillEnfeeblingMagic = 35;
constexpr std::uint8_t SkillEnhancingMagic  = 34;

constexpr std::uint16_t IDHordeLullaby   = 376;
constexpr std::uint16_t IDHordeLullabyII = 377;
constexpr std::uint16_t IDMagicFinale    = 462;
constexpr std::uint16_t IDFoeLullaby     = 463;
constexpr std::uint16_t IDFoeLullabyII   = 471;

constexpr std::uint8_t JobSCH = 20;

constexpr float DefaultRecastReductionCap = 80.0f;
constexpr float FastCastRecastCap         = 40.0f;

constexpr std::int32_t HasteMagicMin = -10000;
constexpr std::int32_t HasteMagicMax = 4375;
constexpr std::int32_t HasteGearMin  = -2500;
constexpr std::int32_t HasteGearMax  = 2500;

constexpr auto ShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

// Floor multiply matching chrono::floor<milliseconds>(duration * factor) when
// duration is whole milliseconds.
inline auto FloorMulMs(const std::int64_t ms, const float factor) -> std::int64_t
{
    return static_cast<std::int64_t>(std::floor(static_cast<float>(ms) * factor));
}

inline auto RecastCapFloorMs(const std::int64_t baseMs, const float reductionCap) -> std::int64_t
{
    // Match Go (100-cap)/100 so clean percent steps stay exact in float
    // (1.0f - cap/100.0f can yield 0.1999… for cap=80 → floor 1999).
    return FloorMulMs(baseMs, (100.0f - reductionCap) / 100.0f);
}

inline auto MaxI64(const std::int64_t a, const std::int64_t b) -> std::int64_t
{
    return a > b ? a : b;
}

struct SpellRecastParams
{
    std::int64_t  baseMs{};
    std::uint16_t spellID{};
    std::uint16_t spellGroup{};
    std::uint8_t  aoe{};
    std::uint8_t  skillType{};

    float recastReductionCap{ DefaultRecastReductionCap }; // settings SPELL_RECAST_REDUCTION_CAP

    std::int16_t fastCast{};
    std::int16_t inspirationFastCast{};
    std::int32_t hasteMagic{};
    std::int32_t hasteGear{};

    std::int32_t elementalMagicRecast{};
    std::int32_t blueMagicRecast{};
    std::int32_t healingMagicRecast{};
    std::int32_t enfeeblingMagicRecast{};
    std::int32_t enhancingMagicRecast{};

    std::int16_t blackMagicRecast{};
    std::int16_t whiteMagicRecast{};
    std::int16_t grimoireSpellcasting{};

    std::int32_t songRecastDelaySec{};
    std::int32_t finaleRecastMeritSec{};
    std::int32_t lullabyRecastMeritSec{};
    bool         isPC{};

    std::uint16_t alacrityCelerityEffect{};

    bool nightingale{};
    bool composure{};
    bool hassoOrSeigan{};
    bool manifestation{};
    bool accession{};
    bool darkArtsOrAddendumBlack{};
    bool lightArtsOrAddendumWhite{};
    bool alacrity{};
    bool celerity{};

    std::uint8_t mainJob{};
    bool         weatherMatchesElement{};
};

inline auto SkillTypeRecastMod(const SpellRecastParams& p) -> std::int32_t
{
    switch (p.skillType)
    {
        case SkillElementalMagic:
            return p.elementalMagicRecast;
        case SkillBlueMagic:
            return p.blueMagicRecast;
        case SkillHealingMagic:
            return p.healingMagicRecast;
        case SkillEnfeeblingMagic:
            return p.enfeeblingMagicRecast;
        case SkillEnhancingMagic:
            return p.enhancingMagicRecast;
        default:
            return 0;
    }
}

// Returns recast duration in milliseconds (may be 0; never negative).
inline auto CalculateSpellRecastMs(const SpellRecastParams& p) -> std::int64_t
{
    const auto baseMs = p.baseMs;
    auto       recast = baseMs;

    auto cap = p.recastReductionCap;
    if (cap == 0.0f)
    {
        cap = DefaultRecastReductionCap;
    }
    const auto alacCap = cap + 10.0f;

    auto fastCastReduction = std::clamp(static_cast<float>(p.fastCast) / 2.0f, 0.0f, FastCastRecastCap);
    auto inspirationRecast = static_cast<float>(p.inspirationFastCast) / 2.0f;
    recast                 = FloorMulMs(recast, (100.0f - (fastCastReduction + inspirationRecast)) / 100.0f);

    auto hasteMagic = std::clamp(p.hasteMagic, HasteMagicMin, HasteMagicMax);
    auto hasteGear  = std::clamp(p.hasteGear, HasteGearMin, HasteGearMax);
    auto haste      = hasteMagic + hasteGear;
    recast          = FloorMulMs(recast, (10000.0f - static_cast<float>(haste)) / 10000.0f);

    if (p.spellGroup == SpellGroupSong)
    {
        if (p.nightingale)
        {
            recast = FloorMulMs(recast, 0.5f);
        }
        if (p.isPC)
        {
            if (p.spellID == IDMagicFinale)
            {
                recast -= static_cast<std::int64_t>(p.finaleRecastMeritSec) * 1000;
            }
            if (p.spellID == IDFoeLullaby || p.spellID == IDFoeLullabyII || p.spellID == IDHordeLullaby || p.spellID == IDHordeLullabyII)
            {
                recast -= static_cast<std::int64_t>(p.lullabyRecastMeritSec) * 1000;
            }
        }
        recast -= static_cast<std::int64_t>(p.songRecastDelaySec) * 1000;
    }

    if (p.composure)
    {
        recast = FloorMulMs(recast, 1.25f);
    }
    if (p.hassoOrSeigan)
    {
        recast = FloorMulMs(recast, 1.5f);
    }

    recast = MaxI64(recast, RecastCapFloorMs(baseMs, cap));

    const auto recastMod = SkillTypeRecastMod(p);
    recast               = FloorMulMs(recast, (100.0f + static_cast<float>(recastMod)) / 100.0f);

    if (p.spellGroup == SpellGroupBlack)
    {
        if (p.aoe == AOERadialMani && p.manifestation)
        {
            recast *= (p.mainJob == JobSCH) ? 2 : 3;
        }
        else if (p.darkArtsOrAddendumBlack)
        {
            recast = FloorMulMs(recast, (100.0f + static_cast<float>(p.blackMagicRecast) + static_cast<float>(p.grimoireSpellcasting)) / 100.0f);
        }
        else
        {
            recast = FloorMulMs(recast, (100.0f + static_cast<float>(p.blackMagicRecast)) / 100.0f);
        }

        recast = MaxI64(recast, RecastCapFloorMs(baseMs, cap));

        if (p.alacrity)
        {
            recast = FloorMulMs(recast, 0.60f);
            recast = MaxI64(recast, RecastCapFloorMs(baseMs, alacCap));
            if (p.weatherMatchesElement)
            {
                recast = FloorMulMs(recast, (100.0f - static_cast<float>(p.alacrityCelerityEffect)) / 100.0f);
                recast = MaxI64(recast, RecastCapFloorMs(baseMs, alacCap));
            }
        }
    }
    else if (p.spellGroup == SpellGroupWhite)
    {
        if (p.aoe == AOERadialAcce && p.accession)
        {
            recast *= (p.mainJob == JobSCH) ? 2 : 3;
        }

        if (p.lightArtsOrAddendumWhite)
        {
            recast = FloorMulMs(recast, (100.0f + static_cast<float>(p.whiteMagicRecast) + static_cast<float>(p.grimoireSpellcasting)) / 100.0f);
        }
        else
        {
            recast = FloorMulMs(recast, (100.0f + static_cast<float>(p.whiteMagicRecast)) / 100.0f);
        }

        recast = MaxI64(recast, RecastCapFloorMs(baseMs, cap));

        if (p.celerity)
        {
            recast = FloorMulMs(recast, 0.60f);
            recast = MaxI64(recast, RecastCapFloorMs(baseMs, alacCap));
            if (p.weatherMatchesElement)
            {
                recast = FloorMulMs(recast, (100.0f - static_cast<float>(p.alacrityCelerityEffect)) / 100.0f);
                recast = MaxI64(recast, RecastCapFloorMs(baseMs, alacCap));
            }
        }
    }

    if (recast < 0)
    {
        return 0;
    }
    return recast;
}

} // namespace spellrecasthelpers
