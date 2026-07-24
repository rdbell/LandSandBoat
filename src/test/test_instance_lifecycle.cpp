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

    static auto overlayID(uint32 overlayId) -> uint32
    {
        return CInstance::publicOverlayID(overlayId);
    }

    struct LoadedSettings
    {
        std::string         instanceName;
        timer::duration     timeLimit;
        uint16              entrance;
        uint32              overlayId;
        position_t          entryLoc;
        zoneMusicOverride_t musicOverrides;
    };

    static auto applySettings(const std::string& instanceName, uint32 timeLimitMinutes, uint16 entrance, uint32 overlayId, position_t entryLoc, zoneMusicOverride_t musicOverrides) -> LoadedSettings
    {
        instanceLoadSettings_t settings;
        settings.instanceName     = instanceName;
        settings.timeLimitMinutes = timeLimitMinutes;
        settings.entrance         = entrance;
        settings.overlayId        = overlayId;
        settings.entryLoc         = entryLoc;
        settings.musicOverrides   = musicOverrides;

        LoadedSettings loaded;
        CInstance::applyLoadSettings(loaded.instanceName, loaded.timeLimit, loaded.entrance, loaded.overlayId, loaded.entryLoc, loaded.musicOverrides, settings);
        return loaded;
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
    ok = expect(!zoneinstance::shouldCleanupInstance(true, true, false), "occupied terminal instance stays") && ok;
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

auto testOverlayID() -> bool
{
    bool ok = true;
    ok = expect(InstanceTestAccess::overlayID(0) == 0, "default overlay id is zero") && ok;
    ok = expect(InstanceTestAccess::overlayID(0xFEDCBA98) == 0xFEDCBA98, "overlay id preserves full uint32") && ok;
    return ok;
}

auto testLoadSettings() -> bool
{
    position_t entryLoc{};
    entryLoc.x        = 1.5f;
    entryLoc.y        = -2.5f;
    entryLoc.z        = 3.25f;
    entryLoc.rotation = 127;
    zoneMusicOverride_t musicOverrides;
    musicOverrides.m_songDay = 101;
    musicOverrides.m_bSongS  = 0;

    const auto loaded = InstanceTestAccess::applySettings("A Crystalline Prophecy", 45, 55, 0xFEDCBA98, entryLoc, musicOverrides);
    bool       ok     = true;
    ok                = expect(loaded.instanceName == "A Crystalline Prophecy", "load settings keeps instance name") && ok;
    ok                = expect(loaded.timeLimit == 45min, "load settings converts minutes") && ok;
    ok                = expect(loaded.entrance == 55, "load settings keeps entrance") && ok;
    ok                = expect(loaded.overlayId == 0xFEDCBA98, "load settings keeps overlay id") && ok;
    ok                = expect(loaded.entryLoc.x == 1.5f && loaded.entryLoc.y == -2.5f && loaded.entryLoc.z == 3.25f && loaded.entryLoc.rotation == 127,
                               "load settings keeps entry location") &&
                        ok;
    ok                = expect(loaded.musicOverrides.m_songDay && *loaded.musicOverrides.m_songDay == 101, "load settings keeps day override") && ok;
    ok                = expect(!loaded.musicOverrides.m_songNight, "load settings keeps null night override") && ok;
    ok                = expect(loaded.musicOverrides.m_bSongS && *loaded.musicOverrides.m_bSongS == 0, "load settings keeps explicit zero solo override") && ok;
    ok                = expect(!loaded.musicOverrides.m_bSongM, "load settings keeps null party override") && ok;
    return ok;
}

auto testInstanceEraseIdentity() -> bool
{
    auto* const first  = reinterpret_cast<CInstance*>(uintptr_t{ 1 });
    auto* const second = reinterpret_cast<CInstance*>(uintptr_t{ 2 });
    bool ok = true;
    ok = expect(zoneinstance::shouldEraseInstance(first, first), "same instance identity erases") && ok;
    ok = expect(!zoneinstance::shouldEraseInstance(first, second), "different instance identity stays") && ok;
    ok = expect(zoneinstance::shouldEraseInstance(nullptr, nullptr), "null identities compare equal") && ok;
    return ok;
}

auto testInstanceLookupStop() -> bool
{
    bool ok = true;
    ok = expect(!zoneinstance::shouldStopInstanceSearch(false), "missing entity continues instance search") && ok;
    ok = expect(zoneinstance::shouldStopInstanceSearch(true), "found entity stops instance search") && ok;
    return ok;
}

auto testInstanceOperationRouting() -> bool
{
    bool ok = true;
    ok = expect(!zoneinstance::shouldBroadcastInstanceOperation(true), "entity operation targets one instance") && ok;
    ok = expect(zoneinstance::shouldBroadcastInstanceOperation(false), "unbound operation broadcasts to instances") && ok;
    return ok;
}

auto testInstanceOperationForwarding() -> bool
{
    bool ok = true;
    ok = expect(zoneinstance::shouldForwardInstanceOperation(true), "assigned entity forwards to instance") && ok;
    ok = expect(!zoneinstance::shouldForwardInstanceOperation(false), "unassigned entity skips instance") && ok;
    return ok;
}

auto testOwnedInstanceBroadcast() -> bool
{
    std::vector<int> visited;
    const std::vector<int> instances{ 3, 7, 11 };
    zoneinstance::forEachOwnedInstance(instances, [&](int id)
    {
        visited.push_back(id);
    });

    bool ok = true;
    ok = expect(visited == instances, "owned instance broadcast preserves list order") && ok;
    zoneinstance::forEachOwnedInstance(std::vector<int>{}, [&](int)
    {
        ok = false;
    });
    ok = expect(ok, "empty owned instance list does not visit") && ok;
    return ok;
}

auto testFirstInstanceEntryDelay() -> bool
{
    return expect(zoneinstance::afterInstanceRegisterDelay() == 400ms, "first instance entry queues after-register at 400ms");
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
    ok      = testOverlayID() && ok;
    ok      = testLoadSettings() && ok;
    ok      = testInstanceEraseIdentity() && ok;
    ok      = testInstanceLookupStop() && ok;
    ok      = testInstanceOperationRouting() && ok;
    ok      = testInstanceOperationForwarding() && ok;
    ok      = testOwnedInstanceBroadcast() && ok;
    ok      = testFirstInstanceEntryDelay() && ok;
    return ok;
}
