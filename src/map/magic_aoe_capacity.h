#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure xi.combat.magicAoE.calculateTypeAndRadius / calculateSongRadius.
// Parity: internal/magicaoe (slice 0849); production wire slice 1581.

namespace magicaoehelpers
{

constexpr std::uint8_t AOENone       = 0;
constexpr std::uint8_t AOERadial     = 1;
constexpr std::uint8_t AOEConal      = 2;
constexpr std::uint8_t AOERadialMani = 3;
constexpr std::uint8_t AOERadialAcce = 4;
constexpr std::uint8_t AOEPianissimo = 5;
constexpr std::uint8_t AOEDiffusion  = 6;

constexpr std::uint8_t SpellGroupSong = 1;
constexpr std::uint8_t SpellGroupBlue = 3;

constexpr std::uint16_t SpellFamilyCure     = 1;
constexpr std::uint16_t SpellFamilyNa       = 4;
constexpr std::uint16_t SpellFamilyProtect  = 10;
constexpr std::uint16_t SpellFamilyUtsusemi = 96;
constexpr std::uint16_t SpellFamilyFira     = 148;
constexpr std::uint16_t SpellFamilyWatera   = 153;

constexpr std::uint16_t SpellErase = 143;

constexpr std::uint8_t JobBRD               = 10;
constexpr std::uint8_t SkillStringInstrument = 41;
constexpr std::uint8_t ElementNone          = 0;
constexpr std::uint8_t SkillRankC           = 7; // xi.skillRank.C

constexpr int OverrideRadius = 10;

struct Result
{
    std::uint8_t type{};
    int          radius{};
    bool         consumePianissimo{};
};

inline auto SongRadius(const int           baseRadius,
                       const std::uint8_t  baseType,
                       const bool          hasPianissimo,
                       const std::uint8_t  mainJob,
                       const std::uint8_t  rangedSkillType,
                       const std::uint8_t  spellGroup,
                       const int           stringSkill,
                       const int           skillCap) -> int
{
    if (baseType == AOENone)
    {
        return 0;
    }
    if (baseType == AOEPianissimo && hasPianissimo)
    {
        return 0;
    }
    if (mainJob != JobBRD || rangedSkillType != SkillStringInstrument || spellGroup != SpellGroupSong)
    {
        return baseRadius;
    }
    if (skillCap <= 0)
    {
        return baseRadius;
    }
    double mult = static_cast<double>(stringSkill) / static_cast<double>(skillCap);
    mult        = std::max(1.0, std::min(2.0, mult));
    return static_cast<int>(std::floor(static_cast<double>(baseRadius) * mult));
}

constexpr auto MobAoE(const std::uint8_t baseType, const int baseRadius) -> Result
{
    switch (baseType)
    {
        case AOERadialMani:
        case AOERadialAcce:
        case AOEDiffusion:
            return Result{ AOENone, 0, false };
        case AOEPianissimo:
            return Result{ AOERadial, baseRadius, false };
        default:
            return Result{ baseType, baseRadius, false };
    }
}

struct Params
{
    std::uint8_t  baseType{};
    int           baseRadius{};
    std::uint16_t spellFamily{};
    std::uint8_t  spellGroup{};
    std::uint16_t spellID{};
    std::uint8_t  element{};
    bool          isPC{};
    bool          isTrust{};
    bool          hasMajesty{};
    bool          hasAccession{};
    bool          hasManifestation{};
    bool          hasTheurgicFocus{};
    bool          hasPianissimo{};
    bool          hasDiffusion{};
    bool          hasConvergence{};
    bool          divineVeilProc{};
    int           utsusemiAOEMod{};
    std::uint8_t  mainJob{};
    std::uint8_t  rangedSkillType{};
    int           stringSkill{};
    int           skillCap{};
};

inline auto TypeAndRadius(const Params& p) -> Result
{
    if (!p.isPC && !p.isTrust)
    {
        return MobAoE(p.baseType, p.baseRadius);
    }

    if (p.hasMajesty)
    {
        if (p.spellFamily == SpellFamilyCure || p.spellFamily == SpellFamilyProtect)
        {
            return Result{ AOERadial, OverrideRadius, false };
        }
    }

    if (p.baseType == AOERadialAcce)
    {
        if (p.hasAccession)
        {
            return Result{ AOERadial, OverrideRadius, false };
        }
        if ((p.spellFamily == SpellFamilyNa || p.spellID == SpellErase) && p.divineVeilProc)
        {
            return Result{ AOERadial, OverrideRadius, false };
        }
        return Result{ AOENone, 0, false };
    }

    if (p.hasManifestation && p.baseType == AOERadialMani)
    {
        return Result{ AOERadial, OverrideRadius, false };
    }

    if (p.hasTheurgicFocus && p.spellFamily >= SpellFamilyFira && p.spellFamily <= SpellFamilyWatera)
    {
        return Result{ AOERadial, p.baseRadius / 2, false };
    }

    if (p.spellGroup == SpellGroupSong)
    {
        if (p.baseType == AOENone)
        {
            return Result{ AOENone, 0, false };
        }
        if (p.hasPianissimo && p.baseType == AOEPianissimo)
        {
            return Result{ AOENone, 0, true };
        }
        const int r = SongRadius(
            p.baseRadius,
            p.baseType,
            p.hasPianissimo,
            p.mainJob,
            p.rangedSkillType,
            p.spellGroup,
            p.stringSkill,
            p.skillCap);
        return Result{ AOERadial, r, false };
    }

    if (p.hasDiffusion && p.baseType == AOEDiffusion)
    {
        return Result{ AOERadial, OverrideRadius, false };
    }

    if (p.hasConvergence && p.spellGroup == SpellGroupBlue && p.element != ElementNone)
    {
        return Result{ AOENone, 0, false };
    }

    if (p.utsusemiAOEMod != 0 && p.spellFamily == SpellFamilyUtsusemi)
    {
        return Result{ AOERadial, OverrideRadius, false };
    }

    return Result{ p.baseType, p.baseRadius, false };
}

// Divine Veil proc inject: trait && (divineSeal || roll1to100 <= aoeNaMod).
constexpr auto DivineVeilProc(const bool hasTrait, const bool hasDivineSeal, const int aoeNaMod, const int roll1to100) -> bool
{
    if (!hasTrait)
    {
        return false;
    }
    return hasDivineSeal || roll1to100 <= aoeNaMod;
}

} // namespace magicaoehelpers
