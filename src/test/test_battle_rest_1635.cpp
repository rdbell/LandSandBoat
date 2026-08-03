#include "test_battle_rest_1635.h"

#include "map/battle_rest_capacity.h"

#include <iostream>

auto runBattleRest1635SelfTests() -> bool
{
    using namespace battleresthelpers;

    bool ok = true;

    ok = ok && ResolveResourcePercent(0, 100) == 0;
    ok = ok && ResolveResourcePercent(50, 100) == 50;
    ok = ok && ResolveResourcePercent(1, 1000) == 1; // floor would be 0.1 → max(1,0)=1
    ok = ok && ResolveResourcePercent(100, 100) == 100;
    ok = ok && ResolveResourcePercent(10, 0) == 0;
    // Preserve the float32 intermediate: float64 arithmetic floors 57.999... to 57.
    ok = ok && ResolveResourcePercent(29, 50) == 58;

    ok = ok && CanRest(false, false);
    ok = ok && !CanRest(true, false);
    ok = ok && !CanRest(false, true);

    {
        const auto p = ResolveRestPlan(50, 100, 50, 100, 0, 0.1f);
        ok           = ok && p.recoverResources && p.recoverHP == 10 && p.recoverMP == 10 && !p.drainTP && p.didRest;
    }
    {
        // full HP/MP but has TP
        const auto p = ResolveRestPlan(100, 100, 100, 100, 300, 0.1f);
        ok           = ok && !p.recoverResources && p.drainTP && p.tpDelta == -50 && p.didRest;
    }
    {
        // already full and no TP
        const auto p = ResolveRestPlan(100, 100, 100, 100, 0, 0.1f);
        ok           = ok && !p.didRest;
    }
    {
        // both recover and drain
        const auto p = ResolveRestPlan(50, 100, 100, 100, 1000, 0.2f);
        ok           = ok && p.recoverResources && p.recoverHP == 20 && p.recoverMP == 20 && p.drainTP && p.tpDelta == -100 && p.didRest;
    }

    if (!ok)
    {
        std::cerr << "battle rest 1635 self-test failed\n";
    }
    return ok;
}
