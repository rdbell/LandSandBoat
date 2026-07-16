#include "test_fishingcontest_placeholders_2645.h"

#include "map/fishingcontest.h"

#include <cstring>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest placeholder self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runFishingContestPlaceholders2645SelfTests() -> bool
{
    bool ok = true;
    ok = expect(fishingcontest::GeneratePlaceholderEntries(FISHING_CONTEST_STATUS::ACCEPTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 0).empty(), "presenting gate") && ok;
    ok = expect(fishingcontest::GeneratePlaceholderEntries(FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 4).empty(), "full real count gate") && ok;

    const auto greatest = fishingcontest::GeneratePlaceholderEntries(FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 2);
    ok                  = expect(greatest.size() == 4, "native max-entry loop quirk") && ok;
    if (greatest.size() == 4)
    {
        const auto& first = greatest.front();
        const auto& last  = greatest.back();
        ok = expect(std::strcmp(first.name, " SmallFisher01 ") == 0 && first.mjob == 1 && first.sjob == 0 && first.mlvl == 75 && first.slvl == 0 && first.race == 1 && first.allegiance == 0 && first.fishRank == 1 && first.score == 2 && first.submitTime == 0 && first.contestRank == 3 && first.resultCount == 0 && first.share == 0 && first.dataset_b == 0, "first generated fields") && ok;
        ok = expect(std::strcmp(last.name, " SmallFisher04 ") == 0 && last.mjob == 4 && last.mlvl == 40 && last.race == 4 && last.allegiance == 0 && last.score == std::numeric_limits<uint32>::max() && last.contestRank == 6, "greatest ladder underflow") && ok;
    }

    const auto smallest = fishingcontest::GeneratePlaceholderEntries(FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::SMALLEST, 20, 18);
    ok                  = expect(smallest.size() == 20 && smallest.front().score == 9998 && smallest.back().score == 10017, "smallest ladder") && ok;
    if (smallest.size() == 20)
    {
        ok = expect(smallest[18].mjob == 1 && smallest[5].mlvl == 75 && smallest[7].race == 1 && smallest[3].allegiance == 0, "field cycles") && ok;
    }
    return ok;
}
