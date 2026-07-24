#include "test_zone_char_sync_apply_6262.h"

#include "map/zone_char_sync_apply.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone character-sync apply 6262 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnPCs' production-wired candidate execution guards.
auto runZoneCharSyncApply6262SelfTests() -> bool
{
    using namespace zonecharsyncapply;

    bool ok = true;
    ok      = expect(!ShouldStopForSwapLimit(4, 5), "below swap limit continues") && ok;
    ok      = expect(ShouldStopForSwapLimit(5, 5), "swap limit stops execution") && ok;
    ok      = expect(!CanReplaceAtCapacity(false, 100.0f, 0.0f), "empty replacement pool cannot make room") && ok;
    ok      = expect(CanReplaceAtCapacity(true, 6.0f, 5.0f), "strictly higher candidate replaces lowest") && ok;
    ok      = expect(!CanReplaceAtCapacity(true, 5.0f, 5.0f), "equal candidate keeps existing entry") && ok;
    ok      = expect(!CanReplaceAtCapacity(true, 4.0f, 5.0f), "lower candidate keeps existing entry") && ok;
    return ok;
}
