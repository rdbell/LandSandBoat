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

#include "test_world_daily_tally.h"

#include "common/cbasetypes.h"
#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "common/settings.h"
#include "world/daily_tally.h"

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{

const auto updateQuery = std::string("UPDATE char_points "
                                     "SET char_points.daily_tally = LEAST(?, char_points.daily_tally + ?) "
                                     "WHERE char_points.daily_tally > -1");
const auto deleteQuery = std::string("DELETE FROM char_vars WHERE varname = 'gobbieBoxUsed'");

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>& params) -> std::unique_ptr<db::ResultSet> override
    {
        queries.push_back(query);
        boundParams.push_back(params);
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
    std::vector<std::vector<db::BoundValue>>    boundParams;
    std::vector<std::unique_ptr<db::ResultSet>> results;
};

struct DatabaseResetGuard
{
    ~DatabaseResetGuard()
    {
        db::setDatabase(nullptr);
    }
};

class DailyTallySettingsGuard
{
public:
    DailyTallySettingsGuard()
    : enabled_(settings::get<bool>("main.ENABLE_DAILY_TALLY"))
    , limit_(settings::get<int32>("main.DAILY_TALLY_LIMIT"))
    , amount_(settings::get<int32>("main.DAILY_TALLY_AMOUNT"))
    {
    }

    ~DailyTallySettingsGuard()
    {
        settings::set("main.ENABLE_DAILY_TALLY", enabled_);
        settings::set("main.DAILY_TALLY_LIMIT", static_cast<double>(limit_));
        settings::set("main.DAILY_TALLY_AMOUNT", static_cast<double>(amount_));
    }

private:
    bool  enabled_;
    int32 limit_;
    int32 amount_;
};

auto emptyResult(const std::string& query) -> std::unique_ptr<db::ResultSet>
{
    return std::make_unique<db::LibMariaDBResultSet>(query, std::make_shared<db::ColumnSchema>(), std::vector<db::LibMariaDBResultSet::Row>{});
}

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "world daily tally self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectInt32Param(const db::BoundValue& value, const int32 expected, const std::string& label) -> bool
{
    if (const auto actual = std::get_if<int32>(&value))
    {
        return expectEqual(*actual, expected, label);
    }

    std::cerr << "world daily tally self-test failed: " << label << " was not int32\n";
    return false;
}

auto setDailyTallySettings(const bool enabled, const int32 limit, const int32 amount) -> void
{
    settings::set("main.ENABLE_DAILY_TALLY", enabled);
    settings::set("main.DAILY_TALLY_LIMIT", static_cast<double>(limit));
    settings::set("main.DAILY_TALLY_AMOUNT", static_cast<double>(amount));
}

auto testDailyTallyDisabledSkipsDatabase() -> bool
{
    bool ok = true;

    DailyTallySettingsGuard settingsGuard;
    setDailyTallySettings(false, 500, 10);

    FakeDatabase fake;
    db::setDatabase(&fake);
    DatabaseResetGuard databaseGuard;

    dailytally::UpdateDailyTallyPoints();

    ok = expectEqual(fake.queries.size(), std::size_t{ 0 }, "disabled query count") && ok;

    return ok;
}

auto testDailyTallyUpdatesAndDeletes() -> bool
{
    bool ok = true;

    DailyTallySettingsGuard settingsGuard;
    setDailyTallySettings(true, 500, 10);

    FakeDatabase fake;
    fake.results.emplace_back(emptyResult(updateQuery));
    fake.results.emplace_back(emptyResult(deleteQuery));
    db::setDatabase(&fake);
    DatabaseResetGuard databaseGuard;

    dailytally::UpdateDailyTallyPoints();

    ok = expectEqual(fake.queries.size(), std::size_t{ 2 }, "query count") && ok;
    if (fake.queries.size() == 2)
    {
        ok = expectEqual(fake.queries[0], updateQuery, "update query") && ok;
        ok = expectEqual(fake.queries[1], deleteQuery, "delete query") && ok;
    }
    ok = expectEqual(fake.boundParams.size(), std::size_t{ 2 }, "bound params query count") && ok;
    if (!fake.boundParams.empty())
    {
        ok = expectEqual(fake.boundParams[0].size(), std::size_t{ 2 }, "update param count") && ok;
        if (fake.boundParams[0].size() == 2)
        {
            ok = expectInt32Param(fake.boundParams[0][0], 500, "limit param") && ok;
            ok = expectInt32Param(fake.boundParams[0][1], 10, "amount param") && ok;
        }
        if (fake.boundParams.size() == 2)
        {
            ok = expectEqual(fake.boundParams[1].size(), std::size_t{ 0 }, "delete param count") && ok;
        }
    }

    return ok;
}

auto testDailyTallyClampsSettings() -> bool
{
    bool ok = true;

    DailyTallySettingsGuard settingsGuard;
    setDailyTallySettings(true, 70000, -5);

    FakeDatabase fake;
    fake.results.emplace_back(emptyResult(updateQuery));
    fake.results.emplace_back(emptyResult(deleteQuery));
    db::setDatabase(&fake);
    DatabaseResetGuard databaseGuard;

    dailytally::UpdateDailyTallyPoints();

    ok = expectEqual(fake.boundParams.size(), std::size_t{ 2 }, "bound params query count") && ok;
    if (!fake.boundParams.empty() && fake.boundParams[0].size() == 2)
    {
        ok = expectInt32Param(fake.boundParams[0][0], 65535, "clamped limit") && ok;
        ok = expectInt32Param(fake.boundParams[0][1], 0, "clamped amount") && ok;
    }
    else
    {
        std::cerr << "world daily tally self-test failed: missing clamp params\n";
        ok = false;
    }

    return ok;
}

auto testDailyTallyUpdateFailureStillDeletes() -> bool
{
    bool ok = true;

    DailyTallySettingsGuard settingsGuard;
    setDailyTallySettings(true, 200, 20);

    FakeDatabase fake;
    fake.results.emplace_back(nullptr);
    fake.results.emplace_back(emptyResult(deleteQuery));
    db::setDatabase(&fake);
    DatabaseResetGuard databaseGuard;

    dailytally::UpdateDailyTallyPoints();

    ok = expectEqual(fake.queries.size(), std::size_t{ 2 }, "update failure query count") && ok;
    if (fake.queries.size() == 2)
    {
        ok = expectEqual(fake.queries[0], updateQuery, "failed update query") && ok;
        ok = expectEqual(fake.queries[1], deleteQuery, "delete after failed update query") && ok;
    }

    return ok;
}

} // namespace

auto runWorldDailyTallySelfTests() -> bool
{
    bool ok = true;

    ok = testDailyTallyDisabledSkipsDatabase() && ok;
    ok = testDailyTallyUpdatesAndDeletes() && ok;
    ok = testDailyTallyClampsSettings() && ok;
    ok = testDailyTallyUpdateFailureStillDeletes() && ok;

    return ok;
}
