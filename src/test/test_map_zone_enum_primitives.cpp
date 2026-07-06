/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_map_zone_enum_primitives.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "map/zone.h"

namespace
{

struct EnumCase
{
    std::uint64_t actual;
    std::uint64_t expected;
    std::string   label;
};

template <typename T>
auto enumValue(T value) -> std::uint64_t
{
    return static_cast<std::uint64_t>(value);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map zone enum primitive self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "map zone enum primitive self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto testNationTypes() -> bool
{
    return expectAll({
        { enumValue(NATION_SANDORIA), 0x00, "NATION_SANDORIA" },
        { enumValue(NATION_BASTOK), 0x01, "NATION_BASTOK" },
        { enumValue(NATION_WINDURST), 0x02, "NATION_WINDURST" },
        { enumValue(NATION_BEASTMEN), 0x03, "NATION_BEASTMEN" },
        { enumValue(NATION_NEUTRAL), 0xFF, "NATION_NEUTRAL" },
    });
}

auto testRegionTypes() -> bool
{
    return expectAll({
        { enumValue(REGION_TYPE::RONFAURE), 0, "REGION_TYPE::RONFAURE" },
        { enumValue(REGION_TYPE::ZULKHEIM), 1, "REGION_TYPE::ZULKHEIM" },
        { enumValue(REGION_TYPE::NORVALLEN), 2, "REGION_TYPE::NORVALLEN" },
        { enumValue(REGION_TYPE::GUSTABERG), 3, "REGION_TYPE::GUSTABERG" },
        { enumValue(REGION_TYPE::DERFLAND), 4, "REGION_TYPE::DERFLAND" },
        { enumValue(REGION_TYPE::SARUTABARUTA), 5, "REGION_TYPE::SARUTABARUTA" },
        { enumValue(REGION_TYPE::KOLSHUSHU), 6, "REGION_TYPE::KOLSHUSHU" },
        { enumValue(REGION_TYPE::ARAGONEU), 7, "REGION_TYPE::ARAGONEU" },
        { enumValue(REGION_TYPE::FAUREGANDI), 8, "REGION_TYPE::FAUREGANDI" },
        { enumValue(REGION_TYPE::VALDEAUNIA), 9, "REGION_TYPE::VALDEAUNIA" },
        { enumValue(REGION_TYPE::QUFIMISLAND), 10, "REGION_TYPE::QUFIMISLAND" },
        { enumValue(REGION_TYPE::LITELOR), 11, "REGION_TYPE::LITELOR" },
        { enumValue(REGION_TYPE::KUZOTZ), 12, "REGION_TYPE::KUZOTZ" },
        { enumValue(REGION_TYPE::VOLLBOW), 13, "REGION_TYPE::VOLLBOW" },
        { enumValue(REGION_TYPE::ELSHIMO_LOWLANDS), 14, "REGION_TYPE::ELSHIMO_LOWLANDS" },
        { enumValue(REGION_TYPE::ELSHIMO_UPLANDS), 15, "REGION_TYPE::ELSHIMO_UPLANDS" },
        { enumValue(REGION_TYPE::TULIA), 16, "REGION_TYPE::TULIA" },
        { enumValue(REGION_TYPE::MOVALPOLOS), 17, "REGION_TYPE::MOVALPOLOS" },
        { enumValue(REGION_TYPE::TAVNAZIA), 18, "REGION_TYPE::TAVNAZIA" },
        { enumValue(REGION_TYPE::SANDORIA), 19, "REGION_TYPE::SANDORIA" },
        { enumValue(REGION_TYPE::BASTOK), 20, "REGION_TYPE::BASTOK" },
        { enumValue(REGION_TYPE::WINDURST), 21, "REGION_TYPE::WINDURST" },
        { enumValue(REGION_TYPE::JEUNO), 22, "REGION_TYPE::JEUNO" },
        { enumValue(REGION_TYPE::DYNAMIS), 23, "REGION_TYPE::DYNAMIS" },
        { enumValue(REGION_TYPE::TAVNAZIAN_MARQ), 24, "REGION_TYPE::TAVNAZIAN_MARQ" },
        { enumValue(REGION_TYPE::PROMYVION), 25, "REGION_TYPE::PROMYVION" },
        { enumValue(REGION_TYPE::LUMORIA), 26, "REGION_TYPE::LUMORIA" },
        { enumValue(REGION_TYPE::LIMBUS), 27, "REGION_TYPE::LIMBUS" },
        { enumValue(REGION_TYPE::WEST_AHT_URHGAN), 28, "REGION_TYPE::WEST_AHT_URHGAN" },
        { enumValue(REGION_TYPE::MAMOOL_JA_SAVAGE), 29, "REGION_TYPE::MAMOOL_JA_SAVAGE" },
        { enumValue(REGION_TYPE::HALVUNG), 30, "REGION_TYPE::HALVUNG" },
        { enumValue(REGION_TYPE::ARRAPAGO), 31, "REGION_TYPE::ARRAPAGO" },
        { enumValue(REGION_TYPE::ALZADAAL), 32, "REGION_TYPE::ALZADAAL" },
        { enumValue(REGION_TYPE::RONFAURE_FRONT), 33, "REGION_TYPE::RONFAURE_FRONT" },
        { enumValue(REGION_TYPE::NORVALLEN_FRONT), 34, "REGION_TYPE::NORVALLEN_FRONT" },
        { enumValue(REGION_TYPE::GUSTABERG_FRONT), 35, "REGION_TYPE::GUSTABERG_FRONT" },
        { enumValue(REGION_TYPE::DERFLAND_FRONT), 36, "REGION_TYPE::DERFLAND_FRONT" },
        { enumValue(REGION_TYPE::SARUTA_FRONT), 37, "REGION_TYPE::SARUTA_FRONT" },
        { enumValue(REGION_TYPE::ARAGONEAU_FRONT), 38, "REGION_TYPE::ARAGONEAU_FRONT" },
        { enumValue(REGION_TYPE::FAUREGANDI_FRONT), 39, "REGION_TYPE::FAUREGANDI_FRONT" },
        { enumValue(REGION_TYPE::VALDEAUNIA_FRONT), 40, "REGION_TYPE::VALDEAUNIA_FRONT" },
        { enumValue(REGION_TYPE::ABYSSEA), 41, "REGION_TYPE::ABYSSEA" },
        { enumValue(REGION_TYPE::THE_THRESHOLD), 42, "REGION_TYPE::THE_THRESHOLD" },
        { enumValue(REGION_TYPE::ABDHALJS), 43, "REGION_TYPE::ABDHALJS" },
        { enumValue(REGION_TYPE::ADOULIN_ISLANDS), 44, "REGION_TYPE::ADOULIN_ISLANDS" },
        { enumValue(REGION_TYPE::EAST_ULBUKA), 45, "REGION_TYPE::EAST_ULBUKA" },
        { enumValue(REGION_TYPE::UNKNOWN), 255, "REGION_TYPE::UNKNOWN" },
    });
}

auto testContinentTypes() -> bool
{
    return expectAll({
        { enumValue(CONTINENT_TYPE::THE_MIDDLE_LANDS), 1, "CONTINENT_TYPE::THE_MIDDLE_LANDS" },
        { enumValue(CONTINENT_TYPE::THE_ARADJIAH_CONTINENT), 2, "CONTINENT_TYPE::THE_ARADJIAH_CONTINENT" },
        { enumValue(CONTINENT_TYPE::THE_SHADOWREIGN_ERA), 3, "CONTINENT_TYPE::THE_SHADOWREIGN_ERA" },
        { enumValue(CONTINENT_TYPE::OTHER_AREAS), 4, "CONTINENT_TYPE::OTHER_AREAS" },
    });
}

auto testZoneTypes() -> bool
{
    bool ok = expectAll({
        { enumValue(UNKNOWN), 0x0000, "ZONE_TYPE::UNKNOWN" },
        { enumValue(CITY), 0x0001, "ZONE_TYPE::CITY" },
        { enumValue(OUTDOORS), 0x0002, "ZONE_TYPE::OUTDOORS" },
        { enumValue(DUNGEON), 0x0004, "ZONE_TYPE::DUNGEON" },
        { enumValue(SIGNET), 0x0008, "ZONE_TYPE::SIGNET" },
        { enumValue(SANCTION), 0x0010, "ZONE_TYPE::SANCTION" },
        { enumValue(SIGIL), 0x0020, "ZONE_TYPE::SIGIL" },
        { enumValue(IONIS), 0x0040, "ZONE_TYPE::IONIS" },
        { enumValue(DYNAMIS), 0x0080, "ZONE_TYPE::DYNAMIS" },
        { enumValue(INSTANCED), 0x0100, "ZONE_TYPE::INSTANCED" },
    });

    const auto flags = enumValue(CITY) | enumValue(SIGNET) | enumValue(SIGIL);
    ok               = expectEqualInt(flags, 0x0029, "ZONE_TYPE mixed flag bits") && ok;
    ok               = expectTrue((flags & enumValue(SIGNET)) == enumValue(SIGNET), "ZONE_TYPE has SIGNET") && ok;
    ok               = expectTrue((flags & enumValue(DUNGEON)) == 0, "ZONE_TYPE missing DUNGEON") && ok;
    return ok;
}

auto testGlobalMessageTypes() -> bool
{
    return expectAll({
        { enumValue(CHAR_INRANGE), 0, "GLOBAL_MESSAGE_TYPE::CHAR_INRANGE" },
        { enumValue(CHAR_INRANGE_SELF), 1, "GLOBAL_MESSAGE_TYPE::CHAR_INRANGE_SELF" },
        { enumValue(CHAR_INSHOUT), 2, "GLOBAL_MESSAGE_TYPE::CHAR_INSHOUT" },
        { enumValue(CHAR_INZONE), 3, "GLOBAL_MESSAGE_TYPE::CHAR_INZONE" },
    });
}

auto testTeleportTypes() -> bool
{
    return expectAll({
        { enumValue(TELEPORT_TYPE::OUTPOST_SANDY), 0, "TELEPORT_TYPE::OUTPOST_SANDY" },
        { enumValue(TELEPORT_TYPE::OUTPOST_BASTOK), 1, "TELEPORT_TYPE::OUTPOST_BASTOK" },
        { enumValue(TELEPORT_TYPE::OUTPOST_WINDY), 2, "TELEPORT_TYPE::OUTPOST_WINDY" },
        { enumValue(TELEPORT_TYPE::RUNIC_PORTAL), 3, "TELEPORT_TYPE::RUNIC_PORTAL" },
        { enumValue(TELEPORT_TYPE::PAST_MAW), 4, "TELEPORT_TYPE::PAST_MAW" },
        { enumValue(TELEPORT_TYPE::ABYSSEA_CONFLUX), 5, "TELEPORT_TYPE::ABYSSEA_CONFLUX" },
        { enumValue(TELEPORT_TYPE::CAMPAIGN_SANDY), 6, "TELEPORT_TYPE::CAMPAIGN_SANDY" },
        { enumValue(TELEPORT_TYPE::CAMPAIGN_BASTOK), 7, "TELEPORT_TYPE::CAMPAIGN_BASTOK" },
        { enumValue(TELEPORT_TYPE::CAMPAIGN_WINDY), 8, "TELEPORT_TYPE::CAMPAIGN_WINDY" },
        { enumValue(TELEPORT_TYPE::HOMEPOINT), 9, "TELEPORT_TYPE::HOMEPOINT" },
        { enumValue(TELEPORT_TYPE::SURVIVAL), 10, "TELEPORT_TYPE::SURVIVAL" },
        { enumValue(TELEPORT_TYPE::WAYPOINT), 11, "TELEPORT_TYPE::WAYPOINT" },
        { enumValue(TELEPORT_TYPE::ESCHAN_PORTAL), 12, "TELEPORT_TYPE::ESCHAN_PORTAL" },
    });
}

auto testZoneMiscFlags() -> bool
{
    bool ok = expectAll({
        { enumValue(MISC_NONE), 0x0000, "ZONEMISC::MISC_NONE" },
        { enumValue(MISC_ESCAPE), 0x0001, "ZONEMISC::MISC_ESCAPE" },
        { enumValue(MISC_FELLOW), 0x0002, "ZONEMISC::MISC_FELLOW" },
        { enumValue(MISC_MOUNT), 0x0004, "ZONEMISC::MISC_MOUNT" },
        { enumValue(MISC_MAZURKA), 0x0008, "ZONEMISC::MISC_MAZURKA" },
        { enumValue(MISC_TRACTOR), 0x0010, "ZONEMISC::MISC_TRACTOR" },
        { enumValue(MISC_MOGMENU), 0x0020, "ZONEMISC::MISC_MOGMENU" },
        { enumValue(MISC_COSTUME), 0x0040, "ZONEMISC::MISC_COSTUME" },
        { enumValue(MISC_PET), 0x0080, "ZONEMISC::MISC_PET" },
        { enumValue(MISC_TREASURE), 0x0100, "ZONEMISC::MISC_TREASURE" },
        { enumValue(MISC_AH), 0x0200, "ZONEMISC::MISC_AH" },
        { enumValue(MISC_YELL), 0x0400, "ZONEMISC::MISC_YELL" },
        { enumValue(MISC_TRUST), 0x0800, "ZONEMISC::MISC_TRUST" },
        { enumValue(MISC_LOS_PLAYER_BLOCK), 0x1000, "ZONEMISC::MISC_LOS_PLAYER_BLOCK" },
        { enumValue(MISC_LOS_OFF), 0x2000, "ZONEMISC::MISC_LOS_OFF" },
        { enumValue(MISC_ASSIST), 0x4000, "ZONEMISC::MISC_ASSIST" },
    });

    const auto flags = enumValue(MISC_YELL) | enumValue(MISC_ASSIST) | enumValue(MISC_MOGMENU);
    ok               = expectEqualInt(flags, 0x4420, "ZONEMISC mixed flag bits") && ok;
    ok               = expectTrue((flags & enumValue(MISC_ASSIST)) == enumValue(MISC_ASSIST), "ZONEMISC has MISC_ASSIST") && ok;
    ok               = expectTrue((flags & enumValue(MISC_AH)) == 0, "ZONEMISC missing MISC_AH") && ok;
    return ok;
}

} // namespace

auto runMapZoneEnumPrimitiveSelfTests() -> bool
{
    bool ok = true;
    ok      = testNationTypes() && ok;
    ok      = testRegionTypes() && ok;
    ok      = testContinentTypes() && ok;
    ok      = testZoneTypes() && ok;
    ok      = testGlobalMessageTypes() && ok;
    ok      = testTeleportTypes() && ok;
    ok      = testZoneMiscFlags() && ok;
    return ok;
}
