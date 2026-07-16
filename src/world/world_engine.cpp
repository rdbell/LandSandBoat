/*
===========================================================================

  Copyright (c) 2022 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "world_engine.h"

#include <common/application.h>
#include <common/logging.h>

#include <map/map_constants.h>

#include "besieged_system.h"
#include "campaign_system.h"
#include "colonization_system.h"
#include "conquest_system.h"
#include "http_server.h"
#include "ipc_server.h"
#include "party_system.h"
#include "time_server.h"

namespace
{

class SchedulerTaskToken final : public WorldEngineTaskToken
{
public:
    explicit SchedulerTaskToken(Scheduler::Token token)
    : token_(std::move(token))
    {
    }

private:
    Scheduler::Token token_;
};

class SchedulerWorldEngineTaskRegistrar final : public WorldEngineTaskRegistrar
{
public:
    explicit SchedulerWorldEngineTaskRegistrar(Scheduler& scheduler)
    : scheduler_(scheduler)
    {
    }

    auto registerTimeServer(std::chrono::steady_clock::duration interval, std::function<Task<void>()> callback) -> std::unique_ptr<WorldEngineTaskToken> override
    {
        return std::make_unique<SchedulerTaskToken>(scheduler_.intervalOnMainThread(interval, std::move(callback)));
    }

    auto registerIncomingIPCPump(std::chrono::steady_clock::duration interval, std::function<void()> callback) -> std::unique_ptr<WorldEngineTaskToken> override
    {
        return std::make_unique<SchedulerTaskToken>(scheduler_.intervalOnMainThread(interval, std::move(callback)));
    }

private:
    Scheduler& scheduler_;
};

} // namespace

WorldEngine::WorldEngine(Scheduler& scheduler, ZMQService& zmqService, EnableHTTPServer enableHTTPServer)
: scheduler_(scheduler)
, ipcServer_(std::make_unique<IPCServer>(*this, zmqService))
, partySystem_(std::make_unique<PartySystem>(*this))
, conquestSystem_(std::make_unique<ConquestSystem>(*this))
, besiegedSystem_(std::make_unique<BesiegedSystem>(*this))
, campaignSystem_(std::make_unique<CampaignSystem>(*this))
, colonizationSystem_(std::make_unique<ColonizationSystem>(*this))
, httpServer_(enableHTTPServer ? std::make_unique<HTTPServer>(scheduler_) : nullptr)
{
    SchedulerWorldEngineTaskRegistrar registrar(scheduler_);
    recurringTasks_ = registerWorldEngineRecurringTasks(
        registrar,
        {
            .timeServer = [this]() -> Task<void>
            {
                co_await time_server(this);
            },
            .incomingIPCPump = [this]()
            {
                ipcServer_->handleIncomingMessages();
            },
        });
}

WorldEngine::~WorldEngine() = default;
