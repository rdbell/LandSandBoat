#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

// Pure skillchain damage product (skillchain.lua calculateSkillchainDamage floors).
// Parity: internal/skillchaindmg
// Multiplier injects (day/weather, staff, affinity, nullification, absorption,
// magic-taken) remain host-resolved.

namespace skillchaindamagehelpers
{

constexpr std::uint8_t ElementNone    = 0;
constexpr std::uint8_t ElementFire    = 1;
constexpr std::uint8_t ElementIce     = 2;
constexpr std::uint8_t ElementWind    = 3;
constexpr std::uint8_t ElementEarth   = 4;
constexpr std::uint8_t ElementThunder = 5;
constexpr std::uint8_t ElementWater   = 6;
constexpr std::uint8_t ElementLight   = 7;
constexpr std::uint8_t ElementDark    = 8;

constexpr std::uint8_t SCNone          = 0;
constexpr std::uint8_t SCTransfixion   = 1;
constexpr std::uint8_t SCCompression   = 2;
constexpr std::uint8_t SCLiquefaction  = 3;
constexpr std::uint8_t SCScission      = 4;
constexpr std::uint8_t SCReverberation = 5;
constexpr std::uint8_t SCDetonation    = 6;
constexpr std::uint8_t SCInduration    = 7;
constexpr std::uint8_t SCImpaction     = 8;
constexpr std::uint8_t SCGravitation   = 9;
constexpr std::uint8_t SCDistortion    = 10;
constexpr std::uint8_t SCFusion        = 11;
constexpr std::uint8_t SCFragmentation = 12;
constexpr std::uint8_t SCLight         = 13;
constexpr std::uint8_t SCDarkness      = 14;
constexpr std::uint8_t SCLightII       = 15;
constexpr std::uint8_t SCDarknessII    = 16;

constexpr std::uint8_t MinLevel = 1;
constexpr std::uint8_t MaxLevel = 4;
constexpr std::uint8_t MinCount = 1;
constexpr std::uint8_t MaxCount = 6;

constexpr int ResRankMin = -3;
constexpr int ResRankMax = 11;

constexpr std::int32_t DamageClampMax = 99999;

// chainMultipliers[level-1][count-1]
constexpr double kChainMultipliers[4][6] = {
    { 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 },
    { 0.60, 0.75, 1.00, 1.25, 1.50, 1.75 },
    { 1.00, 1.50, 1.75, 2.00, 2.25, 2.50 },
    { 1.50, 1.80, 2.10, 2.40, 2.70, 3.00 },
};

// resistanceRankMultiplier for ranks −3..11 (offset +3)
constexpr double kResRankMultiplier[15] = {
    1.50, 1.30, 1.15, 1.00, 0.85, 0.70, 0.60, 0.50, 0.40, 0.30, 0.25, 0.20, 0.15, 0.10, 0.05,
};

// skillchainElementTable[element-1][scType-1] FIRE..DARK × Transfixion..DarknessII
// Matches GetSkillchainMagicElement membership (not Lua skillchainType+1 quirk).
constexpr std::uint8_t kSkillchainElementTable[8][16] = {
    // T  C  L  S  R  D  I  I  G  D  F  F  L  D  L  D
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0 }, // Fire
    { 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1 }, // Ice
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // Wind
    { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1 }, // Earth
    { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0 }, // Thunder
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 }, // Water
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0 }, // Light
    { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1 }, // Dark
};

constexpr auto ChainMultiplier(const std::uint8_t level, const std::uint8_t count) -> double
{
    if (level < MinLevel || level > MaxLevel || count < MinCount || count > MaxCount)
    {
        return 0.0;
    }
    return kChainMultipliers[level - 1][count - 1];
}

constexpr auto ClampResRank(int rank) -> int
{
    if (rank < ResRankMin)
    {
        return ResRankMin;
    }
    if (rank > ResRankMax)
    {
        return ResRankMax;
    }
    return rank;
}

constexpr auto ResRankMultiplier(int rank) -> double
{
    rank = ClampResRank(rank);
    return kResRankMultiplier[rank - ResRankMin];
}

constexpr auto HasElement(const std::uint8_t skillchainType, const std::uint8_t element) -> bool
{
    if (skillchainType < SCTransfixion || skillchainType > SCDarknessII)
    {
        return false;
    }
    if (element < ElementFire || element > ElementDark)
    {
        return false;
    }
    return kSkillchainElementTable[element - ElementFire][skillchainType - SCTransfixion] > 0;
}

