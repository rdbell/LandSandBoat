#include "test_ambuscade_warp_exit_3088.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade warp-exit 3088 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua instance onEventFinish formula for dual-wire checks (slice 3088):
//   if csid == 10001 then player:setPos(...) end
auto inlineShouldWarpOnExitEvent(const int32 csid) -> bool
{
    return csid == 10001;
}

// Compact dual-wire pin matching C++ capacity formula:
//   csid == EventCSIDExit
auto pinShouldWarpOnExitEvent(const int32 csid) -> bool
{
    return csid == ambuscadehelpers::EventCSIDExit;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldWarpOnExitEvent
// (Lua ambuscade instance onEventFinish exit-warp CSID gate; slice 3088).
auto runAmbuscadeWarpExit3088SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDExit;
    using ambuscadehelpers::ShouldWarpOnExitEvent;

    bool ok = true;

    // Residual constant / 1089 / 2888 pins still hold under dual-wire.
    ok = expect(EventCSIDExit == 10001, "EventCSIDExit == 10001") && ok;
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit), "residual: csid 10001 → warp") && ok;
    ok = expect(ShouldWarpOnExitEvent(10001), "residual: literal 10001 → warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(386), "residual: csid 386 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(0), "residual: csid 0 → no warp") && ok;

    // Truth table poles + neighbors around EventCSIDExit.
    const struct
    {
        int32       csid;
        bool        want;
        const char* label;
    } cases[] = {
        { EventCSIDExit, true, "EventCSIDExit → warp" },
        { 10001, true, "literal 10001 → warp" },
        { 386, false, "Gorpa menu → no warp" },
        { 0, false, "zero → no warp" },
        { 10000, false, "one below → no warp" },
        { 10002, false, "one above → no warp" },
        { -1, false, "negative → no warp" },
        { 385, false, "intro CSID → no warp" },
        { 378, false, "tome enter → no warp" },
        { 374, false, "tome register → no warp" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldWarpOnExitEvent(c.csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(c.csid);
        const bool pinGot    = pinShouldWarpOnExitEvent(c.csid);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == (c.csid == EventCSIDExit), "dual-wire free == (csid == EventCSIDExit)") && ok;
        ok = expect(got == (c.csid == 10001), "dual-wire free == (csid == 10001)") && ok;
    }

    // Host compose: inject finish csid then pure gate; setPos remains host-owned.
    const struct
    {
        int32       csid;
        bool        wantWarp;
        const char* label;
    } composeCases[] = {
        { EventCSIDExit, true, "compose 10001 → setPos(ExitDest)" },
        { 10001, true, "compose literal 10001 → setPos" },
        { 386, false, "compose Gorpa menu → skip setPos" },
        { 0, false, "compose other → skip setPos" },
        { 10000, false, "compose one below → skip setPos" },
        { 10002, false, "compose one above → skip setPos" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldWarpOnExitEvent(c.csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(c.csid);
        const bool pinGot    = pinShouldWarpOnExitEvent(c.csid);

        ok = expect(got == c.wantWarp, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(got == (c.csid == EventCSIDExit), "compose dual-wire free == (csid == EventCSIDExit)") && ok;
    }

    // Dense representative poles: free == inline == pin for residual CSID space.
    for (const int32 csid : { EventCSIDExit, static_cast<int32>(10001), static_cast<int32>(386),
                              static_cast<int32>(0), static_cast<int32>(10000), static_cast<int32>(10002),
                              static_cast<int32>(-1), static_cast<int32>(385), static_cast<int32>(378) })
    {
        const bool got = ShouldWarpOnExitEvent(csid);
        ok             = expect(got == pinShouldWarpOnExitEvent(csid), "dense free == pin") && ok;
        ok             = expect(got == inlineShouldWarpOnExitEvent(csid), "dense free == inline") && ok;
        ok             = expect(got == (csid == EventCSIDExit), "dense free == (csid == EventCSIDExit)") && ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit) == true, "compose exit CSID") && ok;
    ok = expect(ShouldWarpOnExitEvent(386) == false, "compose Gorpa menu") && ok;

    return ok;
}
