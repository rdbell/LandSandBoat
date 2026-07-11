#include "test_world_chat_message_party_1259.h"

#include "world/chat_message_party.h"
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

constexpr auto partyEndpointsQuery =
    "SELECT server_addr, server_port, MIN(charid) FROM accounts_sessions JOIN accounts_parties USING (charid) "
    "WHERE IF (allianceid <> 0, allianceid = (SELECT MAX(allianceid) FROM accounts_parties WHERE partyid = ?), "
    "partyid = ?) GROUP BY server_addr, server_port";

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageParty 1259 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::ChatMessageParty& left, const ipc::ChatMessageParty& right) -> bool
{
    return left.partyId == right.partyId && left.senderId == right.senderId && left.senderName == right.senderName &&
           left.message == right.message && left.zoneId == right.zoneId && left.gmLevel == right.gmLevel &&
           left.messageType == right.messageType;
}

auto testFanoutPreservesLookupAndEndpointOrder() -> bool
{
    const ipc::ChatMessageParty message{
        .partyId     = std::numeric_limits<uint32>::max(),
        .senderId    = 0,
        .senderName  = "Sender",
        .message     = "payload",
        .zoneId      = std::numeric_limits<uint16>::max(),
        .gmLevel     = std::numeric_limits<uint8>::max(),
        .messageType = MESSAGE_SYSTEM_3,
    };
    const std::vector<IPP> endpoints{ IPP{}, IPP(0x04030201, 54321), IPP{} };
    int                    lookupCalls{};
    uint32                 lookedUpParty{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::ChatMessageParty> sentMessages{};

    worldipc::HandleChatMessageParty(
        message,
        [&](const uint32 partyId)
        {
            ++lookupCalls;
            lookedUpParty = partyId;
            return endpoints;
        },
        [&](const IPP& endpoint, const ipc::ChatMessageParty& delivered)
        {
            sentEndpoints.push_back(endpoint);
            sentMessages.push_back(delivered);
        });

    bool unchanged = sentMessages.size() == endpoints.size();
    for (const auto& delivered : sentMessages)
    {
        unchanged = unchanged && sameMessage(delivered, message);
    }
    bool endpointOrder = sentEndpoints.size() == endpoints.size();
    for (std::size_t index = 0; endpointOrder && index < endpoints.size(); ++index)
    {
        endpointOrder = sentEndpoints[index].getRawIPP() == endpoints[index].getRawIPP();
    }
    return expect(lookupCalls == 1 && lookedUpParty == message.partyId, "exact party lookup once") &&
           expect(endpointOrder, "endpoint order and duplicate zero endpoints preserved") &&
           expect(unchanged, "unchanged party message per endpoint");
}

auto testEmptyFanoutSendsNothing() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleChatMessageParty(
        ipc::ChatMessageParty{},
        [&](const uint32 partyId)
        {
            ++lookupCalls;
            return partyId == 0 ? std::vector<IPP>{} : std::vector<IPP>{ IPP{} };
        },
        [&](const IPP&, const ipc::ChatMessageParty&)
        {
            ++sendCalls;
        });
    return expect(lookupCalls == 1 && sendCalls == 0, "empty endpoint list sends nothing");
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
    return std::make_unique<db::LibMariaDBResultSet>(partyEndpointsQuery, std::move(schema), std::move(rows));
}

auto boundUint32(const db::BoundValue& value, const uint32 expected) -> bool
{
    return std::holds_alternative<uint32>(value) && std::get<uint32>(value) == expected;
}

auto testDatabaseLookup() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(endpointRows({ { uint64{ 0x04030201 }, uint64{ 54321 } }, { uint64{}, uint64{} } }));
    fake.results.push_back(endpointRows({}));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto found   = world::ipc::LookupPartyEndpoints(std::numeric_limits<uint32>::max());
    const auto missing = world::ipc::LookupPartyEndpoints(0);
    const auto failed  = world::ipc::LookupPartyEndpoints(7);

    return expect(found.size() == 2 && found[0].getIP() == 0x04030201 && found[0].getPort() == 54321 &&
                      found[1].getRawIPP() == 0,
                  "database rows preserve order and zero endpoint") &&
           expect(missing.empty() && failed.empty(), "empty and failed query results") &&
           expect(fake.queries == std::vector<std::string>{ partyEndpointsQuery, partyEndpointsQuery, partyEndpointsQuery },
                  "exact party endpoint SQL") &&
           expect(fake.bindings.size() == 3 && fake.bindings[0].size() == 2 &&
                      boundUint32(fake.bindings[0][0], std::numeric_limits<uint32>::max()) &&
                      boundUint32(fake.bindings[0][1], std::numeric_limits<uint32>::max()) &&
                      boundUint32(fake.bindings[1][0], 0) && boundUint32(fake.bindings[1][1], 0) &&
                      boundUint32(fake.bindings[2][0], 7) && boundUint32(fake.bindings[2][1], 7),
                  "party id bound twice on every lookup");
}

} // namespace

auto runWorldChatMessageParty1259SelfTests() -> bool
{
    return testFanoutPreservesLookupAndEndpointOrder() && testEmptyFanoutSendsNothing() && testDatabaseLookup();
}
