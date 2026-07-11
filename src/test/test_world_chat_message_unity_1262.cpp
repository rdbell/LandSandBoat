#include "test_world_chat_message_unity_1262.h"

#include "world/chat_message_unity.h"
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

constexpr auto unityEndpointsQuery = "SELECT server_addr, server_port FROM accounts_sessions "
                                     "WHERE unitychat = ? GROUP BY server_addr, server_port";

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageUnity 1262 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::ChatMessageUnity& left, const ipc::ChatMessageUnity& right) -> bool
{
    return left.unityLeaderId == right.unityLeaderId && left.senderId == right.senderId && left.senderName == right.senderName &&
           left.message == right.message && left.zoneId == right.zoneId && left.gmLevel == right.gmLevel &&
           left.messageType == right.messageType;
}

auto testFanout() -> bool
{
    const ipc::ChatMessageUnity message{
        .unityLeaderId = std::numeric_limits<uint32>::max(),
        .senderId      = 0,
        .senderName    = "Sender",
        .message       = "payload",
        .zoneId        = std::numeric_limits<uint16>::max(),
        .gmLevel       = std::numeric_limits<uint8>::max(),
        .messageType   = MESSAGE_SYSTEM_3,
    };
    const std::vector<IPP> endpoints{ IPP{}, IPP(0x100F0E0D, 34567), IPP{} };
    int                    lookupCalls{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::ChatMessageUnity> sentMessages{};
    worldipc::HandleChatMessageUnity(
        message,
        [&](const uint32 unityId)
        {
            ++lookupCalls;
            return unityId == message.unityLeaderId ? endpoints : std::vector<IPP>{};
        },
        [&](const IPP& endpoint, const ipc::ChatMessageUnity& delivered)
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
    worldipc::HandleChatMessageUnity(
        ipc::ChatMessageUnity{},
        [&](const uint32 unityId)
        {
            ++emptyLookups;
            return unityId == 0 ? std::vector<IPP>{} : endpoints;
        },
        [&](const IPP&, const ipc::ChatMessageUnity&) { ++emptySends; });
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
    return std::make_unique<db::LibMariaDBResultSet>(unityEndpointsQuery, std::move(schema), std::move(rows));
}

auto testDatabaseLookup() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(endpointRows({ { uint64{ 0x100F0E0D }, uint64{ 34567 } }, { uint64{}, uint64{} } }));
    fake.results.push_back(endpointRows({}));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto found   = world::ipc::LookupUnityEndpoints(std::numeric_limits<uint32>::max());
    const auto missing = world::ipc::LookupUnityEndpoints(0);
    const auto failed  = world::ipc::LookupUnityEndpoints(7);
    const auto bound = [&](const std::size_t index, const uint32 expected)
    {
        return fake.bindings[index].size() == 1 && std::holds_alternative<uint32>(fake.bindings[index][0]) &&
               std::get<uint32>(fake.bindings[index][0]) == expected;
    };

    return expect(found.size() == 2 && found[0].getIP() == 0x100F0E0D && found[0].getPort() == 34567 &&
                      found[1].getRawIPP() == 0,
                  "ordered database rows and zero endpoint") &&
           expect(missing.empty() && failed.empty(), "empty and failed results") &&
           expect(fake.queries == std::vector<std::string>{ unityEndpointsQuery, unityEndpointsQuery, unityEndpointsQuery },
                  "exact unity SQL") &&
           expect(bound(0, std::numeric_limits<uint32>::max()) && bound(1, 0) && bound(2, 7), "exact unity id bindings");
}

} // namespace

auto runWorldChatMessageUnity1262SelfTests() -> bool
{
    return testFanout() && testDatabaseLookup();
}
