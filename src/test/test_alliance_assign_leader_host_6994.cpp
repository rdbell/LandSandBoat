#include "test_alliance_assign_leader_host_6994.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"

#include <iostream>

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
        std::cerr << "alliance assign leader host 6994 self-test failed: " << label << '\n';
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

// Direct CAlliance::assignAllianceLeader characterization (slice 6994). A
// successful DB lookup promotes the named party leader, rewrites the alliance
// ID and restores the local main party when that leader is on this process.
auto runAllianceAssignLeaderHost6994SelfTests() -> bool
{
    db::SQLiteDatabase database("file:alliance_assign_leader_6994?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, charname TEXT NOT NULL);
        CREATE TABLE accounts_sessions (charid INTEGER NOT NULL);
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO chars (charid, charname) VALUES (101, 'Successor');
        INSERT INTO accounts_sessions (charid) VALUES (101);
        INSERT INTO accounts_parties (charid, allianceid, partyflag) VALUES (101, 700, 4);
        INSERT INTO accounts_parties (charid, allianceid, partyflag) VALUES (102, 700, 8);
    )sql");

    CAlliance    alliance(700);
    CParty       previousMain(10);
    CParty       successorParty(20);
    CCharEntity  successor;
    successor.name = "Successor";
    successorParty.members = { &successor };
    alliance.partyList = { &previousMain, &successorParty };
    alliance.setMainParty(&previousMain);

    alliance.assignAllianceLeader("Successor");

    const auto successorRow = db::preparedStmt("SELECT allianceid, partyflag FROM accounts_parties WHERE charid = ?", 101);
    const auto formerRow    = db::preparedStmt("SELECT allianceid, partyflag FROM accounts_parties WHERE charid = ?", 102);
    const bool successorPersisted = successorRow && successorRow->rowsCount() == 1 && successorRow->next() &&
                                    successorRow->get<uint32>("allianceid") == 101 &&
                                    (successorRow->get<uint16>("partyflag") & ALLIANCE_LEADER) != 0;
    const bool formerLeaderCleared = formerRow && formerRow->rowsCount() == 1 && formerRow->next() &&
                                     formerRow->get<uint32>("allianceid") == 101 &&
                                     (formerRow->get<uint16>("partyflag") & ALLIANCE_LEADER) == 0;

    return expect(alliance.m_AllianceID == 101, "alliance ID follows new leader") &&
           expect(alliance.getMainParty() == &successorParty, "local successor becomes main party") &&
           expect(successorPersisted, "successor ID and alliance flag persisted") &&
           expect(formerLeaderCleared, "former alliance leader flag cleared");
}
