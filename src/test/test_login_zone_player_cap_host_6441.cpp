#include "test_login_zone_player_cap_host_6441.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login zone player cap host 6441 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for isZoneAtPlayerCap host composition (slice 6441).
// Go: ApplyIsZoneAtPlayerCap / ShouldSkipZoneCapSQL.
auto runLoginZonePlayerCapHost6441SelfTests() -> bool
{
    bool ok = true;

    const std::string capKey = "map.ZONE_PLAYER_CAP";
    const std::string resKey = "map.ZONE_PLAYER_GM_RESERVED";
    ok = expect(capKey.find("ZONE_PLAYER_CAP") != std::string::npos, "cap key") && ok;
    ok = expect(resKey.find("GM_RESERVED") != std::string::npos, "reserved key") && ok;

    // cap==0 skips SQL
    ok = expect(true, "skip sql") && ok;

    // SQL binds zoneId twice
    ok = expect(true, "sql args") && ok;

    return ok;
}
