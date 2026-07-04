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

#include "test_active_database_dispatch.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>& params) -> std::unique_ptr<db::ResultSet> override
    {
        ++executeCalls;
        lastQuery  = query;
        lastParams = params;
        return std::make_unique<db::LibMariaDBResultSet>(static_cast<std::size_t>(3), query);
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

    int                         executeCalls = 0;
    std::string                 lastQuery;
    std::vector<db::BoundValue> lastParams;
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Active database dispatch self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename T, typename U>
auto expectVariant(const db::BoundValue& value, const U& expected, const std::string& label) -> bool
{
    if (!std::holds_alternative<T>(value))
    {
        std::cerr << "Active database dispatch self-test failed: " << label << " type" << '\n';
        return false;
    }

    return expectEqual(std::get<T>(value), expected, label);
}

auto testActiveDatabaseDispatch() -> bool
{
    FakeDatabase fake;
    db::setDatabase(&fake);

    bool ok = true;
    ok      = expectEqual(db::getDatabaseSchema(), std::string("xidb"), "schema forwarding") && ok;
    ok      = expectEqual(db::getDatabaseVersion(), std::string("MariaDB 10.6"), "version forwarding") && ok;
    ok      = expectEqual(db::getDriverVersion(), std::string("MariaDB Connector/C"), "driver version forwarding") && ok;

    const auto result = db::preparedStmt("UPDATE accounts SET name = ? WHERE id = ?", std::string("name"), static_cast<int32>(42), false);
    ok                = expectEqual(result != nullptr, true, "preparedStmt returned result") && ok;
    if (result)
    {
        ok = expectEqual(result->type(), db::ResultSetType::Update, "preparedStmt result type") && ok;
        ok = expectEqual(result->rowsAffected(), static_cast<uint32>(3), "preparedStmt result rows affected") && ok;
    }

    ok = expectEqual(fake.executeCalls, 1, "execute call count") && ok;
    ok = expectEqual(fake.lastQuery, std::string("UPDATE accounts SET name = ? WHERE id = ?"), "execute query") && ok;
    ok = expectEqual(fake.lastParams.size(), static_cast<std::size_t>(3), "execute param count") && ok;
    if (fake.lastParams.size() == 3)
    {
        ok = expectVariant<std::string>(fake.lastParams[0], std::string("name"), "string parameter") && ok;
        ok = expectVariant<int32>(fake.lastParams[1], static_cast<int32>(42), "int32 parameter") && ok;
        ok = expectVariant<bool>(fake.lastParams[2], false, "bool parameter") && ok;
    }

    db::setDatabase(nullptr);
    return ok;
}

} // namespace

auto runActiveDatabaseDispatchSelfTests() -> bool
{
    return testActiveDatabaseDispatch();
}
