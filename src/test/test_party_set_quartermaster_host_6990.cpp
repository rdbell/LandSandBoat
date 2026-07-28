#include "test_party_set_quartermaster_host_6990.h"

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
        std::cerr << "party set quartermaster host 6990 self-test failed: " << label << '\n';
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

// Direct CParty::SetQuarterMaster characterization (slice 6990). The named
// local member becomes quartermaster; a missed lookup clears the pointer.
auto runPartySetQuarterMasterHost6990SelfTests() -> bool
{
    db::SQLiteDatabase database("file:party_set_quartermaster_6990?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, charname TEXT NOT NULL);
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, partyflag INTEGER NOT NULL);
        INSERT INTO chars (charid, charname) VALUES (101, 'Alice'), (102, 'Bob');
        INSERT INTO accounts_parties (charid, partyid, partyflag) VALUES (101, 1, 16), (102, 1, 0);
    )sql");

    CParty        party(1);
    CBattleEntity alice;
    CBattleEntity bob;

    alice.name = "Alice";
    bob.name   = "Bob";
    party.members = { &alice, &bob };

    party.SetQuarterMaster("Bob");
    const auto bobFlags = db::preparedStmt("SELECT partyflag FROM accounts_parties WHERE charid = ?", 102);
    const bool assigned = expect(party.GetQuaterMaster() == &bob, "case-insensitive member assigned") &&
                          expect(bobFlags && bobFlags->next() && bobFlags->get<uint16>(0) == PARTY_QM, "selected QM flag set");

    party.SetQuarterMaster("Carol");
    const auto aliceFlags = db::preparedStmt("SELECT partyflag FROM accounts_parties WHERE charid = ?", 101);
    const auto clearedBobFlags = db::preparedStmt("SELECT partyflag FROM accounts_parties WHERE charid = ?", 102);
    const bool cleared = expect(party.GetQuaterMaster() == nullptr, "unknown member clears quartermaster") &&
                         expect(aliceFlags && aliceFlags->next() && aliceFlags->get<uint16>(0) == 0, "old QM flag cleared") &&
                         expect(clearedBobFlags && clearedBobFlags->next() && clearedBobFlags->get<uint16>(0) == 0, "miss does not set QM flag");

    return assigned && cleared;
}
