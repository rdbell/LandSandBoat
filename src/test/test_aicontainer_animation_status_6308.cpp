#include "test_aicontainer_animation_status_6308.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer animation status 6308 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for IsEngaged / IsRoaming animation identity gates
// (OmegaXI internal/aicontainer; slice 6308).
auto runAicontainerAnimationStatus6308SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanFollowPath;
    using aicontainerhelpers::IsEngagedAnimation;
    using aicontainerhelpers::IsRoamingAnimation;
    using aicontainerhelpers::InternalUseItemHasCharEntity;

    bool ok = true;

    ok = expect(IsEngagedAnimation(true) && !IsEngagedAnimation(false), "IsEngagedAnimation identity") && ok;
    ok = expect(IsRoamingAnimation(true) && !IsRoamingAnimation(false), "IsRoamingAnimation identity") && ok;

    for (const bool v : { false, true })
    {
        ok = expect(IsEngagedAnimation(v) == v, "engaged free==identity") && ok;
        ok = expect(IsRoamingAnimation(v) == v, "roaming free==identity") && ok;
    }

    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6308") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6308") &&
         ok;
    ok = expect(InternalUseItemHasCharEntity(true) && !InternalUseItemHasCharEntity(false),
                "6307 residual: use-item char gate still holds") &&
         ok;
    ok = expect(CanFollowPath(true, false, false) && !CanFollowPath(false, false, true),
                "6306 residual: CanFollowPath still holds") &&
         ok;

    return ok;
}
