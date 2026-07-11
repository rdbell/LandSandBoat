/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#include "connect_engine.h"

#include "connect_cleanup.h"

#include "common/timer.h"

namespace
{

auto getZMQEndpointString() -> std::string
{
    return loginHelpers::FormatZMQEndpointString(
        settings::get<std::string>("network.ZMQ_TRANSPORT"),
        settings::get<std::string>("network.ZMQ_IP"),
        settings::get<uint16>("network.ZMQ_PORT"));
}

auto getZMQRoutingId() -> uint64
{
    // We will only ever have a single login server, so we can use different logic for the routing id

    const auto ip   = str2ip(settings::get<std::string>("network.LOGIN_AUTH_IP"));
    const auto port = settings::get<uint16>("network.LOGIN_AUTH_PORT");

    return loginHelpers::ConnectDealerRoutingID(ip, port);
}

} // namespace

ConnectEngine::ConnectEngine(Scheduler& scheduler, ZMQService& zmqService)
: scheduler_(scheduler)
, dealerChannel_(zmqService.registerDealer(getZMQEndpointString(), getZMQRoutingId()))
, m_authHandler(scheduler_, settings::get<uint32>("network.LOGIN_AUTH_PORT"), dealerChannel_)
, m_dataHandler(scheduler_, settings::get<uint32>("network.LOGIN_DATA_PORT"), dealerChannel_)
, m_viewHandler(scheduler_, settings::get<uint32>("network.LOGIN_VIEW_PORT"), dealerChannel_)
{
    periodicCleanupToken_ = scheduler.intervalOnMainThread(
        loginHelpers::SessionCleanInterval,
        [this]()
        {
            periodicCleanup();
        });
}

ConnectEngine::~ConnectEngine()
{
}

void ConnectEngine::periodicCleanup()
{
    auto& sessions       = loginHelpers::getAuthenticatedSessions();
    auto  ipAddrIterator = sessions.begin();
    while (ipAddrIterator != sessions.end())
    {
        auto sessionIterator = ipAddrIterator->second.begin();
        while (sessionIterator != ipAddrIterator->second.end())
        {
            session_t& session = sessionIterator->second;

            // If it's been 15 minutes, erase it from the session list
            if (loginHelpers::ShouldEraseIdleSession(
                    session.data_session != nullptr,
                    session.view_session != nullptr,
                    loginHelpers::IsSessionExpired(timer::now(), session.authorizedTime)))
            {
                sessionIterator = ipAddrIterator->second.erase(sessionIterator);
            }
            else
            {
                ++sessionIterator;
            }
        }

        // If this map entry is empty, clean it up
        if (loginHelpers::ShouldEraseEmptyIPEntry(ipAddrIterator->second.size() == 0))
        {
            ipAddrIterator = sessions.erase(ipAddrIterator);
        }
        else
        {
            ++ipAddrIterator;
        }
    }
}
