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

#include "test_mariadb_connection_loss.h"

#include <iostream>
#include <string>
#include <vector>

namespace db::detail::libmariadb
{
auto isConnectionLost(unsigned int errnum) -> bool;
} // namespace db::detail::libmariadb

namespace
{

struct Case
{
    unsigned int errnum;
    bool         expected;
    std::string  label;
};

auto expectEqual(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "MariaDB connection loss self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runMariaDBConnectionLossSelfTests() -> bool
{
    bool ok = true;

    const std::vector<Case> cases{
        { 2002, true, "CR_CONNECTION_ERROR" },
        { 2003, true, "CR_CONN_HOST_ERROR" },
        { 2006, true, "CR_SERVER_GONE_ERROR" },
        { 2013, true, "CR_SERVER_LOST" },
        { 2055, true, "CR_SERVER_LOST_EXTENDED" },
        { 0, false, "zero" },
        { 1045, false, "access denied" },
        { 2001, false, "neighbor before 2002" },
        { 2004, false, "neighbor after 2003" },
        { 2005, false, "neighbor before 2006" },
        { 2007, false, "neighbor after 2006" },
        { 2012, false, "neighbor before 2013" },
        { 2014, false, "neighbor after 2013" },
        { 2054, false, "neighbor before 2055" },
        { 2056, false, "neighbor after 2055" },
    };

    for (const auto& test : cases)
    {
        ok = expectEqual(
                 db::detail::libmariadb::isConnectionLost(test.errnum),
                 test.expected,
                 test.label) &&
             ok;
    }

    return ok;
}
