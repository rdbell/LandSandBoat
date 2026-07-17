#include "test_ambuscade_warp_exit_3356.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade warp-exit 3356 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua instance onEventFinish formula for dual-wire checks (slice 3356):
//   if csid == 10001 then player:setPos(...) end
// Direct return only — same formula as capacity body.
auto inlineShouldWarpOnExitEvent(const int32 csid) -> bool
{
    return csid == 10001;
}

// Compact dual-wire pin matching C++ capacity formula / Go pinShouldWarpOnExitEvent3356:
//   return csid == EventCSIDExit
auto pinShouldWarpOnExitEvent(const int32 csid) -> bool
{
    return csid == ambuscadehelpers::EventCSIDExit;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldWarpOnExitEvent
// (Lua ambuscade instance onEventFinish exit-warp CSID gate; OmegaXI
// internal/ambuscade; dedicated slice 3356 expand residual 2888; prior
// dedicated 3088). Formula unchanged:
//
//   ShouldWarpOnExitEvent(csid) = csid == EventCSIDExit  // 10001
//
// Coverage:
//   - free == inline == pin == (csid == EventCSIDExit)
//   - residual 2888 / 3088 / 1089 pins still hold
//   - residual poles + neighbors around EventCSIDExit
//   - dense residual CSID space + dense compose range
//   - host onEventFinish compose poles
auto runAmbuscadeWarpExit3356SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDExit;
    using ambuscadehelpers::ShouldWarpOnExitEvent;

    bool ok = true;

    // Residual constant / 1089 / 2888 / prior dedicated 3088 pins still hold.
    ok = expect(EventCSIDExit == 10001, "EventCSIDExit == 10001") && ok;
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit), "residual: csid 10001 → warp") && ok;
    ok = expect(ShouldWarpOnExitEvent(10001), "residual: literal 10001 → warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(386), "residual: csid 386 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(0), "residual: csid 0 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(10000), "residual: csid 10000 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(10002), "residual: csid 10002 → no warp") && ok;
    ok = expect(!ShouldWarpOnExitEvent(-1), "residual: csid -1 → no warp") && ok;

    // Residual poles: free == inline == pin (direct return).
    const struct
    {
        int32       csid;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2888 / prior dedicated 3088 classic dual poles.
        { EventCSIDExit, true, "residual EventCSIDExit → warp" },
        { 10001, true, "residual literal 10001 → warp" },
        { 386, false, "residual Gorpa menu → no warp" },
        { 0, false, "residual zero → no warp" },
        { 10000, false, "residual one below → no warp" },
        { 10002, false, "residual one above → no warp" },
        { -1, false, "residual negative → no warp" },
        { 385, false, "residual intro CSID → no warp" },
        { 378, false, "residual tome enter → no warp" },
        { 374, false, "residual tome register → no warp" },

        // Host inject path poles (onEventFinish csid).
        { EventCSIDExit, true, "host exit CSID → setPos(ExitDest)" },
        { 10001, true, "host literal 10001 → setPos" },
        { 386, false, "host Gorpa menu → skip setPos" },
        { 0, false, "host other → skip setPos" },
    };

    for (const auto& p : poles)
    {
        const bool got       = ShouldWarpOnExitEvent(p.csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(p.csid);
        const bool pinGot    = pinShouldWarpOnExitEvent(p.csid);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineGot && got == pinGot, "dual-wire free == inline == pin") && ok;
        ok = expect(got == (p.csid == EventCSIDExit), "dual-wire free == (csid == EventCSIDExit)") && ok;
        ok = expect(got == (p.csid == 10001), "dual-wire free == (csid == 10001)") && ok;
    }

    // Dense residual CSID space: free == inline == pin.
    for (const int32 csid : { EventCSIDExit, static_cast<int32>(10001), static_cast<int32>(386),
                              static_cast<int32>(0), static_cast<int32>(10000), static_cast<int32>(10002),
                              static_cast<int32>(-1), static_cast<int32>(385), static_cast<int32>(378),
                              static_cast<int32>(374) })
    {
        const bool got       = ShouldWarpOnExitEvent(csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(csid);
        const bool pinGot    = pinShouldWarpOnExitEvent(csid);

        ok = expect(got == (csid == EventCSIDExit), "dense free == (csid == EventCSIDExit)") && ok;
        ok = expect(got == inlineGot && got == pinGot, "dense free == inline == pin") && ok;
        ok = expect(got == (csid == 10001), "dense free == (csid == 10001)") && ok;
    }

    // Dense compose range identity around EventCSIDExit.
    for (int32 csid = EventCSIDExit - 2; csid <= EventCSIDExit + 5; ++csid)
    {
        const bool got       = ShouldWarpOnExitEvent(csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(csid);
        const bool pinGot    = pinShouldWarpOnExitEvent(csid);

        ok = expect(got == (csid == EventCSIDExit), "dense compose free == formula") && ok;
        ok = expect(got == inlineGot && got == pinGot, "dense compose free == inline == pin") && ok;
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
        { 385, false, "compose intro → skip setPos" },
        { 378, false, "compose tome enter → skip setPos" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldWarpOnExitEvent(c.csid);
        const bool inlineGot = inlineShouldWarpOnExitEvent(c.csid);
        const bool pinGot    = pinShouldWarpOnExitEvent(c.csid);

        ok = expect(got == c.wantWarp, c.label) && ok;
        ok = expect(got == inlineGot && got == pinGot, "compose free == inline == pin") && ok;
        ok = expect(got == (c.csid == EventCSIDExit), "compose free == (csid == EventCSIDExit)") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit) == true, "compose exit CSID") && ok;
    ok = expect(ShouldWarpOnExitEvent(386) == false, "compose Gorpa menu") && ok;
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit) == pinShouldWarpOnExitEvent(EventCSIDExit), "free == pin exit") && ok;
    ok = expect(ShouldWarpOnExitEvent(0) == pinShouldWarpOnExitEvent(0), "free == pin zero") && ok;
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit) == inlineShouldWarpOnExitEvent(EventCSIDExit), "free == inline exit") && ok;
    ok = expect(ShouldWarpOnExitEvent(0) == inlineShouldWarpOnExitEvent(0), "free == inline zero") && ok;

    // Production path semantics (host inject model for onEventFinish).
    ok = expect(ShouldWarpOnExitEvent(EventCSIDExit) == inlineShouldWarpOnExitEvent(EventCSIDExit), "host warp path free == inline") && ok;
    ok = expect(ShouldWarpOnExitEvent(386) == inlineShouldWarpOnExitEvent(386), "host skip path free == inline") && ok;

    return ok;
}
