#pragma once

#include "common/cbasetypes.h"

namespace charbazaarhelpers
{

template <typename InventoryGetter, typename SizeGetter, typename PriceGetter>
inline auto HasBazaar(
    const bool isSettingBazaarPrices,
    InventoryGetter&& inventoryGetter,
    SizeGetter&& sizeGetter,
    PriceGetter&& priceGetter) -> bool
{
    if (isSettingBazaarPrices)
    {
        return false;
    }

    auto* inventory = inventoryGetter();
    if (inventory == nullptr)
    {
        return false;
    }

    const uint8 size = sizeGetter(inventory);
    for (uint8 slotId = 1; slotId <= size; ++slotId)
    {
        if (priceGetter(inventory, slotId) != 0)
        {
            return true;
        }
    }
    return false;
}

} // namespace charbazaarhelpers
