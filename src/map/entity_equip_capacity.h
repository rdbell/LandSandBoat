#pragma once

#include <cstdint>

// Pure GetEntityArmor / GetEntityWeapon slot gates.
// SLOTTYPE values match battle_entity.h.

namespace entityequiphelpers
{

constexpr std::uint8_t SlotMain  = 0x00;
constexpr std::uint8_t SlotAmmo  = 0x03;
constexpr std::uint8_t SlotHead  = 0x04;
constexpr std::uint8_t SlotLink2 = 0x11;

constexpr auto IsValidArmorSlot(const std::uint8_t slot) -> bool
{
    return slot >= SlotHead && slot <= SlotLink2;
}

constexpr auto IsValidWeaponSlot(const std::uint8_t slot) -> bool
{
    return slot >= SlotMain && slot <= SlotAmmo;
}

constexpr auto ShouldReturnPCArmor(const bool isPC) -> bool
{
    return isPC;
}

// DAYTYPE → ELEMENT for GetDayElement (common/vana_time.h DAYTYPE order).
constexpr std::uint8_t ElementNone    = 0;
constexpr std::uint8_t ElementFire    = 1;
constexpr std::uint8_t ElementIce     = 2;
constexpr std::uint8_t ElementWind    = 3;
constexpr std::uint8_t ElementEarth   = 4;
constexpr std::uint8_t ElementThunder = 5;
constexpr std::uint8_t ElementWater   = 6;
constexpr std::uint8_t ElementLight   = 7;
constexpr std::uint8_t ElementDark    = 8;

// weekday is vanadiel_time weekday / DAYTYPE ordinal 0..7 (FIRESDAY..DARKSDAY).
constexpr auto DayElementFromWeekday(const std::uint8_t weekday) -> std::uint8_t
{
    switch (weekday)
    {
        case 0: // FIRESDAY
            return ElementFire;
        case 1: // EARTHSDAY
            return ElementEarth;
        case 2: // WATERSDAY
            return ElementWater;
        case 3: // WINDSDAY
            return ElementWind;
        case 4: // ICEDAY
            return ElementIce;
        case 5: // LIGHTNINGDAY
            return ElementThunder;
        case 6: // LIGHTSDAY
            return ElementLight;
        case 7: // DARKSDAY
            return ElementDark;
        default:
            return ElementNone;
    }
}

} // namespace entityequiphelpers
