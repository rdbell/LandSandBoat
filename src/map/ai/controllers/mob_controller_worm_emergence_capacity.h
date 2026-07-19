#pragma once

namespace mobcontrollerwormemergence
{
// ShouldEmerge reports whether a completed roam path should bring up a worm.
constexpr auto ShouldEmerge(const bool worm, const bool untargetable) -> bool
{
    return worm && untargetable;
}
} // namespace mobcontrollerwormemergence
