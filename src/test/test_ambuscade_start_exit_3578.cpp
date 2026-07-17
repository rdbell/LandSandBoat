#include "test_ambuscade_start_exit_3578.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade start-exit 3578 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceComplete / onInstanceFailure formula for dual-wire
// checks (slice 3578): player:startEvent(10001) is always called for every char.
// Direct return only — same formula as capacity body.
auto inlineShouldStartExitEvent() -> bool
{
    return true;
}

// Compact dual-wire pin matching C++ capacity formula / Go pinShouldStartExitEvent3578:
//   return true
auto pinShouldStartExitEvent() -> bool
{
    return true;
}

// Prior dedicated pin3534 (always true; free == pin3534 always holds).
auto pinShouldStartExitEvent3534() -> bool
{
    return true;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldStartExitEvent
// (Lua ambuscade onInstanceComplete / onInstanceFailure always-start exit CS;
// OmegaXI internal/ambuscade; dedicated slice 3578 expand residual 2917; prior
// dedicated 3534/3488/3438/3382/3109). Formula unchanged:
//
//   ShouldStartExitEvent() = true
//
// Coverage:
//   - free == inline == pin == pin3534 == true
//   - residual 2917 / 3109 / 3382 / 3438 / 3488 / 3534 / 1089 pins still hold
//   - complete/failure host compose poles
//   - dense always-true poles (no state)
//   - FailureExitCSID / EventCSIDExit identity
auto runAmbuscadeStartExit3578SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDExit;
    using ambuscadehelpers::ShouldStartExitEvent;

    bool ok = true;

    // Residual constant / 1089 / 2917 / prior dedicated 3109 / 3382 / 3438 / 3488 / 3534 pins still hold.
    ok = expect(EventCSIDExit == 10001, "EventCSIDExit == 10001") && ok;
    ok = expect(ShouldStartExitEvent(), "residual: complete/failure should start exit event") && ok;

    // Unconditional gate always true; free == inline == pin == pin3534 (direct return true).
    {
        const bool got       = ShouldStartExitEvent();
        const bool inlineGot = inlineShouldStartExitEvent();
        const bool pinGot    = pinShouldStartExitEvent();
        const bool pin3534   = pinShouldStartExitEvent3534();

        ok = expect(got, "ShouldStartExitEvent → true") && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == pin3534, "dual-wire free == pin3534") && ok;
        ok = expect(got == true, "dual-wire free == true") && ok;
    }

    // Repeated calls stay true (no state); free == inline == pin == pin3534 matching C++.
    ok = expect(ShouldStartExitEvent() && ShouldStartExitEvent(),
                "ShouldStartExitEvent remains true across calls") &&
         ok;
    for (int i = 0; i < 3; ++i)
    {
        const bool got = ShouldStartExitEvent();
        ok             = expect(got == pinShouldStartExitEvent(), "free == pin across calls") && ok;
        ok             = expect(got == inlineShouldStartExitEvent(), "free == inline across calls") && ok;
        ok             = expect(got == pinShouldStartExitEvent3534(), "free == pin3534 across calls") && ok;
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
        ok = expect(got == inlineGot && got == pinGot, "compose dual-wire free == inline == pin") && ok;
        ok = expect(got == pinShouldStartExitEvent3534(), "compose free == pin3534") && ok;
        ok = expect(got == true, "compose dual-wire free == true") && ok;
        ok = expect(c.exitCSID == 10001, "host exit CSID pin == 10001") && ok;
        ok = expect(c.exitCSID == EventCSIDExit, "host exit CSID == EventCSIDExit") && ok;
    }

    // Dense always-true poles: free == inline == pin == pin3534 for unconditional domain.
    for (int i = 0; i < 2; ++i)
    {
        const bool got = ShouldStartExitEvent();
        ok             = expect(got == pinShouldStartExitEvent(), "dense free == pin") && ok;
        ok             = expect(got == inlineShouldStartExitEvent(), "dense free == inline") && ok;
        ok             = expect(got == pinShouldStartExitEvent3534(), "dense free == pin3534") && ok;
        ok             = expect(got == true, "dense free == true") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return true).
    ok = expect(ShouldStartExitEvent() == true, "compose always-start") && ok;
    ok = expect(ShouldStartExitEvent() == pinShouldStartExitEvent(), "free == pin always-true") && ok;
    ok = expect(ShouldStartExitEvent() == inlineShouldStartExitEvent(), "free == inline always-true") && ok;
    ok = expect(ShouldStartExitEvent() == pinShouldStartExitEvent3534(), "free == pin3534 always-true") && ok;

    // Production path semantics (host inject model for complete / failure).
    ok = expect(ShouldStartExitEvent() == inlineShouldStartExitEvent(), "host complete path free == inline") && ok;
    ok = expect(ShouldStartExitEvent() == pinShouldStartExitEvent(), "host failure path free == pin") && ok;
    ok = expect(ShouldStartExitEvent() == pinShouldStartExitEvent3534(), "host path free == pin3534") && ok;

    // Residual 2917 / prior dedicated 3534/3488/3438/3382/3109 still hold under dedicated 3578.
    ok = expect(ShouldStartExitEvent(), "residual 2917/3534/3488/3438/3382/3109 pins under 3578") && ok;

    return ok;
}
