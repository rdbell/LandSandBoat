#include "test_daily_rare_fallback_2747.h"

#include "daily_system.h"

#include <iostream>
#include <vector>

namespace
{

auto testRareOwnedSelectionUsesGobbieJunk() -> bool
{
    const std::vector<uint16> materials{ 100, 101 };
    const std::vector<uint16> food{};
    const std::vector<uint16> medicine{};
    const std::vector<uint16> sundries1{};
    const std::vector<uint16> sundries2{};
    const std::vector<uint16> special{};
    const std::vector<uint16> junk{ 900, 901, 902 };
    const daily::DailyDialPools pools{ materials, food, medicine, sundries1, sundries2, special, junk };
    std::vector<size_t>        randomSizes;
    std::vector<uint16>        rareChecks;
    std::vector<uint16>        ownershipChecks;

    const auto selection = daily::SelectItemWithRareFallback(
        pools,
        1,
        [&](const size_t size)
        {
            randomSizes.push_back(size);
            return size == materials.size() ? 1u : 2u;
        },
        [&](const uint16 itemId)
        {
            rareChecks.push_back(itemId);
            return itemId == 101;
        },
        [&](const uint16 itemId)
        {
            ownershipChecks.push_back(itemId);
            return itemId == 101;
        });

    if (selection != 902 || randomSizes != std::vector<size_t>{ 2, 3 } ||
        rareChecks != std::vector<uint16>{ 101 } || ownershipChecks != std::vector<uint16>{ 101 })
    {
        std::cerr << "daily rare fallback self-test failed\n";
        return false;
    }
    return true;
}

auto testNonRareSelectionDoesNotUseFallback() -> bool
{
    const std::vector<uint16> source{ 100 };
    const std::vector<uint16> empty{};
    const std::vector<uint16> junk{ 900 };
    const daily::DailyDialPools pools{ source, empty, empty, empty, empty, empty, junk };
    size_t                     randomCalls = 0;
    size_t                     ownershipCalls = 0;

    const auto selection = daily::SelectItemWithRareFallback(
        pools,
        1,
        [&](const size_t)
        {
            ++randomCalls;
            return 0u;
        },
        [](const uint16) { return false; },
        [&](const uint16)
        {
            ++ownershipCalls;
            return true;
        });

    if (selection != 100 || randomCalls != 1 || ownershipCalls != 0)
    {
        std::cerr << "daily non-rare selection self-test failed\n";
        return false;
    }
    return true;
}

} // namespace

auto runDailyRareFallback2747SelfTests() -> bool
{
    return testRareOwnedSelectionUsesGobbieJunk() && testNonRareSelectionDoesNotUseFallback();
}
