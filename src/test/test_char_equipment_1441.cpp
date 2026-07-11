#include "test_char_equipment_1441.h"

#include "map/char_equipment_capacity.h"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

namespace
{
struct TestItem
{
    int   id{};
    uint8 container{};
    uint8 slot{};
};
} // namespace

auto runCharEquipment1441SelfTests() -> bool
{
    std::array<TestItem*, 18> slots{};
    TestItem                  oldItem{ 1, 3, 7 };
    TestItem                  newItem{ 2, 5, 9 };

    bool ok = charequipmenthelpers::IsValidSlot<18>(0) &&
              charequipmenthelpers::IsValidSlot<18>(17) &&
              !charequipmenthelpers::IsValidSlot<18>(18) &&
              charequipmenthelpers::Get(slots, 18) == nullptr;

    slots[4]       = &oldItem;
    const auto loc = charequipmenthelpers::Location(
        slots,
        4,
        [](TestItem* item) { return std::pair<uint8, uint8>{ item->container, item->slot }; });
    ok = loc == std::optional{ std::pair<uint8, uint8>{ 3, 7 } } &&
         !charequipmenthelpers::Location(
              slots,
              5,
              [](TestItem* item) { return std::pair<uint8, uint8>{ item->container, item->slot }; }) &&
         ok;

    std::vector<int> calls{};
    const auto bound = charequipmenthelpers::Bind(
        slots,
        4,
        &newItem,
        [&](TestItem* item)
        {
            calls.push_back(10 + item->id);
            return true;
        },
        [&](TestItem* item)
        {
            calls.push_back(20 + item->id);
            return true;
        });
    ok = bound && slots[4] == &newItem && calls == std::vector<int>{ 12, 21 } && ok;

    calls.clear();
    slots[4] = &oldItem;
    const auto rejected = charequipmenthelpers::Bind(
        slots,
        4,
        &newItem,
        [&](TestItem* item)
        {
            calls.push_back(30 + item->id);
            return false;
        },
        [&](TestItem* item)
        {
            calls.push_back(40 + item->id);
            return true;
        });
    ok = !rejected && slots[4] == &oldItem && calls == std::vector<int>{ 32 } && ok;

    calls.clear();
    const auto replacedAfterReleaseFailure = charequipmenthelpers::Bind(
        slots,
        4,
        &newItem,
        [&](TestItem* item)
        {
            calls.push_back(50 + item->id);
            return true;
        },
        [&](TestItem* item)
        {
            calls.push_back(60 + item->id);
            return false;
        });
    ok = replacedAfterReleaseFailure && slots[4] == &newItem && calls == std::vector<int>{ 52, 61 } && ok;

    charequipmenthelpers::Clear(slots, 4, [](TestItem*) { return false; });
    ok = slots[4] == &newItem && ok;
    charequipmenthelpers::Clear(slots, 4, [](TestItem*) { return true; });
    ok = slots[4] == nullptr && ok;

    calls.clear();
    ok = !charequipmenthelpers::Bind(
             slots,
             18,
             &newItem,
             [&](TestItem*)
             {
                 calls.push_back(1);
                 return true;
             },
             [](TestItem*) { return true; }) &&
         !charequipmenthelpers::Bind(
             slots,
             0,
             static_cast<TestItem*>(nullptr),
             [&](TestItem*)
             {
                 calls.push_back(2);
                 return true;
             },
             [](TestItem*) { return true; }) &&
         calls.empty() && ok;

    if (!ok)
    {
        std::cerr << "char equipment 1441 self-test failed\n";
    }
    return ok;
}
