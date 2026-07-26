#include "test_char_stats_save_host_7067.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "common/timer.h"
#include "map/entities/char_entity.h"
#include "map/entities/pet_entity.h"
#include "map/utils/charutils.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "character stats save host 7067 self-test failed: " << label << '\n';
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

auto charVarValue(uint32 characterID, const char* name) -> int32
{
    const auto row = db::preparedStmt("SELECT value FROM char_vars WHERE charid = ? AND varname = ?", characterID, name);
    if (!row || row->rowsCount() != 1 || !row->next())
    {
        return 0;
    }
    return row->get<int32>("value");
}

auto hasCharVar(uint32 characterID, const char* name) -> bool
{
    const auto row = db::preparedStmt("SELECT value FROM char_vars WHERE charid = ? AND varname = ?", characterID, name);
    return row && row->rowsCount() == 1;
}

} // namespace

// Direct charutils::SaveCharStats characterization (slice 7067). It writes
// character/pet state and reconciles the two jug-pet character variables.
auto runCharStatsSaveHost7067SelfTests() -> bool
{
    db::SQLiteDatabase database("file:char_stats_save_host_7067?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_stats (charid INTEGER PRIMARY KEY, hp INTEGER, mp INTEGER, mhflag INTEGER, mjob INTEGER, sjob INTEGER, pet_id INTEGER, pet_type INTEGER, pet_hp INTEGER, pet_mp INTEGER, pet_level INTEGER);
        CREATE TABLE char_vars (charid INTEGER NOT NULL, varname TEXT NOT NULL, value INTEGER NOT NULL, expiry INTEGER NOT NULL, PRIMARY KEY (charid, varname));
        INSERT INTO char_stats (charid) VALUES (77);
        INSERT INTO char_vars (charid, varname, value, expiry) VALUES (77, 'jugpet-spawn-time', 99, 0);
        INSERT INTO char_vars (charid, varname, value, expiry) VALUES (77, 'jugpet-duration-seconds', 99, 0);
    )sql");

    CCharEntity character;
    character.id                     = 77;
    character.health.hp              = 4321;
    character.health.mp              = 1234;
    character.profile.mhflag         = 2;
    character.petZoningInfo.petID    = 55;
    character.petZoningInfo.petType  = PET_TYPE::JUG_PET;
    character.petZoningInfo.petHP    = 444;
    character.petZoningInfo.petMP    = 222;
    character.petZoningInfo.petLevel = 99;
    character.SetMJob(1);
    character.SetSJob(2);

    charutils::SaveCharStats(&character);

    const auto stats              = db::preparedStmt("SELECT hp, mp, mhflag, mjob, sjob, pet_id, pet_type, pet_hp, pet_mp, pet_level FROM char_stats WHERE charid = ?", character.id);
    const bool statsPersisted     = stats && stats->rowsCount() == 1 && stats->next() &&
                                    stats->get<int32>("hp") == character.health.hp &&
                                    stats->get<int32>("mp") == character.health.mp &&
                                    stats->get<uint8>("mhflag") == character.profile.mhflag &&
                                    stats->get<uint8>("mjob") == character.GetMJob() &&
                                    stats->get<uint8>("sjob") == character.GetSJob(true) &&
                                    stats->get<uint8>("pet_id") == character.petZoningInfo.petID &&
                                    stats->get<uint8>("pet_type") == static_cast<uint8>(character.petZoningInfo.petType) &&
                                    stats->get<int16>("pet_hp") == character.petZoningInfo.petHP &&
                                    stats->get<int16>("pet_mp") == character.petZoningInfo.petMP &&
                                    stats->get<uint8>("pet_level") == character.petZoningInfo.petLevel;
    const bool noJugPetClearsVars = !hasCharVar(character.id, "jugpet-spawn-time") &&
                                    !hasCharVar(character.id, "jugpet-duration-seconds");

    character.petZoningInfo.jugSpawnTime = timer::now();
    character.petZoningInfo.jugDuration  = std::chrono::seconds(123);
    charutils::SaveCharStats(&character);
    const bool jugPetPersistsVars = charVarValue(character.id, "jugpet-spawn-time") > 0 &&
                                    charVarValue(character.id, "jugpet-duration-seconds") == 123;

    return expect(statsPersisted, "character and pet stats persisted") &&
           expect(noJugPetClearsVars, "absent jug pet clears variables") &&
           expect(jugPetPersistsVars, "jug pet timestamp and duration persisted");
}
