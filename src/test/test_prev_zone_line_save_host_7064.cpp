#include "test_prev_zone_line_save_host_7064.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "previous zone line save host 7064 self-test failed: " << label << '\n';
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

// Direct charutils::SavePrevZoneLineID characterization (slice 7064). It
// saves the given previous zone-line ID for the character.
auto runPrevZoneLineSaveHost7064SelfTests() -> bool
{
    db::SQLiteDatabase database("file:prev_zone_line_save_host_7064?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, pos_prevzonelineid INTEGER NOT NULL);
        INSERT INTO chars (charid, pos_prevzonelineid) VALUES (77, 0);
    )sql");

    CCharEntity character;
    character.id                = 77;
    constexpr uint32 zoneLineID = 0xFEDCBA98;

    charutils::SavePrevZoneLineID(&character, zoneLineID);

    const auto row       = db::preparedStmt("SELECT pos_prevzonelineid FROM chars WHERE charid = ?", character.id);
    const bool persisted = row && row->rowsCount() == 1 && row->next() && row->get<uint32>("pos_prevzonelineid") == zoneLineID;

    return expect(persisted, "previous zone-line ID persisted");
}
