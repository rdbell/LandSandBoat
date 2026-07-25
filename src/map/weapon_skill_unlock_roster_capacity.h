#pragma once

#include "weapon_skill_roster_capacity.h"

#include <cstdint>
#include <vector>

namespace weaponskillunlockrosterhelpers
{
struct Candidate
{
    uint16_t id{};
    uint16_t skillLevel{};
    bool     canUse{};
};

struct Facts
{
    uint16_t               currentSkill{};
    std::vector<Candidate> candidates{};
};

struct Plan
{
    std::vector<uint16_t> unlockWeaponSkillIDs{};
};

inline auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan;
    for (const auto& candidate : facts.candidates)
    {
        if (weaponskillrosterhelpers::ShouldUnlockWeaponSkillOnSkillUp(facts.currentSkill, candidate.skillLevel, candidate.canUse))
        {
            plan.unlockWeaponSkillIDs.push_back(candidate.id);
        }
    }
    return plan;
}
} // namespace weaponskillunlockrosterhelpers
