#pragma once

namespace zoneentityvisibility
{

// ShouldCacheAlwaysRelevantNPC mirrors rebuildSpatialGrid's NPC cache fill.
constexpr auto ShouldCacheAlwaysRelevantNPC(const bool alwaysRelevant) -> bool
{
    return alwaysRelevant;
}

} // namespace zoneentityvisibility
