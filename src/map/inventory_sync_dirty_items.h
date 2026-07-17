#pragma once

#include "common/cbasetypes.h"

#include <functional>
#include <utility>

// The dirty-item portion of InventorySyncState is deliberately expressed as a
// small host-independent traversal. Character storage, persistence, and packet
// emission remain supplied by the production caller.
namespace inventorysyncdirtyitems
{

template <typename GetStorage, typename GetSize, typename GetItem, typename IsDirty, typename Persist, typename SendItemAttr, typename MarkClean>
inline void Flush(
    const uint8     maxContainerId,
    GetStorage&&    getStorage,
    GetSize&&       getSize,
    GetItem&&       getItem,
    IsDirty&&       isDirty,
    Persist&&       persist,
    SendItemAttr&&  sendItemAttr,
    MarkClean&&     markClean)
{
    for (uint8 loc = 0; loc < maxContainerId; ++loc)
    {
        auto* container = std::invoke(getStorage, loc);
        if (!container)
        {
            continue;
        }

        // CItemContainer slots are inclusive: slot zero through GetSize().
        for (uint8 slot = 0; slot <= std::invoke(getSize, container); ++slot)
        {
            auto* item = std::invoke(getItem, container, slot);
            if (!item || !std::invoke(isDirty, item))
            {
                continue;
            }

            std::invoke(persist, item, loc, slot);
            std::invoke(sendItemAttr, item, loc, slot);
            std::invoke(markClean, item);
        }
    }
}

} // namespace inventorysyncdirtyitems
