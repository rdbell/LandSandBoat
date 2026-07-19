#pragma once

namespace mobcontrollerscriptedpath
{
// ShouldFollow reports whether scripted path movement takes this tick.
constexpr auto ShouldFollow(const bool canFollowPath, const bool followingScriptedPath) -> bool
{
    return canFollowPath && followingScriptedPath;
}
} // namespace mobcontrollerscriptedpath
