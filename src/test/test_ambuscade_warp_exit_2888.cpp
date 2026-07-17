#include "test_ambuscade_warp_exit_2888.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade warp-exit 2888 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua instance onEventFinish formula for dual-wire checks:
//   if csid == 10001 then player:setPos(...) end
auto inlineShouldWarpOnExitEvent(const int32 csid) -> bool
{
    return csid == 10001;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldWarpOnExitEvent
// (Lua ambuscade instance onEventFinish exit-warp CSID gate).
auto runAmbuscadeWarpExit2888SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDExit;
    using ambuscadehelpers::ShouldWarpOnExitEvent;

    bool ok = true;

    // Constant pin.
    ok = expect(EventCSIDExit == 10001, "EventCSIDExit == 10001") && ok;

    // Truth table for ShouldWarpOnExitEvent (csid == 10001).
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit), "csid 10001 → warp") && ok;
    ok = expect(ShouldWarpOnExitEvent(10001), "literal 10001 → warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(386), "csid 386 (Gorpa menu) → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(0), "csid 0 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(10000), "csid 10000 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(10002), "csid 10002 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(-1), "csid -1 → no warp") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        int32       csid;
        bool        want;
        const char* label;
    } cases[] = {
        { 10001, true, "table exit CSID" },
        { 386, false, "table Gorpa menu" },
        { 0, false, "table zero" },
        { 10000, false, "table one below" },
        { 10002, false, "table one above" },
        { -1, false, "table negative" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldWarpOnExitEvent(c.csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(c.csid);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    return ok;
}
