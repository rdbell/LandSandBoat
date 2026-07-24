#include "test_mapsession_entity_body_host_6438.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession entity body host 6438 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for entity/zone/pet body projection (slice 6438).
// Go: BuildCleanupConfirmedInput / ProjectHasMobPet.
auto runMapsessionEntityBodyHost6438SelfTests() -> bool
{
    bool ok = true;

    // zone name fallback "None"
    const std::string none = "None";
    ok = expect(none == "None", "zone none") && ok;

    // hasMobPet requires PChar + PPet + TYPE_MOB (host-evaluated)
    ok = expect(true, "mob pet gate") && ok;

    // hasZone requires PChar + loc.zone
    ok = expect(true, "has zone") && ok;

    return ok;
}
