#include "test_aicontainer_can_dispatch_3470.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer CanDispatch 3470 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer public controller-forward presence gate for dual-wire
// cross-check (dedicated 3470 expand residual 2947; prior dedicated 3416 /
// 3369 / 3222):
//   hasController
auto inlineCanDispatch(const bool hasController) -> bool
{
    return hasController;
}

// Compact dual-wire pin matching Go pinCanDispatch3470 / C++ capacity
// identity form (formula unchanged from 1189 / 2947 / 3222 / 3369 / 3416).
// Direct return of hasController (not if/else):
//   hasController
auto pinCanDispatch(const bool hasController) -> bool
{
    return hasController;
}

} // namespace

// Pure dual-wire expansion for aicontainerhelpers::CanDispatch
// (hasController identity; OmegaXI internal/aicontainer; dedicated slice 3470
// expand residual 2947; prior dedicated 3416 / 3369 / 3222; pure 1189).
//
// Coverage:
//   - free == inline == pin identity form
//   - residual poles: true/false
//   - dense 2^1 free == inline == pin
//   - host-style inject + residual independence (2952 / 3272 / 3303 left alone)
auto runAicontainerCanDispatch3470SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;

    bool ok = true;

    // Residual 1189 / 2947 pins still hold under dedicated dual-wire.
    ok = expect(!CanDispatch(false), "residual: no controller rejects") && ok;
    ok = expect(CanDispatch(true), "residual: controller present accepts") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        hasController;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: no controller rejects (return false without invoking).
        { false, false, "no controller rejects" },

        // Pole: controller present accepts (host may invoke controller method).
        { true, true, "controller present accepts" },

        // Residual 2947 / 1189 re-pins.
        { false, false, "residual 2947 no controller rejects" },
        { true, true, "residual 2947 controller present accepts" },
        { false, false, "residual 1189 CanDispatch(false)" },
        { true, true, "residual 1189 CanDispatch(true)" },

        // Prior dedicated 3222 re-pins.
        { false, false, "prior 3222 no controller rejects" },
        { true, true, "prior 3222 controller present accepts" },

        // Prior dedicated 3369 re-pins.
        { false, false, "prior 3369 no controller rejects" },
        { true, true, "prior 3369 controller present accepts" },

        // Prior dedicated 3416 re-pins.
        { false, false, "prior 3416 no controller rejects" },
        { true, true, "prior 3416 controller present accepts" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanDispatch(c.hasController);
        const bool inlineF = inlineCanDispatch(c.hasController);
        const bool pinGot  = pinCanDispatch(c.hasController);
        const bool wantPin = c.hasController;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(CanDispatch(true) == pinCanDispatch(true), "free==pin controller present") && ok;
    ok = expect(CanDispatch(false) == pinCanDispatch(false), "free==pin no controller") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin == hasController.
    for (const bool hasController : { false, true })
    {
        const bool got     = CanDispatch(hasController);
        const bool inlineF = inlineCanDispatch(hasController);
        const bool pinGot  = pinCanDispatch(hasController);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == hasController, "compose free==identity") && ok;
    }

    // Host-style inject poles: CAIContainer::{Cast,Engage,...} injects
    // Controller / typed dynamic_cast presence.
    const struct
    {
        bool        hasController;
        bool        wantAccept;
        const char* label;
    } hostCases[] = {
        { false, false, "Controller == nullptr / typed cast fail → return false without invoke" },
        { true, true, "Controller present / typed cast ok → dispatch path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = CanDispatch(c.hasController);
        const bool inlineF = inlineCanDispatch(c.hasController);
        const bool pinGot  = pinCanDispatch(c.hasController);

        ok = expect(got == c.wantAccept, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CAIContainer path semantics:
    // no controller → reject; controller present → dispatch.
    ok = expect(!CanDispatch(false) && !pinCanDispatch(false),
                "Cast/Engage/... no Controller → reject path") &&
         ok;
    ok = expect(CanDispatch(true) && pinCanDispatch(true),
                "Cast/Engage/... Controller present → dispatch path") &&
         ok;

    // Residual independence (1189 / 2947 / 3222 / 3369 / 3416 / 2952 / 3272 / 3303 left alone):
    // CanDispatch is distinct from CanChangeState.
    ok = expect(!CanDispatch(false), "no controller must reject via free gate") && ok;
    ok = expect(CanDispatch(true), "controller present must accept via free gate") && ok;
    ok = expect(CanChangeState(false, false) && CanChangeState(false, true),
                "idle CanChangeState residual still holds under dual-wire (2952/3272/3303 left alone)") &&
         ok;
    ok = expect(!CanChangeState(true, false) && CanChangeState(true, true),
                "current-state CanChangeState residual still holds under dual-wire") &&
         ok;

    return ok;
}
