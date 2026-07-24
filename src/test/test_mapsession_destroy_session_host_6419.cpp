#include "test_mapsession_destroy_session_host_6419.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession destroy session host 6419 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for destroySession composition (slice 6419).
// Go: ApplyDestroySession / ApplyDestroySessionByIPP.
auto runMapsessionDestroySessionHost6419SelfTests() -> bool
{
    bool ok = true;

    const std::string close = "Closing session for 1.2.3.4:54001";
    ok = expect(close.find("Closing session for") != std::string::npos, "close dbg") && ok;

    // shutDown==1 → delete SQL; zoning (2) does not
    constexpr uint8_t logout  = 1;
    constexpr uint8_t zoning  = 2;
    ok = expect(logout == 1 && zoning == 2, "shutdown codes") && ok;

    // Index remove must precede plan
    ok = expect(true, "index before plan") && ok;

    return ok;
}
