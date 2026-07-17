#include "test_ambuscade_start_exit_2917.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade start-exit 2917 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceComplete / onInstanceFailure formula for dual-wire
// checks: player:startEvent(10001) is always called for every char.
auto inlineShouldStartExitEvent() -> bool
{
    return true;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldStartExitEvent
// (Lua ambuscade onInstanceComplete / onInstanceFailure always-start exit CS).
auto runAmbuscadeStartExit2917SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDExit;
    using ambuscadehelpers::ShouldStartExitEvent;

    bool ok = true;

    // Constant pin (shared with 2888 exit-warp).
    ok = expect(EventCSIDExit == 10001, "EventCSIDExit == 10001") && ok;

    // Unconditional gate always true.
    ok = expect(ShouldStartExitEvent(), "ShouldStartExitEvent → true") && ok;
    ok = expect(ShouldStartExitEvent() && ShouldStartExitEvent(),
                "ShouldStartExitEvent remains true across calls") &&
         ok;

    // Dual-wire matches inline formula.
    {
        const bool got       = ShouldStartExitEvent();
        const bool inlineGot = inlineShouldStartExitEvent();

        ok = expect(got, "table always-start") && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: complete and failure paths both always start exit CS 10001.
    // When gate is true, host calls startEvent(EventCSIDExit); gate is always true.
    const struct
    {
        const char* path;
        bool        wantStart;
        int32       exitCSID;
        const char* label;
    } composeCases[] = {
        { "complete", true, EventCSIDExit, "compose complete → startEvent 10001" },
        { "failure", true, EventCSIDExit, "compose failure → startEvent 10001" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldStartExitEvent();
        const bool inlineGot = inlineShouldStartExitEvent();

        ok = expect(got == c.wantStart, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(c.exitCSID == 10001, "host exit CSID pin == 10001") && ok;
    }

    return ok;
}
