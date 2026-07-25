#include "test_char_inventory_equipment_slots_6897.h"

#include "map/char_inventory_equipment_slots.h"

#include <iostream>

auto runCharInventoryEquipmentSlots6897SelfTests() -> bool
{
    const auto plan = inventoryequiphelpers::BuildStandardSlotPlan();
    bool       ok   = plan.size() == SLOT_LINK1;
    for (uint8 slot = SLOT_MAIN; slot < plan.size(); ++slot)
    {
        ok = plan[slot] == static_cast<SLOTTYPE>(slot) && ok;
    }
    if (!ok)
    {
        std::cerr << "inventory equipment slots 6897 self-test failed\n";
    }
    return ok;
}
