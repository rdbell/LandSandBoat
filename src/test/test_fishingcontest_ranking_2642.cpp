#include "test_fishingcontest_ranking_2642.h"

#include "map/fishingcontest.h"

#include <iostream>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest ranking self-test failed: " << label << '\n';
    }
    return condition;
}

auto entry(const uint32 score, const uint32 submitTime) -> FishingContestEntry
{
    FishingContestEntry result{};
    result.score      = score;
    result.submitTime = submitTime;
    return result;
}

auto testGreatestRanking() -> bool
{
    std::vector<FishingContestEntry> entries{
        entry(10, 30), entry(20, 20), entry(20, 10), entry(5, 40),
    };
    fishingcontest::RankContestEntries(entries, FISHING_CONTEST_MEASURE::GREATEST);

    bool ok = true;
    ok = expect(entries[0].score == 20 && entries[0].submitTime == 10, "greatest score then earliest submit") && ok;
    ok = expect(entries[1].score == 20 && entries[1].submitTime == 20, "greatest tie submit order") && ok;
    ok = expect(entries[2].score == 10 && entries[3].score == 5, "greatest score order") && ok;
    ok = expect(entries[0].contestRank == 1 && entries[1].contestRank == 1 && entries[2].contestRank == 3 && entries[3].contestRank == 4, "shared sequential ranks") && ok;
    for (const auto& value : entries)
    {
        const auto expectedShare = value.score == 20 ? 2 : 1;
        ok = expect(value.share == expectedShare && value.dataset_b == expectedShare && value.resultCount == 4, "greatest packet fields") && ok;
    }
    return ok;
}

auto testSmallestRanking() -> bool
{
    std::vector<FishingContestEntry> entries{
        entry(10, 30), entry(5, 20), entry(5, 10), entry(20, 40),
    };
    fishingcontest::RankContestEntries(entries, FISHING_CONTEST_MEASURE::SMALLEST);

    bool ok = true;
    ok = expect(entries[0].score == 5 && entries[0].submitTime == 10, "smallest score then earliest submit") && ok;
    ok = expect(entries[1].score == 5 && entries[1].submitTime == 20, "smallest tie submit order") && ok;
    ok = expect(entries[2].score == 10 && entries[3].score == 20, "smallest score order") && ok;
    ok = expect(entries[0].contestRank == 1 && entries[1].contestRank == 1 && entries[2].contestRank == 3 && entries[3].contestRank == 4, "smallest shared sequential ranks") && ok;
    return ok;
}

auto testZeroScoreRanking() -> bool
{
    std::vector<FishingContestEntry> entries{ entry(0, 20), entry(0, 10), entry(1, 30) };
    fishingcontest::RankContestEntries(entries, FISHING_CONTEST_MEASURE::SMALLEST);

    bool ok = true;
    ok = expect(entries[0].score == 0 && entries[0].submitTime == 10, "zero score earliest submit first") && ok;
    ok = expect(entries[1].score == 0 && entries[1].submitTime == 20, "zero score second submit") && ok;
    ok = expect(entries[0].contestRank == 1 && entries[1].contestRank == 1 && entries[2].contestRank == 3, "zero score shared rank") && ok;
    ok = expect(entries[0].share == 2 && entries[1].share == 2 && entries[2].share == 1, "zero score shares") && ok;
    ok = expect(entries[0].dataset_b == 2 && entries[1].dataset_b == 2 && entries[2].dataset_b == 1, "zero score datasets") && ok;
    ok = expect(entries[0].resultCount == 3 && entries[1].resultCount == 3 && entries[2].resultCount == 3, "zero score result count") && ok;
    return ok;
}

} // namespace

auto runFishingContestRanking2642SelfTests() -> bool
{
    return testGreatestRanking() && testSmallestRanking() && testZeroScoreRanking() &&
           expect(!fishingcontest::IsStageDue(100, 100), "stage equality is not due") &&
           expect(fishingcontest::IsStageDue(101, 100), "stage strictly after is due");
}
