#include "test_status_aura_refresh_2798.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status aura refresh 2798 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const statuseffecthelpers::AuraExistingEffectPlan& plan,
    const bool                                         wantRefresh,
    const bool                                         wantUpdatePower,
    const char* const                                  label) -> bool
{
    const bool ok = plan.refreshStartTime == wantRefresh && plan.updatePower == wantUpdatePower;
    return expect(ok, label);
}

} // namespace

auto runStatusAuraRefresh2798SelfTests() -> bool
{
    using statuseffecthelpers::PlanAuraExistingAlwaysExpiring;
    using statuseffecthelpers::ResolveAuraEffectIcon;
    using statuseffecthelpers::ResolveAuraSubIcon;
    using statuseffecthelpers::ShouldRefreshAlwaysExpiringAura;
    using statuseffecthelpers::ShouldUpdateAuraPower;

    bool ok = true;

    // AlwaysExpiring path: always refresh start time; update power when mismatched.
    ok = expectPlan(PlanAuraExistingAlwaysExpiring(true, 10, 20), true, true, "refresh + power change") && ok;
    ok = expectPlan(PlanAuraExistingAlwaysExpiring(true, 15, 15), true, false, "refresh same power") && ok;
    ok = expectPlan(PlanAuraExistingAlwaysExpiring(true, 0, 0), true, false, "refresh zero power") && ok;
    ok = expectPlan(PlanAuraExistingAlwaysExpiring(true, 1, 0), true, true, "refresh power to zero") && ok;

    // Host should take add-new path when !hasAlwaysExpiringFlag; plan is inert.
    ok = expectPlan(PlanAuraExistingAlwaysExpiring(false, 10, 20), false, false, "no flag inert") && ok;
    ok = expectPlan(PlanAuraExistingAlwaysExpiring(false, 5, 5), false, false, "no flag same power inert") && ok;

    // Compose existing 1366 helpers: plan fields match ShouldUpdateAuraPower when flag set.
    ok = expect(ShouldRefreshAlwaysExpiringAura(true, true) && !ShouldRefreshAlwaysExpiringAura(true, false), "helper path gate") && ok;
    ok = expect(ShouldRefreshAlwaysExpiringAura(false, true) == false, "helper needs effect") && ok;
    ok = expect(ShouldUpdateAuraPower(1, 2) && !ShouldUpdateAuraPower(2, 2), "helper power") && ok;

    const struct
    {
        bool        hasFlag;
        uint16      currentPower;
        uint16      newSubPower;
        const char* label;
    } composeCases[] = {
        { true, 3, 9, "compose mismatch" },
        { true, 7, 7, "compose match" },
        { false, 3, 9, "compose no flag" },
        { false, 0, 0, "compose no flag zeros" },
    };
    for (const auto& c : composeCases)
    {
        const auto plan = PlanAuraExistingAlwaysExpiring(c.hasFlag, c.currentPower, c.newSubPower);
        const bool wantRefresh = c.hasFlag;
        const bool wantUpdate  = c.hasFlag && ShouldUpdateAuraPower(c.currentPower, c.newSubPower);
        ok                     = expectPlan(plan, wantRefresh, wantUpdate, c.label) && ok;
    }

    // New-effect path icon: subIcon > 0 ? subIcon : subID.
    ok = expect(ResolveAuraSubIcon(0, 42) == 42, "subicon fallback subID") && ok;
    ok = expect(ResolveAuraSubIcon(9, 42) == 9, "subicon prefer subIcon") && ok;
    ok = expect(ResolveAuraSubIcon(0, 0) == 0, "subicon both zero") && ok;
    ok = expect(ResolveAuraSubIcon(1, 0) == 1, "subicon nonzero over zero") && ok;

    // ResolveAuraSubIcon matches ResolveAuraEffectIcon (slice 1366 alias).
    ok = expect(ResolveAuraSubIcon(0, 99) == ResolveAuraEffectIcon(0, 99), "alias fallback") && ok;
    ok = expect(ResolveAuraSubIcon(12, 99) == ResolveAuraEffectIcon(12, 99), "alias prefer") && ok;

    return ok;
}
