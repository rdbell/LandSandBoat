#pragma once

namespace mobcontrollerownerdeclaim
{
// ShouldClear reports whether a stale owner claim may be cleared this tick.
constexpr auto ShouldClear(const bool hasOwner, const bool ownerClaimsMob, const bool deadlineReached) -> bool
{
    return hasOwner && !ownerClaimsMob && deadlineReached;
}
} // namespace mobcontrollerownerdeclaim
