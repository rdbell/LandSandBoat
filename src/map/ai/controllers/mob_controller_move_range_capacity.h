#pragma once

#include <cstdint>

namespace mobcontrollermoverange
{
struct Result { float attackRange; float closeDistance; };

constexpr auto Resolve(
    const float meleeRange, const float attackSkillRange, const bool rangedEnabled, const float rangedRange,
    const int16_t targetDistanceOffset) -> Result
{
    float attackRange = attackSkillRange > 0 ? attackSkillRange : meleeRange;
    if (rangedEnabled)
    {
        attackRange = rangedRange;
    }
    const float offset = targetDistanceOffset == 0 ? 0.4f : static_cast<float>(targetDistanceOffset) / 10.0f;
    const float closeDistance = attackRange - offset < 0 ? 0 : attackRange - offset;
    return { attackRange, closeDistance };
}
} // namespace mobcontrollermoverange
