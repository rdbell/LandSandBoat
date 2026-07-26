#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace synthskilluphelpers
{

constexpr auto IsEligible(const uint16 charSkill, const uint16 maxSkill, const int16 baseDiff, const bool modernSystem,
                          const bool synthesisFailed) -> bool
{
    if (charSkill >= maxSkill)
    {
        return false;
    }

    const int8 minDiff = modernSystem ? -11 : 0;
    if (baseDiff <= minDiff)
    {
        return false;
    }

    return !synthesisFailed || (baseDiff <= 5 && baseDiff > 0);
}

struct SpecializationPlan
{
    uint8  skillHighest;
    uint16 skillCumulation;
    bool   removeSkill;
};

constexpr auto MakeSpecializationPlan(const uint8 firstCraftSkillID, const uint8 skillID, const uint16 charSkill,
                                      const uint8 skillUpAmount, const uint16 craftCommonCap,
                                      const uint16 specializationPoints, const std::array<uint16, 8>& craftSkills) -> SpecializationPlan
{
    SpecializationPlan plan{ skillID, skillUpAmount, false };
    uint16             skillHighestValue = craftCommonCap;

    if (charSkill + skillUpAmount > craftCommonCap)
    {
        for (uint8 index = 0; index < 8; ++index)
        {
            const uint16 skill = craftSkills[index];
            if (skill > craftCommonCap)
            {
                plan.skillCumulation += skill - craftCommonCap;
                const uint8 candidateSkillID = firstCraftSkillID + index;
                if (candidateSkillID != skillID && skill > skillHighestValue)
                {
                    plan.skillHighest = candidateSkillID;
                    skillHighestValue = skill;
                }
            }
        }
    }

    plan.removeSkill = plan.skillCumulation > specializationPoints;
    return plan;
}

} // namespace synthskilluphelpers
