#include "test_mob_geode_element_2701.h"

#include "map/entities/mob_geode_element.h"

#include <iostream>

auto runMobGeodeElement2701SelfTests() -> bool
{
    using mobgeodehelpers::ResolveElement;
    int dayCalls = 0;
    const auto day = [&](const uint8 element) { return [&, element] { ++dayCalls; return element; }; };
    const bool ok =
        ResolveElement(Weather::None, day(ELEMENT_LIGHT)) == ELEMENT_LIGHT &&
        ResolveElement(Weather::Fog, day(ELEMENT_DARK)) == ELEMENT_DARK &&
        ResolveElement(Weather::HotSpell, day(ELEMENT_WATER)) == ELEMENT_FIRE &&
        ResolveElement(Weather::HeatWave, day(ELEMENT_WATER)) == ELEMENT_FIRE &&
        ResolveElement(Weather::Rain, day(ELEMENT_FIRE)) == ELEMENT_WATER &&
        ResolveElement(Weather::Squall, day(ELEMENT_FIRE)) == ELEMENT_WATER &&
        ResolveElement(Weather::DustStorm, day(ELEMENT_FIRE)) == ELEMENT_EARTH &&
        ResolveElement(Weather::SandStorm, day(ELEMENT_FIRE)) == ELEMENT_EARTH &&
        ResolveElement(Weather::Wind, day(ELEMENT_FIRE)) == ELEMENT_WIND &&
        ResolveElement(Weather::Gales, day(ELEMENT_FIRE)) == ELEMENT_WIND &&
        ResolveElement(Weather::Snow, day(ELEMENT_FIRE)) == ELEMENT_ICE &&
        ResolveElement(Weather::Blizzards, day(ELEMENT_FIRE)) == ELEMENT_ICE &&
        ResolveElement(Weather::Thunder, day(ELEMENT_FIRE)) == ELEMENT_THUNDER &&
        ResolveElement(Weather::Thunderstorms, day(ELEMENT_FIRE)) == ELEMENT_THUNDER &&
        ResolveElement(Weather::Auroras, day(ELEMENT_FIRE)) == ELEMENT_LIGHT &&
        ResolveElement(Weather::StellarGlare, day(ELEMENT_FIRE)) == ELEMENT_LIGHT &&
        ResolveElement(Weather::Gloom, day(ELEMENT_FIRE)) == ELEMENT_DARK &&
        ResolveElement(Weather::Darkness, day(ELEMENT_FIRE)) == ELEMENT_DARK &&
        ResolveElement(static_cast<Weather>(20), day(ELEMENT_WIND)) == ELEMENT_WIND &&
        dayCalls == 3;
    if (!ok)
    {
        std::cerr << "mob geode element 2701 failed\n";
    }
    return ok;
}
