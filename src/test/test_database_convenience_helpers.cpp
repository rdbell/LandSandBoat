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

#include "test_database_convenience_helpers.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "omega_self_test_registry.h"

#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
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
        queries.push_back(query);
        this->params.push_back(params);
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
        return version;
    }

    auto getDriverVersion() -> std::string override
    {
        return "MariaDB Connector/C";
    }

    std::vector<std::string>                 queries;
    std::vector<std::vector<db::BoundValue>> params;
    std::vector<std::unique_ptr<db::ResultSet>> results;
    std::string                              version = "MariaDB 10.6";
};

struct DatabaseResetGuard
{
    ~DatabaseResetGuard()
    {
        db::setDatabase(nullptr);
    }
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Database convenience helper self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto updateResult(const std::string& query = "UPDATE helper") -> std::unique_ptr<db::ResultSet>
{
    return std::make_unique<db::LibMariaDBResultSet>(static_cast<std::size_t>(1), query);
}

auto selectResult(const std::string& query, const std::string& column, std::vector<db::LibMariaDBResultSet::Cell> cells) -> std::unique_ptr<db::ResultSet>
{
    auto schema = std::make_shared<db::ColumnSchema>();
    schema->names.push_back(column);
    schema->index[column] = 0;

    auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
    for (auto& cell : cells)
    {
        auto row = db::LibMariaDBResultSet::Row{};
        row.emplace_back(std::move(cell));
        rows.emplace_back(std::move(row));
    }

    return std::make_unique<db::LibMariaDBResultSet>(query, std::move(schema), std::move(rows));
}

auto expectInt32Param(const db::BoundValue& value, int32 expected, const std::string& label) -> bool
{
    if (!std::holds_alternative<int32>(value))
    {
        std::cerr << "Database convenience helper self-test failed: " << label << " type" << '\n';
        return false;
    }
    return expectEqual(std::get<int32>(value), expected, label);
}

auto expectStringParam(const db::BoundValue& value, const std::string& expected, const std::string& label) -> bool
{
    if (!std::holds_alternative<std::string>(value))
    {
        std::cerr << "Database convenience helper self-test failed: " << label << " type" << '\n';
        return false;
    }
    return expectEqual(std::get<std::string>(value), expected, label);
}

auto testAutoCommitHelpers() -> bool
{
    DatabaseResetGuard guard;
    FakeDatabase       fake;
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(selectResult("SELECT @@autocommit", "@@autocommit", { static_cast<uint64>(1) }));
    fake.results.push_back(selectResult("SELECT @@autocommit", "@@autocommit", { static_cast<uint64>(0) }));
    fake.results.push_back(selectResult("SELECT @@autocommit", "@@autocommit", {}));
    db::setDatabase(&fake);

    bool ok = true;
    ok      = expectEqual(db::setAutoCommit(true), true, "set autocommit true") && ok;
    ok      = expectInt32Param(fake.params[0][0], static_cast<int32>(1), "set autocommit true param") && ok;
    ok      = expectEqual(db::setAutoCommit(false), true, "set autocommit false") && ok;
    ok      = expectInt32Param(fake.params[1][0], static_cast<int32>(0), "set autocommit false param") && ok;
    ok      = expectEqual(db::getAutoCommit(), true, "get autocommit true") && ok;
    ok      = expectEqual(db::getAutoCommit(), false, "get autocommit false") && ok;
    ok      = expectEqual(db::getAutoCommit(), false, "get autocommit empty") && ok;

    return ok;
}

auto testTransactionHelpers() -> bool
{
    DatabaseResetGuard guard;
    FakeDatabase       fake;
    fake.results.push_back(selectResult("SELECT @@autocommit", "@@autocommit", { static_cast<uint64>(1) }));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(updateResult("START TRANSACTION"));
    fake.results.push_back(updateResult("COMMIT"));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(selectResult("SELECT @@autocommit", "@@autocommit", { static_cast<uint64>(1) }));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(updateResult("START TRANSACTION"));
    fake.results.push_back(updateResult("ROLLBACK"));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(selectResult("SELECT @@autocommit", "@@autocommit", { static_cast<uint64>(1) }));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    fake.results.push_back(updateResult("START TRANSACTION"));
    fake.results.push_back(updateResult("ROLLBACK"));
    fake.results.push_back(updateResult("SET @@autocommit = ?"));
    db::setDatabase(&fake);

    bool ok = true;
    int  callbacks = 0;
    ok = expectEqual(
             db::transaction(
                 [&]()
                 {
                     ++callbacks;
                 }),
             true,
             "successful transaction") &&
         ok;
    ok = expectEqual(callbacks, 1, "successful transaction callback count") && ok;

    const auto successQueries = std::vector<std::string>{
        "SELECT @@autocommit",
        "SET @@autocommit = ?",
        "START TRANSACTION",
        "COMMIT",
        "SET @@autocommit = ?",
    };
    for (std::size_t i = 0; i < successQueries.size(); ++i)
    {
        ok = expectEqual(fake.queries[i], successQueries[i], "successful transaction query") && ok;
    }
    ok = expectInt32Param(fake.params[1][0], static_cast<int32>(0), "successful transaction disable autocommit") && ok;
    ok = expectInt32Param(fake.params[4][0], static_cast<int32>(1), "successful transaction restore autocommit") && ok;

    ok = expectEqual(
             db::transaction(
                 []()
                 {
                     throw std::runtime_error("boom");
                 }),
             false,
             "failing transaction") &&
         ok;
    const auto failureOffset = successQueries.size();
    const auto failureQueries = std::vector<std::string>{
        "SELECT @@autocommit",
        "SET @@autocommit = ?",
        "START TRANSACTION",
        "ROLLBACK",
        "SET @@autocommit = ?",
    };
    for (std::size_t i = 0; i < failureQueries.size(); ++i)
    {
        ok = expectEqual(fake.queries[failureOffset + i], failureQueries[i], "failing transaction query") && ok;
    }
    ok = expectInt32Param(fake.params[failureOffset + 1][0], static_cast<int32>(0), "failing transaction disable autocommit") && ok;
    ok = expectInt32Param(fake.params[failureOffset + 4][0], static_cast<int32>(1), "failing transaction restore autocommit") && ok;

    const auto             emptyOffset = failureOffset + failureQueries.size();
    std::function<void()>  emptyTransactionFn;
    ok = expectEqual(db::transaction(emptyTransactionFn), false, "empty transaction function") && ok;
    const auto emptyQueries = std::vector<std::string>{
        "SELECT @@autocommit",
        "SET @@autocommit = ?",
        "START TRANSACTION",
        "ROLLBACK",
        "SET @@autocommit = ?",
    };
    for (std::size_t i = 0; i < emptyQueries.size(); ++i)
    {
        ok = expectEqual(fake.queries[emptyOffset + i], emptyQueries[i], "empty transaction function query") && ok;
    }
    ok = expectInt32Param(fake.params[emptyOffset + 1][0], static_cast<int32>(0), "empty transaction function disable autocommit") && ok;
    ok = expectInt32Param(fake.params[emptyOffset + 4][0], static_cast<int32>(1), "empty transaction function restore autocommit") && ok;

    return ok;
}

auto testTableColumnNames() -> bool
{
    DatabaseResetGuard guard;
    FakeDatabase       fake;
    fake.results.push_back(selectResult("SELECT columns", "COLUMN_NAME", { std::string("charid"), std::string("charname") }));
    fake.results.push_back(selectResult("SELECT columns", "COLUMN_NAME", {}));
    db::setDatabase(&fake);

    bool ok = true;

    const auto names = db::getTableColumnNames("chars");
    ok = expectEqual(names.size(), static_cast<std::size_t>(2), "table column count") && ok;
    if (names.size() == 2)
    {
        ok = expectEqual(names[0], std::string("charid"), "first table column") && ok;
        ok = expectEqual(names[1], std::string("charname"), "second table column") && ok;
    }
    ok = expectEqual(fake.queries[0], std::string("SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_NAME = ? AND TABLE_SCHEMA = ?"), "table column query") && ok;
    ok = expectStringParam(fake.params[0][0], std::string("chars"), "table column table param") && ok;
    ok = expectStringParam(fake.params[0][1], std::string("xidb"), "table column schema param") && ok;

    const auto emptyNames = db::getTableColumnNames("missing");
    ok = expectEqual(emptyNames.empty(), true, "empty table column result") && ok;

    return ok;
}

auto testTableColumnNamesSQLite() -> bool
{
    DatabaseResetGuard guard;
    FakeDatabase       fake;
    fake.version = "SQLite 3.45.1";
    fake.results.push_back(selectResult("SELECT columns", "name", { std::string("charid"), std::string("charname") }));
    db::setDatabase(&fake);

    const auto names = db::getTableColumnNames("chars");
    bool       ok    = expectEqual(names.size(), static_cast<std::size_t>(2), "SQLite table column count");
    if (names.size() == 2)
    {
        ok = expectEqual(names[0], std::string("charid"), "SQLite first table column") && ok;
        ok = expectEqual(names[1], std::string("charname"), "SQLite second table column") && ok;
    }
    ok = expectEqual(fake.queries.size(), static_cast<std::size_t>(1), "SQLite table column query count") && ok;
    if (fake.queries.size() == 1)
    {
        ok = expectEqual(fake.queries[0], std::string("SELECT name FROM pragma_table_info(?)"), "SQLite table column query") && ok;
    }
    ok = expectEqual(fake.params.size(), static_cast<std::size_t>(1), "SQLite table column parameter count") && ok;
    if (fake.params.size() == 1)
    {
        ok = expectStringParam(fake.params[0][0], std::string("chars"), "SQLite table column table parameter") && ok;
    }
    return ok;
}

} // namespace

auto runDatabaseConvenienceHelpersSelfTests() -> bool
{
    return testAutoCommitHelpers() && testTransactionHelpers() && testTableColumnNames();
}

OMEGA_REGISTER_SELF_TEST("database-table-columns-sqlite-8803", testTableColumnNamesSQLite);
