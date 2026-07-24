/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_instance_lifecycle.h"

#include "map/instance.h"
#include "map/zone_instance.h"

#include <chrono>
#include <iostream>
#include <set>
#include <vector>

using namespace std::chrono_literals;

class InstanceTestAccess
{
public:
    static auto elapsed(timer::time_point start, timer::time_point tick) -> timer::duration
    {
        return CInstance::elapsedTime(start, tick);
    }

    static auto due(timer::time_point start, timer::time_point last, timer::time_point tick, bool charsEmpty, bool failed) -> bool
    {
        return CInstance::timeCheckDue(start, last, tick, charsEmpty, failed);
    }

    static void registerChar(std::vector<uint32>& chars, uint32& commander, uint32 id)
    {
        CInstance::registerChar(chars, commander, id);
    }

    static auto firstEntry(std::set<uint32>& entered, uint32 id) -> bool
    {
        return CInstance::checkFirstEntry(entered, id);
    }

    static auto music(const Maybe<uint16>& overrideMusic, uint16 zoneMusic) -> uint16
    {
        return CInstance::musicOrDefault(overrideMusic, zoneMusic);
    }

    static auto shouldClearState(bool alive) -> bool
    {
        return CInstance::shouldClearEntityState(alive);
    }

    static auto entryRotation(float rot) -> uint8
    {
        return CInstance::entryRotation(rot);
    }

    static auto cancelStatus() -> INSTANCE_STATUS
    {
        return CInstance::cancelStatus();
    }

    static auto publicID(uint32 instanceid) -> uint16
    {
        return CInstance::publicInstanceID(instanceid);
    }
};

