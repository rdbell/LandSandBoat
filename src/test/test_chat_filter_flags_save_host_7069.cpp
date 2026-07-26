#include "test_chat_filter_flags_save_host_7069.h"

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
        std::cerr << "chat filter flags save host 7069 self-test failed: " << label << '\n';
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

// Direct charutils::SaveChatFilterFlags characterization (slice 7069). It
// saves the serialized MessageFilter and MessageFilter2 bitfield words.
auto runChatFilterFlagsSaveHost7069SelfTests() -> bool
{
    db::SQLiteDatabase database("file:chat_filter_flags_save_host_7069?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, chatfilters_1 INTEGER NOT NULL, chatfilters_2 INTEGER NOT NULL);
        INSERT INTO chars (charid, chatfilters_1, chatfilters_2) VALUES (77, 0, 0);
    )sql");

    CCharEntity character;
    character.id                                                   = 77;
    character.playerConfig.MessageFilter.say                       = 1;
    character.playerConfig.MessageFilter.lot_results               = 1;
    character.playerConfig.MessageFilter2.yell                     = 1;
    character.playerConfig.MessageFilter2.messages_from_alter_egos = 1;
    uint32 expectedFirst{};
    uint32 expectedSecond{};
    std::memcpy(&expectedFirst, &character.playerConfig.MessageFilter, sizeof(expectedFirst));
    std::memcpy(&expectedSecond, &character.playerConfig.MessageFilter2, sizeof(expectedSecond));

    charutils::SaveChatFilterFlags(&character);

    const auto row       = db::preparedStmt("SELECT chatfilters_1, chatfilters_2 FROM chars WHERE charid = ?", character.id);
    const bool persisted = row && row->rowsCount() == 1 && row->next() &&
                           row->get<uint32>("chatfilters_1") == expectedFirst &&
                           row->get<uint32>("chatfilters_2") == expectedSecond;
    return expect(persisted, "both filter words persisted");
}
