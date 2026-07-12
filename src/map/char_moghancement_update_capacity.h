#pragma once

#include <cstdint>
#include <functional>
#include <utility>

namespace charmoghancementupdatehelpers
{

template <typename Show, typename RemoveKeyItem, typename AddKeyItem, typename PushTable,
          typename SaveKeyItems, typename SetMoghancement, typename SaveMoghancement>
inline void Update(
    const std::uint16_t newID,
    const std::uint16_t currentID,
    Show&&              show,
    RemoveKeyItem&&     removeKeyItem,
    AddKeyItem&&        addKeyItem,
    PushTable&&         pushTable,
    SaveKeyItems&&      saveKeyItems,
    SetMoghancement&&   setMoghancement,
    SaveMoghancement&&  saveMoghancement)
{
    if (newID != 0)
    {
        std::invoke(std::forward<Show>(show), newID);
    }

    if (newID == currentID)
    {
        return;
    }

    if (currentID != 0)
    {
        std::invoke(std::forward<RemoveKeyItem>(removeKeyItem), currentID);
    }
    if (newID != 0)
    {
        std::invoke(std::forward<AddKeyItem>(addKeyItem), newID);
    }

    const auto newTable     = static_cast<std::uint8_t>(newID >> 9);
    const auto currentTable = static_cast<std::uint8_t>(currentID >> 9);
    if (newTable == currentTable)
    {
        std::invoke(pushTable, newTable);
    }
    else
    {
        if (newTable != 0)
        {
            std::invoke(pushTable, newTable);
        }
        if (currentTable != 0)
        {
            std::invoke(pushTable, currentTable);
        }
    }

    std::invoke(std::forward<SaveKeyItems>(saveKeyItems));
    std::invoke(std::forward<SetMoghancement>(setMoghancement), newID);
    std::invoke(std::forward<SaveMoghancement>(saveMoghancement));
}

} // namespace charmoghancementupdatehelpers
