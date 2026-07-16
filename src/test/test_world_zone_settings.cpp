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

#include "test_world_zone_settings.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "map/zone.h"
#include "world/zone_settings.h"

#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>&) -> std::unique_ptr<db::ResultSet> override
    {
        queries.push_back(query);
        if (results.empty())
        {
            return nullptr;
        }

        auto result = std::move(results.front());
        results.erase(results.begin());
        return result;
    }

    auto getSchema() -> std::string override
    {
        return "xidb";
    }

    auto getVersion() -> std::string override
    {
        return "MariaDB 10.6";
    }

    auto getDriverVersion() -> std::string override
    {
        return "MariaDB Connector/C";
    }

    std::vector<std::string>                    queries;
    std::vector<std::unique_ptr<db::ResultSet>> results;
};

struct DatabaseResetGuard
{
    ~DatabaseResetGuard()
    {
        db::setDatabase(nullptr);
    }
};

auto zoneSettingsResult(const std::string& query, std::vector<db::LibMariaDBResultSet::Row> rows) -> std::unique_ptr<db::ResultSet>
{
    auto schema = std::make_shared<db::ColumnSchema>();
    schema->names.push_back("zoneid");
    schema->names.push_back("zoneip");
    schema->names.push_back("zoneport");
    schema->names.push_back("misc");
    schema->index["zoneid"]   = 0;
    schema->index["zoneip"]   = 1;
    schema->index["zoneport"] = 2;
    schema->index["misc"]     = 3;

    return std::make_unique<db::LibMariaDBResultSet>(query, std::move(schema), std::move(rows));
}

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "world zone settings self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectIPP(const IPP& actual, const std::string& ip, const uint16 port, const std::string& label) -> bool
{
    bool ok = true;

    ok = expectEqual(actual.getIPString(), ip, label + " ip") && ok;
    ok = expectEqual(actual.getPort(), port, label + " port") && ok;

    return ok;
}

auto expectEndpoints(const std::vector<IPP>& actual, const std::vector<IPP>& expected, const std::string& label) -> bool
{
    bool ok = true;

    ok = expectEqual(actual.size(), expected.size(), label + " size") && ok;
    if (!ok)
    {
        return false;
    }

    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        ok = expectEqual(actual[i].getRawIPP(), expected[i].getRawIPP(), label + " endpoint " + std::to_string(i)) && ok;
    }

    return ok;
}

auto testZoneSettingsIndexesRowsFromDatabase() -> bool
{
    bool ok = true;

    const auto query = std::string("SELECT zoneid, zoneip, zoneport, misc FROM zone_settings");

    FakeDatabase fake;
    fake.results.emplace_back(zoneSettingsResult(
        query,
        {
            { uint64{ 230 }, std::string("127.0.0.1"), uint64{ 54230 }, uint64{ static_cast<uint16>(MISC_YELL) | static_cast<uint16>(MISC_ASSIST) } },
            { uint64{ 231 }, std::string("127.0.0.1"), uint64{ 54230 }, uint64{ static_cast<uint16>(MISC_YELL) } },
            { uint64{ 232 }, std::string("10.0.0.5"), uint64{ 54000 }, uint64{ static_cast<uint16>(MISC_ASSIST) } },
            { uint64{ 286 }, std::string("127.0.0.1"), uint64{ 0 }, uint64{ static_cast<uint16>(MISC_NONE) } },
            { uint64{ 230 }, std::string("127.0.0.1"), uint64{ 54231 }, uint64{ static_cast<uint16>(MISC_ASSIST) } },
        }));

    db::setDatabase(&fake);
    DatabaseResetGuard guard;

    const ZoneSettings settings;

    ok = expectEqual(fake.queries.size(), std::size_t{ 1 }, "query count") && ok;
    if (!fake.queries.empty())
    {
        ok = expectEqual(fake.queries.front(), query, "query text") && ok;
    }

    ok = expectEqual(settings.zoneSettingsMap_.size(), std::size_t{ 4 }, "zone settings map size") && ok;
    ok = expectEndpoints(
             settings.mapEndpoints_,
             { IPP(str2ip("127.0.0.1"), 0), IPP(str2ip("127.0.0.1"), 54230), IPP(str2ip("127.0.0.1"), 54231), IPP(str2ip("10.0.0.5"), 54000) },
             "map endpoints") &&
         ok;
    ok = expectEndpoints(settings.yellMapEndpoints_, { IPP(str2ip("127.0.0.1"), 54230) }, "yell endpoints") && ok;
    ok = expectEndpoints(
             settings.assistMapEndpoints_,
             { IPP(str2ip("127.0.0.1"), 54230), IPP(str2ip("127.0.0.1"), 54231), IPP(str2ip("10.0.0.5"), 54000) },
             "assist endpoints") &&
         ok;

    if (const auto it = settings.zoneSettingsMap_.find(230); it != settings.zoneSettingsMap_.end())
    {
        ok = expectEqual(it->second.zoneid, uint16{ 230 }, "Southern San d'Oria zone id") && ok;
        ok = expectIPP(it->second.ipp, "127.0.0.1", 54231, "duplicate zone endpoint") && ok;
        ok = expectEqual(it->second.misc, uint32{ static_cast<uint16>(MISC_ASSIST) }, "duplicate zone misc") && ok;
    }
    else
    {
        std::cerr << "world zone settings self-test failed: missing duplicate zone 230\n";
        ok = false;
    }

    if (const auto it = settings.zoneSettingsMap_.find(286); it != settings.zoneSettingsMap_.end())
    {
        ok = expectEqual(it->second.zoneid, uint16{ 286 }, "disabled zone id") && ok;
        ok = expectIPP(it->second.ipp, "127.0.0.1", 0, "disabled zone endpoint") && ok;
        ok = expectEqual(it->second.misc, uint32{ 0 }, "disabled zone misc") && ok;
    }
    else
    {
        std::cerr << "world zone settings self-test failed: missing zone 286\n";
        ok = false;
    }

    return ok;
}

auto testZoneSettingsFailsWhenDatabaseReturnsNoResult() -> bool
{
    FakeDatabase fake;

    db::setDatabase(&fake);
    DatabaseResetGuard guard;

    try
    {
        const ZoneSettings settings;
        (void)settings;
    }
    catch (const std::runtime_error& error)
    {
        bool ok = true;
        ok      = expectEqual(fake.queries.size(), std::size_t{ 1 }, "failed query count") && ok;
        if (!fake.queries.empty())
        {
            ok = expectEqual(fake.queries.front(), std::string("SELECT zoneid, zoneip, zoneport, misc FROM zone_settings"), "failed query text") && ok;
        }
        ok = expectEqual(std::string(error.what()), std::string("Message Server: Failed to load zone settings from database"), "failed query error") && ok;
        return ok;
    }

    std::cerr << "world zone settings self-test failed: ZoneSettings unexpectedly accepted a missing result\n";
    return false;
}

} // namespace

auto runWorldZoneSettingsSelfTests() -> bool
{
    bool ok = true;

    ok = testZoneSettingsIndexesRowsFromDatabase() && ok;
    ok = testZoneSettingsFailsWhenDatabaseReturnsNoResult() && ok;

    return ok;
}
