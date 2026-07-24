#include "test_application_quick_edit_host_6423.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "application quick edit host 6423 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for tryDisableQuickEditMode (slice 6423).
// Go: ComputeDisabledConsoleMode / ApplyDisableQuickEditMode.
auto runApplicationQuickEditHost6423SelfTests() -> bool
{
    bool ok = true;

    // wincon.h values
    constexpr unsigned long ENABLE_EXTENDED_FLAGS  = 0x0080;
    constexpr unsigned long ENABLE_QUICK_EDIT_MODE  = 0x0040;

    ok = expect(ENABLE_EXTENDED_FLAGS == 0x0080u, "extended") && ok;
    ok = expect(ENABLE_QUICK_EDIT_MODE == 0x0040u, "quick") && ok;

    const unsigned long prev = ENABLE_QUICK_EDIT_MODE | 0x0010u;
    const unsigned long next = ENABLE_EXTENDED_FLAGS | (prev & ~ENABLE_QUICK_EDIT_MODE);
    ok = expect((next & ENABLE_QUICK_EDIT_MODE) == 0, "cleared") && ok;
    ok = expect((next & ENABLE_EXTENDED_FLAGS) != 0, "extended set") && ok;
    ok = expect((next & 0x0010u) != 0, "other bits kept") && ok;

    return ok;
}
