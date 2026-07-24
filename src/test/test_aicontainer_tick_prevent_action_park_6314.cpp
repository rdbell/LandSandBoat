#include "test_aicontainer_tick_prevent_action_park_6314.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer TickPreventActionPark 6314 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlinePark(const bool hasBattle, const bool alive, const bool inactive, const bool magic, const bool mobskill, const bool prevent)
    -> bool
{
    return hasBattle && alive && !inactive && !magic && !mobskill && prevent;
}

} // namespace

// Pure dual-wire suite for Tick prevent-action park admission (slice 6314).
auto runAicontainerTickPreventActionPark6314SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::TickPreventActionParkAllowed;
    using aicontainerhelpers::TickStateLoopContinue;

    bool ok = true;

    ok = expect(TickPreventActionParkAllowed(true, true, false, false, false, true),
                "full admit → park") &&
         ok;
    ok = expect(!TickPreventActionParkAllowed(false, true, false, false, false, true), "no battle") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, false, false, false, false, true), "dead") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, true, false, false, true), "inactive") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, false, true, false, true), "magic") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, false, false, true, true), "mobskill") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, false, false, false, false), "no prevent") && ok;

    const bool got = TickPreventActionParkAllowed(true, true, false, false, false, true);
    ok             = expect(got == inlinePark(true, true, false, false, false, true), "free==inline") && ok;

    ok = expect(InternalEngageShouldResumeInactive(true) && !InternalEngageShouldResumeInactive(false),
                "6291 residual: resume inactive still holds") &&
         ok;
    ok = expect(TickStateLoopContinue(32) && !TickStateLoopContinue(33),
                "6313 residual: TickStateLoopContinue still holds") &&
         ok;
    ok = expect(CanPushState(10) && !CanPushState(11), "6312 residual: CanPushState still holds") && ok;
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6314") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6314") &&
         ok;

    return ok;
}
