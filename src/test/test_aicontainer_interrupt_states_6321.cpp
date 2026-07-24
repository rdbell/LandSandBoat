#include "test_aicontainer_interrupt_states_6321.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer InterruptStates 6321 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineShouldInterrupt(const bool hasCurrent, const bool canInterrupt) -> bool
{
    return hasCurrent && canInterrupt;
}

} // namespace

// Pure dual-wire suite for InterruptStates loop admission (slice 6321).
auto runAicontainerInterruptStates6321SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::ShouldInterruptCurrent;
    using aicontainerhelpers::TickPreventActionParkAllowed;

    bool ok = true;

    ok = expect(ShouldInterruptCurrent(true, true), "current + canInterrupt → continue") && ok;
    ok = expect(!ShouldInterruptCurrent(true, false), "current + !canInterrupt → stop") && ok;
    ok = expect(!ShouldInterruptCurrent(false, true), "no current → stop") && ok;
    ok = expect(!ShouldInterruptCurrent(false, false), "no current !can → stop") && ok;

    for (const bool has : { false, true })
    {
        for (const bool can : { false, true })
        {
            const bool got     = ShouldInterruptCurrent(has, can);
            const bool inlineF = inlineShouldInterrupt(has, can);
            const bool want    = has && can;
            ok                 = expect(got == want, "free==want") && ok;
            ok                 = expect(got == inlineF, "free==inline") && ok;
        }
    }

    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6321") &&
         ok;
    ok = expect(TickPreventActionParkAllowed(true, true, false, false, false, true),
                "6314 residual: prevent-action park still admits") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6321") &&
         ok;

    return ok;
}
