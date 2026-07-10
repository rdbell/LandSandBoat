/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_time_triggers.h"

#include "map/timetriggers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

class TimeTriggerTestAccess
{
public:
    static void reset(CTriggerHandler& handler)
    {
        handler.triggerList.clear();
    }

    static void insert(CTriggerHandler& handler, Trigger_t trigger, vanadiel_time::duration now)
    {
        handler.insertTriggerAt(trigger, now);
    }

    static void tick(CTriggerHandler& handler, vanadiel_time::duration now, const std::function<void(CNpcEntity*, uint8)>& callback)
    {
        handler.triggerTimerAt(now, callback);
    }

    static auto lastTrigger(const CTriggerHandler& handler, std::size_t index) -> uint32
    {
        return handler.triggerList.at(index).lastTrigger;
    }
};

namespace
{

using VanaDuration = vanadiel_time::duration;

constexpr auto vanaMinute(std::int64_t count) -> VanaDuration
{
    return VanaDuration{ count * 60 * 1000 };
}

struct NpcStorage
{
    alignas(CNpcEntity) std::array<std::byte, sizeof(CNpcEntity)> bytes{};

    auto pointer() -> CNpcEntity*
    {
        return reinterpret_cast<CNpcEntity*>(bytes.data());
    }
};

struct CallbackCall
{
    CNpcEntity* npc;
    uint8       id;
};

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "time trigger self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectCall(const CallbackCall& actual, CNpcEntity* npc, uint8 id, const char* label) -> bool
{
    bool ok = true;
    ok      = expectEqual(actual.npc, npc, label) && ok;
    ok      = expectEqual(actual.id, id, label) && ok;
    return ok;
}

auto testAlignedInsertionBoundaryAndIdentity() -> bool
{
    auto& handler = *CTriggerHandler::getInstance();
    TimeTriggerTestAccess::reset(handler);

    NpcStorage npc;
    Trigger_t  trigger{ 7, npc.pointer(), vanaMinute(100), vanaMinute(50), 999 };
    TimeTriggerTestAccess::insert(handler, trigger, vanaMinute(349));

    std::vector<CallbackCall> calls;
    auto callback = [&](CNpcEntity* calledNpc, uint8 id)
    {
        calls.emplace_back(CallbackCall{ calledNpc, id });
    };

    bool ok = true;
    ok      = expectEqual(TimeTriggerTestAccess::lastTrigger(handler, 0), static_cast<uint32>(2), "aligned insertion bucket") && ok;

    TimeTriggerTestAccess::tick(handler, vanaMinute(349), callback);
    ok = expectEqual(calls.size(), static_cast<std::size_t>(0), "same bucket callback count") && ok;

    TimeTriggerTestAccess::tick(handler, vanaMinute(350), callback);
    ok = expectEqual(calls.size(), static_cast<std::size_t>(1), "offset boundary callback count") && ok;
    if (!calls.empty())
    {
        ok = expectCall(calls.front(), npc.pointer(), 7, "NPC identity and trigger ID") && ok;
    }
    return ok;
}

auto testMultiPeriodJumpAndLastBucketAdvance() -> bool
{
    auto& handler = *CTriggerHandler::getInstance();
    TimeTriggerTestAccess::reset(handler);
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 1, nullptr, vanaMinute(10), VanaDuration::zero(), 0 }, vanaMinute(15));

    std::size_t calls = 0;
    auto callback = [&](CNpcEntity*, uint8)
    {
        ++calls;
    };

    TimeTriggerTestAccess::tick(handler, vanaMinute(96), callback);
    bool ok = true;
    ok      = expectEqual(calls, static_cast<std::size_t>(1), "one callback after multi-period jump") && ok;
    ok      = expectEqual(TimeTriggerTestAccess::lastTrigger(handler, 0), static_cast<uint32>(9), "last bucket after jump") && ok;

    TimeTriggerTestAccess::tick(handler, vanaMinute(99), callback);
    ok = expectEqual(calls, static_cast<std::size_t>(1), "no repeat within advanced bucket") && ok;

    TimeTriggerTestAccess::tick(handler, vanaMinute(100), callback);
    ok = expectEqual(calls, static_cast<std::size_t>(2), "next boundary callback") && ok;
    return ok;
}

auto testIndependentPeriodsOffsetsAndOrder() -> bool
{
    auto& handler = *CTriggerHandler::getInstance();
    TimeTriggerTestAccess::reset(handler);

    NpcStorage npcA;
    NpcStorage npcB;
    NpcStorage npcC;
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 11, npcA.pointer(), vanaMinute(30), VanaDuration::zero(), 0 }, vanaMinute(59));
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 12, npcB.pointer(), vanaMinute(20), vanaMinute(5), 0 }, vanaMinute(59));
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 13, npcC.pointer(), vanaMinute(60), VanaDuration::zero(), 0 }, vanaMinute(59));

    std::vector<CallbackCall> calls;
    auto callback = [&](CNpcEntity* npc, uint8 id)
    {
        calls.emplace_back(CallbackCall{ npc, id });
    };

    TimeTriggerTestAccess::tick(handler, vanaMinute(60), callback);
    bool ok = true;
    ok      = expectEqual(calls.size(), static_cast<std::size_t>(2), "simultaneous due callback count") && ok;
    if (calls.size() >= 2)
    {
        ok = expectCall(calls[0], npcA.pointer(), 11, "first insertion-order callback") && ok;
        ok = expectCall(calls[1], npcC.pointer(), 13, "second insertion-order callback") && ok;
    }

    TimeTriggerTestAccess::tick(handler, vanaMinute(65), callback);
    ok = expectEqual(calls.size(), static_cast<std::size_t>(3), "independent offset callback count") && ok;
    if (calls.size() >= 3)
    {
        ok = expectCall(calls[2], npcB.pointer(), 12, "offset callback") && ok;
    }
    return ok;
}

