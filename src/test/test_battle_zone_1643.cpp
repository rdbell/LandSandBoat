#include "test_battle_zone_1643.h"

#include "map/battle_zone_capacity.h"

#include <iostream>

namespace
{
using namespace battlezonehelpers;

auto Check() -> bool
{
    // --- Constant pins ---
    if (ZoneTypeDynamis != 0x0080 || ZoneTypeInstanced != 0x0100)
    {
        return false;
    }
    if (RegionWestAhtUrhgan != 28 || RegionAlzadaal != 32)
    {
        return false;
    }
    if (ZoneWesternAdoulin != 256 || ZoneEasternAdoulin != 257 ||
        ZoneMogGarden != 280 || ZoneSilverKnife != 283 ||
        ZoneCelenniaMemorialLibrary != 284)
    {
        return false;
    }

    // --- IsInDynamis ---
    if (IsInDynamis(false, ZoneTypeDynamis) || IsInDynamis(false, 0))
    {
        return false;
    }
    if (!IsInDynamis(true, ZoneTypeDynamis))
    {
        return false;
    }
    if (!IsInDynamis(true, static_cast<std::uint16_t>(ZoneTypeDynamis | 0x0001)))
    {
        return false;
    }
    if (IsInDynamis(true, ZoneTypeInstanced) || IsInDynamis(true, 0) || IsInDynamis(true, 0x0001))
    {
        return false;
    }

    // --- IsInAssault ---
    if (IsInAssault(false, ZoneTypeInstanced, RegionWestAhtUrhgan))
    {
        return false;
    }
    // Inclusive region range 28..32
    if (!IsInAssault(true, ZoneTypeInstanced, RegionWestAhtUrhgan) ||
        !IsInAssault(true, ZoneTypeInstanced, 29) ||
        !IsInAssault(true, ZoneTypeInstanced, 30) ||
        !IsInAssault(true, ZoneTypeInstanced, 31) ||
        !IsInAssault(true, ZoneTypeInstanced, RegionAlzadaal))
    {
        return false;
    }
    // Exclusive boundaries
    if (IsInAssault(true, ZoneTypeInstanced, static_cast<std::uint8_t>(RegionWestAhtUrhgan - 1)) ||
        IsInAssault(true, ZoneTypeInstanced, static_cast<std::uint8_t>(RegionAlzadaal + 1)))
    {
        return false;
    }
    // Instanced required
    if (IsInAssault(true, 0, RegionWestAhtUrhgan) ||
        IsInAssault(true, ZoneTypeDynamis, RegionWestAhtUrhgan))
    {
        return false;
    }
    // Combined bits still ok
    if (!IsInAssault(true, static_cast<std::uint16_t>(ZoneTypeInstanced | 0x0001), RegionWestAhtUrhgan))
    {
        return false;
    }
    if (!IsInAssault(true, static_cast<std::uint16_t>(ZoneTypeInstanced | ZoneTypeDynamis), RegionWestAhtUrhgan))
    {
        return false;
    }

    // --- IsInAdoulin ---
    const std::uint16_t adoulinZones[] = {
        ZoneWesternAdoulin,
        ZoneEasternAdoulin,
        ZoneMogGarden,
        ZoneSilverKnife,
        ZoneCelenniaMemorialLibrary,
    };
    for (const auto z : adoulinZones)
    {
        if (!IsInAdoulin(true, z) || IsInAdoulin(false, z))
        {
            return false;
        }
    }
    const std::uint16_t nonAdoulin[] = { 0, 1, 100, 255, 258, 279, 281, 282, 285, 999 };
    for (const auto z : nonAdoulin)
    {
        if (IsInAdoulin(true, z))
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runBattleZone1643SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "battle_zone_1643 self-tests failed\n";
        return false;
    }
    return true;
}
