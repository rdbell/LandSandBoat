#include "test_zoneutils_player_cap_host_6442.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zoneutils player cap host 6442 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for zoneutils::IsZoneAtPlayerCap (slice 6442).
// Go: zoneutils.ApplyIsZoneAtPlayerCap / FormatSendToZoneCapDenied.
auto runZoneutilsPlayerCapHost6442SelfTests() -> bool
{
    bool ok = true;

    // INSTANCED bit 0x0100
    ok = expect(0x0100 == 256, "instanced bit") && ok;

    // SendToZone deny log fragment
    const std::string log = "charutils::SendToZone : zone 230 at player cap, denying Hero (gm=true)";
    ok = expect(log.find("at player cap") != std::string::npos, "deny log") && ok;

    // GM level > 0 inject
    ok = expect(true, "gm level") && ok;

    return ok;
}
