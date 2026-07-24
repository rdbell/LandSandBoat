#pragma once

#include <utility>

namespace zonespawnlistsync
{

// ShouldRemoveSpawnListEntity mirrors the stale-entry removal pass.
constexpr auto ShouldRemoveSpawnListEntity(const bool visible) -> bool
{
    return !visible;
}

// ShouldIncludeSpawnListCandidate mirrors tryAdd's type-first, visibility
// second filter. Passing visibility lazily preserves the original short-circuit
// behavior for mismatched entity types.
template <typename Visible>
inline auto ShouldIncludeSpawnListCandidate(const bool typeMatches, Visible&& visible) -> bool
{
    return typeMatches && std::forward<Visible>(visible)();
}

} // namespace zonespawnlistsync
