#include "test_daily_item_classification_2643.h"

#include <array>
#include <iostream>

#include "map/daily_system.h"

namespace
{

auto classify(const uint16 itemID, const uint16 auctionHouseCategory, const ItemFlag flags = ItemFlag::None) -> uint8
{
    const auto result = daily::ClassifyDailyItem({ itemID, auctionHouseCategory, flags });
    return result.special ? result.numberedDial : 0;
}

} // namespace

auto runDailyItemClassification2643SelfTests() -> bool
{
    bool ok = true;

    // LoadDailyItems only hands this helper rows admitted by its flags & 4
    // query; every such row must therefore join the special dial, including
    // rows excluded from a numbered dial.
    const auto specialRows = std::array<daily::DailyItemRow, 34>{
        daily::DailyItemRow{ 1000, 38, ItemFlag::None }, daily::DailyItemRow{ 1000, 39, ItemFlag::None },
        daily::DailyItemRow{ 1000, 40, ItemFlag::None }, daily::DailyItemRow{ 1000, 41, ItemFlag::None },
        daily::DailyItemRow{ 1000, 42, ItemFlag::None }, daily::DailyItemRow{ 1000, 43, ItemFlag::None },
        daily::DailyItemRow{ 1000, 44, ItemFlag::None }, daily::DailyItemRow{ 1000, 50, ItemFlag::None },
        daily::DailyItemRow{ 1000, 52, ItemFlag::None }, daily::DailyItemRow{ 1000, 53, ItemFlag::None },
        daily::DailyItemRow{ 1000, 54, ItemFlag::None }, daily::DailyItemRow{ 1000, 55, ItemFlag::None },
        daily::DailyItemRow{ 1000, 56, ItemFlag::None }, daily::DailyItemRow{ 1000, 57, ItemFlag::None },
        daily::DailyItemRow{ 1000, 58, ItemFlag::None }, daily::DailyItemRow{ 1000, 33, ItemFlag::None },
        daily::DailyItemRow{ 1000, 15, ItemFlag::CanUse }, daily::DailyItemRow{ 1000, 36, ItemFlag::CanUse },
        daily::DailyItemRow{ 1000, 49, ItemFlag::CanUse }, daily::DailyItemRow{ 1000, 15, ItemFlag::None },
        daily::DailyItemRow{ 1000, 36, ItemFlag::None }, daily::DailyItemRow{ 1000, 49, ItemFlag::None },
        daily::DailyItemRow{ 1000, 47, ItemFlag::None }, daily::DailyItemRow{ 1000, 51, ItemFlag::None },
        daily::DailyItemRow{ 489, 47, ItemFlag::None }, daily::DailyItemRow{ 17386, 51, ItemFlag::None },
        daily::DailyItemRow{ 605, 0, ItemFlag::None }, daily::DailyItemRow{ 1020, 0, ItemFlag::None },
        daily::DailyItemRow{ 1021, 0, ItemFlag::None }, daily::DailyItemRow{ 1022, 0, ItemFlag::None },
        daily::DailyItemRow{ 1023, 0, ItemFlag::None }, daily::DailyItemRow{ 15453, 0, ItemFlag::None },
        daily::DailyItemRow{ 15454, 0, ItemFlag::None }, daily::DailyItemRow{ 999, 0, ItemFlag::None },
    };
    for (const auto& row : specialRows)
    {
        ok = daily::ClassifyDailyItem(row).special && ok;
    }

    for (const auto category : std::array<uint16, 8>{ 38, 39, 40, 41, 42, 43, 44, 50 })
    {
        ok = classify(1000, category) == 1 && ok;
    }
    for (const auto category : std::array<uint16, 7>{ 52, 53, 54, 55, 56, 57, 58 })
    {
        ok = classify(1000, category) == 2 && ok;
    }

    ok = classify(1000, 33) == 3 && ok;
    for (const auto category : std::array<uint16, 3>{ 15, 36, 49 })
    {
        ok = classify(1000, category, ItemFlag::CanUse) == 4 && ok;
        ok = classify(1000, category) == 0 && ok;
    }
    for (const auto category : std::array<uint16, 2>{ 47, 51 })
    {
        ok = classify(1000, category) == 5 && ok;
    }
    ok = classify(489, 47) == 0 && ok;
    ok = classify(17386, 51) == 0 && ok;
    for (const auto itemID : std::array<uint16, 7>{ 605, 1020, 1021, 1022, 1023, 15453, 15454 })
    {
        ok = classify(itemID, 0) == 5 && ok;
    }
    ok = classify(999, 0) == 0 && ok;

    if (!ok)
    {
        std::cerr << "daily item classification self-test failed\\n";
    }
    return ok;
}
