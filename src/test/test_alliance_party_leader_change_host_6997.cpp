#include "test_alliance_party_leader_change_host_6997.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"

#include <iostream>

#define private public
#include "map/alliance.h"
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance party leader change host 6997 self-test failed: " << label << '\n';
    }
    return condition;
}

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

// Direct CParty::SetLeader characterization with a local CAlliance (slice
// 6997). The alliance keeps the same party pointer while its identity follows
// the new leader when the old party was the alliance ID.
auto runAlliancePartyLeaderChangeHost6997SelfTests() -> bool
{
    db::SQLiteDatabase database("file:alliance_party_leader_change_6997?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, charname TEXT NOT NULL);
        CREATE TABLE accounts_sessions (charid INTEGER NOT NULL);
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO chars (charid, charname) VALUES (101, 'Successor');
        INSERT INTO accounts_sessions (charid) VALUES (101);
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES (100, 700, 700, 12);
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES (101, 700, 700, 0);
    )sql");

    CAlliance    alliance(700);
    CParty       party(700);
    CBattleEntity previousLeader;
    CBattleEntity successor;
    previousLeader.name = "Previous";
    successor.name      = "Successor";
    party.members       = { &previousLeader, &successor };
    party.m_PLeader     = &previousLeader;
    party.m_PAlliance   = &alliance;
    alliance.partyList  = { &party };
    alliance.setMainParty(&party);

    party.SetLeader("Successor");

    return expect(party.GetPartyID() == 101, "party ID follows new leader") &&
           expect(alliance.m_AllianceID == 101, "alliance ID follows party leader") &&
           expect(alliance.getMainParty() == &party, "main party pointer retained") &&
           expect(alliance.partyList.size() == 1 && alliance.partyList.front() == &party, "party list pointer retained");
}
