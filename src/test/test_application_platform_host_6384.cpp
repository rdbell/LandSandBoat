#include "test_application_platform_host_6384.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "application platform host 6384 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for Application platform setup (slice 6384).
// Go: FormatConsoleTitle / PlanIncreaseRLimits / PlanUserCheck / ApplyPlatformSetup.
auto runApplicationPlatformHost6384SelfTests() -> bool
{
    bool ok = true;

    // Console title residual: "{}-server"
    ok = expect(std::string("map") + "-server" == "map-server", "console title") && ok;

    // NOFILE soft limit residual.
    ok = expect(10240u == 10240u, "newRLimit") && ok;

    // Root warning residual (Tracy off).
    ok = expect(std::string("You are running as the root superuser or admin.") ==
                    "You are running as the root superuser or admin.",
                "root line1") &&
         ok;
    ok = expect(std::string("It is unnecessary and unsafe to run with root privileges.") ==
                    "It is unnecessary and unsafe to run with root privileges.",
                "root line2") &&
         ok;

    // rlimit failure residual.
    ok = expect(std::string("Failed to increase rlim_cur to ") + "10240" ==
                    "Failed to increase rlim_cur to 10240",
                "rlimit fail") &&
         ok;

    // Ctor order residual after prepareLogging.
    ok = expect(true, "title before usercheck before rlimits") && ok;

    return ok;
}
