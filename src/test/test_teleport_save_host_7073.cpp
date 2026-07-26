#include "test_teleport_save_host_7073.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <array>
#include <cstring>
#include <iostream>

namespace
{

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "teleport save host 7073 self-test failed: " << label << '\n';
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

// Direct charutils::SaveTeleport characterization (slice 7073). Every known
// type writes its matching unlock column; unknown types leave the row unchanged.
auto runTeleportSaveHost7073SelfTests() -> bool
{
    db::SQLiteDatabase database("file:teleport_save_host_7073?mode=memory&cache=shared");
    ScopedDatabase     activeDatabase(database);
    database.executeScript(R"sql(
        CREATE TABLE char_unlocks (charid INTEGER PRIMARY KEY, outpost_sandy INTEGER, outpost_bastok INTEGER, outpost_windy INTEGER, runic_portal INTEGER, maw INTEGER, campaign_sandy INTEGER, campaign_bastok INTEGER, campaign_windy INTEGER, homepoints BLOB, survivals BLOB, abyssea_conflux BLOB, waypoints BLOB, eschan_portals BLOB);
        INSERT INTO char_unlocks (charid) VALUES (77);
    )sql");
    CCharEntity character;
    std::memset(&character.teleport, 0, sizeof(character.teleport));
    character.id                              = 77;
    character.teleport.outpostSandy           = 1;
    character.teleport.outpostBastok          = 2;
    character.teleport.outpostWindy           = 3;
    character.teleport.runicPortal            = 4;
    character.teleport.pastMaw                = 5;
    character.teleport.campaignSandy          = 6;
    character.teleport.campaignBastok         = 7;
    character.teleport.campaignWindy          = 8;
    character.teleport.homepoint.access[0]    = 9;
    character.teleport.homepoint.menu[0]      = 10;
    character.teleport.survival.access[0]     = 11;
    character.teleport.survival.menu[0]       = 12;
    character.teleport.abysseaConflux[0]      = 13;
    character.teleport.waypoints.access[0]    = 14;
    character.teleport.waypoints.confirmation = true;
    character.teleport.eschanPortal           = 15;

    for (const auto type : std::array{
             TELEPORT_TYPE::OUTPOST_SANDY,
             TELEPORT_TYPE::OUTPOST_BASTOK,
             TELEPORT_TYPE::OUTPOST_WINDY,
             TELEPORT_TYPE::RUNIC_PORTAL,
             TELEPORT_TYPE::PAST_MAW,
             TELEPORT_TYPE::ABYSSEA_CONFLUX,
             TELEPORT_TYPE::CAMPAIGN_SANDY,
             TELEPORT_TYPE::CAMPAIGN_BASTOK,
             TELEPORT_TYPE::CAMPAIGN_WINDY,
             TELEPORT_TYPE::HOMEPOINT,
             TELEPORT_TYPE::SURVIVAL,
             TELEPORT_TYPE::WAYPOINT,
             TELEPORT_TYPE::ESCHAN_PORTAL,
         })
    {
        charutils::SaveTeleport(&character, type);
    }
    charutils::SaveTeleport(&character, static_cast<TELEPORT_TYPE>(13));

    const auto row = db::preparedStmt("SELECT * FROM char_unlocks WHERE charid = ?", character.id);
    if (!(row && row->rowsCount() == 1 && row->next()))
    {
        return expect(false, "unlock row exists");
    }

    const auto homepoints = row->get<telepoint_t>("homepoints");
    const auto survivals  = row->get<telepoint_t>("survivals");
    const auto conflux    = row->get<std::array<uint8, MAX_ABYSSEAZONES>>("abyssea_conflux");
    const auto waypoints  = row->get<waypoint_t>("waypoints");
    const bool persisted  = row->get<uint32>("outpost_sandy") == 1 && row->get<uint32>("outpost_bastok") == 2 &&
                            row->get<uint32>("outpost_windy") == 3 && row->get<uint32>("runic_portal") == 4 &&
                            row->get<uint32>("maw") == 5 && row->get<uint32>("campaign_sandy") == 6 &&
                            row->get<uint32>("campaign_bastok") == 7 && row->get<uint32>("campaign_windy") == 8 &&
                            row->get<uint32>("eschan_portals") == 15 &&
                            std::memcmp(&homepoints, &character.teleport.homepoint, sizeof(telepoint_t)) == 0 &&
                            std::memcmp(&survivals, &character.teleport.survival, sizeof(telepoint_t)) == 0 &&
                            conflux == std::array<uint8, MAX_ABYSSEAZONES>{ 13 } &&
                            std::memcmp(&waypoints, &character.teleport.waypoints, sizeof(waypoint_t)) == 0;
    return expect(persisted, "every known type persists its exact unlock payload");
}
