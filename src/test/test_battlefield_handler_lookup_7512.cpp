#include "test_battlefield_handler_lookup_7512.h"

#include "map/battlefield_handler_lookup.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield handler lookup 7512 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattlefieldHandlerLookup7512SelfTests() -> bool
{
    using battlefieldhandlerhelpers::ShouldSelectBattlefield;

    bool ok = true;
    ok = expect(ShouldSelectBattlefield(true, true, true, false), "registered player matches") && ok;
    ok = expect(!ShouldSelectBattlefield(true, true, false, true), "registered player ignores entered membership") && ok;
    ok = expect(ShouldSelectBattlefield(true, false, false, true), "non-player falls back to entered membership") && ok;
    ok = expect(ShouldSelectBattlefield(false, true, false, true), "default lookup uses entered membership") && ok;
    return ok;
}
