#include "test_npc_post_tick_2651.h"

#include <chrono>
#include <iostream>

#include "map/entities/npc_post_tick.h"

using namespace std::chrono_literals;

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "npc post-tick self-test failed: " << label << '\n';
    }
    return condition;
}

auto testGuardsAndStrictExpiry() -> bool
{
    const auto now  = timer::time_point{} + 1s;
    const auto next = now;
    bool       ok   = true;
    ok              = expect(!npcentity::PlanPostTick(false, 1, false, now + 1ms, next).sendUpdate, "zone guard") && ok;
    ok              = expect(!npcentity::PlanPostTick(true, 0, false, now + 1ms, next).sendUpdate, "mask guard") && ok;
    ok              = expect(!npcentity::PlanPostTick(true, 1, true, now + 1ms, next).sendUpdate, "disappear guard") && ok;
    return expect(!npcentity::PlanPostTick(true, 1, false, now, next).sendUpdate, "strict equality does not flush") && ok;
}

auto testFlushAndThrottle() -> bool
{
    const auto now  = timer::time_point{} + 1s;
    const auto plan = npcentity::PlanPostTick(true, 0x80, false, now, now - 1ms);
    bool       ok   = expect(plan.sendUpdate, "expired update flushes");
    ok              = expect(plan.nextUpdateTimer == now + 250ms, "successful flush advances by 250ms") && ok;
    ok              = expect(!npcentity::PlanPostTick(true, 0x80, false, now + 249ms, plan.nextUpdateTimer).sendUpdate, "pre-expiry tick is suppressed") && ok;
    ok              = expect(!npcentity::PlanPostTick(true, 0x80, false, plan.nextUpdateTimer, plan.nextUpdateTimer).sendUpdate, "expiry equality is suppressed") && ok;
    return expect(npcentity::PlanPostTick(true, 0x80, false, plan.nextUpdateTimer + 1ms, plan.nextUpdateTimer).sendUpdate, "post-expiry tick flushes") && ok;
}

} // namespace

auto runNpcPostTick2651SelfTests() -> bool
{
    return testGuardsAndStrictExpiry() && testFlushAndThrottle();
}
