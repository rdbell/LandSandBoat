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

auto testMainPartyPromotionLookup() -> bool
{
    db::SQLiteDatabase database("file:alliance_remove_party_promote?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE accounts_sessions (charid INTEGER PRIMARY KEY, timestamp INTEGER NOT NULL);
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, charname TEXT NOT NULL);
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO chars (charid, charname) VALUES (100, 'Oldest'), (101, 'Newer');
        INSERT INTO accounts_sessions (charid, timestamp) VALUES (100, 10), (101, 20);
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES
            (10, 10, 1, 12), (100, 20, 1, 4), (101, 30, 1, 4);
    )sql");

    CAlliance alliance(1);
    CParty    departingMain(10);
    CParty    oldestCandidate(20);
    CParty    newerCandidate(30);
    departingMain.m_PartyType = PARTY_MOBS;
    alliance.partyList        = { &departingMain, &oldestCandidate, &newerCandidate };
    departingMain.m_PAlliance = &alliance;
    oldestCandidate.m_PAlliance = &alliance;
    newerCandidate.m_PAlliance  = &alliance;
    alliance.setMainParty(&departingMain);

    alliance.removeParty(&departingMain);

    return expect(alliance.m_AllianceID == 100, "oldest other-party leader becomes alliance ID") &&
           expect(alliance.getMainParty() == nullptr, "remote successor clears local main party") &&
           expect(departingMain.m_PAlliance == nullptr, "departing main party detached");
}
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
                  "leaving party's alliance flags are cleared") &&
           testMainPartyPromotionLookup();
}
