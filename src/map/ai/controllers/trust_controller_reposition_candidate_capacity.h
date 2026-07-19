#pragma once

namespace trustcontrollerrepositioncandidate
{
// ShouldSelect reports whether this is the first valid, visible reposition candidate.
template <typename IsValid, typename CanSee>
constexpr auto ShouldSelect(const bool positionFound, IsValid&& isValid, CanSee&& canSee) -> bool
{
    return !positionFound && isValid() && canSee();
}
} // namespace trustcontrollerrepositioncandidate
