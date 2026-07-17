#include "test_world_http_zone_path_2740.h"

#include "world/http_server_api.h"
#include "world/http_zone_path.h"

#include <iostream>
#include <optional>
#include <string_view>

namespace
{

auto expectParse(const std::string_view input, const std::optional<uint16> expected, const std::string_view label) -> bool
{
    if (worldhttp::ParseZonePathID(input) == expected)
    {
        return true;
    }

    std::cerr << "world HTTP zone path 2740 self-test failed: " << label << '\n';
    return false;
}

} // namespace

auto runWorldHTTPZonePath2740SelfTests() -> bool
{
    bool ok = true;
    ok      = expectParse("1", 1, "ordinary zone") && ok;
    ok      = expectParse("65537", 1, "uint16 narrowing wraps") && ok;
    ok      = expectParse("", std::nullopt, "empty rejected") && ok;
    ok      = expectParse("1a", std::nullopt, "non-decimal rejected") && ok;
    ok      = expectParse("9223372036854775807", 65535, "long maximum narrows") && ok;
    ok      = expectParse("999999999999999999999999", 65535, "strtol overflow narrows saturated long") && ok;

    HTTPServerAPIDataCache cache{};
    cache.zonePlayerCounts[1] = 74;
    const auto response       = makeHTTPServerAPIResponse("/api/zones/65537", cache, {});
    if (response.status != 200 || response.contentType != "application/json" || response.body != "74")
    {
        std::cerr << "world HTTP zone path 2740 self-test failed: route uses narrowed ID\n";
        ok = false;
    }

    return ok;
}
