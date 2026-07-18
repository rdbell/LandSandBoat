#pragma once

namespace mobcontrolleraggro
{
constexpr auto CanAggroTarget(
    const bool hasTarget, const bool sameBattle, const bool alwaysAggro, const bool mobAggro, const bool neutral,
    const bool mobDead, const bool noAggro, const bool fomor, const bool notorious, const bool zoneIsCoP,
    const bool targetIsPlayer, const int fomorHate, const bool worm, const bool nameHidden, const bool targetDead,
    const bool targetMounted, const bool hasMaster, const bool spawned, const bool engaged, const bool detectsTarget) -> bool
{
    if (!hasTarget || !sameBattle || (!alwaysAggro && !mobAggro) || neutral || mobDead || noAggro)
    {
        return false;
    }
    if (fomor && !notorious && zoneIsCoP && targetIsPlayer && fomorHate < 8)
    {
        return false;
    }
    if ((worm && nameHidden) || targetDead || targetMounted)
    {
        return false;
    }
    return !hasMaster && spawned && !engaged && detectsTarget;
}
} // namespace mobcontrolleraggro
