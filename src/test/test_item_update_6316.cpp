#include "test_item_update_6316.h"

#include "map/ai/states/item_update.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "item Update 6316 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineShouldFinishCast(const bool tickAfter, const bool completed) -> bool
{
    return !completed && tickAfter;
}

auto inlineShouldExit(const bool tickAfter, const bool completed) -> bool
{
    return completed && tickAfter;
}

} // namespace

// Pure dual-wire suite for CItemState::Update phase gates
// (OmegaXI internal/aistate; slice 6316).
auto runItemUpdate6316SelfTests() -> bool
{
    using itemupdate::shouldExit;
    using itemupdate::shouldFinishCast;

    bool ok = true;

    // shouldFinishCast: !completed && tickAfter
    ok = expect(shouldFinishCast(true, false), "cast elapsed incomplete → finish cast") && ok;
    ok = expect(!shouldFinishCast(true, true), "cast elapsed but completed → no finish") && ok;
    ok = expect(!shouldFinishCast(false, false), "cast not elapsed → no finish") && ok;
    ok = expect(!shouldFinishCast(false, true), "not elapsed completed → no finish") && ok;

    for (const bool tickAfter : { false, true })
    {
        for (const bool completed : { false, true })
        {
            const bool got     = shouldFinishCast(tickAfter, completed);
            const bool inlineF = inlineShouldFinishCast(tickAfter, completed);
            const bool want    = !completed && tickAfter;
            ok                 = expect(got == want, "finish free==want") && ok;
            ok                 = expect(got == inlineF, "finish free==inline") && ok;
        }
    }

    // shouldExit: completed && tickAfter cast+anim
    ok = expect(shouldExit(true, true), "completed + anim elapsed → exit") && ok;
    ok = expect(!shouldExit(true, false), "anim elapsed incomplete → no exit") && ok;
    ok = expect(!shouldExit(false, true), "completed anim not elapsed → no exit") && ok;
    ok = expect(!shouldExit(false, false), "neither → no exit") && ok;

    for (const bool tickAfter : { false, true })
    {
        for (const bool completed : { false, true })
        {
            const bool got     = shouldExit(tickAfter, completed);
            const bool inlineF = inlineShouldExit(tickAfter, completed);
            const bool want    = completed && tickAfter;
            ok                 = expect(got == want, "exit free==want") && ok;
            ok                 = expect(got == inlineF, "exit free==inline") && ok;
        }
    }

    // Host-style Update poles: finish branch takes priority over exit when both
    // could be true is impossible (finish requires !completed; exit requires completed).
    ok = expect(shouldFinishCast(true, false) && !shouldExit(true, false),
                "finish-cast path exclusive of exit") &&
         ok;
    ok = expect(!shouldFinishCast(true, true) && shouldExit(true, true),
                "exit path exclusive of finish-cast") &&
         ok;

    return ok;
}
