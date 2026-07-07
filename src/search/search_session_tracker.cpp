/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "search_session_tracker.h"

auto GetSearchSessionsInUse(const std::map<std::string, uint16>& ipAddrsInUse, const std::unordered_set<std::string>& ipWhitelist, const std::string& ipAddressStr) -> uint16
{
    if (ipWhitelist.find(ipAddressStr) != ipWhitelist.end())
    {
        return 0;
    }

    if (ipAddrsInUse.find(ipAddressStr) != ipAddrsInUse.end())
    {
        return ipAddrsInUse.at(ipAddressStr);
    }

    return 0;
}

void AddSearchSession(std::map<std::string, uint16>& ipAddrsInUse, const std::unordered_set<std::string>& ipWhitelist, const std::string& ipAddressStr)
{
    if (ipWhitelist.find(ipAddressStr) != ipWhitelist.end())
    {
        return;
    }

    if (ipAddrsInUse.find(ipAddressStr) == ipAddrsInUse.end())
    {
        ipAddrsInUse[ipAddressStr] = 1;
    }
    else
    {
        ipAddrsInUse[ipAddressStr] += 1;
    }
}

void RemoveSearchSession(std::map<std::string, uint16>& ipAddrsInUse, const std::unordered_set<std::string>& ipWhitelist, const std::string& ipAddressStr)
{
    if (ipWhitelist.find(ipAddressStr) != ipWhitelist.end())
    {
        return;
    }

    if (ipAddrsInUse.find(ipAddressStr) != ipAddrsInUse.end())
    {
        ipAddrsInUse[ipAddressStr] -= 1;
    }
    else // Removing nothing, do nothing.
    {
        return;
    }

    // If we got here, check if we want to remove an IP from the map
    if (ipAddrsInUse[ipAddressStr] <= 0)
    {
        ipAddrsInUse.erase(ipAddressStr);
    }
}
