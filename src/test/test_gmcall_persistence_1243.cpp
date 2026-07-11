#include "test_gmcall_persistence_1243.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "map/gmcall_persistence.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{

constexpr auto pendingQuery     = "SELECT id, response FROM help_desk WHERE charid = ? AND response IS NOT NULL AND deleted_at IS NULL ORDER BY id ASC LIMIT 1";
constexpr auto acknowledgeQuery = "UPDATE help_desk SET deleted_at = NOW() WHERE charid = ? AND response IS NOT NULL AND deleted_at IS NULL ORDER BY id ASC LIMIT 1";

auto selectResult(std::string query, std::vector<std::string> columns, std::vector<db::LibMariaDBResultSet::Row> rows) -> std::unique_ptr<db::ResultSet>
{
    auto schema   = std::make_shared<db::ColumnSchema>();
    schema->names = std::move(columns);
    for (std::size_t i = 0; i < schema->names.size(); ++i)
    {
        schema->index[schema->names[i]] = i;
    }
    return std::make_unique<db::LibMariaDBResultSet>(std::move(query), std::move(schema), std::move(rows));
}

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>& params) -> std::unique_ptr<db::ResultSet> override
    {
        queries.push_back(query);
        bindings.push_back(params);

        if (query == "SELECT @@autocommit")
        {
            return selectResult(query, { "@@autocommit" }, { { static_cast<uint64>(1) } });
        }
        if (query == "SELECT LAST_INSERT_ID() AS id")
        {
            auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
            if (insertId)
            {
                rows.push_back({ static_cast<uint64>(*insertId) });
            }
            return selectResult(query, { "id" }, std::move(rows));
        }
        if (query == pendingQuery)
        {
            auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
            if (pending)
            {
                rows.push_back({ static_cast<uint64>(pending->first), pending->second });
            }
            return selectResult(query, { "id", "response" }, std::move(rows));
        }
        if (failTransactionStart && query == "START TRANSACTION")
        {
            return nullptr;
        }
        return std::make_unique<db::LibMariaDBResultSet>(1, query);
    }

    auto getSchema() -> std::string override
    {
        return "xidb";
    }

    auto getVersion() -> std::string override
    {
        return "fake";
    }

    auto getDriverVersion() -> std::string override
    {
        return "fake";
    }

    std::optional<uint32_t>                         insertId;
    std::optional<std::pair<uint32_t, std::string>> pending;
    bool                                            failTransactionStart = false;
    std::vector<std::string>                        queries;
    std::vector<std::vector<db::BoundValue>>        bindings;
};

struct DatabaseGuard
{
    ~DatabaseGuard()
    {
        db::setDatabase(nullptr);
    }
};

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "GM call persistence 1243 self-test failed: " << label << '\n';
    }
    return condition;
}

auto boundUint32(const db::BoundValue& value, uint32_t expected) -> bool
{
    return std::holds_alternative<uint32>(value) && std::get<uint32>(value) == expected;
}

auto boundString(const db::BoundValue& value, const std::string& expected) -> bool
{
    return std::holds_alternative<std::string>(value) && std::get<std::string>(value) == expected;
}

auto testPersistCallTransactionAndBindings() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.insertId = 4242;
    db::setDatabase(&fake);

    const auto message         = std::string{ "line\0tail", 9 };
    const auto callId          = gmcall::detail::PersistCall(77, message);
    const auto expectedQueries = std::vector<std::string>{
        "SELECT @@autocommit",
        "SET @@autocommit = ?",
        "START TRANSACTION",
        "INSERT INTO help_desk (charid, message) VALUES (?, ?)",
        "SELECT LAST_INSERT_ID() AS id",
        "COMMIT",
        "SET @@autocommit = ?",
    };

    return expect(callId == 4242, "generated ID") &&
           expect(fake.queries == expectedQueries, "transaction query order") &&
           expect(fake.bindings[3].size() == 2 && boundUint32(fake.bindings[3][0], 77) && boundString(fake.bindings[3][1], message), "insert bindings");
}

auto testPersistCallMissingIDReturnsZero() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    db::setDatabase(&fake);

    return expect(gmcall::detail::PersistCall(88, "missing") == 0, "missing generated ID") &&
           expect(fake.queries.size() == 7, "missing ID still commits transaction");
}

auto testPersistCallSetupFailureSkipsInsert() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.insertId             = 99;
    fake.failTransactionStart = true;
    db::setDatabase(&fake);

    const auto callId = gmcall::detail::PersistCall(89, "not inserted");
    return expect(callId == 0, "transaction setup failure ID") &&
           expect(fake.queries == std::vector<std::string>{ "SELECT @@autocommit", "SET @@autocommit = ?", "START TRANSACTION", "SET @@autocommit = ?" },
                  "transaction setup failure query order");
}

auto testOldestPendingResponse() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    const auto    response = std::string{ "reply\0tail", 10 };
    fake.pending           = std::pair<uint32_t, std::string>{ 91, response };
    db::setDatabase(&fake);

    const auto found = gmcall::detail::OldestPendingResponse(33);
    fake.pending.reset();
    const auto missing = gmcall::detail::OldestPendingResponse(34);

    return expect(found && found->callId == 91 && found->response == response, "pending response row") &&
           expect(!missing, "missing pending response") &&
           expect(fake.queries == std::vector<std::string>{ pendingQuery, pendingQuery }, "pending response query") &&
           expect(boundUint32(fake.bindings[0][0], 33) && boundUint32(fake.bindings[1][0], 34), "pending response bindings");
}

auto testAcknowledgeOldestResponse() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    db::setDatabase(&fake);

    gmcall::detail::AcknowledgeOldestResponse(55);
    return expect(fake.queries == std::vector<std::string>{ acknowledgeQuery }, "acknowledgment query") &&
           expect(fake.bindings.size() == 1 && fake.bindings[0].size() == 1 && boundUint32(fake.bindings[0][0], 55), "acknowledgment binding");
}

} // namespace

auto runGMCallPersistence1243SelfTests() -> bool
{
    bool ok = true;
    ok      = testPersistCallTransactionAndBindings() && ok;
    ok      = testPersistCallMissingIDReturnsZero() && ok;
    ok      = testPersistCallSetupFailureSkipsInsert() && ok;
    ok      = testOldestPendingResponse() && ok;
    ok      = testAcknowledgeOldestResponse() && ok;
    return ok;
}
