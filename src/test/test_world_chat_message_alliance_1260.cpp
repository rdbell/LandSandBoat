#include "test_world_chat_message_alliance_1260.h"

#include "world/chat_message_alliance.h"
#include "world/ipc_lookup.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{

constexpr auto allianceEndpointsQuery =
    "SELECT server_addr, server_port, MIN(charid) FROM accounts_sessions JOIN accounts_parties USING (charid) "
    "WHERE allianceid = ? GROUP BY server_addr, server_port";

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageAlliance 1260 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::ChatMessageAlliance& left, const ipc::ChatMessageAlliance& right) -> bool
{
    return left.allianceId == right.allianceId && left.senderId == right.senderId && left.senderName == right.senderName &&
           left.message == right.message && left.zoneId == right.zoneId && left.gmLevel == right.gmLevel &&
           left.messageType == right.messageType;
}

auto testFanout() -> bool
{
    const ipc::ChatMessageAlliance message{
        .allianceId  = std::numeric_limits<uint32>::max(),
        .senderId    = 0,
        .senderName  = "Sender",
        .message     = "payload",
        .zoneId      = std::numeric_limits<uint16>::max(),
        .gmLevel     = std::numeric_limits<uint8>::max(),
        .messageType = MESSAGE_SYSTEM_3,
    };
    const std::vector<IPP> endpoints{ IPP{}, IPP(0x08070605, 12345), IPP{} };
    int                    lookupCalls{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::ChatMessageAlliance> sentMessages{};

    worldipc::HandleChatMessageAlliance(
        message,
        [&](const uint32 allianceId)
        {
            ++lookupCalls;
            return allianceId == message.allianceId ? endpoints : std::vector<IPP>{};
        },
        [&](const IPP& endpoint, const ipc::ChatMessageAlliance& delivered)
        {
            sentEndpoints.push_back(endpoint);
            sentMessages.push_back(delivered);
        });

    bool exact = lookupCalls == 1 && sentEndpoints.size() == endpoints.size() && sentMessages.size() == endpoints.size();
    for (std::size_t index = 0; exact && index < endpoints.size(); ++index)
    {
        exact = sentEndpoints[index].getRawIPP() == endpoints[index].getRawIPP() && sameMessage(sentMessages[index], message);
    }

    int emptyLookups{};
    int emptySends{};
    worldipc::HandleChatMessageAlliance(
        ipc::ChatMessageAlliance{},
        [&](const uint32 allianceId)
        {
            ++emptyLookups;
            return allianceId == 0 ? std::vector<IPP>{} : endpoints;
        },
        [&](const IPP&, const ipc::ChatMessageAlliance&) { ++emptySends; });
    return expect(exact, "exact lookup and ordered unchanged fanout") &&
           expect(emptyLookups == 1 && emptySends == 0, "empty fanout still looks up once");
}

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
    auto getSchema() -> std::string override { return "xidb"; }
    auto getVersion() -> std::string override { return "fake"; }
    auto getDriverVersion() -> std::string override { return "fake"; }

    std::vector<std::string>                    queries{};
    std::vector<std::vector<db::BoundValue>>    bindings{};
    std::vector<std::unique_ptr<db::ResultSet>> results{};
};

struct DatabaseGuard
{
    ~DatabaseGuard() { db::setDatabase(nullptr); }
};

auto endpointRows(std::vector<db::LibMariaDBResultSet::Row> rows) -> std::unique_ptr<db::ResultSet>
{
    auto schema                  = std::make_shared<db::ColumnSchema>();
    schema->names                = { "server_addr", "server_port" };
    schema->index["server_addr"] = 0;
    schema->index["server_port"] = 1;
    return std::make_unique<db::LibMariaDBResultSet>(allianceEndpointsQuery, std::move(schema), std::move(rows));
}

auto testDatabaseLookup() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(endpointRows({ { uint64{ 0x08070605 }, uint64{ 12345 } }, { uint64{}, uint64{} } }));
    fake.results.push_back(endpointRows({}));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto found   = world::ipc::LookupAllianceEndpoints(std::numeric_limits<uint32>::max());
    const auto missing = world::ipc::LookupAllianceEndpoints(0);
    const auto failed  = world::ipc::LookupAllianceEndpoints(7);
    const auto bound = [&](const std::size_t index, const uint32 expected)
    {
        return fake.bindings[index].size() == 1 && std::holds_alternative<uint32>(fake.bindings[index][0]) &&
               std::get<uint32>(fake.bindings[index][0]) == expected;
    };

    return expect(found.size() == 2 && found[0].getIP() == 0x08070605 && found[0].getPort() == 12345 &&
                      found[1].getRawIPP() == 0,
                  "ordered database rows and zero endpoint") &&
           expect(missing.empty() && failed.empty(), "empty and failed results") &&
           expect(fake.queries == std::vector<std::string>{ allianceEndpointsQuery, allianceEndpointsQuery, allianceEndpointsQuery },
                  "exact alliance SQL") &&
           expect(bound(0, std::numeric_limits<uint32>::max()) && bound(1, 0) && bound(2, 7), "exact uint32 bindings");
}

} // namespace

auto runWorldChatMessageAlliance1260SelfTests() -> bool
{
    return testFanout() && testDatabaseLookup();
}
