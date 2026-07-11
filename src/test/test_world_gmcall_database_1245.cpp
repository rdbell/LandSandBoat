#include "test_world_gmcall_database_1245.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "world/gmcall_response_persistence.h"
#include "world/ipc_lookup.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{

constexpr auto responseQuery = "UPDATE help_desk SET response = ?, responded_at = NOW() WHERE id = ?";
constexpr auto endpointQuery = "SELECT server_addr, server_port FROM accounts_sessions WHERE charid = ? LIMIT 1";
constexpr auto nameEndpointQuery = "SELECT server_addr, server_port FROM accounts_sessions LEFT JOIN chars ON "
                                   "accounts_sessions.charid = chars.charid WHERE charname = ? LIMIT 1";

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>& params) -> std::unique_ptr<db::ResultSet> override
    {
        queries.push_back(query);
        bindings.push_back(params);
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
        return "fake";
    }

    auto getDriverVersion() -> std::string override
    {
        return "fake";
    }

    std::vector<std::string>                    queries;
    std::vector<std::vector<db::BoundValue>>    bindings;
    std::vector<std::unique_ptr<db::ResultSet>> results;
};

struct DatabaseGuard
{
    ~DatabaseGuard()
    {
        db::setDatabase(nullptr);
    }
};

auto updateResult(const std::size_t affected = 1) -> std::unique_ptr<db::ResultSet>
{
    return std::make_unique<db::LibMariaDBResultSet>(affected, responseQuery);
}

auto endpointResultFor(const std::string& query, const bool found, const uint32_t ip = 0, const uint16_t port = 0)
    -> std::unique_ptr<db::ResultSet>
{
    auto schema                  = std::make_shared<db::ColumnSchema>();
    schema->names                = { "server_addr", "server_port" };
    schema->index["server_addr"] = 0;
    schema->index["server_port"] = 1;
    auto rows                    = std::vector<db::LibMariaDBResultSet::Row>{};
    if (found)
    {
        rows.push_back({ static_cast<uint64>(ip), static_cast<uint64>(port) });
    }
    return std::make_unique<db::LibMariaDBResultSet>(query, std::move(schema), std::move(rows));
}

auto endpointResult(const bool found, const uint32_t ip = 0, const uint16_t port = 0) -> std::unique_ptr<db::ResultSet>
{
    return endpointResultFor(endpointQuery, found, ip, port);
}

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world GM-call database 1245 self-test failed: " << label << '\n';
    }
    return condition;
}

auto boundUint32(const db::BoundValue& value, const uint32_t expected) -> bool
{
    return std::holds_alternative<uint32>(value) && std::get<uint32>(value) == expected;
}

auto boundString(const db::BoundValue& value, const std::string& expected) -> bool
{
    return std::holds_alternative<std::string>(value) && std::get<std::string>(value) == expected;
}

auto testResponsePersistence() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(updateResult());
    fake.results.push_back(updateResult(0));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto response = std::string{ "reply\0tail", 10 };
    world::gmcall::PersistResponse(0xFEDCBA98, response);
    world::gmcall::PersistResponse(7, "zero affected");
    world::gmcall::PersistResponse(8, "failed");

    return expect(fake.queries == std::vector<std::string>{ responseQuery, responseQuery, responseQuery }, "response queries") &&
           expect(fake.bindings.size() == 3 && fake.bindings[0].size() == 2 &&
                      boundString(fake.bindings[0][0], response) && boundUint32(fake.bindings[0][1], 0xFEDCBA98),
                  "response bindings") &&
           expect(boundString(fake.bindings[1][0], "zero affected") && boundUint32(fake.bindings[1][1], 7),
                  "zero-affected bindings") &&
           expect(boundString(fake.bindings[2][0], "failed") && boundUint32(fake.bindings[2][1], 8),
                  "failed-update bindings");
}

auto testCharacterEndpointLookup() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(endpointResult(true, 0x04030201, 54321));
    fake.results.push_back(endpointResult(true, 0x08070605, 12345));
    fake.results.push_back(endpointResult(true));
    fake.results.push_back(endpointResult(false));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto found     = world::ipc::LookupCharacterEndpoint(10);
    const auto refreshed = world::ipc::LookupCharacterEndpoint(10);
    const auto zero      = world::ipc::LookupCharacterEndpoint(11);
    const auto missing   = world::ipc::LookupCharacterEndpoint(12);
    const auto failed    = world::ipc::LookupCharacterEndpoint(13);

    return expect(found && found->getIP() == 0x04030201 && found->getPort() == 54321, "endpoint row") &&
           expect(refreshed && refreshed->getIP() == 0x08070605 && refreshed->getPort() == 12345, "same-character database refresh") &&
           expect(zero && zero->getRawIPP() == 0, "zero endpoint row remains present") &&
           expect(!missing, "empty endpoint result") &&
           expect(!failed, "failed endpoint query") &&
           expect(fake.queries == std::vector<std::string>{ endpointQuery, endpointQuery, endpointQuery, endpointQuery, endpointQuery }, "endpoint queries") &&
           expect(boundUint32(fake.bindings[0][0], 10) && boundUint32(fake.bindings[1][0], 10) &&
                      boundUint32(fake.bindings[2][0], 11) && boundUint32(fake.bindings[3][0], 12) &&
                      boundUint32(fake.bindings[4][0], 13),
                  "endpoint bindings");
}

auto testCharacterNameEndpointLookup() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(endpointResultFor(nameEndpointQuery, true, 0x04030201, 54321));
    fake.results.push_back(endpointResultFor(nameEndpointQuery, true));
    fake.results.push_back(endpointResultFor(nameEndpointQuery, false));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto found   = world::ipc::LookupCharacterNameEndpoint("");
    const auto zero    = world::ipc::LookupCharacterNameEndpoint("Zero");
    const auto missing = world::ipc::LookupCharacterNameEndpoint("Missing");
    const auto failed  = world::ipc::LookupCharacterNameEndpoint("Failed");

    return expect(found && found->getIP() == 0x04030201 && found->getPort() == 54321, "name endpoint row") &&
           expect(zero && zero->getRawIPP() == 0, "zero name endpoint row remains present") &&
           expect(!missing, "empty name endpoint result") &&
           expect(!failed, "failed name endpoint query") &&
           expect(fake.queries == std::vector<std::string>{ nameEndpointQuery, nameEndpointQuery, nameEndpointQuery, nameEndpointQuery },
                  "name endpoint queries") &&
           expect(boundString(fake.bindings[0][0], "") && boundString(fake.bindings[1][0], "Zero") &&
                      boundString(fake.bindings[2][0], "Missing") && boundString(fake.bindings[3][0], "Failed"),
                  "name endpoint bindings");
}

} // namespace

auto runWorldGMCallDatabase1245SelfTests() -> bool
{
    bool ok = true;
    ok      = testResponsePersistence() && ok;
    ok      = testCharacterEndpointLookup() && ok;
    ok      = testCharacterNameEndpointLookup() && ok;
    return ok;
}
