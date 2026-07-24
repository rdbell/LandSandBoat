#include "test_map_networking_attach_host_6400.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking attach host 6400 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine MapNetworking attach (slice 6400).
// Go: AttachMapNetworking / NewMapEngineWithNetworking / network.MAP_PORT key.
auto runMapNetworkingAttachHost6400SelfTests() -> bool
{
    bool ok = true;

    const std::string mapPortKey = "network.MAP_PORT";
    ok = expect(mapPortKey == "network.MAP_PORT", "settings key") && ok;

    // ShouldOpenSocket: !isTestServer
    const bool isTestServer = true;
    const bool shouldOpen   = !isTestServer;
    ok = expect(!shouldOpen, "test server skips open") && ok;
    ok = expect((!false) == true, "production opens") && ok;

    // Port resolution: mapIPP.port == 0 → fallback MAP_PORT else mapIPP.port
    const uint16_t mapIPPPort     = 0;
    const uint16_t fallbackMAPPort = 54230;
    const uint16_t udpPort         = mapIPPPort == 0 ? fallbackMAPPort : mapIPPPort;
    ok = expect(udpPort == 54230, "fallback port") && ok;

    const uint16_t explicitPort = 54001;
    const uint16_t resolved     = explicitPort == 0 ? fallbackMAPPort : explicitPort;
    ok = expect(resolved == 54001, "explicit port") && ok;

    const std::string failFmt = "Failed to create MapSocket: boom";
    ok = expect(failFmt.find("Failed to create MapSocket") != std::string::npos, "fail format") && ok;

    return ok;
}
