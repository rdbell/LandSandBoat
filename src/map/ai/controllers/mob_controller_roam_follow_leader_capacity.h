#pragma once

namespace mobcontrollerroamfollowleader
{
inline auto ShouldPath(bool outsideLeash, bool leaderMoving) -> bool
{
    return outsideLeash && leaderMoving;
}
} // namespace mobcontrollerroamfollowleader
