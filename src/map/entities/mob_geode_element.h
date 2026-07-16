#pragma once

#include "common/cbasetypes.h"
#include "enums/weather.h"
#include "utils/battleutils.h"

namespace mobgeodehelpers
{

// Resolves the elemental drop alignment for GetEligibleGeodes. Weather takes
// priority over the day element; this intentionally does not use
// zoneutils::GetWeatherElement because that helper has a documented element
// ordering issue in this source tree.
template <typename DayElement>
inline auto ResolveElement(const Weather weather, DayElement&& dayElement) -> uint8
{
    switch (weather)
    {
        case Weather::HotSpell:
        case Weather::HeatWave:
            return ELEMENT_FIRE;
        case Weather::Rain:
        case Weather::Squall:
            return ELEMENT_WATER;
        case Weather::DustStorm:
        case Weather::SandStorm:
            return ELEMENT_EARTH;
        case Weather::Wind:
        case Weather::Gales:
            return ELEMENT_WIND;
        case Weather::Snow:
        case Weather::Blizzards:
            return ELEMENT_ICE;
        case Weather::Thunder:
        case Weather::Thunderstorms:
            return ELEMENT_THUNDER;
        case Weather::Auroras:
        case Weather::StellarGlare:
            return ELEMENT_LIGHT;
        case Weather::Gloom:
        case Weather::Darkness:
            return ELEMENT_DARK;
        default:
            return dayElement();
    }
}

} // namespace mobgeodehelpers
