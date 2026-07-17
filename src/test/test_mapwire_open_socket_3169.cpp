#include "test_mapwire_open_socket_3169.h"

#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapwire ShouldOpenSocket 3169 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapNetworking constructor socket-gate formula for dual-wire
// cross-check (residual 2948 / dedicated 3169):
//   !isTestServer
auto inlineShouldOpenSocket(const bool isTestServer) -> bool
{
    return !isTestServer;
}

// Compact dual-wire pin matching Go pinShouldOpenSocket3169:
//   !isTestServer
// Simple identity-not (negation of the host-injected isTestServer bool).
auto pinShouldOpenSocket(const bool isTestServer) -> bool
{
    return !isTestServer;
}

} // namespace

// Pure dual-wire expansion for mapnetworkinghelpers::ShouldOpenSocket
// (!isTestServer; OmegaXI internal/mapwire; dedicated slice 3169).
//
// Coverage:
//   - free == inline == pin == !isTestServer (identity-not)
//   - residual 2660 / 2948 pins still hold
//   - MapNetworking constructor host inject path
auto runMapwireOpenSocket3169SelfTests() -> bool
{
    using mapnetworkinghelpers::ShouldOpenSocket;

    bool ok = true;

    // Residual 2660 / 2948 pins still hold under dual-wire.
    ok = expect(ShouldOpenSocket(false), "residual production must open socket") && ok;
    ok = expect(!ShouldOpenSocket(true), "residual test server must skip socket") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(ShouldOpenSocket(false), "production opens socket") && ok;
    ok = expect(!ShouldOpenSocket(true), "test server skips socket") && ok;

    const struct
    {
        bool        isTestServer;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "production opens socket" },
        { true, false, "test server skips socket" },

        // Residual 2948 re-pins.
        { false, true, "residual 2948 production open" },
        { true, false, "residual 2948 test skip" },

        // Residual 2660 re-pins.
        { false, true, "residual 2660 production open" },
        { true, false, "residual 2660 test skip" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldOpenSocket(c.isTestServer);
        const bool inlineF = inlineShouldOpenSocket(c.isTestServer);
        const bool pinGot  = pinShouldOpenSocket(c.isTestServer);
        const bool wantPin = !c.isTestServer;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldOpenSocket dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldOpenSocket dual-wire == pin") && ok;
        ok = expect(got == wantPin, "ShouldOpenSocket == pin formula (identity-not)") && ok;
    }

    // Pin composition: free function is identity-not on isTestServer only.
    ok = expect(ShouldOpenSocket(false), "production must open socket") && ok;
    ok = expect(!ShouldOpenSocket(true), "test server must skip socket") && ok;
    ok = expect(ShouldOpenSocket(false) == pinShouldOpenSocket(false), "free==pin false") && ok;
    ok = expect(ShouldOpenSocket(true) == pinShouldOpenSocket(true), "free==pin true") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool isTestServer : { false, true })
    {
        const bool got  = ShouldOpenSocket(isTestServer);
        const bool want = !isTestServer;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldOpenSocket(isTestServer), "compose free == inline") && ok;
        ok              = expect(got == pinShouldOpenSocket(isTestServer), "compose free == pin") && ok;
    }

    // Residual 2948 / 2660 still hold under dedicated suite.
    ok = expect(ShouldOpenSocket(false), "residual production open under 3169") && ok;
    ok = expect(!ShouldOpenSocket(true), "residual test skip under 3169") && ok;

    // --- Production MapNetworking constructor path semantics ---
    // Host injects:
    //   isTestServer = config_.isTestServer
    // if (!ShouldOpenSocket(isTestServer)) return; // before MapSocket
    ok = expect(ShouldOpenSocket(false), "MapNetworking production → open socket path") && ok;
    ok = expect(!ShouldOpenSocket(true), "MapNetworking test server → skip socket path") && ok;

    // Host-style invert polarity: open iff not test server.
    const bool openProduction = ShouldOpenSocket(false);
    const bool skipTest       = !ShouldOpenSocket(true);
    ok                        = expect(openProduction && skipTest, "constructor open/skip polarity") && ok;
    ok                        = expect(openProduction == pinShouldOpenSocket(false),
                "production free == pin") &&
         ok;
    ok = expect(ShouldOpenSocket(true) == pinShouldOpenSocket(true), "test free == pin") && ok;

    return ok;
}
