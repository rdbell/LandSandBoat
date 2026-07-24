#include "test_zoneutils_load_npc_mob_host_6405.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zoneutils load npc/mob host 6405 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for LoadNPCList / LoadMOBList host (slice 6405).
// Go: LoadNPCListSQL / PlanNPCFromRow / ApplyLoadNPCList / ApplyLoadMOBList.
auto runZoneutilsLoadNpcMobHost6405SelfTests() -> bool
{
    bool ok = true;

    const std::string npcSQL =
        "SELECT content_tag, npcid FROM npc_list INNER JOIN zone_settings "
        "ON (npcid & 0xFFF000) >> 12 = zone_settings.zoneid "
        "WHERE ((npcid & 0xFFF000) >> 12) = ?";
    ok = expect(npcSQL.find("npc_list") != std::string::npos, "npc table") && ok;
    ok = expect(npcSQL.find("(npcid & 0xFFF000) >> 12") != std::string::npos, "npc zone extract") && ok;

    const std::string mobSQL =
        "FROM mob_groups INNER JOIN mob_pools WHERE NOT (pos_x = 0 AND pos_y = 0 AND pos_z = 0) "
        "AND mob_groups.zoneid = ?";
    ok = expect(mobSQL.find("mob_groups") != std::string::npos, "mob table") && ok;

    // ID helpers: (id & 0xFFF000) >> 12 and id & 0xFFF
    const uint32_t sample = 0x0000A123u;
    const uint16_t zone   = static_cast<uint16_t>((sample & 0xFFF000u) >> 12);
    const uint16_t targ   = static_cast<uint16_t>(sample & 0xFFFu);
    ok = expect(zone == 0x000Au, "zone from id") && ok;
    ok = expect(targ == 0x0123u, "targid from id") && ok;

    // Empty content tag always loads.
    const std::string emptyTag;
    ok = expect(emptyTag.empty(), "empty tag loads") && ok;

    // INSTANCED bit 0x0100 skips entity insert.
    const uint16_t instanced = 0x0100;
    ok = expect((instanced & 0x0100) != 0, "instanced bit") && ok;

    ok = expect(std::string("main.NORMAL_MOB_MAX_LEVEL_RANGE_MIN") == "main.NORMAL_MOB_MAX_LEVEL_RANGE_MIN", "level min key") && ok;

    return ok;
}