auto testDuplicateRegistrationsRemainIndependent() -> bool
{
    auto& handler = *CTriggerHandler::getInstance();
    TimeTriggerTestAccess::reset(handler);

    NpcStorage npc;
    Trigger_t  trigger{ 19, npc.pointer(), vanaMinute(10), VanaDuration::zero(), 0 };
    TimeTriggerTestAccess::insert(handler, trigger, vanaMinute(5));
    TimeTriggerTestAccess::insert(handler, trigger, vanaMinute(15));

    std::vector<CallbackCall> calls;
    TimeTriggerTestAccess::tick(handler, vanaMinute(20), [&](CNpcEntity* calledNpc, uint8 id)
    {
        calls.emplace_back(CallbackCall{ calledNpc, id });
    });

    bool ok = true;
    ok      = expectEqual(calls.size(), static_cast<std::size_t>(2), "duplicate callback count") && ok;
    if (calls.size() >= 2)
    {
        ok = expectCall(calls[0], npc.pointer(), 19, "first duplicate callback") && ok;
        ok = expectCall(calls[1], npc.pointer(), 19, "second duplicate callback") && ok;
    }
    return ok;
}

auto testBackwardClockDoesNotRewindLastBucket() -> bool
{
    auto& handler = *CTriggerHandler::getInstance();
    TimeTriggerTestAccess::reset(handler);
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 0, nullptr, vanaMinute(10), VanaDuration::zero(), 0 }, vanaMinute(25));

    std::size_t calls = 0;
    auto callback = [&](CNpcEntity*, uint8)
    {
        ++calls;
    };

    TimeTriggerTestAccess::tick(handler, vanaMinute(30), callback);
    TimeTriggerTestAccess::tick(handler, vanaMinute(19), callback);
    TimeTriggerTestAccess::tick(handler, vanaMinute(30), callback);

    bool ok = true;
    ok      = expectEqual(calls, static_cast<std::size_t>(1), "rollback and restored bucket callback count") && ok;
    ok      = expectEqual(TimeTriggerTestAccess::lastTrigger(handler, 0), static_cast<uint32>(3), "last bucket after rollback") && ok;

    TimeTriggerTestAccess::tick(handler, vanaMinute(40), callback);
    ok = expectEqual(calls, static_cast<std::size_t>(2), "later bucket after rollback callback count") && ok;
    return ok;
}

auto testChronoDivisionAndUint32Conversion() -> bool
{
    auto& handler = *CTriggerHandler::getInstance();
    bool  ok      = true;

    TimeTriggerTestAccess::reset(handler);
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 0, nullptr, VanaDuration{ 10 }, VanaDuration::zero(), 0 }, VanaDuration{ -9 });
    ok = expectEqual(TimeTriggerTestAccess::lastTrigger(handler, 0), static_cast<uint32>(0), "negative fractional quotient truncates") && ok;

    TimeTriggerTestAccess::reset(handler);
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 0, nullptr, VanaDuration{ 10 }, VanaDuration::zero(), 0 }, VanaDuration{ -10 });
    ok = expectEqual(TimeTriggerTestAccess::lastTrigger(handler, 0), UINT32_MAX, "negative quotient wraps to uint32") && ok;

    TimeTriggerTestAccess::reset(handler);
    constexpr auto overflowBucket = (std::uint64_t{ 1 } << 32) + 3;
    TimeTriggerTestAccess::insert(handler, Trigger_t{ 0, nullptr, VanaDuration{ 1 }, VanaDuration::zero(), 0 }, VanaDuration{ static_cast<std::int64_t>(overflowBucket) });
    ok = expectEqual(TimeTriggerTestAccess::lastTrigger(handler, 0), static_cast<uint32>(3), "positive quotient wraps to uint32") && ok;
    return ok;
}

} // namespace

auto runTimeTriggerSelfTests() -> bool
{
    bool ok = true;
    ok      = testAlignedInsertionBoundaryAndIdentity() && ok;
    ok      = testMultiPeriodJumpAndLastBucketAdvance() && ok;
    ok      = testIndependentPeriodsOffsetsAndOrder() && ok;
    ok      = testDuplicateRegistrationsRemainIndependent() && ok;
    ok      = testBackwardClockDoesNotRewindLastBucket() && ok;
    ok      = testChronoDivisionAndUint32Conversion() && ok;
    TimeTriggerTestAccess::reset(*CTriggerHandler::getInstance());
    return ok;
}
