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

#include "connect_application.h"

#include "cert_helpers.h"
#include "common/console_service.h"
#include "common/timer.h"
#include "connect_cleanup.h"
#include "connect_engine.h"

namespace
{

auto appConfig() -> ApplicationConfig
{
    return ApplicationConfig{
        .serverName = loginHelpers::ConnectServerName,
        .arguments  = {},
    };
}

} // namespace

ConnectApplication::ConnectApplication(const int argc, char** argv)
: Application(appConfig(), argc, argv)
{
}

ConnectApplication::~ConnectApplication() = default;

auto ConnectApplication::createEngine() -> std::unique_ptr<Engine>
{
    certificateHelpers::generateSelfSignedCert();
    return std::make_unique<ConnectEngine>(scheduler_, zmqService_);
}

void ConnectApplication::registerCommands(ConsoleService& console)
{
    console.registerCommand(
        loginHelpers::ConnectStatsCommandName,
        loginHelpers::ConnectStatsCommandHelp,
        [](std::vector<std::string>& inputs)
        {
            ShowInfo("%s", loginHelpers::FormatConnectStats(
                               loginHelpers::authenticated_ip_count(),
                               loginHelpers::authenticated_session_count()));
        });

    console.registerCommand(
        loginHelpers::ConnectClearCommandName,
        loginHelpers::ConnectClearCommandHelp,
        [](std::vector<std::string>& inputs)
        {
            auto& sessions       = loginHelpers::getAuthenticatedSessions();
            auto  ipAddrIterator = sessions.begin();
            while (ipAddrIterator != sessions.end())
            {
                auto sessionIterator = ipAddrIterator->second.begin();
                while (sessionIterator != ipAddrIterator->second.end())
                {
                    session_t& session = sessionIterator->second;

                    // Console "clear" intentionally uses now > authorizedTime
                    // without the 15-minute TTL used by ConnectEngine::periodicCleanup.
                    if (loginHelpers::ShouldEraseOnClearCommand(
                            session.data_session != nullptr,
                            session.view_session != nullptr,
                            loginHelpers::IsAuthorizedTimePassed(timer::now(), session.authorizedTime)))
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
        });
}
