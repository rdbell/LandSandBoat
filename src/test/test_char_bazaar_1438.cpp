#include "test_char_bazaar_1438.h"

#include "map/char_bazaar_capacity.h"

#include <array>
#include <iostream>
#include <vector>

auto runCharBazaar1438SelfTests() -> bool
{
    struct Inventory
    {
        uint8                  size{};
        std::array<uint32, 5> prices{};
    } inventory{ 4, { 999, 0, 0, 77, 88 } };

    unsigned           inventoryCalls = 0;
    std::vector<uint8> visitedSlots{};
    Inventory*         currentInventory = &inventory;
    const auto getInventory = [&]()
    {
        ++inventoryCalls;
        return currentInventory;
    };
    const auto getSize = [](const Inventory* value) { return value->size; };
    const auto getPrice = [&](const Inventory* value, const uint8 slotId)
    {
        visitedSlots.push_back(slotId);
        return value->prices[slotId];
    };

    bool ok = !charbazaarhelpers::HasBazaar(true, getInventory, getSize, getPrice) &&
              inventoryCalls == 0 && visitedSlots.empty();

    currentInventory = nullptr;
    ok = !charbazaarhelpers::HasBazaar(false, getInventory, getSize, getPrice) &&
         inventoryCalls == 1 && visitedSlots.empty() && ok;

    currentInventory = &inventory;
    ok = charbazaarhelpers::HasBazaar(false, getInventory, getSize, getPrice) &&
         inventoryCalls == 2 && visitedSlots == std::vector<uint8>{ 1, 2, 3 } && ok;

    visitedSlots.clear();
    inventory.prices = { 500, 0, 0, 0, 91 };
    ok = charbazaarhelpers::HasBazaar(false, getInventory, getSize, getPrice) &&
         visitedSlots == std::vector<uint8>{ 1, 2, 3, 4 } && ok;

    visitedSlots.clear();
    inventory.prices = { 500, 0, 0, 0, 0 };
    ok = !charbazaarhelpers::HasBazaar(false, getInventory, getSize, getPrice) &&
         visitedSlots == std::vector<uint8>{ 1, 2, 3, 4 } && ok;

    inventory.size = 0;
    visitedSlots.clear();
    ok = !charbazaarhelpers::HasBazaar(false, getInventory, getSize, getPrice) && visitedSlots.empty() && ok;

    if (!ok)
    {
        std::cerr << "char bazaar 1438 self-test failed\n";
    }
    return ok;
}
