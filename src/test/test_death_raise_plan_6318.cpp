#include "test_death_raise_plan_6318.h"

#include "map/ai/states/death_raise_plan.h"
#include "map/ai/states/death_pc_update.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "death raise plan 6318 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for allowSendRaise / acceptRaise plans (slice 6318).
auto runDeathRaisePlan6318SelfTests() -> bool
{
    using deathpcupdate::shouldExitAfterRaise;
    using deathpcupdate::shouldOfferRaiseMenu;
    using deathraise::acceptRaisePlan;
    using deathraise::allowSendRaisePlan;

    bool ok = true;

    const auto now = timer::time_point{};

    const auto allow = allowSendRaisePlan(now);
    ok               = expect(allow.raiseTime == now + 12s, "allow raiseTime = now+12s") && ok;
    ok               = expect(!allow.raiseSent, "allow raiseSent false") && ok;

    const auto accept = acceptRaisePlan(now);
    ok                = expect(accept.raiseAcceptedTime == now, "accept time = now") && ok;
    ok                = expect(accept.raiseAccepted, "accept raiseAccepted true") && ok;
    ok                = expect(accept.complete, "accept complete true") && ok;

    // Residual: 6293 PC Update gates still hold.
    ok = expect(shouldExitAfterRaise(true, true, true) && !shouldExitAfterRaise(true, true, false),
                "6293 residual: shouldExitAfterRaise still holds") &&
         ok;
    ok = expect(shouldOfferRaiseMenu(true, false, true, true) && !shouldOfferRaiseMenu(true, true, true, true),
                "6293 residual: shouldOfferRaiseMenu still holds") &&
         ok;

    return ok;
}
