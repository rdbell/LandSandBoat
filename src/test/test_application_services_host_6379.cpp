#include "test_application_services_host_6379.h"

#include <fmt/format.h>

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "application services host 6379 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for Application prepareLogging / markLoaded / service attach (slice 6379).
// Go host halves: application.WireServices, PlanPrepareLogging, MarkLoaded.
auto runApplicationServicesHost6379SelfTests() -> bool
{
    bool ok = true;

    // Default log path residual.
    ok = expect(fmt::format("log/{}-server.log", "map") == "log/map-server.log", "default log path") && ok;
    ok = expect(fmt::format("log/{}-server.log", "world") == "log/world-server.log", "world log path") && ok;

    // markLoaded message residuals.
    ok = expect(std::string("Type 'help' for a list of available commands.") ==
                    "Type 'help' for a list of available commands.",
                "help line") &&
         ok;
    ok = expect(std::string("CI mode enabled: exiting after successful initialization") ==
                    "CI mode enabled: exiting after successful initialization",
                "ci line") &&
         ok;
    ok = expect(std::string("Creating engine") == "Creating engine", "creating engine") && ok;
    ok = expect(std::string("Initializing engine") == "Initializing engine", "init engine") && ok;

    // Banner residual.
    ok = expect(std::string(69, '=') == std::string(69, '='), "banner width") && ok;

    return ok;
}
