#include "test_alliance_construct_from_party_6975.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"

#include "map/alliance.h"
#include "map/entities/char_entity.h"
#include "map/party.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance construct-from-party 6975 self-test failed: " << label << '\n';
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

// Direct CAlliance(CBattleEntity*) characterization (slice 6975). A valid
// entity party becomes the new alliance's sole local and main party.
auto runAllianceConstructFromParty6975SelfTests() -> bool
{
    db::SQLiteDatabase database("file:alliance_construct_from_party_6975?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO accounts_parties (charid, partyid, allianceid, partyflag) VALUES (1, 42, 0, 4);
    )sql");

    CCharEntity leader;
    CParty      party(42);
    leader.PParty = &party;

    CAlliance alliance(&leader);

    const auto persistedLeader = db::preparedStmt("SELECT allianceid, partyflag FROM accounts_parties WHERE charid = ?", 1);

    const bool allianceIDComesFromParty = alliance.m_AllianceID == 42;
    const bool partyIsAttached          = party.m_PAlliance == &alliance;
    const bool partyIsSoleListEntry     = alliance.partyList.size() == 1 && alliance.partyList.front() == &party;
    const bool partyIsMain              = alliance.getMainParty() == &party;
    const bool leaderPersisted          = persistedLeader && persistedLeader->next() && persistedLeader->get<uint32>("allianceid") == 42 &&
                                          persistedLeader->get<uint16>("partyflag") == (PARTY_LEADER | ALLIANCE_LEADER);

    party.m_PAlliance = nullptr;
    leader.PParty     = nullptr;

    return expect(allianceIDComesFromParty, "alliance ID comes from party") &&
           expect(partyIsAttached, "party is attached") &&
           expect(partyIsSoleListEntry, "party is sole list entry") &&
           expect(partyIsMain, "party is main party") &&
           expect(leaderPersisted, "leader receives persisted alliance flags");
}
