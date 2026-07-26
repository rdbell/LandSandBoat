#pragma once

#include "common/cbasetypes.h"

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

} // namespace synthskilluphelpers
