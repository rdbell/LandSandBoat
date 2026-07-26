#include "test_title_save_host_7062.h"

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
        std::cerr << "title save host 7062 self-test failed: " << label << '\n';
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

// Direct charutils::SaveTitles characterization (slice 7062). It persists
// the complete title bitmap before updating the active profile title.
auto runTitleSaveHost7062SelfTests() -> bool
{
    db::SQLiteDatabase database("file:title_save_host_7062?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, titles BLOB NOT NULL);
        CREATE TABLE char_stats (charid INTEGER PRIMARY KEY, title INTEGER NOT NULL);
        INSERT INTO chars (charid, titles) VALUES (77, X'');
        INSERT INTO char_stats (charid, title) VALUES (77, 0);
    )sql");

    CCharEntity character;
    character.id                                             = 77;
    character.m_TitleList[0]                                 = 0x12;
    character.m_TitleList[sizeof(character.m_TitleList) - 1] = 0x34;
    character.profile.title                                  = 1234;

    charutils::SaveTitles(&character);

    const auto titles                 = db::preparedStmt("SELECT titles FROM chars WHERE charid = ?", character.id);
    const auto stats                  = db::preparedStmt("SELECT title FROM char_stats WHERE charid = ?", character.id);
    const auto expectedTitles         = std::string(reinterpret_cast<const char*>(character.m_TitleList), sizeof(character.m_TitleList));
    const bool bitmapPersisted        = titles && titles->rowsCount() == 1 && titles->next() && titles->getBlobBytes("titles") == expectedTitles;
    const bool selectedTitlePersisted = stats && stats->rowsCount() == 1 && stats->next() && stats->get<uint16>("title") == character.profile.title;

    return expect(bitmapPersisted, "title bitmap persisted") &&
           expect(selectedTitlePersisted, "selected profile title persisted");
}
