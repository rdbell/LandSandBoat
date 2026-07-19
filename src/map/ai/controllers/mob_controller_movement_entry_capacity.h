#pragma once

namespace mobcontrollermovemententry
{
// ShouldEnter reports whether Move should enter normal target-chase handling.
constexpr auto ShouldEnter(const bool outOfRange, const bool alreadyMoving, const bool canFollowPath) -> bool
{
    return (outOfRange || alreadyMoving) && canFollowPath;
}
} // namespace mobcontrollermovemententry
