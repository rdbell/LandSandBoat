#include "test_aicontainer_can_push_state_6312.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer CanPushState 6312 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineCanPushState(const std::size_t stateCount) -> bool
{
    return stateCount <= 10;
}

} // namespace

// Pure dual-wire suite for CanPushState stack ceiling (slice 6312).
auto runAicontainerCanPushState6312SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::IsStateStackEmpty;

    bool ok = true;

    ok = expect(CanPushState(0), "0 → allow") && ok;
    ok = expect(CanPushState(10), "10 at ceiling → allow") && ok;
    ok = expect(!CanPushState(11), "11 exceeds → reject") && ok;
    ok = expect(!CanPushState(100), "100 → reject") && ok;

    for (const std::size_t n : { std::size_t{ 0 }, std::size_t{ 10 }, std::size_t{ 11 } })
    {
        const bool got     = CanPushState(n);
        const bool inlineF = inlineCanPushState(n);
        const bool want    = n <= 10;
        ok                 = expect(got == want, "free==want") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
    }

    ok = expect(IsStateStackEmpty(false) && !IsStateStackEmpty(true),
                "6311 residual: IsStateStackEmpty still holds") &&
         ok;
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6312") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6312") &&
         ok;

    return ok;
}
