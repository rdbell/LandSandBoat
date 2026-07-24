#include "test_zone_nearby_spawn_6249.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone nearby spawn 6249 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pins CZoneEntities::TryAddToNearbySpawnLists visibility admission before it
// updates a recipient's corresponding spawn list and sends ENTITY_SPAWN.
auto runZoneNearbySpawn6249SelfTests() -> bool
{
    using zonehelpers::ShouldSpawnNearbyNPC;
    using zonehelpers::ShouldSpawnNearbyNonNPC;
    using zonehelpers::ShouldSpawnNearbyPC;

    bool ok = true;

    for (const bool inRange : { false, true })
    {
        ok = expect(ShouldSpawnNearbyNPC(inRange) == inRange, "NPC requires horizontal render range") && ok;
    }

    for (const bool inRange : { false, true })
    {
        for (const bool withinVerticalRange : { false, true })
        {
            const bool want = inRange && withinVerticalRange;
            ok              = expect(ShouldSpawnNearbyNonNPC(inRange, withinVerticalRange) == want,
                         "mob, pet, trust, and PC require horizontal and vertical range") &&
                 ok;
        }
    }

    for (const bool sameMogHouse : { false, true })
    {
        for (const bool hiddenGM : { false, true })
        {
            const bool want = sameMogHouse && !hiddenGM;
            ok              = expect(ShouldSpawnNearbyPC(sameMogHouse, hiddenGM) == want,
                         "PC requires matching Mog House and visible GM state") &&
                 ok;
        }
    }

    return ok;
}
