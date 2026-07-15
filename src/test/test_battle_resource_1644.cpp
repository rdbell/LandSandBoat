#include "test_battle_resource_1644.h"

#include "map/battle_resource_capacity.h"

#include <iostream>

namespace
{
using namespace battleresourcehelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle resource 1644 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleResource1644SelfTests() -> bool
{
    bool ok = true;

    // --- IsFullyHealed ---
    ok = expect(!IsFullyHealed(false, 100, 100, 50, 50), "dead never full") && ok;
    ok = expect(IsFullyHealed(true, 100, 100, 50, 50), "alive full exact") && ok;
    ok = expect(IsFullyHealed(true, 120, 100, 60, 50), "alive over max still full") && ok;
    ok = expect(!IsFullyHealed(true, 99, 100, 50, 50), "alive hp short") && ok;
    ok = expect(!IsFullyHealed(true, 100, 100, 49, 50), "alive mp short") && ok;
    ok = expect(IsFullyHealed(true, 0, 0, 0, 0), "alive zero max both full") && ok;

    // --- addHP heal / cap ---
    {
        const auto r = ResolveAddHP(50, 30, 100, false);
        ok           = expect(r.NewValue == 80 && r.ReturnedAbsDelta == 30 && r.AppliedDelta == -30, "hp heal mid values") && ok;
        ok           = expect(r.SetUpdateMask && !r.StandUp && !r.ForceUnkillableOne, "hp heal mid flags") && ok;
    }
    {
        const auto r = ResolveAddHP(90, 50, 100, false);
        ok           = expect(r.NewValue == 100 && r.ReturnedAbsDelta == 10 && r.AppliedDelta == -10, "hp heal cap") && ok;
        ok           = expect(r.SetUpdateMask && !r.StandUp, "hp heal cap flags") && ok;
    }
    {
        const auto r = ResolveAddHP(100, 25, 100, false);
        ok           = expect(r.NewValue == 100 && r.ReturnedAbsDelta == 0 && r.AppliedDelta == 0, "hp already full") && ok;
        ok           = expect(!r.SetUpdateMask && !r.StandUp, "hp already full flags") && ok;
    }

    // --- addHP damage ---
    {
        const auto r = ResolveAddHP(100, -40, 100, false);
        ok           = expect(r.NewValue == 60 && r.ReturnedAbsDelta == 40 && r.AppliedDelta == 40, "hp damage mid") && ok;
        ok           = expect(r.SetUpdateMask && r.StandUp && !r.ForceUnkillableOne, "hp damage mid flags") && ok;
    }
    {
        const auto r = ResolveAddHP(30, -100, 100, false);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 30 && r.AppliedDelta == 30, "hp lethal") && ok;
        ok           = expect(r.SetUpdateMask && r.StandUp && !r.ForceUnkillableOne, "hp lethal flags") && ok;
    }

    // --- dead short-circuit ---
    {
        const auto r = ResolveAddHP(0, -50, 100, false);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 0 && r.AppliedDelta == 0, "dead damage values") && ok;
        ok           = expect(!r.SetUpdateMask && !r.StandUp && !r.ForceUnkillableOne, "dead damage flags") && ok;
    }
    {
        const auto r = ResolveAddHP(0, 25, 100, false);
        ok           = expect(r.NewValue == 25 && r.ReturnedAbsDelta == 25 && r.AppliedDelta == -25, "raise via addHP") && ok;
        ok           = expect(r.SetUpdateMask && !r.StandUp, "raise flags") && ok;
    }
    {
        const auto r = ResolveAddHP(0, 0, 100, false);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 0 && !r.SetUpdateMask, "dead zero delta") && ok;
    }

    // --- unkillable ---
    {
        const auto r = ResolveAddHP(50, -100, 100, true);
        ok           = expect(r.NewValue == 1 && r.ReturnedAbsDelta == 50 && r.AppliedDelta == 50, "unkillable lethal values") && ok;
        ok           = expect(r.SetUpdateMask && r.StandUp && r.ForceUnkillableOne, "unkillable lethal flags") && ok;
    }
    {
        const auto r = ResolveAddHP(0, -1, 100, true);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 0 && !r.ForceUnkillableOne, "dead unkillable still short-circuits") && ok;
    }
    {
        const auto r = ResolveAddHP(50, -10, 100, true);
        ok           = expect(r.NewValue == 40 && !r.ForceUnkillableOne, "unkillable non-lethal") && ok;
    }
    {
        const auto r = ResolveAddHP(1, -1, 100, true);
        ok           = expect(r.NewValue == 1 && r.ReturnedAbsDelta == 1 && r.ForceUnkillableOne, "unkillable exact kill") && ok;
    }

    // --- zero delta ---
    {
        const auto r = ResolveAddHP(75, 0, 100, false);
        ok           = expect(r.NewValue == 75 && r.ReturnedAbsDelta == 0 && !r.SetUpdateMask && !r.StandUp, "hp zero delta") && ok;
    }

    // --- maxHP 0 ---
    {
        const auto r = ResolveAddHP(0, 5, 0, false);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 0, "maxHP 0 heal") && ok;
    }
    {
        const auto r = ResolveAddHP(10, -5, 0, false);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 10 && r.AppliedDelta == 10, "maxHP 0 damage") && ok;
    }

    // --- addMP ---
    {
        const auto r = ResolveAddMP(20, 30, 100);
        ok           = expect(r.NewValue == 50 && r.ReturnedAbsDelta == 30 && r.AppliedDelta == -30, "mp heal values") && ok;
        ok           = expect(r.SetUpdateMask && !r.StandUp && !r.ForceUnkillableOne, "mp heal flags") && ok;
    }
    {
        const auto r = ResolveAddMP(90, 50, 100);
        ok           = expect(r.NewValue == 100 && r.ReturnedAbsDelta == 10, "mp cap") && ok;
    }
    {
        const auto r = ResolveAddMP(40, -100, 100);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 40 && r.AppliedDelta == 40, "mp drain") && ok;
        ok           = expect(r.SetUpdateMask && !r.StandUp, "mp drain flags") && ok;
    }
    {
        const auto r = ResolveAddMP(0, -10, 100);
        ok           = expect(r.NewValue == 0 && r.ReturnedAbsDelta == 0 && !r.SetUpdateMask, "mp already empty drain") && ok;
    }
    {
        const auto r = ResolveAddMP(0, 15, 100);
        ok           = expect(r.NewValue == 15 && r.ReturnedAbsDelta == 15, "mp from empty") && ok;
    }
    {
        const auto r = ResolveAddMP(50, 0, 100);
        ok           = expect(r.NewValue == 50 && r.ReturnedAbsDelta == 0 && !r.SetUpdateMask, "mp zero delta") && ok;
    }

    return ok;
}
