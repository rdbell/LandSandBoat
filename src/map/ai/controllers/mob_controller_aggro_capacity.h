#pragma once

namespace mobcontrolleraggro
{
// CanAggroTarget reports whether static aggro admission gates pass and the
// target is detected. canDetectTarget is evaluated only after static admission.
template <typename CanDetectTarget>
constexpr auto CanAggroTarget(
    const bool hasTarget, const bool sameBattle, const bool alwaysAggro, const bool mobAggro, const bool neutral,
    const bool mobDead, const bool noAggro, const bool fomor, const bool notorious, const bool zoneIsCoP,
    const bool targetIsPlayer, const int fomorHate, const bool worm, const bool nameHidden, const bool targetDead,
    const bool targetMounted, const bool hasMaster, const bool spawned, const bool engaged, CanDetectTarget&& canDetectTarget) -> bool
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
    if (hasMaster || !spawned || engaged)
    {
        return false;
    }
    return canDetectTarget();
}

// CanAggroTarget preserves the direct-result form used by existing callers.
constexpr auto CanAggroTarget(
    const bool hasTarget, const bool sameBattle, const bool alwaysAggro, const bool mobAggro, const bool neutral,
    const bool mobDead, const bool noAggro, const bool fomor, const bool notorious, const bool zoneIsCoP,
    const bool targetIsPlayer, const int fomorHate, const bool worm, const bool nameHidden, const bool targetDead,
    const bool targetMounted, const bool hasMaster, const bool spawned, const bool engaged, const bool detectsTarget) -> bool
{
    return CanAggroTarget(
        hasTarget, sameBattle, alwaysAggro, mobAggro, neutral, mobDead, noAggro, fomor, notorious, zoneIsCoP,
        targetIsPlayer, fomorHate, worm, nameHidden, targetDead, targetMounted, hasMaster, spawned, engaged,
        [&]() { return detectsTarget; });
}
} // namespace mobcontrolleraggro
