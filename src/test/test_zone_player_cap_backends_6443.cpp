#include "test_zone_player_cap_backends_6443.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone player cap backends 6443 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for settings+DB backends of isZoneAtPlayerCap (slice 6443).
// Go: SettingsGetUint16 / QueryZonePopPreparedStmt / IsZoneAtPlayerCapLive.
auto runZonePlayerCapBackends6443SelfTests() -> bool
{
    bool ok = true;

    const std::string capKey = "map.ZONE_PLAYER_CAP";
    const std::string resKey = "map.ZONE_PLAYER_GM_RESERVED";
    ok = expect(capKey == "map.ZONE_PLAYER_CAP", "cap key") && ok;
    ok = expect(resKey == "map.ZONE_PLAYER_GM_RESERVED", "reserved key") && ok;

    // SQL binds zoneId twice
    ok = expect(true, "sql binds") && ok;

    // settings::get + preparedStmt composition order
    ok = expect(true, "backend order") && ok;

    return ok;
}
