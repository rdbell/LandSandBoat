#include "test_char_equip_flush_1447.h"

#include "map/char_equip_flush_capacity.h"

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace
{

struct TestItem
{
    int  location{};
    int  slot{};
    bool charged{};

    auto getLocationID() const -> int
    {
        return location;
    }

    auto getSlotID() const -> int
    {
        return slot;
    }
};

struct TestChange
{
    int       container{};
    int       containerSlotId{};
    int       equipSlot{};
    TestItem* item{};
    bool      equipping{};
};

} // namespace

auto runCharEquipFlush1447SelfTests() -> bool
{
    constexpr int inventory = 0;
    constexpr int normal    = 0;
    constexpr int noDrop    = 5;

    std::vector<TestChange> emptyChanges{};
    std::set<int>           emptyDirty{};
    std::vector<std::string> calls{};
    charequipflushhelpers::Apply(
        emptyChanges,
        emptyDirty,
        inventory,
        normal,
        noDrop,
        [&](int) { calls.emplace_back("synced"); return true; },
        [&](TestItem*) { calls.emplace_back("charged"); return false; },
        [&](int, int, int) { calls.emplace_back("equip"); },
        [&]() { calls.emplace_back("grap"); },
        [&](TestItem*, int, int) { calls.emplace_back("attr"); },
        [&](TestItem*, int) { calls.emplace_back("list"); },
        [&](int) { calls.emplace_back("same container"); },
        [&]() { calls.emplace_back("same char"); },
        [&]() { calls.emplace_back("magic"); },
        [&]() { calls.emplace_back("command"); },
        [&]() { calls.emplace_back("clear"); });
    bool ok = calls.empty();

    TestItem equipped{ 11, 7, false };
    TestItem unequipped{ 2, 4, true };
    TestItem normalUnequipped{ 2, 5, false };
    std::vector<TestChange> changes{
        { 11, 7, 0, &equipped, true },
        { 0, 0, 1, &unequipped, false },
        { 0, 0, 4, &normalUnequipped, false },
    };
    std::set<int> dirty{ 2, 11 };
    bool          cleared = false;
    calls.clear();
    charequipflushhelpers::Apply(
        changes,
        dirty,
        inventory,
        normal,
        noDrop,
        [&](int container)
        {
            calls.emplace_back("synced:" + std::to_string(container));
            return container == 11;
        },
        [&](TestItem* item)
        {
            calls.emplace_back("charged:" + std::to_string(item->slot));
            return item->charged;
        },
        [&](int containerSlot, int equipSlot, int container)
        {
            calls.emplace_back(
                "equip:" + std::to_string(containerSlot) + ":" +
                std::to_string(equipSlot) + ":" + std::to_string(container));
        },
        [&]() { calls.emplace_back("grap"); },
        [&](TestItem* item, int container, int slot)
        {
            calls.emplace_back(
                "attr:" + std::to_string(item->slot) + ":" +
                std::to_string(container) + ":" + std::to_string(slot));
        },
        [&](TestItem* item, int lockFlag)
        {
            calls.emplace_back(
                "list:" + std::to_string(item->slot) + ":" + std::to_string(lockFlag));
        },
        [&](int container) { calls.emplace_back("same container:" + std::to_string(container)); },
        [&]() { calls.emplace_back("same char"); },
        [&]() { calls.emplace_back("magic"); },
        [&]() { calls.emplace_back("command"); },
        [&]()
        {
            calls.emplace_back("clear");
            cleared = true;
        });

    const std::vector<std::string> expected{
        "equip:7:0:11",
        "grap",
        "equip:0:1:0",
        "grap",
        "equip:0:4:0",
        "grap",
        "charged:4",
        "attr:4:2:4",
        "charged:5",
        "list:5:0",
        "synced:2",
        "charged:7",
        "list:7:5",
        "synced:11",
        "same container:11",
        "same char",
        "magic",
        "command",
        "clear",
    };
    ok = cleared && calls == expected && ok;

    if (!ok)
    {
        std::cerr << "char equip flush 1447 self-test failed\n";
    }
    return ok;
}
