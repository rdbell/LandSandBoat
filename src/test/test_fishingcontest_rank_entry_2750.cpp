#include "test_fishingcontest_rank_entry_2750.h"

#include "fishingcontest_rank_entry.h"

#include <iostream>
#include <vector>

namespace
{

auto testRankEntryFallbackPolicy() -> bool
{
    std::vector<int> entries{ 10, 20 };
    std::vector<int> placeholders{ 100, 200, 300 };

    auto* first = fishingcontest::RankEntryAt(entries, placeholders, 0);
    auto* second = fishingcontest::RankEntryAt(entries, placeholders, 1);
    auto* fallbackFirst = fishingcontest::RankEntryAt(entries, placeholders, 2);
    auto* fallbackWrap = fishingcontest::RankEntryAt(entries, placeholders, 6);
    std::vector<int> noPlaceholders;
    auto* missing = fishingcontest::RankEntryAt(entries, noPlaceholders, 2);

    if (!first || !second || !fallbackFirst || !fallbackWrap || *first != 10 || *second != 20 ||
        *fallbackFirst != 100 || *fallbackWrap != 200 || missing != nullptr)
    {
        std::cerr << "fishing contest rank entry self-test failed\n";
        return false;
    }
    return true;
}

} // namespace

auto runFishingContestRankEntry2750SelfTests() -> bool
{
    return testRankEntryFallbackPolicy();
}
