#include "test_party_set_leader_host_6996.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party set leader host 6996 self-test failed: " << label << '\n';
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

// Direct CParty::SetLeader characterization (slice 6996). A successful PC
// lookup rewrites the party identity, clears old leader flags, and assigns the
// locally hosted successor as party leader.
auto runPartySetLeaderHost6996SelfTests() -> bool
{
    db::SQLiteDatabase database("file:party_set_leader_6996?mode=memory&cache=shared");
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

    CParty        party(700);
    CBattleEntity previousLeader;
    CBattleEntity successor;
    previousLeader.name = "Previous";
    successor.name      = "Successor";
    party.members       = { &previousLeader, &successor };
    party.m_PLeader     = &previousLeader;

    party.SetLeader("Successor");

    const auto successorRow = db::preparedStmt("SELECT partyid, allianceid, partyflag FROM accounts_parties WHERE charid = ?", 101);
    const auto formerRow    = db::preparedStmt("SELECT partyid, allianceid, partyflag FROM accounts_parties WHERE charid = ?", 100);
    const bool successorPersisted = successorRow && successorRow->rowsCount() == 1 && successorRow->next() &&
                                    successorRow->get<uint32>("partyid") == 101 &&
                                    successorRow->get<uint32>("allianceid") == 101 &&
                                    successorRow->get<uint16>("partyflag") == (PARTY_LEADER | ALLIANCE_LEADER);
    const bool formerLeaderCleared = formerRow && formerRow->rowsCount() == 1 && formerRow->next() &&
                                     formerRow->get<uint32>("partyid") == 101 &&
                                     formerRow->get<uint32>("allianceid") == 101 &&
                                     formerRow->get<uint16>("partyflag") == 0;

    return expect(party.GetPartyID() == 101, "party ID follows new leader") &&
           expect(party.GetLeader() == &successor, "local successor becomes leader") &&
           expect(successorPersisted, "successor party identity and flags persisted") &&
           expect(formerLeaderCleared, "former leader flags cleared");
}
