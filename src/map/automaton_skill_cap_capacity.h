#pragma once

#include <cstdint>

// Pure xi.pets.automaton.skillCaps rank resolution for puppetutils::getSkillCap.
// Parity: internal/automaton SkillCapRank (slice 1588).
// Production wire: puppetutils.cpp getSkillCap.

namespace automatonskillcaphelpers
{

// AutomatonFrame / AutomatonHead enum values.
constexpr std::uint8_t FrameHarlequin  = 0x20;
constexpr std::uint8_t FrameValoredge  = 0x21;
constexpr std::uint8_t FrameSharpshot  = 0x22;
constexpr std::uint8_t FrameStormwaker = 0x23;

constexpr std::uint8_t HeadHarlequin    = 0x01;
constexpr std::uint8_t HeadValoredge    = 0x02;
constexpr std::uint8_t HeadSharpshot    = 0x03;
constexpr std::uint8_t HeadStormwaker   = 0x04;
constexpr std::uint8_t HeadSoulsoother  = 0x05;
constexpr std::uint8_t HeadSpiritreaver = 0x06;

// SKILL_AUTOMATON_*
constexpr std::uint8_t SkillMelee  = 22;
constexpr std::uint8_t SkillRanged = 23;
constexpr std::uint8_t SkillMagic  = 24;

// xi.skillRank ordinals (lower is better).
constexpr std::int8_t RankAPlus  = 1;
constexpr std::int8_t RankBPlus  = 3;
constexpr std::int8_t RankBMinus = 5;
constexpr std::int8_t RankCPlus  = 6;
constexpr std::int8_t RankF      = 11;

constexpr std::int8_t HeadBonus        = -2;
constexpr std::int8_t HeadOnlyRankRemap = 13;

constexpr auto IsAutomatonSkill(const std::uint8_t skill) -> bool
{
    return skill >= SkillMelee && skill <= SkillMagic;
}

constexpr auto CapLevel(const std::uint8_t level) -> std::uint8_t
{
    return level > 99 ? 99 : level;
}

// Frame base rank for skill, or 0 if unlisted.
constexpr auto FrameSkillRank(const std::uint8_t frame, const std::uint8_t skill) -> std::int8_t
{
    switch (frame)
    {
        case FrameHarlequin:
            switch (skill)
            {
                case SkillMelee:
                case SkillRanged:
                case SkillMagic:
                    return RankBMinus;
                default:
                    return 0;
            }
        case FrameValoredge:
            return skill == SkillMelee ? RankBPlus : 0;
        case FrameSharpshot:
            if (skill == SkillMelee)
            {
                return RankCPlus;
            }
            if (skill == SkillRanged)
            {
                return RankBPlus;
            }
            return 0;
        case FrameStormwaker:
            if (skill == SkillMelee)
            {
                return RankCPlus;
            }
            if (skill == SkillMagic)
            {
                return RankBPlus;
            }
            return 0;
        default:
            return 0;
    }
}

// Whether the frame lists the skill innately (unlisted → no base rank).
constexpr auto FrameHasSkill(const std::uint8_t frame, const std::uint8_t skill) -> bool
{
    switch (frame)
    {
        case FrameHarlequin:
            return skill == SkillMelee || skill == SkillRanged || skill == SkillMagic;
        case FrameValoredge:
            return skill == SkillMelee;
        case FrameSharpshot:
            return skill == SkillMelee || skill == SkillRanged;
        case FrameStormwaker:
            return skill == SkillMelee || skill == SkillMagic;
        default:
            return false;
    }
}

// Head rank delta for skill, or 0 if the head does not list that skill.
constexpr auto HeadSkillBonus(const std::uint8_t head, const std::uint8_t skill) -> std::int8_t
{
    switch (head)
    {
        case HeadValoredge:
            return skill == SkillMelee ? HeadBonus : 0;
        case HeadSharpshot:
            return skill == SkillRanged ? HeadBonus : 0;
        case HeadStormwaker:
        case HeadSoulsoother:
        case HeadSpiritreaver:
            return skill == SkillMagic ? HeadBonus : 0;
        default:
            return 0;
    }
}

// Rank passed to battleutils::GetMaxSkill (0 when neither grants skill).
constexpr auto SkillCapRank(const std::uint8_t frame, const std::uint8_t head, const std::uint8_t skill) -> std::uint8_t
{
    std::int8_t rank = 0;
    if (FrameHasSkill(frame, skill))
    {
        rank = FrameSkillRank(frame, skill);
    }
    rank += HeadSkillBonus(head, skill);
    if (rank < 0)
    {
        rank = static_cast<std::int8_t>(HeadOnlyRankRemap + rank);
    }
    if (rank < 0)
    {
        return 0;
    }
    return static_cast<std::uint8_t>(rank);
}

// Known skillCaps frame keys.
constexpr auto IsKnownFrame(const std::uint8_t frame) -> bool
{
    switch (frame)
    {
        case FrameHarlequin:
        case FrameValoredge:
        case FrameSharpshot:
        case FrameStormwaker:
            return true;
        default:
            return false;
    }
}

} // namespace automatonskillcaphelpers
