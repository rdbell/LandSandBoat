#include "test_char_exp_save_host_7072.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/merit.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "character experience save host 7072 self-test failed: " << label << '\n';
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

// Direct charutils::SaveCharExp characterization (slice 7072). Playable jobs
// save their experience plus merit and limit points; guarded jobs do not.
auto runCharExpSaveHost7072SelfTests() -> bool
{
    db::SQLiteDatabase database("file:char_exp_save_host_7072?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_exp (charid INTEGER PRIMARY KEY, war INTEGER NOT NULL, run INTEGER NOT NULL, merits INTEGER NOT NULL, limits INTEGER NOT NULL);
        INSERT INTO char_exp (charid, war, run, merits, limits) VALUES (77, 0, 0, 0, 0);
    )sql");
    CCharEntity character;
    character.id                = 77;
    character.PMeritPoints      = std::make_unique<CMeritPoints>(&character);
    character.jobs.exp[JOB_WAR] = 12345;
    character.jobs.exp[JOB_RUN] = 54321;
    character.PMeritPoints->SetMeritPoints(10);
    character.PMeritPoints->SetLimitPoints(4321);

    charutils::SaveCharExp(&character, JOB_WAR);
    charutils::SaveCharExp(&character, JOB_RUN);
    charutils::SaveCharExp(&character, JOB_MON);
    charutils::SaveCharExp(&character, JOB_NON);
    charutils::SaveCharExp(&character, static_cast<JOBTYPE>(MAX_JOBTYPE));

    const auto row       = db::preparedStmt("SELECT war, run, merits, limits FROM char_exp WHERE charid = ?", character.id);
    const bool persisted = row && row->rowsCount() == 1 && row->next() &&
                           row->get<uint32>("war") == 12345 && row->get<uint32>("run") == 54321 &&
                           row->get<uint8>("merits") == 10 && row->get<uint16>("limits") == 4321;
    return expect(persisted, "playable columns and point values update while guarded jobs do not");
}
