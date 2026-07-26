#include "test_player_settings_save_host_7068.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <cstring>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "player settings save host 7068 self-test failed: " << label << '\n';
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

// Direct charutils::SavePlayerSettings characterization (slice 7068). It
// persists the first uint32 of SAVE_CONF as chars.settings.
auto runPlayerSettingsSaveHost7068SelfTests() -> bool
{
    db::SQLiteDatabase database("file:player_settings_save_host_7068?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, settings INTEGER NOT NULL);
        INSERT INTO chars (charid, settings) VALUES (77, 0);
    )sql");

    CCharEntity character;
    character.id                               = 77;
    character.playerConfig.NewAdventurerOffFlg = 1;
    character.playerConfig.AnonymityFlg        = 1;
    character.playerConfig.DisplayHeadOffFlg   = 1;
    uint32 expectedSettings{};
    std::memcpy(&expectedSettings, &character.playerConfig, sizeof(expectedSettings));

    charutils::SavePlayerSettings(&character);

    const auto row       = db::preparedStmt("SELECT settings FROM chars WHERE charid = ?", character.id);
    const bool persisted = row && row->rowsCount() == 1 && row->next() && row->get<uint32>("settings") == expectedSettings;
    return expect(persisted, "SAVE_CONF word persisted");
}
