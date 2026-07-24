#include "test_zone_transport_spawn_6263.h"

#include "map/zone_transport_spawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone transport spawn 6263 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnTransport's production-wired always-relevant filter.
auto runZoneTransportSpawn6263SelfTests() -> bool
{
    using namespace zonetransportspawn;

    bool ok = true;
    ok      = expect(ShouldSpawnForRecipient(true), "always-relevant transport spawns") && ok;
    ok      = expect(!ShouldSpawnForRecipient(false), "proximity transport is skipped") && ok;
    return ok;
}
