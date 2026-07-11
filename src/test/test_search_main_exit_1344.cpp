#include "test_search_main_exit_1344.h"

#include "search/search_application_config.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "search main exit 1344 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runSearchMainExit1344SelfTests() -> bool
{
    bool ok = true;

    // Search always returns 0 after run() (same polarity as world/connect; unlike map).
    ok = expect(SearchMainExitCode == 0, "search exit always 0") && ok;
    ok = expect(SearchApplicationServerName() == "search", "server name still search") && ok;

    return ok;
}
