#include "test_party_remote_add_host_7020.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party remote add host 7020 self-test failed: " << label << '\n';
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

// Direct CParty::AddMember(uint32) characterization (slice 7020). A remote
// PC member is persisted without becoming a local roster entity; the remote
// capacity check prevents an insert once the persisted party reaches six.
auto runPartyRemoteAddHost7020SelfTests() -> bool
{
    db::SQLiteDatabase database("file:party_remote_add_7020?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
    )sql");

    CParty party(700);
    party.AddMember(101);
    const auto inserted = db::preparedStmt("SELECT partyid, allianceid, partyflag FROM accounts_parties WHERE charid = ?", 101);
    const bool remoteMemberPersisted = inserted && inserted->rowsCount() == 1 && inserted->next() &&
                                       inserted->get<uint32>("partyid") == 700 &&
                                       inserted->get<uint32>("allianceid") == 0 &&
                                       inserted->get<uint16>("partyflag") == 0 && party.members.empty();

    database.executeScript(R"sql(
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES
            (1, 700, 0, 0), (2, 700, 0, 0), (3, 700, 0, 0), (4, 700, 0, 0), (5, 700, 0, 0);
    )sql");
    party.AddMember(102);
    const auto fullPartyInsert = db::preparedStmt("SELECT COUNT(*) FROM accounts_parties WHERE charid = ?", 102);
    const bool remoteFullRejects = fullPartyInsert && fullPartyInsert->rowsCount() == 1 && fullPartyInsert->next() &&
                                   fullPartyInsert->get<uint32>(0) == 0;

    return expect(remoteMemberPersisted, "remote member persists with plain party flags") &&
           expect(remoteFullRejects, "full remote party rejects insert");
}
