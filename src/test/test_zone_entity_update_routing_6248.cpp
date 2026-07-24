#include "test_zone_entity_update_routing_6248.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone entity update routing 6248 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pins CZoneEntities::UpdateEntityPacket's three routing predicates: hidden
// GM suppression, the grid fast path, and fallback recipient admission.
auto runZoneEntityUpdateRouting6248SelfTests() -> bool
{
    using zonehelpers::ShouldDispatchEntityUpdateToRecipient;
    using zonehelpers::ShouldSuppressHiddenGMEntityUpdate;
    using zonehelpers::ShouldUseGridEntityUpdateRouting;

    bool ok = true;

    for (const bool hidden : { false, true })
    {
        for (const bool despawn : { false, true })
        {
            const bool want = hidden && !despawn;
            ok              = expect(ShouldSuppressHiddenGMEntityUpdate(hidden, despawn) == want,
                         "hidden GM suppresses every update except despawn") &&
                 ok;
        }
    }

    for (const bool entityUpdate : { false, true })
    {
        for (const bool alwaysInclude : { false, true })
        {
            for (const bool gridNonEmpty : { false, true })
            {
                const bool want = entityUpdate && !alwaysInclude && gridNonEmpty;
                ok              = expect(ShouldUseGridEntityUpdateRouting(entityUpdate, alwaysInclude, gridNonEmpty) == want,
                             "grid route requires update, no override, and populated grid") &&
                     ok;
            }
        }
    }

    for (const bool alwaysInclude : { false, true })
    {
        for (const bool spawn : { false, true })
        {
            for (const bool despawn : { false, true })
            {
                for (const bool spawned : { false, true })
                {
                    const bool want = alwaysInclude || spawn || despawn || spawned;
                    ok              = expect(ShouldDispatchEntityUpdateToRecipient(alwaysInclude, spawn, despawn, spawned) == want,
                                 "fallback recipient admission") &&
                         ok;
                }
            }
        }
    }

    return ok;
}
