#include "test_char_equip_save_host_7065.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/items/item_equipment.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "character equipment save host 7065 self-test failed: " << label << '\n';
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

// Direct charutils::SaveCharEquip characterization (slice 7065). Each of
// the 18 equip slots is reconciled: bound items upsert and empty slots delete.
auto runCharEquipSaveHost7065SelfTests() -> bool
{
    db::SQLiteDatabase database("file:char_equip_save_host_7065?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_equip (
            charid INTEGER NOT NULL,
            equipslotid INTEGER NOT NULL,
            slotid INTEGER NOT NULL,
            containerid INTEGER NOT NULL,
            PRIMARY KEY (charid, equipslotid)
        );
        INSERT INTO char_equip (charid, equipslotid, slotid, containerid) VALUES (77, 3, 99, 99);
    )sql");

    CCharEntity    character;
    CItemEquipment item(0x1234);
    character.id = 77;
    item.setLocationID(LOC_MOGSATCHEL);
    item.setSlotID(12);
    if (!character.bindEquip(0, &item))
    {
        return expect(false, "item bound to slot zero");
    }

    charutils::SaveCharEquip(&character);

    const auto persisted         = db::preparedStmt("SELECT slotid, containerid FROM char_equip WHERE charid = ? AND equipslotid = ?", character.id, 0);
    const auto rows              = db::preparedStmt("SELECT COUNT(*) AS count FROM char_equip WHERE charid = ?", character.id);
    const bool itemPersisted     = persisted && persisted->rowsCount() == 1 && persisted->next() &&
                                   persisted->get<uint8>("slotid") == item.getSlotID() &&
                                   persisted->get<uint8>("containerid") == static_cast<uint8>(item.getLocationID());
    const bool emptySlotsDeleted = rows && rows->rowsCount() == 1 && rows->next() && rows->get<uint32>("count") == 1;

    return expect(itemPersisted, "bound item upserted") &&
           expect(emptySlotsDeleted, "empty equipment rows deleted");
}
