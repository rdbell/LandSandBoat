/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_action_queue.h"

#include "map/ai/ai_container.h"
#include "map/ai/helpers/action_queue.h"
#include "map/entities/base_entity.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace
{

using namespace std::chrono_literals;

class TestEntity final : public CBaseEntity
{
public:
    auto Tick(timer::time_point) -> Task<void> override
    {
        co_return;
    }

    void PostTick() override
    {
    }
};

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "action queue self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testStrictDeadlinePriorityAndIdentity() -> bool
{
    TestEntity entity;
    entity.PAI = std::make_unique<CAIContainer>(&entity);
    CAIActionQueue queue(&entity);
    const auto base = timer::time_point{ 100s };

    std::vector<std::string> calls;
    auto callback = [&](const char* name)
    {
        return [&, name](CBaseEntity* calledEntity)
        {
            calls.emplace_back(calledEntity == &entity ? name : "wrong-entity");
        };
    };

    auto stateLate = queueAction_t(30ms, true, callback("state-late"));
    stateLate.start_time = base;
    queue.pushAction(std::move(stateLate));
    auto stateEarly = queueAction_t(20ms, true, callback("state-early"));
    stateEarly.start_time = base;
    queue.pushAction(std::move(stateEarly));
    auto timerEarly = queueAction_t(10ms, false, callback("timer-early"));
    timerEarly.start_time = base;
    queue.pushAction(std::move(timerEarly));
    auto timerLate = queueAction_t(40ms, false, callback("timer-late"));
    timerLate.start_time = base;
    queue.pushAction(std::move(timerLate));

    queue.checkAction(base + 10ms);
    bool ok = true;
    ok = expect(calls.empty(), "exact deadline is not due") && ok;

    queue.checkAction(base + 31ms);
    ok = expect(calls == std::vector<std::string>{ "timer-early", "state-early", "state-late" }, "timer-first and within-queue priority") && ok;

    queue.checkAction(base + 41ms);
    ok = expect(calls == std::vector<std::string>{ "timer-early", "state-early", "state-late", "timer-late" }, "remaining timer and entity identity") && ok;
    ok = expect(queue.isEmpty(), "empty after drain") && ok;
    return ok;
}

auto testIndependentCancellation() -> bool
{
    TestEntity entity;
    entity.PAI = std::make_unique<CAIContainer>(&entity);
    const auto base = timer::time_point{ 200s };
    std::vector<std::string> calls;

    CAIActionQueue clearActions(&entity);
    auto state = queueAction_t(1ms, true, [&](CBaseEntity*) { calls.emplace_back("cancelled-state"); });
    state.start_time = base;
    clearActions.pushAction(std::move(state));
    auto timer = queueAction_t(1ms, false, [&](CBaseEntity*) { calls.emplace_back("preserved-timer"); });
    timer.start_time = base;
    clearActions.pushAction(std::move(timer));
    clearActions.clearActionQueue();
    clearActions.checkAction(base + 2ms);

    bool ok = true;
    ok = expect(calls == std::vector<std::string>{ "preserved-timer" }, "clearing actions preserves timers") && ok;
    ok = expect(clearActions.isEmpty(), "empty after preserved timer") && ok;

    CAIActionQueue clearTimers(&entity);
    auto preservedState = queueAction_t(1ms, true, [&](CBaseEntity*) { calls.emplace_back("preserved-state"); });
    preservedState.start_time = base;
    clearTimers.pushAction(std::move(preservedState));
    auto cancelledTimer = queueAction_t(1ms, false, [&](CBaseEntity*) { calls.emplace_back("cancelled-timer"); });
    cancelledTimer.start_time = base;
    clearTimers.pushAction(std::move(cancelledTimer));
    clearTimers.clearTimerQueue();
    clearTimers.checkAction(base + 2ms);

    ok = expect(calls == std::vector<std::string>{ "preserved-timer", "preserved-state" }, "clearing timers preserves actions") && ok;
    ok = expect(clearTimers.isEmpty(), "empty after preserved action") && ok;
    return ok;
}

} // namespace

auto runActionQueueSelfTests() -> bool
{
    bool ok = true;
    ok = testStrictDeadlinePriorityAndIdentity() && ok;
    ok = testIndependentCancellation() && ok;
    return ok;
}
