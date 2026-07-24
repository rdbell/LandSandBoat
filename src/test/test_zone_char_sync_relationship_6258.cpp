#include "test_zone_char_sync_relationship_6258.h"

#include "map/zone_char_sync_relationship.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone character-sync relationship 6258 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins getSignificanceScore's production-wired party and alliance sampling.
auto runZoneCharSyncRelationship6258SelfTests() -> bool
{
    using zonecharsyncrelationship::Determine;

    bool ok = true;
    auto relationship = Determine(false, 1, true, 1, true, 7, true, 7);
    ok                = expect(!relationship.sameParty && !relationship.sameAlliance, "missing origin party has no relationship") && ok;

    relationship = Determine(true, 1, false, 1, true, 7, true, 7);
    ok           = expect(!relationship.sameParty && !relationship.sameAlliance, "missing target party has no relationship") && ok;

    relationship = Determine(true, 1, true, 1, false, 0, false, 0);
    ok           = expect(relationship.sameParty && !relationship.sameAlliance, "shared party does not require alliance") && ok;

    relationship = Determine(true, 1, true, 2, true, 7, true, 7);
    ok           = expect(!relationship.sameParty && relationship.sameAlliance, "shared alliance is distinct from party") && ok;

    relationship = Determine(true, 1, true, 2, true, 7, false, 0);
    ok           = expect(!relationship.sameParty && !relationship.sameAlliance, "both parties need an alliance") && ok;

    relationship = Determine(true, 1, true, 2, true, 7, true, 8);
    ok           = expect(!relationship.sameParty && !relationship.sameAlliance, "different alliances do not match") && ok;
    return ok;
}
