#pragma once

#include <string>

// Pure CZone::queryEntitiesByName cache policy and ordered resolver. Entity
// ownership and the cache map remain on CZone; callers inject collection walks
// and name matching so this seam is deterministic and fixture-friendly.
namespace zonequeryhelpers
{

struct QueryPlan
{
    bool useCache{};
    bool returnCached{};
    bool cacheResult{};
};

inline auto PlanQueryByName(const std::string& pattern, const bool cacheHit) -> QueryPlan
{
    const bool useCache = !pattern.starts_with("DE_");
    return { useCache, useCache && cacheHit, true };
}

// ResolveMatches preserves LSB's NPC-first, then mob traversal order.
template <typename ForEachNPC, typename ForEachMob, typename Matches, typename Append>
void ResolveMatches(ForEachNPC&& forEachNPC, ForEachMob&& forEachMob, Matches&& matches, Append&& append)
{
    forEachNPC(
        [&](auto* entity)
        {
            if (matches(entity))
            {
                append(entity);
            }
        });
    forEachMob(
        [&](auto* entity)
        {
            if (matches(entity))
            {
                append(entity);
            }
        });
}

} // namespace zonequeryhelpers
