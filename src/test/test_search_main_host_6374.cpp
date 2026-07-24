#include "test_search_main_host_6374.h"

#include "search/search_application_config.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "search main host 6374 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Search main / Application run host residuals (slice 6374).
// Go host halves: searchutil.NewSearchApplication, SearchMain, PlanSearchRegisterCommands.
auto runSearchMainHost6374SelfTests() -> bool
{
    bool ok = true;

    ok = expect(SearchApplicationServerName() == "search", "server name") && ok;
    ok = expect(SearchMainExitCode == 0, "main exit 0") && ok;

    const auto config = SearchApplicationConfig();
    ok = expect(std::string(config.serverName) == "search", "config server name") && ok;
    ok = expect(config.arguments.empty(), "empty CLI catalog") && ok;

    const auto cmds = SearchApplicationConsoleCommandDescriptors(14);
    ok = expect(cmds[0].name == "ah_cleanup", "ah_cleanup name") && ok;
    ok = expect(cmds[1].name == "expire_all", "expire_all name") && ok;
    ok = expect(cmds[0].description.find("14") != std::string::npos, "ah_cleanup days") && ok;

    return ok;
}
