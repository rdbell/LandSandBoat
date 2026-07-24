#include "test_zone_spatial_grid_rebuild_6254.h"

#include "map/zone_spatial_grid_rebuild.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone spatial-grid rebuild 6254 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins CZoneEntities::rebuildSpatialGrid's alwaysRelevantNpcs_ cache filter.
auto runZoneSpatialGridRebuild6254SelfTests() -> bool
{
    bool ok = true;
    for (const bool alwaysRelevant : { false, true })
    {
        ok = expect(zoneentityvisibility::ShouldCacheAlwaysRelevantNPC(alwaysRelevant) == alwaysRelevant,
                    "only always-relevant NPCs enter rebuild cache") &&
             ok;
    }
    return ok;
}
