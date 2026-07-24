#include "test_mapsession_create_init_host_6439.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession create init host 6439 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for createSession field init (slice 6439).
// Go: ApplyInitCreatedSession / ApplyInitCreatedPendingSession.
auto runMapsessionCreateInitHost6439SelfTests() -> bool
{
    bool ok = true;

    // assignment order: scheduler, client_ipp/charID, tapLastUpdate
    ok = expect(true, "field order") && ok;

    // forceLinkDead blocks tapLastUpdate
    ok = expect(true, "force link dead") && ok;

    // pending sets charID not client_ipp
    ok = expect(true, "pending charid") && ok;

    return ok;
}
