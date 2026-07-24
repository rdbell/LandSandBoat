#include "test_aicontainer_is_state_stack_empty_6311.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer IsStateStackEmpty 6311 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineIsStateStackEmpty(const bool hasCurrentState) -> bool
{
    return !hasCurrentState;
}

} // namespace

// Pure dual-wire suite for IsStateStackEmpty (slice 6311).
auto runAicontainerIsStateStackEmpty6311SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::IsSpawnedStatus;
    using aicontainerhelpers::IsStateStackEmpty;
    using aicontainerhelpers::IsUntargetable;

    bool ok = true;

    ok = expect(IsStateStackEmpty(false), "no current → empty") && ok;
    ok = expect(!IsStateStackEmpty(true), "has current → not empty") && ok;

    for (const bool hasCurrent : { false, true })
    {
        const bool got     = IsStateStackEmpty(hasCurrent);
        const bool inlineF = inlineIsStateStackEmpty(hasCurrent);
        const bool want    = !hasCurrent;
        ok                 = expect(got == want, "free==want") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
    }

    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6311") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6311") &&
         ok;
    ok = expect(!IsUntargetable(false, false, false) && IsUntargetable(true, true, false),
                "6310 residual: IsUntargetable still holds") &&
         ok;
    ok = expect(IsSpawnedStatus(false) && !IsSpawnedStatus(true),
                "6309 residual: IsSpawned still holds") &&
         ok;

    return ok;
}
