#include "test_alliance_dissolve_local_host_6995.h"

#include "common/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#define private public
#include "map/alliance.h"
#include "map/party.h"
#undef private

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance dissolve local host 6995 self-test failed: " << label << '\n';
    }
    return condition;
}

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>& params) -> std::unique_ptr<db::ResultSet> override
    {
        ++executeCalls;
        lastQuery  = query;
        lastParams = params;
        return std::make_unique<db::LibMariaDBResultSet>(static_cast<std::size_t>(0), query);
    }

    auto getSchema() -> std::string override
    {
        return {};
    }

    auto getVersion() -> std::string override
    {
        return {};
    }

    auto getDriverVersion() -> std::string override
    {
        return {};
    }

    int                         executeCalls = 0;
    std::string                 lastQuery;
    std::vector<db::BoundValue> lastParams;
};

class ScopedDatabase final
{
public:
    explicit ScopedDatabase(db::Database& replacement)
    : previous_(&db::getDatabase())
    {
        db::setDatabase(&replacement);
    }

    ~ScopedDatabase()
    {
        db::setDatabase(previous_);
    }

private:
    db::Database* previous_;
};

} // namespace

// Direct CAlliance::dissolveAlliance(false) characterization (slice 6995).
// The server branch detaches every local party, clears its alliance slot, and
// then destroys the alliance object.
auto runAllianceDissolveLocalHost6995SelfTests() -> bool
{
    FakeDatabase   database;
    ScopedDatabase activeDatabase(database);
    auto*          alliance = new CAlliance(1);
    CParty         first(10);
    CParty         second(20);

    first.m_PartyType    = PARTY_MOBS; // avoids unrelated PC reload host work
    second.m_PartyType   = PARTY_MOBS;
    first.m_PAlliance    = alliance;
    second.m_PAlliance   = alliance;
    first.m_PartyNumber  = 1;
    second.m_PartyNumber = 2;
    alliance->partyList  = { &first, &second };
    alliance->setMainParty(&first);

    alliance->dissolveAlliance(false);

    return expect(first.m_PAlliance == nullptr, "first party detached") &&
           expect(second.m_PAlliance == nullptr, "second party detached") &&
           expect(first.m_PartyNumber == 0, "first party number reset") &&
           expect(second.m_PartyNumber == 0, "second party number reset") &&
           expect(database.executeCalls == 1, "dissolve persistence executes once") &&
           expect(database.lastQuery == "UPDATE accounts_parties JOIN accounts_sessions USING (charid) "
                                        "SET allianceid = 0, partyflag = partyflag & ~? "
                                        "WHERE allianceid = ? AND IF(? = 0 AND ? = 0, true, server_addr = ? AND server_port = ?)",
                  "dissolve persistence query") &&
           expect(database.lastParams.size() == 6, "dissolve persistence parameter count") &&
           expect(database.lastParams.size() == 6 && std::holds_alternative<uint16>(database.lastParams[0]) &&
                      std::get<uint16>(database.lastParams[0]) == 0x000B,
                  "dissolve clear mask") &&
           expect(database.lastParams.size() == 6 && std::holds_alternative<uint32>(database.lastParams[1]) &&
                      std::get<uint32>(database.lastParams[1]) == 1,
                  "dissolve alliance ID") &&
           expect(database.lastParams.size() == 6 && std::holds_alternative<uint32>(database.lastParams[2]) &&
                      std::get<uint32>(database.lastParams[2]) == 0 && std::holds_alternative<uint16>(database.lastParams[3]) &&
                      std::get<uint16>(database.lastParams[3]) == 0 && std::holds_alternative<uint32>(database.lastParams[4]) &&
                      std::get<uint32>(database.lastParams[4]) == 0 && std::holds_alternative<uint16>(database.lastParams[5]) &&
                      std::get<uint16>(database.lastParams[5]) == 0,
                  "dissolve unfiltered endpoint bindings");
}
