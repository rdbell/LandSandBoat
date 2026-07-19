#pragma once

namespace mobcontrollerattackrangesource
{
// Resolve selects Move's base attack range before distance-offset adjustment.
constexpr auto Resolve(const float meleeRange, const bool hasSkillRange, const float skillRange, const bool rangedEnabled, const float rangedRange) -> float
{
    if (rangedEnabled)
    {
        return rangedRange;
    }
    return hasSkillRange ? skillRange : meleeRange;
}
} // namespace mobcontrollerattackrangesource
