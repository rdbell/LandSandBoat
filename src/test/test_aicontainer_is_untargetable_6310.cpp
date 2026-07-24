#include "test_aicontainer_is_untargetable_6310.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer IsUntargetable 6310 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineIsUntargetable(const bool isInactiveState, const bool inactiveUntargetable, const bool entityUntargetable) -> bool
{
    return (isInactiveState && inactiveUntargetable) || entityUntargetable;
}

auto pinIsUntargetable(const bool isInactiveState, const bool inactiveUntargetable, const bool entityUntargetable) -> bool
{
    return (isInactiveState && inactiveUntargetable) || entityUntargetable;
}

} // namespace

// Pure dual-wire suite for IsUntargetable (OmegaXI internal/aicontainer; slice 6310).
auto runAicontainerIsUntargetable6310SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanFollowPath;
    using aicontainerhelpers::InternalActionTargetAllowed;
    using aicontainerhelpers::IsEngagedAnimation;
    using aicontainerhelpers::IsRoamingAnimation;
    using aicontainerhelpers::IsSpawnedStatus;
    using aicontainerhelpers::IsUntargetable;

    bool ok = true;

    const struct
    {
        bool inactive;
        bool inactiveU;
        bool entityU;
        bool want;
    } cases[] = {
        { false, false, false, false },
        { false, false, true, true },
        { false, true, false, false },
        { false, true, true, true },
        { true, false, false, false },
        { true, false, true, true },
        { true, true, false, true },
        { true, true, true, true },
    };

    for (const auto& c : cases)
    {
        const bool got     = IsUntargetable(c.inactive, c.inactiveU, c.entityU);
        const bool inlineF = inlineIsUntargetable(c.inactive, c.inactiveU, c.entityU);
        const bool pinGot  = pinIsUntargetable(c.inactive, c.inactiveU, c.entityU);
        ok                 = expect(got == c.want, "free==want") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
        ok                 = expect(got == pinGot, "free==pin") && ok;
    }

    ok = expect(IsUntargetable(true, true, false), "inactive untargetable → true") && ok;
    ok = expect(IsUntargetable(false, false, true), "entity untargetable alone → true") && ok;
    ok = expect(!IsUntargetable(true, false, false), "inactive targetable + entity targetable → false") && ok;

    // Residual: 6302 InternalActionTargetAllowed still holds.
    ok = expect(!InternalActionTargetAllowed(true, true) && InternalActionTargetAllowed(true, false),
                "6302 residual: action target gate still holds") &&
         ok;

    // Residual: 6308/6309 still hold.
    ok = expect(IsEngagedAnimation(true) && !IsEngagedAnimation(false), "6308 residual: engaged") && ok;
    ok = expect(IsRoamingAnimation(true) && !IsRoamingAnimation(false), "6308 residual: roaming") && ok;
    ok = expect(IsSpawnedStatus(false) && !IsSpawnedStatus(true), "6309 residual: spawned") && ok;

    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6310") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6310") &&
         ok;
    ok = expect(CanFollowPath(true, false, false) && !CanFollowPath(false, false, true),
                "6306 residual: CanFollowPath still holds") &&
         ok;

    return ok;
}
