#include "test_char_look_save_host_7066.h"

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
        std::cerr << "character look save host 7066 self-test failed: " << label << '\n';
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

auto matchesLook(db::ResultSet* row, const look_t& look) -> bool
{
    return row && row->rowsCount() == 1 && row->next() &&
           row->get<uint16>("head") == look.head &&
           row->get<uint16>("body") == look.body &&
           row->get<uint16>("hands") == look.hands &&
           row->get<uint16>("legs") == look.legs &&
           row->get<uint16>("feet") == look.feet &&
           row->get<uint16>("main") == look.main &&
           row->get<uint16>("sub") == look.sub &&
           row->get<uint16>("ranged") == look.ranged;
}

auto matchesStyle(db::ResultSet* row, const CCharEntity& character) -> bool
{
    return row && row->rowsCount() == 1 && row->next() &&
           row->get<uint16>("head") == character.styleItems[SLOT_HEAD] &&
           row->get<uint16>("body") == character.styleItems[SLOT_BODY] &&
           row->get<uint16>("hands") == character.styleItems[SLOT_HANDS] &&
           row->get<uint16>("legs") == character.styleItems[SLOT_LEGS] &&
           row->get<uint16>("feet") == character.styleItems[SLOT_FEET] &&
           row->get<uint16>("main") == character.styleItems[SLOT_MAIN] &&
           row->get<uint16>("sub") == character.styleItems[SLOT_SUB] &&
           row->get<uint16>("ranged") == character.styleItems[SLOT_RANGED];
}

} // namespace

// Direct charutils::SaveCharLook characterization (slice 7066). The saved
// active look follows the lock state while the style-item record always upserts.
auto runCharLookSaveHost7066SelfTests() -> bool
{
    db::SQLiteDatabase database("file:char_look_save_host_7066?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_look (charid INTEGER PRIMARY KEY, head INTEGER, body INTEGER, hands INTEGER, legs INTEGER, feet INTEGER, main INTEGER, sub INTEGER, ranged INTEGER);
        CREATE TABLE chars (charid INTEGER PRIMARY KEY, isstylelocked INTEGER NOT NULL);
        CREATE TABLE char_style (charid INTEGER PRIMARY KEY, head INTEGER, body INTEGER, hands INTEGER, legs INTEGER, feet INTEGER, main INTEGER, sub INTEGER, ranged INTEGER);
        INSERT INTO char_look (charid) VALUES (77);
        INSERT INTO chars (charid, isstylelocked) VALUES (77, 0);
    )sql");

    CCharEntity character;
    character.id                      = 77;
    character.look.head               = 11;
    character.look.body               = 12;
    character.look.hands              = 13;
    character.look.legs               = 14;
    character.look.feet               = 15;
    character.look.main               = 16;
    character.look.sub                = 17;
    character.look.ranged             = 18;
    character.mainlook.head           = 101;
    character.mainlook.body           = 102;
    character.mainlook.hands          = 103;
    character.mainlook.legs           = 104;
    character.mainlook.feet           = 105;
    character.mainlook.main           = 106;
    character.mainlook.sub            = 107;
    character.mainlook.ranged         = 108;
    character.styleItems[SLOT_HEAD]   = 201;
    character.styleItems[SLOT_BODY]   = 202;
    character.styleItems[SLOT_HANDS]  = 203;
    character.styleItems[SLOT_LEGS]   = 204;
    character.styleItems[SLOT_FEET]   = 205;
    character.styleItems[SLOT_MAIN]   = 206;
    character.styleItems[SLOT_SUB]    = 207;
    character.styleItems[SLOT_RANGED] = 208;

    charutils::SaveCharLook(&character);
    const auto unlockedLook      = db::preparedStmt("SELECT head, body, hands, legs, feet, main, sub, ranged FROM char_look WHERE charid = ?", character.id);
    const auto unlockedFlag      = db::preparedStmt("SELECT isstylelocked FROM chars WHERE charid = ?", character.id);
    const auto savedStyle        = db::preparedStmt("SELECT head, body, hands, legs, feet, main, sub, ranged FROM char_style WHERE charid = ?", character.id);
    const bool unlockedPersisted = matchesLook(unlockedLook.get(), character.look) &&
                                   unlockedFlag && unlockedFlag->rowsCount() == 1 && unlockedFlag->next() && unlockedFlag->get<uint8>("isstylelocked") == 0 &&
                                   matchesStyle(savedStyle.get(), character);

    character.setStyleLocked(true);
    charutils::SaveCharLook(&character);
    const auto lockedLook      = db::preparedStmt("SELECT head, body, hands, legs, feet, main, sub, ranged FROM char_look WHERE charid = ?", character.id);
    const auto lockedFlag      = db::preparedStmt("SELECT isstylelocked FROM chars WHERE charid = ?", character.id);
    const bool lockedPersisted = matchesLook(lockedLook.get(), character.mainlook) &&
                                 lockedFlag && lockedFlag->rowsCount() == 1 && lockedFlag->next() && lockedFlag->get<uint8>("isstylelocked") == 1;

    return expect(unlockedPersisted, "unlocked look, flag, and style persisted") &&
           expect(lockedPersisted, "style-locked main look and flag persisted");
}
