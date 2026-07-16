/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_world_engine_tasks.h"

#include "world/world_engine_tasks.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{

using namespace std::chrono_literals;

class RecordingToken final : public WorldEngineTaskToken
{
public:
    explicit RecordingToken(int& destroyed)
    : destroyed_(destroyed)
    {
    }

    ~RecordingToken() override
    {
        ++destroyed_;
    }

private:
    int& destroyed_;
};

class RecordingRegistrar final : public WorldEngineTaskRegistrar
{
public:
    auto registerTimeServer(std::chrono::steady_clock::duration interval, std::function<Task<void>()> callback) -> std::unique_ptr<WorldEngineTaskToken> override
    {
        registrations.emplace_back("time-server");
        intervals.emplace_back(interval);
        timeServerCallback = std::move(callback);
        return std::make_unique<RecordingToken>(destroyedTokens);
    }

    auto registerIncomingIPCPump(std::chrono::steady_clock::duration interval, std::function<void()> callback) -> std::unique_ptr<WorldEngineTaskToken> override
    {
        registrations.emplace_back("incoming-ipc-pump");
        intervals.emplace_back(interval);
        incomingIPCPumpCallback = std::move(callback);
        return std::make_unique<RecordingToken>(destroyedTokens);
    }

    std::vector<std::string>                           registrations;
    std::vector<std::chrono::steady_clock::duration> intervals;
    std::function<Task<void>()>                        timeServerCallback;
    std::function<void()>                              incomingIPCPumpCallback;
    int                                                destroyedTokens = 0;
};

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "World engine task self-test failed: " << label << '\n';
    }
    return condition;
}

auto completedTask() -> Task<void>
{
    co_return;
}

auto testRegistrationRoutesAndOwnership() -> bool
{
    RecordingRegistrar registrar;
    bool               timeServerCalled = false;
    bool               incomingIPCCalled = false;

    {
        auto tasks = registerWorldEngineRecurringTasks(
            registrar,
            {
                .timeServer = [&]() -> Task<void>
                {
                    timeServerCalled = true;
                    return completedTask();
                },
                .incomingIPCPump = [&]()
                {
                    incomingIPCCalled = true;
                },
            });

        bool ok = true;
        ok      = expect(registrar.registrations == std::vector<std::string>{ "time-server", "incoming-ipc-pump" }, "registration order") && ok;
        ok      = expect(registrar.intervals == std::vector<std::chrono::steady_clock::duration>{ 2400ms, 100ms }, "registration intervals") && ok;
        ok      = expect(static_cast<bool>(registrar.timeServerCallback), "time-server callback registered") && ok;
        ok      = expect(static_cast<bool>(registrar.incomingIPCPumpCallback), "IPC callback registered") && ok;

        static_cast<void>(registrar.timeServerCallback());
        registrar.incomingIPCPumpCallback();
        ok = expect(timeServerCalled, "time-server callback route") && ok;
        ok = expect(incomingIPCCalled, "IPC callback route") && ok;
        ok = expect(registrar.destroyedTokens == 0, "tokens retained by registration owner") && ok;
        if (!ok)
        {
            return false;
        }
    }

    return expect(registrar.destroyedTokens == 2, "tokens destroyed with registration owner");
}

} // namespace

auto runWorldEngineTaskSelfTests() -> bool
{
    return testRegistrationRoutesAndOwnership();
}
