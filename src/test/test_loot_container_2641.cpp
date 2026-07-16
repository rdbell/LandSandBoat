#include "map/utils/itemutils.h"

#include <iostream>
#include <vector>

auto runLootContainer2641SelfTests() -> bool
{
    DropList_t source;
    source.Groups.emplace_back(10);
    source.Items.emplace_back(DROP_NORMAL, 100, 1000);

    LootContainer container(&source);
    container.drops.Groups.emplace_back(20, true);
    container.drops.Items.emplace_back(DROP_STEAL, 200, 500, true);

    // LootContainer retains a pointer, rather than a construction-time copy.
    source.Groups.emplace_back(30);
    source.Items.emplace_back(DROP_DESPOIL, 300, 250);

    std::vector<uint16> groupRates;
    container.ForEachGroup([&](const DropGroup_t& group)
    {
        groupRates.push_back(group.GroupRate);
    });

    std::vector<uint16> itemIDs;
    container.ForEachItem([&](const DropItem_t& item)
    {
        itemIDs.push_back(item.ItemID);
    });

    DropList_t empty;
    LootContainer emptyContainer(&empty);
    uint8 groupCalls = 0;
    uint8 itemCalls  = 0;
    emptyContainer.ForEachGroup([&](const DropGroup_t)
    {
        ++groupCalls;
    });
    emptyContainer.ForEachItem([&](const DropItem_t)
    {
        ++itemCalls;
    });

    const auto valid = groupRates == std::vector<uint16>{ 10, 30, 20 } &&
                       itemIDs == std::vector<uint16>{ 100, 300, 200 } &&
                       groupCalls == 0 && itemCalls == 0;
    if (!valid)
    {
        std::cerr << "loot container 2641 self-test failed\n";
    }
    return valid;
}
