#pragma once

#include <functional>
#include <utility>

namespace charequipflushhelpers
{

template <typename Changes, typename DirtyContainers, typename Container, typename LockFlag,
          typename IsSynced, typename IsCharged, typename SendEquipList, typename SendGrapList,
          typename SendItemAttr, typename SendItemList, typename SendItemSameContainer,
          typename SendItemSameCharacter, typename SendMagicData, typename SendCommandData,
          typename ClearChanges>
inline void Apply(
    const Changes&         changes,
    const DirtyContainers& dirtyContainers,
    const Container        inventoryContainer,
    const LockFlag         normalLockFlag,
    const LockFlag         noDropLockFlag,
    IsSynced&&             isSynced,
    IsCharged&&            isCharged,
    SendEquipList&&        sendEquipList,
    SendGrapList&&         sendGrapList,
    SendItemAttr&&         sendItemAttr,
    SendItemList&&         sendItemList,
    SendItemSameContainer&& sendItemSameContainer,
    SendItemSameCharacter&& sendItemSameCharacter,
    SendMagicData&&        sendMagicData,
    SendCommandData&&      sendCommandData,
    ClearChanges&&         clearChanges)
{
    if (changes.empty())
    {
        return;
    }

    for (const auto& change : changes)
    {
        if (change.equipping)
        {
            std::invoke(
                std::forward<SendEquipList>(sendEquipList),
                change.containerSlotId,
                change.equipSlot,
                change.container);
        }
        else
        {
            std::invoke(
                std::forward<SendEquipList>(sendEquipList),
                0,
                change.equipSlot,
                inventoryContainer);
        }
        std::invoke(std::forward<SendGrapList>(sendGrapList));
    }

    for (const auto& container : dirtyContainers)
    {
        for (const auto& change : changes)
        {
            if (static_cast<Container>(change.item->getLocationID()) != container)
            {
                continue;
            }

            if (std::invoke(std::forward<IsCharged>(isCharged), change.item))
            {
                std::invoke(
                    std::forward<SendItemAttr>(sendItemAttr),
                    change.item,
                    container,
                    change.item->getSlotID());
            }
            else
            {
                std::invoke(
                    std::forward<SendItemList>(sendItemList),
                    change.item,
                    change.equipping ? noDropLockFlag : normalLockFlag);
            }
        }

        if (std::invoke(std::forward<IsSynced>(isSynced), container))
        {
            std::invoke(std::forward<SendItemSameContainer>(sendItemSameContainer), container);
            std::invoke(std::forward<SendItemSameCharacter>(sendItemSameCharacter));
        }
    }

    std::invoke(std::forward<SendMagicData>(sendMagicData));
    std::invoke(std::forward<SendCommandData>(sendCommandData));
    std::invoke(std::forward<ClearChanges>(clearChanges));
}

} // namespace charequipflushhelpers
