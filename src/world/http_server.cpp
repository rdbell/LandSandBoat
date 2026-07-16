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

#include "http_server.h"

#include "common/database.h"
#include "common/logging.h"
#include "common/settings.h"
HTTPServer::HTTPServer(Scheduler& scheduler)
: scheduler_(scheduler)
, apiDataCache_(HTTPServerAPIDataCache{})
{
    // NOTE: Everything registered in here happens off the main thread, so lock any global resources
    //     : you might be using.

    LockingUpdate();

    auto host = settings::get<std::string>("network.HTTP_HOST");
    auto port = settings::get<uint16>("network.HTTP_PORT");

    ShowInfoFmt("Starting HTTP Server on http://{}:{}/api", host, port);

    scheduler_.postToWorkerThread(
        [this, host, port]()
        {
            httpServer_.Get(
                R"(/api.*)",
                [&](const httplib::Request& req, httplib::Response& res)
                {
                    const auto response = APIResponse(req.path);
                    res.status          = response.status;
                    if (!response.contentType.empty())
                    {
                        res.set_content(response.body, response.contentType);
                    }
                });

            httpServer_.set_error_handler(
                [](const httplib::Request& /*req*/, httplib::Response& res)
                {
                    auto str = fmt::format("<p>Error Status: <span style='color:red;'>{} ({})</span></p>",
                                           res.status,
                                           httplib::status_message(res.status));

                    for (const auto& [key, val] : res.headers)
                    {
                        str += fmt::format("<p>{}: {}</p>", key, val);
                    }

                    res.set_content(str, "text/html");
                });

            httpServer_.set_logger(
                [](const httplib::Request& req, const httplib::Response& res)
                {
                    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
                    if (res.status >= 500)
                    {
                        ShowErrorFmt("Server Error: {} ({})", res.status, httplib::status_message(res.status));
                        return;
                    }
                    else if (res.status >= 400)
                    {
                        ShowErrorFmt("Client Error: {} ({})", res.status, httplib::status_message(res.status));
                        return;
                    }
                });

            httpServer_.listen(host, port); // blocks
        });
}

auto HTTPServer::APIResponse(const std::string& path) -> HTTPServerAPIResponse
{
    HTTPServerAPIDataCache cache;
    if (path != "/api" && path != "/api/settings")
    {
        LockingUpdate();
        apiDataCache_.read([&](const auto& apiDataCache) { cache = apiDataCache; });
    }

    HTTPServerAPISettings apiSettings;
    if (path == "/api/settings")
    {
        settings::visit([&](const auto& key, const auto& variant) { apiSettings.emplace(key, variant); });
    }

    return makeHTTPServerAPIResponse(path, cache, apiSettings);
}

HTTPServer::~HTTPServer()
{
    httpServer_.stop();
}

void HTTPServer::LockingUpdate()
{
    auto now = timer::now();
    if (now < (lastUpdate_.load() + 60s))
    {
        return;
    }

    apiDataCache_.write(
        [&](auto& apiDataCache)
        {
            ShowInfoFmt("API data is stale. Updating...");

            // Total active sessions
            {
                auto rset = db::preparedStmt("SELECT COUNT(*) AS `count` FROM accounts_sessions");
                if (rset && rset->next())
                {
                    apiDataCache.activeSessionCount = rset->get<uint32>("count");
                }
            }

            // Total active unique IPs
            {
                auto rset = db::preparedStmt("SELECT COUNT(DISTINCT client_addr) AS `count` FROM accounts_sessions");
                if (rset && rset->next())
                {
                    apiDataCache.activeUniqueIPCount = rset->get<uint32>("count");
                }
            }

            // Chars per zone
            {
                auto rset = db::preparedStmt("SELECT chars.pos_zone, COUNT(*) AS `count` "
                                             "FROM chars "
                                             "INNER JOIN accounts_sessions "
                                             "ON chars.charid = accounts_sessions.charid "
                                             "GROUP BY pos_zone");
                if (rset && rset->rowsCount())
                {
                    while (rset->next())
                    {
                        auto zoneId = rset->get<uint16>("pos_zone");
                        auto count  = rset->get<uint32>("count");

                        apiDataCache.zonePlayerCounts[zoneId] = count;
                    }
                }
            }

            lastUpdate_.store(now);
        });
}
