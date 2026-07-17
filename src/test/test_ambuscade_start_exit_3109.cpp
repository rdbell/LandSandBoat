#include "test_ambuscade_start_exit_3109.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade start-exit 3109 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceComplete / onInstanceFailure formula for dual-wire
// checks (slice 3109): player:startEvent(10001) is always called for every char.
auto inlineShouldStartExitEvent() -> bool
{
    return true;
}

// Compact dual-wire pin matching C++ capacity formula:
//   true
auto pinShouldStartExitEvent() -> bool
{
    return true;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldStartExitEvent
// (Lua ambuscade onInstanceComplete / onInstanceFailure always-start exit CS;
// slice 3109).
auto runAmbuscadeStartExit3109SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDExit;
    using ambuscadehelpers::ShouldStartExitEvent;

    bool ok = true;

    // Residual constant / 1089 / 2917 pins still hold under dual-wire.
    ok = expect(EventCSIDExit == 10001, "EventCSIDExit == 10001") && ok;
    ok = expect(ShouldStartExitEvent(), "residual: complete/failure should start exit event") && ok;

    // Unconditional gate always true; free == inline == pin.
    {
        const bool got       = ShouldStartExitEvent();
        const bool inlineGot = inlineShouldStartExitEvent();
        const bool pinGot    = pinShouldStartExitEvent();

        ok = expect(got, "ShouldStartExitEvent → true") && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == true, "dual-wire free == true") && ok;
    }

    // Repeated calls stay true (no state); free == pin matching C++.
    ok = expect(ShouldStartExitEvent() && ShouldStartExitEvent(),
                "ShouldStartExitEvent remains true across calls") &&
         ok;
    for (int i = 0; i < 3; ++i)
    {
        const bool got = ShouldStartExitEvent();
        ok             = expect(got == pinShouldStartExitEvent(), "free == pin across calls") && ok;
        ok             = expect(got == inlineShouldStartExitEvent(), "free == inline across calls") && ok;
        ok             = expect(got, "always true across calls") && ok;
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
        const bool pinGot    = pinShouldStartExitEvent();

        ok = expect(got == c.wantStart, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(got == true, "compose dual-wire free == true") && ok;
        ok = expect(c.exitCSID == 10001, "host exit CSID pin == 10001") && ok;
        ok = expect(c.exitCSID == EventCSIDExit, "host exit CSID == EventCSIDExit") && ok;
    }

    // Dense always-true poles: free == inline == pin for unconditional domain.
    for (int i = 0; i < 2; ++i)
    {
        const bool got = ShouldStartExitEvent();
        ok             = expect(got == pinShouldStartExitEvent(), "dense free == pin") && ok;
        ok             = expect(got == inlineShouldStartExitEvent(), "dense free == inline") && ok;
        ok             = expect(got == true, "dense free == true") && ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(ShouldStartExitEvent() == true, "compose always-start") && ok;

    return ok;
}
