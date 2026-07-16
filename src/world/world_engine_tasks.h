/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <common/scheduler.h>

#include <chrono>
#include <functional>
#include <memory>

// WorldEngineTaskToken owns one recurring task cancellation token. Destroying
// the token stops its task, matching Scheduler::Token ownership semantics.
class WorldEngineTaskToken
{
public:
    virtual ~WorldEngineTaskToken() = default;
};

// WorldEngineTaskRegistrar keeps WorldEngine's two recurring task
// registrations independently testable from Scheduler, ZMQ, and IPC hosts.
class WorldEngineTaskRegistrar
{
public:
    virtual ~WorldEngineTaskRegistrar() = default;

    virtual auto registerTimeServer(std::chrono::steady_clock::duration interval, std::function<Task<void>()> callback) -> std::unique_ptr<WorldEngineTaskToken> = 0;
    virtual auto registerIncomingIPCPump(std::chrono::steady_clock::duration interval, std::function<void()> callback) -> std::unique_ptr<WorldEngineTaskToken> = 0;
};

struct WorldEngineTaskCallbacks
{
    std::function<Task<void>()> timeServer;
    std::function<void()>       incomingIPCPump;
};

// WorldEngineRecurringTasks owns both registered task tokens. Its destruction
// cancels both recurring tasks.
class WorldEngineRecurringTasks final
{
public:
    WorldEngineRecurringTasks(std::unique_ptr<WorldEngineTaskToken> timeServerToken, std::unique_ptr<WorldEngineTaskToken> incomingIPCPumpToken);
    ~WorldEngineRecurringTasks() = default;

    WorldEngineRecurringTasks(const WorldEngineRecurringTasks&)            = delete;
    WorldEngineRecurringTasks& operator=(const WorldEngineRecurringTasks&) = delete;
    WorldEngineRecurringTasks(WorldEngineRecurringTasks&&) noexcept        = default;
    WorldEngineRecurringTasks& operator=(WorldEngineRecurringTasks&&) noexcept = default;

private:
    std::unique_ptr<WorldEngineTaskToken> timeServerToken_;
    std::unique_ptr<WorldEngineTaskToken> incomingIPCPumpToken_;
};

// registerWorldEngineRecurringTasks preserves WorldEngine's registration
// order: time server first (2400ms), then incoming IPC pump (100ms).
[[nodiscard]] auto registerWorldEngineRecurringTasks(WorldEngineTaskRegistrar& registrar, WorldEngineTaskCallbacks callbacks) -> WorldEngineRecurringTasks;
