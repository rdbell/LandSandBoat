#include "test_aicontainer_is_spawned_6309.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer IsSpawned 6309 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineIsSpawnedStatus(const bool isDisappear) -> bool
{
    return !isDisappear;
}

} // namespace

// Pure dual-wire suite for IsSpawned (!isDisappear) (slice 6309).
auto runAicontainerIsSpawned6309SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::IsEngagedAnimation;
    using aicontainerhelpers::IsRoamingAnimation;
    using aicontainerhelpers::IsSpawnedStatus;

    bool ok = true;

    ok = expect(IsSpawnedStatus(false), "not DISAPPEAR → spawned") && ok;
    ok = expect(!IsSpawnedStatus(true), "DISAPPEAR → not spawned") && ok;

    for (const bool isDisappear : { false, true })
    {
        const bool got     = IsSpawnedStatus(isDisappear);
        const bool inlineF = inlineIsSpawnedStatus(isDisappear);
        const bool want    = !isDisappear;
        ok                 = expect(got == want, "free==want") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
    }

    ok = expect(IsEngagedAnimation(true) && !IsEngagedAnimation(false), "6308 residual: engaged") && ok;
    ok = expect(IsRoamingAnimation(true) && !IsRoamingAnimation(false), "6308 residual: roaming") && ok;
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6309") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6309") &&
         ok;

    return ok;
}
