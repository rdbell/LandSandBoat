#include "test_alliance_remove_party_host_6993.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"

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
        std::cerr << "alliance remove party host 6993 self-test failed: " << label << '\n';
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

// Direct CAlliance::removeParty characterization (slice 6993). A non-main
// party is detached through delParty while the alliance lead remains attached.
auto runAllianceRemovePartyHost6993SelfTests() -> bool
{
    db::SQLiteDatabase database("file:alliance_remove_party_6993?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES (1, 20, 1, 31);
    )sql");

    CAlliance alliance(1);
    CParty    mainParty(10);
    CParty    leavingParty(20);

    mainParty.m_PartyType = PARTY_MOBS; // avoids unrelated PC reload host work
    alliance.partyList = { &mainParty, &leavingParty };
    mainParty.m_PAlliance = &alliance;
    leavingParty.m_PAlliance = &alliance;
    alliance.setMainParty(&mainParty);
    leavingParty.m_PartyNumber = 2;

    alliance.removeParty(&leavingParty);

    const auto persistedParty = db::preparedStmt("SELECT allianceid, partyflag FROM accounts_parties WHERE partyid = ?", 20);

    return expect(alliance.partyList.size() == 1, "leaving party erased") &&
           expect(alliance.partyList.front() == &mainParty, "main party remains") &&
           expect(leavingParty.m_PAlliance == nullptr, "leaving party detached") &&
           expect(leavingParty.m_PartyNumber == 0, "leaving party number reset") &&
           expect(alliance.getMainParty() == &mainParty, "main party retained") &&
           expect(persistedParty && persistedParty->next() && persistedParty->get<uint32>("allianceid") == 0 &&
                      persistedParty->get<uint16>("partyflag") == (PARTY_LEADER | PARTY_QM),
                  "leaving party's alliance flags are cleared");
}
