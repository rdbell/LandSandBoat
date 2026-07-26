#include "test_party_push_effects_completion_6985.h"

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
        std::cerr << "party push effects completion 6985 self-test failed: " << label << '\n';
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

// Direct CParty::PushEffectsPacket characterization (slice 6985). The clean
// gate returns without mutation; a dirty empty local roster completes and
// clears the packet-dirty state after its loop.
auto runPartyPushEffectsCompletion6985SelfTests() -> bool
{
    db::SQLiteDatabase database("file:party_push_effects_completion_6985?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE accounts_parties (charid INTEGER PRIMARY KEY, partyid INTEGER NOT NULL, allianceid INTEGER NOT NULL, partyflag INTEGER NOT NULL, timestamp INTEGER NOT NULL);
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, charname TEXT NOT NULL, pos_zone INTEGER NOT NULL, pos_prevzone INTEGER NOT NULL);
    )sql");

    CParty party(1);

    party.PushEffectsPacket();
    const bool cleanRemainsClean = expect(!party.m_EffectsChanged, "clean gate remains clean");

    party.m_EffectsChanged = true;
    party.PushEffectsPacket();
    const bool dirtyCleared = expect(!party.m_EffectsChanged, "completed packet clears dirty state");

    return cleanRemainsClean && dirtyCleared;
}
