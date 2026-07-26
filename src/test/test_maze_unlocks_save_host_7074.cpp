#include "test_maze_unlocks_save_host_7074.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/enums/packet_s2c.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "maze unlocks save host 7074 self-test failed: " << label << '\n';
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

// Direct charutils::SaveMazeUnlocks characterization (slice 7074). It saves
// both maze bitsets and queues a Dungeon packet with the updated state.
auto runMazeUnlocksSaveHost7074SelfTests() -> bool
{
    db::SQLiteDatabase database("file:maze_unlocks_save_host_7074?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_unlocks (charid INTEGER PRIMARY KEY, maze_vouchers BLOB, maze_runes BLOB);
        INSERT INTO char_unlocks (charid) VALUES (77);
    )sql");
    CCharEntity character;
    character.id = 77;
    character.maze().vouchers.set(0);
    character.maze().vouchers.set(63);
    character.maze().runes.set(1);
    character.maze().runes.set(511);
    character.clearPacketList();

    charutils::SaveMazeUnlocks(&character);

    const auto row = db::preparedStmt("SELECT maze_vouchers, maze_runes FROM char_unlocks WHERE charid = ?", character.id);
    if (!(row && row->rowsCount() == 1 && row->next()))
    {
        return expect(false, "unlock row exists");
    }

    const auto vouchers = row->get<xi::bitset<64>>("maze_vouchers");
    const auto runes    = row->get<xi::bitset<512>>("maze_runes");
    const bool saved    = vouchers.data == character.maze().vouchers.data && runes.data == character.maze().runes.data;
    const bool queued   = character.getPacketCount() == 1 &&
                          character.getPacketList().front()->getType() == static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_DUNGEON);
    return expect(saved && queued, "bitsets persist before Dungeon packet queues");
}
