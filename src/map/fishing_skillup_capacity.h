#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure FishingSkillup planning math from fishingutils.
// Parity: internal/fishingutils/skillup.go (slice 1630).

namespace fishingskilluphelpers
{

constexpr std::uint8_t FishingSuccessNone = 0;
constexpr std::uint16_t RodIDLuShang      = 17386;

struct FishingSkillupParams
{
    std::uint8_t  successType    = 0;
    std::uint8_t  catchLevel     = 0;
    int           charSkillLevel = 0; // floor(RealSkills / 10)
    std::int32_t  charSkill      = 0;
    std::uint8_t  skillRank      = 0;
    bool          hasLuShangRod  = false;
    bool          inCity         = false;
    std::uint8_t  moonPhase      = 0;
    std::uint8_t  moonDirection  = 0;
    float         multiplier     = 1.0f;
};

struct FishingSkillupPlan
{
    bool eligible        = false;
    int  skillRoll       = 0;
    int  maxChance       = 0;
    int  bonusChanceRoll = 0;
    int  maxSkillAmount  = 0;
    int  maxSkill        = 0;
};

inline auto PlanFishingSkillup(const FishingSkillupParams& p) -> FishingSkillupPlan
{
    if (p.successType == FishingSuccessNone)
    {
        return FishingSkillupPlan{};
    }

    const int maxSkill = (static_cast<int>(p.skillRank) + 1) * 100;
    int       levelDifference = 0;
    if (static_cast<int>(p.catchLevel) > p.charSkillLevel)
    {
        levelDifference = static_cast<int>(p.catchLevel) - p.charSkillLevel;
    }

    if (static_cast<int>(p.catchLevel) <= p.charSkillLevel || levelDifference > 50)
    {
        return FishingSkillupPlan{};
    }

    auto skillRoll       = 90;
    auto bonusChanceRoll = 8;

    if (p.hasLuShangRod && p.charSkillLevel < 50)
    {
        skillRoll += 20;
    }

    const double normDist = std::exp(-0.5 * std::log(2 * M_PI) - std::log(5.0) - std::pow(levelDifference - 11, 2) / 50.0);
    const int    distMod           = static_cast<int>(std::floor(normDist * 200));
    const int    lowerLevelBonus   = static_cast<int>(std::floor((100 - p.charSkillLevel) / 10.0));
    const int    skillLevelPenalty = static_cast<int>(std::floor(p.charSkillLevel / 10.0));

    auto maxChance = std::max(4, distMod + lowerLevelBonus - skillLevelPenalty);
    maxChance      = static_cast<int>(static_cast<float>(maxChance) * p.multiplier);

    switch (p.moonDirection)
    {
        case 0:
            if (p.moonPhase == 0)
            {
                skillRoll -= 20;
                bonusChanceRoll -= 3;
            }
            else if (p.moonPhase == 100)
            {
                skillRoll += 10;
                bonusChanceRoll += 3;
            }
            break;
        case 1:
            if (p.moonPhase <= 10)
            {
                skillRoll -= 15;
                bonusChanceRoll -= 2;
            }
            else if (p.moonPhase >= 95 && p.moonPhase <= 100)
            {
                skillRoll += 5;
                bonusChanceRoll += 2;
            }
            break;
        case 2:
            if (p.moonPhase <= 5)
            {
                skillRoll -= 10;
                bonusChanceRoll -= 1;
            }
            else if (p.moonPhase >= 90 && p.moonPhase <= 100)
            {
                bonusChanceRoll += 1;
            }
            break;
        default:
            break;
    }

    if (!p.inCity)
    {
        skillRoll -= 10;
    }

    if (p.charSkillLevel < 50)
    {
        skillRoll -= (20 - static_cast<int>(std::floor(p.charSkillLevel / 3.0)));
    }

    const int skillAmountAdd = 1 + static_cast<int>(std::floor(levelDifference / 5.0));
    const int maxSkillAmount = std::min(skillAmountAdd, 3);

    return FishingSkillupPlan{
        true,
        skillRoll,
        maxChance,
        bonusChanceRoll,
        maxSkillAmount,
        maxSkill,
    };
}

inline auto ClampSkillAmount(const std::int32_t skillAmount, const std::int32_t charSkill, const std::int32_t maxSkill) -> std::int32_t
{
    if (skillAmount + charSkill > maxSkill)
    {
        return maxSkill - charSkill;
    }
    return skillAmount;
}

inline auto SkillLevelCrossed(const std::int32_t charSkill, const std::int32_t skillAmount) -> bool
{
    return (charSkill / 10) < ((charSkill + skillAmount) / 10);
}

} // namespace fishingskilluphelpers
