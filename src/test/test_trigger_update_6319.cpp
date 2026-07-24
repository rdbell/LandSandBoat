#include "test_trigger_update_6319.h"

#include "map/ai/states/trigger_update.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trigger Update 6319 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineShouldOpenDoor(const bool hasChar, const bool door, const bool closeAnim) -> bool
{
    return hasChar && door && closeAnim;
}

auto inlineShouldCloseDoor(const bool close, const bool tickAfter) -> bool
{
    return close && tickAfter;
}

auto inlineShouldExitImmediate(const bool completed, const bool close) -> bool
{
    return completed && !close;
}

} // namespace

// Pure dual-wire suite for CTriggerState::Update gates
// (OmegaXI internal/aistate; slice 6319).
auto runTriggerUpdate6319SelfTests() -> bool
{
    using triggerupdate::shouldCloseDoor;
    using triggerupdate::shouldExitImmediate;
    using triggerupdate::shouldOpenDoor;

    bool ok = true;

    // shouldOpenDoor
    ok = expect(shouldOpenDoor(true, true, true), "char+door+close anim → open") && ok;
    ok = expect(!shouldOpenDoor(false, true, true), "no char → no open") && ok;
    ok = expect(!shouldOpenDoor(true, false, true), "not door → no open") && ok;
    ok = expect(!shouldOpenDoor(true, true, false), "not close anim → no open") && ok;

    for (const bool hasChar : { false, true })
    {
        for (const bool door : { false, true })
        {
            for (const bool closeAnim : { false, true })
            {
                const bool got     = shouldOpenDoor(hasChar, door, closeAnim);
                const bool inlineF = inlineShouldOpenDoor(hasChar, door, closeAnim);
                const bool want    = hasChar && door && closeAnim;
                ok                 = expect(got == want, "open free==want") && ok;
                ok                 = expect(got == inlineF, "open free==inline") && ok;
            }
        }
    }

    // shouldCloseDoor
    ok = expect(shouldCloseDoor(true, true), "close + 7s elapsed → re-close") && ok;
    ok = expect(!shouldCloseDoor(true, false), "close + not elapsed → wait") && ok;
    ok = expect(!shouldCloseDoor(false, true), "not close + elapsed → no re-close") && ok;

    for (const bool close : { false, true })
    {
        for (const bool tickAfter : { false, true })
        {
            const bool got     = shouldCloseDoor(close, tickAfter);
            const bool inlineF = inlineShouldCloseDoor(close, tickAfter);
            const bool want    = close && tickAfter;
            ok                 = expect(got == want, "close free==want") && ok;
            ok                 = expect(got == inlineF, "close free==inline") && ok;
        }
    }

    // shouldExitImmediate
    ok = expect(shouldExitImmediate(true, false), "completed no re-close → exit") && ok;
    ok = expect(!shouldExitImmediate(true, true), "completed re-close pending → no immediate exit") && ok;
    ok = expect(!shouldExitImmediate(false, false), "not completed → no immediate exit") && ok;

    for (const bool completed : { false, true })
    {
        for (const bool close : { false, true })
        {
            const bool got     = shouldExitImmediate(completed, close);
            const bool inlineF = inlineShouldExitImmediate(completed, close);
            const bool want    = completed && !close;
            ok                 = expect(got == want, "exit free==want") && ok;
            ok                 = expect(got == inlineF, "exit free==inline") && ok;
        }
    }

    return ok;
}
