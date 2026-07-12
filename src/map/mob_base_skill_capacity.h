#pragma once

#include <algorithm>
#include <cstdint>

// Pure GetBaseSkill rank→proxy + GetMagicEvasion rank + JobSkillRankToBaseEvaRank map.
// Parity: internal/mobutils base_skill.go / GetMagicEvasion (slice 1599).
// Hosts still call battleutils::GetMaxSkill / GetSkillRank for table data.

namespace mobbaseskillhelpers
{

// SKILL_* / JOB_* numeric pins (battle_entity.h / job enums).
constexpr std::uint16_t SkillGreatAxe = 6;
constexpr std::uint16_t SkillStaff    = 12;
constexpr std::uint16_t SkillArchery  = 25;
constexpr std::uint16_t SkillThrowing = 27;
constexpr std::uint16_t SkillEvasion  = 29;
constexpr std::uint8_t  JobWAR        = 1;
constexpr std::uint8_t  JobMNK        = 2;

// GetBaseSkill rank 1–5 → skill type + job for GetMaxSkill(skill, job, level).
constexpr auto BaseSkillProxy(const std::uint8_t rank, std::uint16_t& skillType, std::uint8_t& job) -> bool
{
    switch (rank)
    {
        case 1:
            skillType = SkillGreatAxe;
            job       = JobWAR;
            return true;
        case 2:
            skillType = SkillStaff;
            job       = JobWAR;
            return true;
        case 3:
            skillType = SkillEvasion;
            job       = JobWAR;
            return true;
        case 4:
            skillType = SkillArchery;
            job       = JobWAR;
            return true;
        case 5:
            skillType = SkillThrowing;
            job       = JobMNK;
            return true;
        default:
            return false;
    }
}

// GetMagicEvasion skill-cap rank: trusts G(12), mobs C(7).
constexpr auto MagicEvasionCapRank(const bool isTrust) -> std::uint8_t
{
    return isTrust ? 12 : 7;
}

constexpr auto CapMagicEvasionLevel(const std::uint8_t mLvl) -> std::uint8_t
{
    return mLvl > 99 ? 99 : mLvl;
}

// JobSkillRankToBaseEvaRank pure map: skill ranks 1–10 → def/eva rank 1–5.
// best is min(main, sub) when both present; invalid → 0 with caller fallback 3.
constexpr auto JobSkillRankToBaseEvaRank(const std::uint8_t bestSkillRank) -> std::uint8_t
{
    switch (bestSkillRank)
    {
        case 1:
        case 2:
            return 1; // A, A+
        case 3:
        case 4:
        case 5:
            return 2; // B+, B, B-
        case 6:
        case 7:
        case 8:
            return 3; // C+, C, C-
        case 9:
            return 4; // D
        case 10:
            return 5; // E
        default:
            return 0; // invalid — host falls back to 3 + ShowError
    }
}

constexpr auto BestEvasionSkillRank(const std::uint8_t mainRank, const std::uint8_t subRank, const bool sjobIsNon) -> std::uint8_t
{
    if (sjobIsNon)
    {
        return mainRank;
    }
    return std::min(mainRank, subRank);
}

} // namespace mobbaseskillhelpers
