#include "test_char_inventory_sync_plan_6896.h"

#include "map/char_inventory_sync_plan.h"

#include <iostream>

auto runCharInventorySyncPlan6896SelfTests() -> bool
{
    constexpr auto expected = std::array<CONTAINER_ID, MAX_CONTAINER_ID>{
        LOC_INVENTORY, LOC_MOGSAFE, LOC_MOGSAFE2, LOC_STORAGE, LOC_RECYCLEBIN,
        LOC_WARDROBE, LOC_WARDROBE2, LOC_WARDROBE3, LOC_WARDROBE4,
        LOC_WARDROBE5, LOC_WARDROBE6, LOC_WARDROBE7, LOC_WARDROBE8,
        LOC_TEMPITEMS, LOC_MOGLOCKER, LOC_MOGSATCHEL, LOC_MOGSACK, LOC_MOGCASE,
    };
    const bool ok = inventorysyncpackethelpers::BuildContainerPlan() == expected;
    if (!ok)
    {
        std::cerr << "inventory sync plan 6896 self-test failed\n";
    }
    return ok;
}
