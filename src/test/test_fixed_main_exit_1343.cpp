#include "test_fixed_main_exit_1343.h"

#include "login/connect_cleanup.h"
#include "world/world_app_config.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "fixed main exit 1343 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runFixedMainExit1343SelfTests() -> bool
{
    bool ok = true;

    // World and connect always return 0 (unlike map, which branches on run success).
    ok = expect(worldapp::WorldMainExitCode == 0, "world exit") && ok;
    ok = expect(loginHelpers::ConnectMainExitCode == 0, "connect exit") && ok;

    return ok;
}
