#include "test_entity_equip_1542.h"

#include "map/entity_equip_capacity.h"

#include <iostream>

namespace
{
using entityequiphelpers::DayElementFromWeekday;
using entityequiphelpers::ElementDark;
using entityequiphelpers::ElementEarth;
using entityequiphelpers::ElementFire;
using entityequiphelpers::ElementIce;
using entityequiphelpers::ElementLight;
using entityequiphelpers::ElementNone;
using entityequiphelpers::ElementThunder;
using entityequiphelpers::ElementWater;
using entityequiphelpers::ElementWind;
using entityequiphelpers::IsValidArmorSlot;
using entityequiphelpers::IsValidWeaponSlot;
using entityequiphelpers::ShouldReturnPCArmor;
using entityequiphelpers::SlotAmmo;
using entityequiphelpers::SlotHead;
using entityequiphelpers::SlotLink2;
using entityequiphelpers::SlotMain;

auto Check() -> bool
{
    if (!IsValidWeaponSlot(SlotMain) || !IsValidWeaponSlot(SlotAmmo) || IsValidWeaponSlot(SlotHead))
    {
        return false;
    }
    if (!IsValidArmorSlot(SlotHead) || !IsValidArmorSlot(SlotLink2) || IsValidArmorSlot(SlotMain) || IsValidArmorSlot(0x12))
    {
        return false;
    }
    if (!ShouldReturnPCArmor(true) || ShouldReturnPCArmor(false))
    {
        return false;
    }
    if (DayElementFromWeekday(0) != ElementFire || DayElementFromWeekday(1) != ElementEarth ||
        DayElementFromWeekday(2) != ElementWater || DayElementFromWeekday(3) != ElementWind ||
        DayElementFromWeekday(4) != ElementIce || DayElementFromWeekday(5) != ElementThunder ||
        DayElementFromWeekday(6) != ElementLight || DayElementFromWeekday(7) != ElementDark ||
        DayElementFromWeekday(8) != ElementNone || DayElementFromWeekday(9) != ElementNone)
    {
        return false;
    }
    return true;
}
} // namespace

auto runEntityEquip1542SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "entity_equip_1542 self-tests failed\n";
        return false;
    }
    return true;
}
