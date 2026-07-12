#include "test_check_equipment_1510.h"

#include "map/check_equipment_capacity.h"

#include <iostream>

namespace
{
using checkequipmenthelpers::EffectiveLevelForGearReq;
using checkequipmenthelpers::IsEquipmentSlotInRange;
using checkequipmenthelpers::IsSubSlot;
using checkequipmenthelpers::JobBitForMainJob;
using checkequipmenthelpers::ShouldCheckUnarmedWeapon;
using checkequipmenthelpers::ShouldKeepEquipment;
using checkequipmenthelpers::ShouldSkipNonEquipment;
using checkequipmenthelpers::ShouldUnequipByLevel;
using checkequipmenthelpers::ShouldUnequipInvalidSub;
using checkequipmenthelpers::SlotBitForEquipSlot;

auto Check() -> bool
{
    if (EffectiveLevelForGearReq(true, 50, 75) != 50 || EffectiveLevelForGearReq(false, 50, 75) != 75)
    {
        return false;
    }
    if (!ShouldUnequipByLevel(60, 50) || ShouldUnequipByLevel(50, 50) || ShouldUnequipByLevel(40, 50))
    {
        return false;
    }
    if (!IsSubSlot(1) || IsSubSlot(0))
    {
        return false;
    }
    if (ShouldUnequipInvalidSub(true, false, false, false))
    {
        return false; // shield always kept by this gate
    }
    if (!ShouldUnequipInvalidSub(false, false, true, false) || !ShouldUnequipInvalidSub(false, true, false, false))
    {
        return false;
    }
    if (ShouldUnequipInvalidSub(false, true, true, false) || ShouldUnequipInvalidSub(false, true, false, true))
    {
        return false;
    }
    if (JobBitForMainJob(1) != 1 || JobBitForMainJob(2) != 2 || JobBitForMainJob(0) != 0)
    {
        return false;
    }
    if (SlotBitForEquipSlot(0) != 1 || SlotBitForEquipSlot(3) != 8)
    {
        return false;
    }
    if (!ShouldKeepEquipment(0x2, 0x2, 0x8, 0x8) || ShouldKeepEquipment(0x2, 0x4, 0x8, 0x8) || ShouldKeepEquipment(0x2, 0x2, 0x4, 0x8))
    {
        return false;
    }
    if (!ShouldCheckUnarmedWeapon(false, true, false) || !ShouldCheckUnarmedWeapon(true, false, false) || !ShouldCheckUnarmedWeapon(true, true, true))
    {
        return false;
    }
    if (ShouldCheckUnarmedWeapon(true, true, false))
    {
        return false;
    }
    if (!IsEquipmentSlotInRange(0) || !IsEquipmentSlotInRange(15) || IsEquipmentSlotInRange(16))
    {
        return false;
    }
    if (!ShouldSkipNonEquipment(false, true) || !ShouldSkipNonEquipment(true, false) || ShouldSkipNonEquipment(true, true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCheckEquipment1510SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "check equipment 1510 self-test failed\n";
    }
    return ok;
}
