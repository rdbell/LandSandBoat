#pragma once

#include "item_container.h"

#include <array>

namespace inventorysyncpackethelpers
{

constexpr auto BuildContainerPlan() -> std::array<CONTAINER_ID, MAX_CONTAINER_ID>
{
    return {
        LOC_INVENTORY, LOC_MOGSAFE, LOC_MOGSAFE2, LOC_STORAGE, LOC_RECYCLEBIN,
        LOC_WARDROBE, LOC_WARDROBE2, LOC_WARDROBE3, LOC_WARDROBE4,
        LOC_WARDROBE5, LOC_WARDROBE6, LOC_WARDROBE7, LOC_WARDROBE8,
        LOC_TEMPITEMS, LOC_MOGLOCKER, LOC_MOGSATCHEL, LOC_MOGSACK, LOC_MOGCASE,
    };
}

} // namespace inventorysyncpackethelpers
