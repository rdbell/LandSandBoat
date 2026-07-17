#include "test_mapwire_open_socket_2948.h"

#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapwire ShouldOpenSocket 2948 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapNetworking constructor socket-gate formula for dual-wire
// cross-check (slice 2948):
//   !isTestServer
auto inlineShouldOpenSocket(const bool isTestServer) -> bool
{
    return !isTestServer;
}

} // namespace

// Pure dual-wire expansion for mapnetworkinghelpers::ShouldOpenSocket
// (!isTestServer; slice 2948).
auto runMapwireOpenSocket2948SelfTests() -> bool
{
    using mapnetworkinghelpers::ShouldOpenSocket;

    bool ok = true;

    const struct
    {
        bool        isTestServer;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "production opens socket" },
        { true, false, "test server skips socket" },

        // Residual 2660 pins.
        { false, true, "residual production open" },
        { true, false, "residual test skip" },

        // Identity / polarity repeats for dual-wire stability.
        { false, true, "identity production" },
        { true, false, "identity test" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldOpenSocket(c.isTestServer);
        const bool inlineF = inlineShouldOpenSocket(c.isTestServer);
        const bool wantPin = !c.isTestServer;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldOpenSocket dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldOpenSocket == pin formula !isTestServer") && ok;
    }

    // Pin composition: NOT of isTestServer.
    ok = expect(ShouldOpenSocket(false), "production must open socket") && ok;
    ok = expect(!ShouldOpenSocket(true), "test server must skip socket") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool isTestServer : { false, true })
    {
        const bool got  = ShouldOpenSocket(isTestServer);
        const bool want = !isTestServer;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldOpenSocket(isTestServer),
                    "compose free == inline") &&
             ok;
    }

    // --- Production MapNetworking constructor path semantics ---
    // Host injects:
    //   isTestServer = config_.isTestServer
    // if (!ShouldOpenSocket(isTestServer)) return; // before MapSocket
    ok = expect(ShouldOpenSocket(false), "MapNetworking production → open socket path") && ok;
    ok = expect(!ShouldOpenSocket(true), "MapNetworking test server → skip socket path") && ok;

    // Residual 2660 pins still hold under dual-wire.
    ok = expect(ShouldOpenSocket(false), "residual production open") && ok;
    ok = expect(!ShouldOpenSocket(true), "residual test skip") && ok;

    // Host-style invert polarity: open iff not test server.
    const bool openProduction = ShouldOpenSocket(false);
    const bool skipTest       = !ShouldOpenSocket(true);
    ok                        = expect(openProduction && skipTest, "constructor open/skip polarity") && ok;
    ok                        = expect(openProduction == inlineShouldOpenSocket(false),
                "production free == inline") &&
         ok;
    ok = expect(ShouldOpenSocket(true) == inlineShouldOpenSocket(true), "test free == inline") && ok;

    return ok;
}
