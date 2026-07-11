#include "test_world_kill_session_1280.h"

#include "world/ipc_lookup.h"
#include "world/kill_session.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

namespace
{

constexpr auto killSessionZonesQuery = "SELECT pos_prevzone, pos_zone from chars where charid = ? LIMIT 1";

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world KillSession 1280 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testRouting() -> bool
{
    const ipc::KillSession message{ .victimId = std::numeric_limits<uint32>::max() };
    int zoneLookups{};
    int endpointLookups{};
    int catalogLookups{};
    std::vector<IPP> sentEndpoints{};
    std::vector<ipc::KillSession> sentMessages{};
    worldipc::HandleKillSession(
        message,
        [&](uint32 victimId) -> Maybe<worldipc::KillSessionZones>
        {
            ++zoneLookups;
            return victimId == message.victimId ? Maybe<worldipc::KillSessionZones>{ { 0x10001, 2 } } : std::nullopt;
        },
        [&](uint16 previousZoneId)
        {
            ++endpointLookups;
            return previousZoneId == 1 ? IPP{} : IPP{ 1, 1 };
        },
        [&]() -> std::vector<IPP>
        {
            ++catalogLookups;
            return { IPP{ 2, 2 } };
        },
        [&](const IPP& endpoint, const ipc::KillSession& delivered)
        {
            sentEndpoints.push_back(endpoint);
            sentMessages.push_back(delivered);
        });

    int equalEndpointLookups{};
    int equalCatalogLookups{};
    int equalSends{};
    worldipc::HandleKillSession(
        ipc::KillSession{},
        [](uint32) -> Maybe<worldipc::KillSessionZones> { return worldipc::KillSessionZones{ 7, 7 }; },
        [&](uint16) { ++equalEndpointLookups; return IPP{}; },
        [&]() -> std::vector<IPP> { ++equalCatalogLookups; return {}; },
        [&](const IPP&, const ipc::KillSession&) { ++equalSends; });

    const std::vector<IPP> broadcast{ IPP{}, IPP{ 3, 30 }, IPP{} };
    int missingZoneLookups{};
    int missingEndpointLookups{};
    int missingCatalogLookups{};
    std::vector<IPP> broadcastEndpoints{};
    worldipc::HandleKillSession(
        ipc::KillSession{ .victimId = 9 },
        [&](uint32 victimId) -> Maybe<worldipc::KillSessionZones>
        {
            ++missingZoneLookups;
            return victimId == 9 ? std::nullopt : Maybe<worldipc::KillSessionZones>{};
        },
        [&](uint16) { ++missingEndpointLookups; return IPP{}; },
        [&]()
        {
            ++missingCatalogLookups;
            return broadcast;
        },
        [&](const IPP& endpoint, const ipc::KillSession&) { broadcastEndpoints.push_back(endpoint); });

    return expect(zoneLookups == 1 && endpointLookups == 1 && catalogLookups == 0, "different zones use truncated previous zone") &&
           expect(sentEndpoints.size() == 1 && sentEndpoints[0].getRawIPP() == 0 && sentMessages[0].victimId == message.victimId,
                  "different zones send full payload to all-zero endpoint") &&
           expect(equalEndpointLookups == 0 && equalCatalogLookups == 0 && equalSends == 0, "equal zones send nothing") &&
           expect(missingZoneLookups == 1 && missingEndpointLookups == 0 && missingCatalogLookups == 1 && broadcastEndpoints == broadcast,
                  "missing row broadcasts in catalog order with duplicates");
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

    std::vector<std::string> queries{};
    std::vector<std::vector<db::BoundValue>> bindings{};
    std::vector<std::unique_ptr<db::ResultSet>> results{};
};

struct DatabaseGuard
{
    ~DatabaseGuard() { db::setDatabase(nullptr); }
};

auto zoneRows(std::vector<db::LibMariaDBResultSet::Row> rows) -> std::unique_ptr<db::ResultSet>
{
    auto schema = std::make_shared<db::ColumnSchema>();
    schema->names = { "pos_prevzone", "pos_zone" };
    schema->index["pos_prevzone"] = 0;
    schema->index["pos_zone"] = 1;
    return std::make_unique<db::LibMariaDBResultSet>(killSessionZonesQuery, std::move(schema), std::move(rows));
}

auto testDatabaseLookup() -> bool
{
    DatabaseGuard guard;
    FakeDatabase fake;
    fake.results.push_back(zoneRows({ { uint64{ 0x10001 }, uint64{ 2 } } }));
    fake.results.push_back(zoneRows({}));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    const auto found = world::ipc::LookupKillSessionZones(std::numeric_limits<uint32>::max());
    const auto missing = world::ipc::LookupKillSessionZones(0);
    const auto failed = world::ipc::LookupKillSessionZones(7);
    const auto bound = [&](std::size_t index, uint32 expected)
    {
        return fake.bindings[index].size() == 1 && std::holds_alternative<uint32>(fake.bindings[index][0]) &&
               std::get<uint32>(fake.bindings[index][0]) == expected;
    };

    return expect(found && found->previous == 0x10001 && found->current == 2, "database row preserves uint32 zones") &&
           expect(!missing && !failed, "missing and failed lookup are absent") &&
           expect(fake.queries == std::vector<std::string>{ killSessionZonesQuery, killSessionZonesQuery, killSessionZonesQuery },
                  "exact KillSession SQL") &&
           expect(bound(0, std::numeric_limits<uint32>::max()) && bound(1, 0) && bound(2, 7), "victim ID bound once");
}

} // namespace

auto runWorldKillSession1280SelfTests() -> bool
{
    return testRouting() && testDatabaseLookup();
}
