#pragma once

#include "common/cbasetypes.h"
#include "items.h"

#include <vector>

namespace mobgeodepoolhelpers
{

inline auto GeodeForElement(const uint8 element) -> uint16
{
    switch (element)
    {
        case 1: return FLAME_GEODE;
        case 2: return SNOW_GEODE;
        case 3: return BREEZE_GEODE;
        case 4: return SOIL_GEODE;
        case 5: return THUNDER_GEODE;
        case 6: return AQUA_GEODE;
        case 7: return LIGHT_GEODE;
        case 8: return SHADOW_GEODE;
        default: return 0;
    }
}

inline auto AvatariteForElement(const uint8 element) -> uint16
{
    switch (element)
    {
        case 1: return IFRITITE;
        case 2: return SHIVITE;
        case 3: return GARUDITE;
        case 4: return TITANITE;
        case 5: return RAMUITE;
        case 6: return LEVIATITE;
        case 7: return CARBITE;
        case 8: return FENRITE;
        default: return 0;
    }
}

inline auto EligibleGeodes(const bool abysseaEnabled, const uint8 level, const uint8 element) -> std::vector<uint16>
{
    if (!abysseaEnabled || level < 50)
    {
        return {};
    }
    if (level >= 80)
    {
        return { GeodeForElement(element), AvatariteForElement(element) };
    }
    return { GeodeForElement(element) };
}

} // namespace mobgeodepoolhelpers
