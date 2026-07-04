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

#include "test_caching_database.h"

#include "common/database/caching_database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "common/settings.h"

#include <atomic>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{

struct BindCall
{
    int            index;
    db::BoundValue value;
};

struct StatementLog
{
    std::vector<BindCall>    binds;
    std::vector<std::string> queryCalls;
    std::vector<std::string> updateCalls;
};

struct ConnectionLog
{
    std::vector<std::string> prepareCalls;
};

struct StatementOutcome
{
    std::unique_ptr<db::ResultSet> result;
    std::optional<std::string>     error;
};

class SettingGuard
{
public:
    explicit SettingGuard(std::string key)
    : key_(std::move(key))
    {
        if (const auto it = settings::settingsMap.find(key_); it != settings::settingsMap.end())
        {
            original_ = it->second;
        }
    }

    ~SettingGuard()
    {
        if (original_)
        {
            settings::settingsMap[key_] = *original_;
        }
        else
        {
            settings::settingsMap.erase(key_);
        }

        settings::detail::generation.fetch_add(1, std::memory_order_release);
    }

private:
    std::string                              key_;
    std::optional<settings::SettingsVariant> original_;
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Caching database self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename T>
auto expectVariant(const db::BoundValue& value, const T& expected, const std::string& label) -> bool
{
    if (!std::holds_alternative<T>(value))
    {
        std::cerr << "Caching database self-test failed: " << label << " type" << '\n';
        return false;
    }

    return expectEqual(std::get<T>(value), expected, label);
}

auto updateResult(const std::string& query, std::size_t affected = 1) -> std::unique_ptr<db::ResultSet>
{
    return std::make_unique<db::LibMariaDBResultSet>(affected, query);
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

auto outcomeResult(std::unique_ptr<db::ResultSet> result) -> StatementOutcome
{
    StatementOutcome outcome;
    outcome.result = std::move(result);
    return outcome;
}

auto outcomeError(std::string error) -> StatementOutcome
{
    StatementOutcome outcome;
    outcome.error = std::move(error);
    return outcome;
}

auto popOutcome(std::vector<StatementOutcome>& outcomes) -> std::unique_ptr<db::ResultSet>
{
    if (outcomes.empty())
    {
        return nullptr;
    }

    auto outcome = std::move(outcomes.front());
    outcomes.erase(outcomes.begin());
    if (outcome.error)
    {
        throw std::runtime_error(*outcome.error);
    }

    return std::move(outcome.result);
}

class FakePreparedStatement final : public db::PreparedStatement
{
public:
    explicit FakePreparedStatement(std::shared_ptr<StatementLog> log)
    : log_(std::move(log))
    {
    }

    auto bind(int index, const db::BoundValue& value) -> void override
    {
        log_->binds.push_back({ index, value });
        if (bindError)
        {
            throw std::runtime_error(*bindError);
        }
    }

    auto executeQuery(const std::string& query) -> std::unique_ptr<db::ResultSet> override
    {
        log_->queryCalls.push_back(query);
        return popOutcome(queryOutcomes);
    }

    auto executeUpdate(const std::string& query) -> std::unique_ptr<db::ResultSet> override
    {
        log_->updateCalls.push_back(query);
        return popOutcome(updateOutcomes);
    }

    std::optional<std::string>       bindError;
    std::vector<StatementOutcome>    queryOutcomes;
    std::vector<StatementOutcome>    updateOutcomes;

private:
    std::shared_ptr<StatementLog> log_;
};

class FakeConnection final : public db::Connection
{
public:
    explicit FakeConnection(std::shared_ptr<ConnectionLog> log)
    : log_(std::move(log))
    {
    }

    auto prepare(const std::string& query) -> std::unique_ptr<db::PreparedStatement> override
    {
        log_->prepareCalls.push_back(query);
        if (prepareError)
        {
            throw std::runtime_error(*prepareError);
        }
        if (statements.empty())
        {
            throw std::runtime_error("unexpected prepare");
        }

        auto statement = std::move(statements.front());
        statements.erase(statements.begin());
        return statement;
    }

    auto schema() -> std::string override
    {
        return schemaValue;
    }

    auto version() -> std::string override
    {
        return versionValue;
    }

    auto driverVersion() -> std::string override
    {
        return driverVersionValue;
    }

    auto isConnectionError(const std::exception& e) const -> bool override
    {
        return std::string(e.what()) == "connection lost";
    }

    std::optional<std::string>                    prepareError;
    std::vector<std::unique_ptr<FakePreparedStatement>> statements;
    std::string                                   schemaValue;
    std::string                                   versionValue;
    std::string                                   driverVersionValue;

private:
    std::shared_ptr<ConnectionLog> log_;
};

class FakeCachingDatabase final : public db::CachingDatabase
{
public:
    auto createConnection() -> std::unique_ptr<db::Connection> override
    {
        ++createConnectionCalls;
        if (connections.empty())
        {
            throw std::runtime_error("unexpected connection");
        }
        auto connection = std::move(connections.front());
        connections.erase(connections.begin());
        return connection;
    }

    int                                      createConnectionCalls = 0;
    std::vector<std::unique_ptr<FakeConnection>> connections;
};

auto makeTestDatabase() -> FakeCachingDatabase&
{
    static std::vector<std::unique_ptr<FakeCachingDatabase>> databases;
    databases.emplace_back(std::make_unique<FakeCachingDatabase>());
    return *databases.back();
}

auto makeStatement(std::shared_ptr<StatementLog> log) -> std::unique_ptr<FakePreparedStatement>
{
    return std::make_unique<FakePreparedStatement>(std::move(log));
}

auto makeConnection(std::shared_ptr<ConnectionLog> log) -> std::unique_ptr<FakeConnection>
{
    return std::make_unique<FakeConnection>(std::move(log));
}

auto testInvalidQueriesBeforeConnecting() -> bool
{
    auto& database = makeTestDatabase();

    bool ok = true;
    ok      = expectEqual(database.execute("WITH rows AS (SELECT 1) SELECT * FROM rows", {}) == nullptr, true, "invalid keyword result") && ok;
    ok      = expectEqual(database.execute("SELECT {}", {}) == nullptr, true, "format hole result") && ok;
    ok      = expectEqual(database.execute("SELECT 1; SELECT 2", {}) == nullptr, true, "semicolon result") && ok;
    ok      = expectEqual(database.createConnectionCalls, 0, "invalid query connection count") && ok;

    return ok;
}

auto testStatementCacheAndDispatch() -> bool
{
    auto& database      = makeTestDatabase();
    auto  connectionLog = std::make_shared<ConnectionLog>();
    auto  selectLog     = std::make_shared<StatementLog>();
    auto  updateLog     = std::make_shared<StatementLog>();

    auto selectStatement = makeStatement(selectLog);
    selectStatement->queryOutcomes.push_back(outcomeResult(selectResult("SELECT * FROM chars WHERE id = ? AND name = ?", "name", { std::string("Prishe") })));
    selectStatement->queryOutcomes.push_back(outcomeResult(selectResult("SELECT * FROM chars WHERE id = ? AND name = ?", "name", { std::string("Tenzen") })));

    auto updateStatement = makeStatement(updateLog);
    updateStatement->updateOutcomes.push_back(outcomeResult(updateResult("UPDATE chars SET name = ?", 3)));

    auto connection = makeConnection(connectionLog);
    connection->statements.push_back(std::move(selectStatement));
    connection->statements.push_back(std::move(updateStatement));
    database.connections.push_back(std::move(connection));

    bool ok = true;

    auto first = database.execute("SELECT * FROM chars WHERE id = ? AND name = ?", { static_cast<int32>(7), std::string("Prishe") });
    ok         = expectEqual(first != nullptr, true, "first select result") && ok;
    if (first)
    {
        ok = expectEqual(first->type(), db::ResultSetType::Select, "first select type") && ok;
        ok = expectEqual(first->next(), true, "first select next") && ok;
        ok = expectEqual(first->get<std::string>(0), std::string("Prishe"), "first select value") && ok;
    }

    auto second = database.execute("SELECT * FROM chars WHERE id = ? AND name = ?", { static_cast<int32>(8), std::string("Tenzen") });
    ok          = expectEqual(second != nullptr, true, "second select result") && ok;
    if (second)
    {
        ok = expectEqual(second->type(), db::ResultSetType::Select, "second select type") && ok;
        ok = expectEqual(second->next(), true, "second select next") && ok;
        ok = expectEqual(second->get<std::string>(0), std::string("Tenzen"), "second select value") && ok;
    }

    auto updated = database.execute("UPDATE chars SET name = ?", { std::string("Omega") });
    ok           = expectEqual(updated != nullptr, true, "update result") && ok;
    if (updated)
    {
        ok = expectEqual(updated->type(), db::ResultSetType::Update, "update type") && ok;
        ok = expectEqual(updated->rowsAffected(), static_cast<uint32>(3), "update rows") && ok;
    }

    ok = expectEqual(connectionLog->prepareCalls.size(), static_cast<std::size_t>(2), "prepare call count") && ok;
    if (connectionLog->prepareCalls.size() == 2)
    {
        ok = expectEqual(connectionLog->prepareCalls[0], std::string("SELECT * FROM chars WHERE id = ? AND name = ?"), "first prepared query") && ok;
        ok = expectEqual(connectionLog->prepareCalls[1], std::string("UPDATE chars SET name = ?"), "second prepared query") && ok;
    }

    ok = expectEqual(selectLog->queryCalls.size(), static_cast<std::size_t>(2), "select execute count") && ok;
    ok = expectEqual(updateLog->updateCalls.size(), static_cast<std::size_t>(1), "update execute count") && ok;
    ok = expectEqual(selectLog->binds.size(), static_cast<std::size_t>(4), "select bind count") && ok;
    if (selectLog->binds.size() == 4)
    {
        ok = expectEqual(selectLog->binds[0].index, 1, "first bind index") && ok;
        ok = expectVariant<int32>(selectLog->binds[0].value, static_cast<int32>(7), "first bind value") && ok;
        ok = expectEqual(selectLog->binds[1].index, 2, "second bind index") && ok;
        ok = expectVariant<std::string>(selectLog->binds[1].value, std::string("Prishe"), "second bind value") && ok;
        ok = expectEqual(selectLog->binds[2].index, 1, "third bind index") && ok;
        ok = expectVariant<int32>(selectLog->binds[2].value, static_cast<int32>(8), "third bind value") && ok;
        ok = expectEqual(selectLog->binds[3].index, 2, "fourth bind index") && ok;
        ok = expectVariant<std::string>(selectLog->binds[3].value, std::string("Tenzen"), "fourth bind value") && ok;
    }
    ok = expectEqual(updateLog->binds.size(), static_cast<std::size_t>(1), "update bind count") && ok;
    if (updateLog->binds.size() == 1)
    {
        ok = expectEqual(updateLog->binds[0].index, 1, "update bind index") && ok;
        ok = expectVariant<std::string>(updateLog->binds[0].value, std::string("Omega"), "update bind value") && ok;
    }

    return ok;
}

auto testConnectionErrorRetry() -> bool
{
    SettingGuard guard("network.SQL_QUERY_RETRY_COUNT");
    settings::set("network.SQL_QUERY_RETRY_COUNT", 1.0);

    auto& database            = makeTestDatabase();
    auto  firstConnectionLog  = std::make_shared<ConnectionLog>();
    auto  firstStatementLog   = std::make_shared<StatementLog>();
    auto  secondConnectionLog = std::make_shared<ConnectionLog>();
    auto  secondStatementLog  = std::make_shared<StatementLog>();

    auto firstStatement = makeStatement(firstStatementLog);
    firstStatement->queryOutcomes.push_back(outcomeError("connection lost"));
    auto firstConnection = makeConnection(firstConnectionLog);
    firstConnection->statements.push_back(std::move(firstStatement));

    auto secondStatement = makeStatement(secondStatementLog);
    secondStatement->queryOutcomes.push_back(outcomeResult(selectResult("SELECT retry", "ok", { static_cast<uint64>(1) })));
    auto secondConnection = makeConnection(secondConnectionLog);
    secondConnection->statements.push_back(std::move(secondStatement));

    database.connections.push_back(std::move(firstConnection));
    database.connections.push_back(std::move(secondConnection));

    bool ok = true;

    auto result = database.execute("SELECT retry", { static_cast<int32>(42) });
    ok          = expectEqual(result != nullptr, true, "retry result") && ok;
    if (result)
    {
        ok = expectEqual(result->type(), db::ResultSetType::Select, "retry type") && ok;
        ok = expectEqual(result->next(), true, "retry next") && ok;
        ok = expectEqual(result->get<uint32>(0), static_cast<uint32>(1), "retry value") && ok;
    }

    ok = expectEqual(database.createConnectionCalls, 2, "retry connection count") && ok;
    ok = expectEqual(firstConnectionLog->prepareCalls.size(), static_cast<std::size_t>(1), "first retry prepare count") && ok;
    ok = expectEqual(secondConnectionLog->prepareCalls.size(), static_cast<std::size_t>(1), "second retry prepare count") && ok;
    ok = expectEqual(firstStatementLog->binds.size(), static_cast<std::size_t>(1), "first retry bind count") && ok;
    ok = expectEqual(secondStatementLog->binds.size(), static_cast<std::size_t>(1), "second retry bind count") && ok;
    if (firstStatementLog->binds.size() == 1 && secondStatementLog->binds.size() == 1)
    {
        ok = expectEqual(firstStatementLog->binds[0].index, 1, "first retry bind index") && ok;
        ok = expectVariant<int32>(firstStatementLog->binds[0].value, static_cast<int32>(42), "first retry bind value") && ok;
        ok = expectEqual(secondStatementLog->binds[0].index, 1, "second retry bind index") && ok;
        ok = expectVariant<int32>(secondStatementLog->binds[0].value, static_cast<int32>(42), "second retry bind value") && ok;
    }

    return ok;
}

auto testPrepareConnectionErrorRetry() -> bool
{
    SettingGuard guard("network.SQL_QUERY_RETRY_COUNT");
    settings::set("network.SQL_QUERY_RETRY_COUNT", 1.0);

    auto& database            = makeTestDatabase();
    auto  firstConnectionLog  = std::make_shared<ConnectionLog>();
    auto  secondConnectionLog = std::make_shared<ConnectionLog>();
    auto  secondStatementLog  = std::make_shared<StatementLog>();

    auto firstConnection        = makeConnection(firstConnectionLog);
    firstConnection->prepareError = "connection lost";

    auto secondStatement = makeStatement(secondStatementLog);
    secondStatement->queryOutcomes.push_back(outcomeResult(selectResult("SELECT prepare_retry", "ok", { static_cast<uint64>(1) })));
    auto secondConnection = makeConnection(secondConnectionLog);
    secondConnection->statements.push_back(std::move(secondStatement));

    database.connections.push_back(std::move(firstConnection));
    database.connections.push_back(std::move(secondConnection));

    bool ok = true;

    auto result = database.execute("SELECT prepare_retry", {});
    ok          = expectEqual(result != nullptr, true, "prepare retry result") && ok;
    if (result)
    {
        ok = expectEqual(result->type(), db::ResultSetType::Select, "prepare retry type") && ok;
        ok = expectEqual(result->next(), true, "prepare retry next") && ok;
        ok = expectEqual(result->get<uint32>(0), static_cast<uint32>(1), "prepare retry value") && ok;
    }

    ok = expectEqual(database.createConnectionCalls, 2, "prepare retry connection count") && ok;
    ok = expectEqual(firstConnectionLog->prepareCalls.size(), static_cast<std::size_t>(1), "first prepare retry count") && ok;
    ok = expectEqual(secondConnectionLog->prepareCalls.size(), static_cast<std::size_t>(1), "second prepare retry count") && ok;
    ok = expectEqual(secondStatementLog->queryCalls.size(), static_cast<std::size_t>(1), "prepare retry execute count") && ok;

    return ok;
}

auto testBindConnectionErrorRetry() -> bool
{
    SettingGuard guard("network.SQL_QUERY_RETRY_COUNT");
    settings::set("network.SQL_QUERY_RETRY_COUNT", 1.0);

    auto& database            = makeTestDatabase();
    auto  firstConnectionLog  = std::make_shared<ConnectionLog>();
    auto  firstStatementLog   = std::make_shared<StatementLog>();
    auto  secondConnectionLog = std::make_shared<ConnectionLog>();
    auto  secondStatementLog  = std::make_shared<StatementLog>();

    auto firstStatement       = makeStatement(firstStatementLog);
    firstStatement->bindError = "connection lost";
    auto firstConnection      = makeConnection(firstConnectionLog);
    firstConnection->statements.push_back(std::move(firstStatement));

    auto secondStatement = makeStatement(secondStatementLog);
    secondStatement->queryOutcomes.push_back(outcomeResult(selectResult("SELECT bind_retry", "ok", { static_cast<uint64>(1) })));
    auto secondConnection = makeConnection(secondConnectionLog);
    secondConnection->statements.push_back(std::move(secondStatement));

    database.connections.push_back(std::move(firstConnection));
    database.connections.push_back(std::move(secondConnection));

    bool ok = true;

    auto result = database.execute("SELECT bind_retry", { static_cast<int32>(99) });
    ok          = expectEqual(result != nullptr, true, "bind retry result") && ok;
    if (result)
    {
        ok = expectEqual(result->type(), db::ResultSetType::Select, "bind retry type") && ok;
        ok = expectEqual(result->next(), true, "bind retry next") && ok;
        ok = expectEqual(result->get<uint32>(0), static_cast<uint32>(1), "bind retry value") && ok;
    }

    ok = expectEqual(database.createConnectionCalls, 2, "bind retry connection count") && ok;
    ok = expectEqual(firstStatementLog->binds.size(), static_cast<std::size_t>(1), "first bind retry bind count") && ok;
    ok = expectEqual(secondStatementLog->binds.size(), static_cast<std::size_t>(1), "second bind retry bind count") && ok;
    if (firstStatementLog->binds.size() == 1 && secondStatementLog->binds.size() == 1)
    {
        ok = expectEqual(firstStatementLog->binds[0].index, 1, "first bind retry index") && ok;
        ok = expectVariant<int32>(firstStatementLog->binds[0].value, static_cast<int32>(99), "first bind retry value") && ok;
        ok = expectEqual(secondStatementLog->binds[0].index, 1, "second bind retry index") && ok;
        ok = expectVariant<int32>(secondStatementLog->binds[0].value, static_cast<int32>(99), "second bind retry value") && ok;
    }
    ok = expectEqual(secondStatementLog->queryCalls.size(), static_cast<std::size_t>(1), "bind retry execute count") && ok;

    return ok;
}

auto testNonConnectionErrorDoesNotRetry() -> bool
{
    SettingGuard guard("network.SQL_QUERY_RETRY_COUNT");
    settings::set("network.SQL_QUERY_RETRY_COUNT", 3.0);

    auto& database      = makeTestDatabase();
    auto  connectionLog = std::make_shared<ConnectionLog>();
    auto  statementLog  = std::make_shared<StatementLog>();

    auto statement = makeStatement(statementLog);
    statement->queryOutcomes.push_back(outcomeError("query failed"));
    auto connection = makeConnection(connectionLog);
    connection->statements.push_back(std::move(statement));
    database.connections.push_back(std::move(connection));

    bool ok = true;
    ok      = expectEqual(database.execute("SELECT fail", {}) == nullptr, true, "query failure result") && ok;
    ok      = expectEqual(database.createConnectionCalls, 1, "query failure connection count") && ok;
    ok      = expectEqual(connectionLog->prepareCalls.size(), static_cast<std::size_t>(1), "query failure prepare count") && ok;
    ok      = expectEqual(statementLog->queryCalls.size(), static_cast<std::size_t>(1), "query failure execute count") && ok;

    return ok;
}

auto testMetadataForwarding() -> bool
{
    auto& database      = makeTestDatabase();
    auto  connectionLog = std::make_shared<ConnectionLog>();
    auto  connection    = makeConnection(connectionLog);
    connection->schemaValue        = "xidb";
    connection->versionValue       = "MariaDB 10.6";
    connection->driverVersionValue = "MariaDB Connector/C 3.2";
    database.connections.push_back(std::move(connection));

    bool ok = true;
    ok      = expectEqual(database.getSchema(), std::string("xidb"), "schema") && ok;
    ok      = expectEqual(database.getVersion(), std::string("MariaDB 10.6"), "version") && ok;
    ok      = expectEqual(database.getDriverVersion(), std::string("MariaDB Connector/C 3.2"), "driver version") && ok;
    ok      = expectEqual(database.createConnectionCalls, 1, "metadata connection count") && ok;

    return ok;
}

} // namespace

auto runCachingDatabaseSelfTests() -> bool
{
    return testInvalidQueriesBeforeConnecting() &&
           testStatementCacheAndDispatch() &&
           testConnectionErrorRetry() &&
           testPrepareConnectionErrorRetry() &&
           testBindConnectionErrorRetry() &&
           testNonConnectionErrorDoesNotRetry() &&
           testMetadataForwarding();
}
