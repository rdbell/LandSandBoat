#include "test_last_logout_save_host_7075.h"

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
        std::cerr << "last logout save host 7075 self-test failed: " << label << '\n';
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

// Direct charutils::SaveLastLogout characterization (slice 7075). The
// database's current timestamp replaces the character's previous logout time.
auto runLastLogoutSaveHost7075SelfTests() -> bool
{
    db::SQLiteDatabase database("file:last_logout_save_host_7075?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, last_logout TEXT NOT NULL);
        INSERT INTO chars (charid, last_logout) VALUES (77, '2000-01-01 00:00:00');
    )sql");
    CCharEntity character;
    character.id = 77;

    charutils::SaveLastLogout(&character);

    const auto row     = db::preparedStmt("SELECT last_logout FROM chars WHERE charid = ?", character.id);
    const bool updated = row && row->rowsCount() == 1 && row->next() &&
                         row->get<std::string>("last_logout") != "2000-01-01 00:00:00";
    return expect(updated, "database current timestamp replaces previous logout time");
}
