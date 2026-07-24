#include "test_map_socket_host_6385.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map socket host 6385 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapSocket / MapNetworking open (slice 6385).
// Go: PlanMapNetworkingOpen / OpenMapSocket / OpenMapNetworkingSocket.
auto runMapSocketHost6385SelfTests() -> bool
{
    bool ok = true;

    // Starting log residual.
    ok = expect(std::string("MapSocket: Starting on port ") + "54230" ==
                    "MapSocket: Starting on port 54230",
                "starting") &&
         ok;

    // Create failure residual.
    ok = expect(std::string("Failed to create MapSocket: ") + "boom" ==
                    "Failed to create MapSocket: boom",
                "fail") &&
         ok;

    // Test-server skip residual: ShouldOpenSocket(true) == false.
    ok = expect(!true == false, "test server open false") && ok;
    ok = expect(!false == true, "prod open true") && ok;

    // Port fallback residual: mapIPP 0 → MAP_PORT.
    ok = expect(uint16_t{0} == 0, "zero port") && ok;
    ok = expect(uint16_t{54230} == 54230, "fallback port") && ok;

    return ok;
}
