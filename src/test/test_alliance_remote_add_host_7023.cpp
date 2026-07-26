#include "test_alliance_remote_add_host_7023.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"

#define private public
#include "map/alliance.h"
#undef private
#include "map/party.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance remote add host 7023 self-test failed: " << label << '\n';
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

// Direct CAlliance::addParty(uint32) characterization (slice 7023). A remote
// party receives the first available persisted alliance slot while preserving
// its unrelated party flags.
auto runAllianceRemoteAddHost7023SelfTests() -> bool
{
    db::SQLiteDatabase database("file:alliance_remote_add_7023?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES
            (1, 10, 700, 0),
            (2, 20, 700, 2),
            (3, 30, 0, 4);
    )sql");

    CAlliance alliance(700);
    alliance.addParty(30);

    const auto remoteParty = db::preparedStmt("SELECT allianceid, partyflag FROM accounts_parties WHERE partyid = ?", 30);
    const bool assignedFirstGap = remoteParty && remoteParty->rowsCount() == 1 && remoteParty->next() &&
                                  remoteParty->get<uint32>("allianceid") == 700 &&
                                  remoteParty->get<uint16>("partyflag") == (PARTY_LEADER | PARTY_SECOND);

    return expect(assignedFirstGap, "remote party fills first persisted alliance slot");
}
