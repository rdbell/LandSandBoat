#include "test_inventory_sync_dirty_items_2744.h"

#include "inventory_sync_dirty_items.h"

#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace
{

struct TestItem
{
    bool dirty{};
};

struct TestContainer
{
    uint8                  size{};
    std::array<TestItem*, 3> items{};
};

auto testFlushesDirtyItemsInContainerAndSlotOrder() -> bool
{
    TestItem clean{};
    TestItem first{ true };
    TestItem last{ true };
    TestContainer firstContainer{ 2, { &first, &clean, &last } };
    std::array<TestContainer*, 3> containers{ &firstContainer, nullptr, nullptr };
    std::vector<std::string>      events;

    inventorysyncdirtyitems::Flush(
        3,
        [&](const uint8 loc) { return containers[loc]; },
        [](const TestContainer* container) { return container->size; },
        [](const TestContainer* container, const uint8 slot) { return container->items[slot]; },
        [](const TestItem* item) { return item->dirty; },
        [&](const TestItem*, const uint8 loc, const uint8 slot)
        {
            events.emplace_back("persist:" + std::to_string(loc) + ":" + std::to_string(slot));
        },
        [&](TestItem*, const uint8 loc, const uint8 slot)
        {
            events.emplace_back("packet:" + std::to_string(loc) + ":" + std::to_string(slot));
        },
        [&](TestItem* item)
        {
            events.emplace_back("clean");
            item->dirty = false;
        });

    const std::vector<std::string> expected{
        "persist:0:0", "packet:0:0", "clean", "persist:0:2", "packet:0:2", "clean",
    };
    if (events != expected || first.dirty || last.dirty || clean.dirty)
    {
        std::cerr << "inventory sync dirty items self-test failed\n";
        return false;
    }
    return true;
}

} // namespace

auto runInventorySyncDirtyItems2744SelfTests() -> bool
{
    return testFlushesDirtyItemsInContainerAndSlotOrder();
}
