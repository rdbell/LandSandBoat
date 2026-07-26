#include "test_char_job_save_host_7070.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "character job save host 7070 self-test failed: " << label << '\n';
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

// Direct charutils::SaveCharJob characterization (slice 7070). Playable jobs
// update their own column and unlock mask; invalid and Monstrosity jobs do not.
auto runCharJobSaveHost7070SelfTests() -> bool
{
    db::SQLiteDatabase database("file:char_job_save_host_7070?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_jobs (charid INTEGER PRIMARY KEY, unlocked INTEGER NOT NULL, war INTEGER NOT NULL, run INTEGER NOT NULL);
        INSERT INTO char_jobs (charid, unlocked, war, run) VALUES (77, 0, 0, 0);
    )sql");
    CCharEntity character;
    character.id                = 77;
    character.jobs.unlocked     = 0x12345678;
    character.jobs.job[JOB_WAR] = 50;
    character.jobs.job[JOB_RUN] = 99;

    charutils::SaveCharJob(&character, JOB_WAR);
    charutils::SaveCharJob(&character, JOB_RUN);
    charutils::SaveCharJob(&character, JOB_MON);
    charutils::SaveCharJob(&character, JOB_NON);
    charutils::SaveCharJob(&character, static_cast<JOBTYPE>(MAX_JOBTYPE));

    const auto row       = db::preparedStmt("SELECT unlocked, war, run FROM char_jobs WHERE charid = ?", character.id);
    const bool persisted = row && row->rowsCount() == 1 && row->next() &&
                           row->get<uint32>("unlocked") == character.jobs.unlocked &&
                           row->get<uint8>("war") == 50 && row->get<uint8>("run") == 99;
    return expect(persisted, "playable columns update and guarded jobs do not");
}
