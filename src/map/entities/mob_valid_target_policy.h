#pragma once

namespace mobvalidtargethelpers
{

template <typename BaseValidTarget>
inline auto ValidTarget(const bool confrontationMatches, BaseValidTarget&& baseValidTarget, const bool targetPlayerDead, const bool raisable, const bool dead,
                        const bool targetPlayer, const bool targetNpc, const bool sameAllegiance, const bool noAssist, const bool charmed) -> bool
{
    if (!confrontationMatches)
    {
        return false;
    }

    if (baseValidTarget())
    {
        return true;
    }

    if (targetPlayerDead && raisable && dead)
    {
        return true;
    }

    return (targetPlayer || targetNpc) && sameAllegiance && !noAssist && !charmed;
}

} // namespace mobvalidtargethelpers