inline auto ElementsFor(const std::uint8_t skillchainType) -> std::vector<std::uint8_t>
{
    std::vector<std::uint8_t> out;
    if (skillchainType < SCTransfixion || skillchainType > SCDarknessII)
    {
        return out;
    }
    out.reserve(4);
    for (std::uint8_t el = ElementFire; el <= ElementDark; ++el)
    {
        if (kSkillchainElementTable[el - ElementFire][skillchainType - SCTransfixion] > 0)
        {
            out.push_back(el);
        }
    }
    return out;
}

// resRankOf(element) → raw resistance-rank mod. Ties: earliest FIRE..DARK via reverse scan.
inline auto SelectElement(const std::uint8_t skillchainType, const std::function<int(std::uint8_t)>& resRankOf) -> std::uint8_t
{
    const auto elements = ElementsFor(skillchainType);
    if (elements.empty())
    {
        return ElementNone;
    }
    if (elements.size() == 1)
    {
        return elements[0];
    }
    int          lowestResRank = ResRankMax;
    std::uint8_t lowestElement = ElementFire;
    for (int j = static_cast<int>(elements.size()) - 1; j >= 0; --j)
    {
        const auto el   = elements[static_cast<std::size_t>(j)];
        const auto rank = resRankOf ? resRankOf(el) : 0;
        if (rank <= lowestResRank)
        {
            lowestResRank = rank;
            lowestElement = el;
        }
    }
    return lowestElement;
}

struct ProductParams
{
    bool          hasEffect{};
    std::uint8_t  type{}; // SC power
    std::uint8_t  level{};
    std::uint8_t  count{};
    std::uint8_t  element{};
    bool          nullified{};
    std::int32_t  baseDamage{};
    double        bonusMult{ 1.0 };
    double        damageMult{ 1.0 };
    double        dayWeatherMult{ 1.0 };
    double        staffMult{ 1.0 };
    double        affinityMult{ 1.0 };
    double        magicTakenMult{ 1.0 };
    double        inninMult{ 1.0 };
    double        sengikoriMult{ 1.0 };
    double        absorbMult{ 1.0 };
    int           resRank{};
    std::int32_t  magicDamage{};
};

struct ProductResult
{
    std::int32_t damage{};
    std::uint8_t element{};
    bool         consumeSengikori{};
    bool         applied{};
};

inline auto Product(const ProductParams& p) -> ProductResult
{
    if (!p.hasEffect || p.type == SCNone)
    {
        return {};
    }
    if (p.level < MinLevel || p.level > MaxLevel)
    {
        return {};
    }
    if (p.count < MinCount || p.count > MaxCount)
    {
        return {};
    }
    if (p.element == ElementNone)
    {
        return {};
    }
    if (p.nullified)
    {
        return ProductResult{ 0, p.element, false, false };
    }

    double final = std::fabs(static_cast<double>(p.baseDamage));
    final        = std::floor(final * ChainMultiplier(p.level, p.count));
    final        = std::floor(final * p.bonusMult) + static_cast<double>(p.magicDamage);
    final        = std::floor(final * p.damageMult);
    final        = std::floor(final * p.dayWeatherMult);
    final        = std::floor(final * p.staffMult);
    final        = std::floor(final * p.affinityMult);
    final        = std::floor(final * ResRankMultiplier(p.resRank));
    final        = std::floor(final * p.magicTakenMult);
    final        = std::floor(final * p.inninMult);
    final        = std::floor(final * p.sengikoriMult);
    final        = std::floor(final * p.absorbMult);

    return ProductResult{
        static_cast<std::int32_t>(final),
        p.element,
        true,
        true,
    };
}

// Clamp damage after phalanx/ofa/stoneskin (Lua utils.clamp 0..99999).
constexpr auto ClampSCDamage(const std::int32_t damage) -> std::int32_t
{
    if (damage < 0)
    {
        return 0;
    }
    if (damage > DamageClampMax)
    {
        return DamageClampMax;
    }
    return damage;
}

// ApplyPhalanx: max(damage - phalanx, 0)
constexpr auto ApplyPhalanx(const std::int32_t damage, const std::int16_t phalanx) -> std::int32_t
{
    if (damage <= 0)
    {
        return damage;
    }
    auto d = damage - static_cast<std::int32_t>(phalanx);
    return d < 0 ? 0 : d;
}

} // namespace skillchaindamagehelpers
