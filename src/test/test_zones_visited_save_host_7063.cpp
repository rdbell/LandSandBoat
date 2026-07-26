#include "test_zones_visited_save_host_7063.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zones visited save host 7063 self-test failed: " << label << '\n';
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

// Direct charutils::SaveZonesVisited characterization (slice 7063). It
// persists the complete 38-byte zone-visit bitmap for the character.
auto runZonesVisitedSaveHost7063SelfTests() -> bool
{
    db::SQLiteDatabase database("file:zones_visited_save_host_7063?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, zones BLOB NOT NULL);
        INSERT INTO chars (charid, zones) VALUES (77, X'');
    )sql");

    CCharEntity character;
    character.id                                                           = 77;
    character.m_ZonesVisitedList[0]                                        = 0x12;
    character.m_ZonesVisitedList[sizeof(character.m_ZonesVisitedList) - 1] = 0x34;

    charutils::SaveZonesVisited(&character);

    const auto zones     = db::preparedStmt("SELECT zones FROM chars WHERE charid = ?", character.id);
    const auto expected  = std::string(reinterpret_cast<const char*>(character.m_ZonesVisitedList), sizeof(character.m_ZonesVisitedList));
    const bool persisted = zones && zones->rowsCount() == 1 && zones->next() && zones->getBlobBytes("zones") == expected;

    return expect(persisted, "zone bitmap persisted");
}
