#pragma once

namespace mobcontrollermovement
{
constexpr auto CanMoveForward(
    float standbackRange, const bool rangedEnabled, const float rangedRange, const float distance, const bool standbackBehavior,
    const bool hasTarget, const bool canSeeTarget, const int standbackThreshold, const bool noStandback, const int hpp,
    const int mpp, const int maxMP, const bool isNinja, const bool hasSpells, const bool canCastSpells,
    const float spawnLeash, const float distanceFromSpawn) -> bool
{
    if (standbackRange <= 0)
    {
        standbackRange = 20;
    }
    const bool closingRanged = rangedEnabled && distance > rangedRange;
    if (!closingRanged && standbackBehavior && distance < standbackRange && canSeeTarget)
    {
        return false;
    }
    if (!closingRanged && distance < standbackRange && standbackThreshold > 0 && !noStandback && hpp >= standbackThreshold &&
        (maxMP == 0 || mpp >= standbackThreshold))
    {
        return !isNinja && hasSpells && !canCastSpells;
    }
    if (hasTarget && !canSeeTarget)
    {
        return true;
    }
    return spawnLeash <= 0 || distanceFromSpawn <= spawnLeash;
}
} // namespace mobcontrollermovement
