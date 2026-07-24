#include "test_aicontainer_tick_state_loop_6313.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer TickStateLoopContinue 6313 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineTickStateLoopContinue(const int guard) -> bool
{
    return guard <= 32;
}

} // namespace

// Pure dual-wire suite for Tick state-drain guard (slice 6313).
auto runAicontainerTickStateLoop6313SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::IsStateStackEmpty;
    using aicontainerhelpers::TickStateLoopContinue;

    bool ok = true;

    ok = expect(TickStateLoopContinue(0), "0 → continue") && ok;
    ok = expect(TickStateLoopContinue(32), "32 at bound → continue") && ok;
    ok = expect(!TickStateLoopContinue(33), "33 exceeds → break") && ok;
    ok = expect(!TickStateLoopContinue(100), "100 → break") && ok;

    for (const int g : { 0, 32, 33 })
    {
        const bool got     = TickStateLoopContinue(g);
        const bool inlineF = inlineTickStateLoopContinue(g);
        const bool want    = g <= 32;
        ok                 = expect(got == want, "free==want") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
    }

    ok = expect(CanPushState(10) && !CanPushState(11), "6312 residual: CanPushState still holds") && ok;
    ok = expect(IsStateStackEmpty(false) && !IsStateStackEmpty(true),
                "6311 residual: IsStateStackEmpty still holds") &&
         ok;
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6313") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6313") &&
         ok;

    return ok;
}
