#pragma once

#include <cstdint>

// Pure TryInterruptSpell ratio assembly after singing/Manafont gates.
// Parity: internal/spellinterrupt

namespace spellinterrupthelpers
{

constexpr float LevelRatioMin      = 0.01f;
constexpr int   LevelRatioBasePC   = 50;
constexpr int   LevelRatioBaseMob  = 5;
constexpr float ZeroSkillRatio     = 10.0f;

constexpr auto LevelRatio(const std::uint8_t attackerLevel,
                          const std::uint8_t defenderLevel,
                          const bool         defenderIsMob) -> float
{
    const auto base  = defenderIsMob ? LevelRatioBaseMob : LevelRatioBasePC;
    auto       ratio = static_cast<float>(base + static_cast<int>(attackerLevel) - static_cast<int>(defenderLevel)) / 100.0f;
    if (ratio < LevelRatioMin)
    {
        ratio = LevelRatioMin;
    }
    return ratio;
}

constexpr auto SkillRatio(const float skillCap, const float skillLevel, const bool isPC) -> float
{
    if (!isPC)
    {
        return 1.0f;
    }
    if (skillLevel <= 0.0f)
    {
        return ZeroSkillRatio;
    }
    return skillCap / skillLevel;
}

constexpr auto SIRDRatio(const std::uint8_t meritReduction, const std::int16_t spellInterruptMod) -> float
{
    return (100.0f - static_cast<float>(meritReduction) - static_cast<float>(spellInterruptMod)) / 100.0f;
}

constexpr auto FinalRatio(const float levelRatio, const float skillRatio, const float sirdRatio) -> float
{
    return levelRatio * skillRatio * sirdRatio;
}

struct Params
{
    std::uint8_t  attackerLevel{};
    std::uint8_t  defenderLevel{};
    bool          defenderIsMob{};
    bool          defenderIsPC{};
    float         skillCap{};
    float         skillLevel{};
    std::uint8_t  meritReduction{};
    std::int16_t  spellInterruptMod{};
    bool          hasAquaveil{};
    std::uint16_t aquaveilPower{};
    float         roll{}; // [0, 1)
};

struct Result
{
    bool          interrupted{};
    float         finalRatio{};
    bool          rollInterrupted{};
    bool          aquaveilConsumed{};
    bool          aquaveilDelete{};
    std::uint16_t aquaveilNewPower{};
};

inline auto Evaluate(const Params& p) -> Result
{
    const auto level = LevelRatio(p.attackerLevel, p.defenderLevel, p.defenderIsMob);
    const auto skill = SkillRatio(p.skillCap, p.skillLevel, p.defenderIsPC);
    const auto sird  = SIRDRatio(p.meritReduction, p.spellInterruptMod);
    const auto final = FinalRatio(level, skill, sird);

    Result res{};
    res.finalRatio = final;

    if (p.roll >= final)
    {
        return res;
    }
    res.rollInterrupted = true;

    if (!p.hasAquaveil)
    {
        res.interrupted = true;
        return res;
    }

    res.aquaveilConsumed = true;
    if (p.aquaveilPower <= 1)
    {
        res.aquaveilDelete   = true;
        res.aquaveilNewPower = 0;
    }
    else
    {
        res.aquaveilNewPower = static_cast<std::uint16_t>(p.aquaveilPower - 1);
    }
    return res;
}

} // namespace spellinterrupthelpers
