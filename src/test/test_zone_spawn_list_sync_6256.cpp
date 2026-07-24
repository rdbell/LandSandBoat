#include "test_zone_spawn_list_sync_6256.h"

#include "map/zone_spawn_list_sync.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone spawn-list sync 6256 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins CZoneEntities::syncSpawnListWithGrid's stale removal and lazy candidate
// type/visibility admission policy.
auto runZoneSpawnListSync6256SelfTests() -> bool
{
    using namespace zonespawnlistsync;
    bool ok = true;

    for (const bool visible : { false, true })
    {
        ok = expect(ShouldRemoveSpawnListEntity(visible) == !visible, "stale entry is removed only when invisible") && ok;
    }

    for (const bool typeMatches : { false, true })
    {
        for (const bool visible : { false, true })
        {
            int calls = 0;
            const auto got = ShouldIncludeSpawnListCandidate(typeMatches, [&]()
            {
                ++calls;
                return visible;
            });
            ok = expect(got == (typeMatches && visible), "candidate requires matching type and visibility") && ok;
            ok = expect(calls == (typeMatches ? 1 : 0), "candidate visibility is lazy after type check") && ok;
        }
    }
    return ok;
}