namespace
{
auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "instance lifecycle self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testElapsedAndTimerCadence() -> bool
{
    const auto start = timer::time_point{ 100s };
    bool       ok    = true;

    ok = expect(InstanceTestAccess::elapsed(start, start - 1ms) == 0s, "elapsed clamps clock rollback") && ok;
    ok = expect(InstanceTestAccess::elapsed(start, start) == 0s, "elapsed at start") && ok;
    ok = expect(InstanceTestAccess::elapsed(start, start + 1500ms) == 1500ms, "positive elapsed") && ok;

    ok = expect(!InstanceTestAccess::due(start, start, start + 39999ms, true, false), "empty initial grace before boundary") && ok;
    ok = expect(InstanceTestAccess::due(start, start, start + 40s, true, false), "empty initial grace inclusive boundary") && ok;
    ok = expect(!InstanceTestAccess::due(start, start, start + 999ms, false, false), "occupied cadence before boundary") && ok;
    ok = expect(InstanceTestAccess::due(start, start, start + 1s, false, false), "occupied cadence inclusive boundary") && ok;
    ok = expect(InstanceTestAccess::due(start, start + 40s, start + 41s, true, false), "grace is not restored after first check") && ok;
    ok = expect(!InstanceTestAccess::due(start, start, start + 1h, false, true), "failed instance never dispatches") && ok;
    return ok;
}

auto testParticipantIdentity() -> bool
{
    std::vector<uint32> chars;
    uint32              commander = 99;
    InstanceTestAccess::registerChar(chars, commander, 10);
    InstanceTestAccess::registerChar(chars, commander, 20);
    InstanceTestAccess::registerChar(chars, commander, 10);

    bool ok = true;
    ok = expect(commander == 10, "first registrant remains commander") && ok;
    ok = expect(chars == std::vector<uint32>{ 10, 20, 10 }, "registration preserves duplicates and order") && ok;

    std::set<uint32> entered;
    ok = expect(InstanceTestAccess::firstEntry(entered, 0), "zero id first entry") && ok;
    ok = expect(!InstanceTestAccess::firstEntry(entered, 0), "zero id repeated entry") && ok;
    ok = expect(InstanceTestAccess::firstEntry(entered, UINT32_MAX), "maximum id first entry") && ok;
    ok = expect(!InstanceTestAccess::firstEntry(entered, UINT32_MAX), "maximum id repeated entry") && ok;
    return ok;
}

auto testMusicOverrides() -> bool
{
    bool ok = true;
    ok = expect(InstanceTestAccess::music(std::nullopt, 123) == 123, "music falls back to zone value") && ok;
    ok = expect(InstanceTestAccess::music(Maybe<uint16>(456), 123) == 456, "music override wins") && ok;
    ok = expect(InstanceTestAccess::music(Maybe<uint16>(0), 123) == 0, "zero music override is preserved") && ok;
    return ok;
}

auto testClearEntityState() -> bool
{
    bool ok = true;
    ok = expect(InstanceTestAccess::shouldClearState(true), "alive entity clears state") && ok;
    ok = expect(!InstanceTestAccess::shouldClearState(false), "dead entity retains state") && ok;
    return ok;
}

auto testEntryRotation() -> bool
{
    bool ok = true;
    ok = expect(InstanceTestAccess::entryRotation(127.75f) == 127, "entry rotation truncates fractional value") && ok;
    ok = expect(InstanceTestAccess::entryRotation(128.0f) == 128, "entry rotation preserves integer value") && ok;
    return ok;
}

auto testCancelStatus() -> bool
{
    return expect(InstanceTestAccess::cancelStatus() == INSTANCE_FAILED, "cancel sets failed status");
}

auto testInstanceCleanup() -> bool
{
    bool ok = true;
    ok = expect(zoneinstance::shouldCleanupInstance(true, false, true), "failed empty instance cleans up") && ok;
    ok = expect(zoneinstance::shouldCleanupInstance(false, true, true), "complete empty instance cleans up") && ok;
    ok = expect(!zoneinstance::shouldCleanupInstance(false, false, true), "normal empty instance stays") && ok;
    ok = expect(!zoneinstance::shouldCleanupInstance(true, false, false), "occupied failed instance stays") && ok;
    return ok;
}

auto testInstanceEntityLookup() -> bool
{
    bool ok = true;
    ok = expect(!zoneinstance::shouldSearchInstancePlayers(0), "empty entity filter skips instances") && ok;
    ok = expect(zoneinstance::shouldSearchInstancePlayers(TYPE_PC), "player filter searches instances") && ok;
    ok = expect(!zoneinstance::shouldSearchInstancePlayers(TYPE_MOB), "mob filter skips instances") && ok;
    ok = expect(zoneinstance::shouldSearchInstancePlayers(TYPE_PC | TYPE_MOB), "combined player filter searches instances") && ok;
    return ok;
}

auto testInstanceWipeExit() -> bool
{
    bool ok = true;
    ok = expect(zoneinstance::shouldSetInstanceWipeTimeOnExit(true, false, false), "empty normal instance records wipe") && ok;
    ok = expect(!zoneinstance::shouldSetInstanceWipeTimeOnExit(false, false, false), "occupied instance skips wipe") && ok;
    ok = expect(!zoneinstance::shouldSetInstanceWipeTimeOnExit(true, true, false), "failed instance skips wipe") && ok;
    ok = expect(!zoneinstance::shouldSetInstanceWipeTimeOnExit(true, false, true), "complete instance skips wipe") && ok;
    return ok;
}

auto testInstanceRestore() -> bool
{
    bool ok = true;
    ok = expect(zoneinstance::shouldRestoreInstance(true), "registered character selects candidate") && ok;
    ok = expect(!zoneinstance::shouldRestoreInstance(false), "unregistered character skips candidate") && ok;
    return ok;
}

auto testPublicInstanceID() -> bool
{
    bool ok = true;
    ok = expect(InstanceTestAccess::publicID(7) == 7, "small instance id is preserved") && ok;
    ok = expect(InstanceTestAccess::publicID(65535) == 65535, "maximum public instance id is preserved") && ok;
    ok = expect(InstanceTestAccess::publicID(65536) == 0, "instance id narrows to uint16") && ok;
    return ok;
}
} // namespace

auto runInstanceLifecycleSelfTests() -> bool
{
    bool ok = true;
    ok      = testElapsedAndTimerCadence() && ok;
    ok      = testParticipantIdentity() && ok;
    ok      = testMusicOverrides() && ok;
    ok      = testClearEntityState() && ok;
    ok      = testEntryRotation() && ok;
    ok      = testCancelStatus() && ok;
    ok      = testInstanceCleanup() && ok;
    ok      = testInstanceEntityLookup() && ok;
    ok      = testInstanceWipeExit() && ok;
    ok      = testInstanceRestore() && ok;
    ok      = testPublicInstanceID() && ok;
    return ok;
}
