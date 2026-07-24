#include "test_aicontainer_inactive_untargetable_host_6371.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Inactive/Untargetable host 6371 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Inactive / Untargetable ForceChange call-site
// residuals (slice 6371). Go hosts drive ForceChangeState under park/engage
// composition with untargetable flag semantics.
auto runAicontainerInactiveUntargetableHost6371SelfTests() -> bool
{
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::InternalDieHasBattleEntity;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::IsUntargetable;
    using aicontainerhelpers::TickPreventActionParkAllowed;

    bool ok = true;

    // Tick park admission residual (6314) — drives Inactive when true.
    ok = expect(TickPreventActionParkAllowed(true, true, false, false, false, true), "park admit") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, false, true, false, true), "park magic block") && ok;

    // Engage resume-inactive residual (6291).
    ok = expect(InternalEngageShouldResumeInactive(true), "engage resume") && ok;
    ok = expect(!InternalEngageShouldResumeInactive(false), "engage no prevent") && ok;

    // IsUntargetable residual (6310) after Untargetable host installs flag.
    ok = expect(IsUntargetable(true, true, false), "inactive untargetable") && ok;
    ok = expect(!IsUntargetable(true, false, false), "inactive targetable") && ok;
    ok = expect(IsUntargetable(false, false, true), "entity untargetable") && ok;

    // ForceChange ceiling residual (6368).
    ok = expect(CanPushState(10), "push ceiling") && ok;

    // 6370 residual still holds.
    ok = expect(InternalDieHasBattleEntity(true), "6370 residual") && ok;

    return ok;
}
