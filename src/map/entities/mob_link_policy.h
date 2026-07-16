#pragma once

namespace moblinkhelpers
{

// Keeps the ordered CMobEntity::CanLink eligibility checks testable without
// materializing a live entity. Deferred predicates preserve short-circuiting.
template <typename FacingTarget, typename WithinLinkRadius, typename CanSeeTarget>
inline auto CanLink(const bool superLinkMatches, const bool neutral, const bool hiddenWorm, const bool hiddenAmbush, const bool sightOnly,
                    FacingTarget&& facingTarget, WithinLinkRadius&& withinLinkRadius, const bool noLink, CanSeeTarget&& canSeeTarget) -> bool
{
    if (superLinkMatches)
    {
        return true;
    }
    if (neutral || hiddenWorm || hiddenAmbush)
    {
        return false;
    }
    if (sightOnly && !facingTarget())
    {
        return false;
    }
    if (!withinLinkRadius() || noLink)
    {
        return false;
    }
    return canSeeTarget();
}

} // namespace moblinkhelpers
