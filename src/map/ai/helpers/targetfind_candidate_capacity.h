#pragma once

namespace targetfindcandidatehelpers
{

// ShouldRejectDuplicateOrDead preserves validEntity's early ordering: an
// already-selected target is rejected before its dead state is queried.
template <typename IsDead>
inline auto ShouldRejectDuplicateOrDead(const bool duplicate, const bool includeDead, IsDead&& isDead) -> bool
{
    return duplicate || (!includeDead && isDead());
}

} // namespace targetfindcandidatehelpers
