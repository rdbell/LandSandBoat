#include "test_fishing_contest_rank_2847.h"

#include "map/fishingcontest.h"
#include "map/fishingcontest_rank_capacity.h"

#include <iostream>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest rank 2847 self-test failed: " << label << '\n';
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

// Pure ShouldRankBefore truth table (measure inject as greatest bool).
auto testShouldRankBefore() -> bool
{
    using fishingcontestrankhelpers::ShouldRankBefore;

    bool ok = true;

    // Greatest: higher score first
    ok = expect(ShouldRankBefore(20, 10, 10, 10, true), "greatest higher before lower") && ok;
    ok = expect(!ShouldRankBefore(10, 10, 20, 10, true), "greatest lower not before higher") && ok;
    // Smallest: production form bScore > aScore
    ok = expect(ShouldRankBefore(5, 10, 10, 10, false), "smallest lower before higher") && ok;
    ok = expect(!ShouldRankBefore(10, 10, 5, 10, false), "smallest higher not before lower") && ok;
    // Equal scores: earlier submit first, independent of measure
    ok = expect(ShouldRankBefore(20, 10, 20, 20, true), "greatest tie earlier submit") && ok;
    ok = expect(!ShouldRankBefore(20, 20, 20, 10, true), "greatest tie later submit") && ok;
    ok = expect(ShouldRankBefore(5, 10, 5, 20, false), "smallest tie earlier submit") && ok;
    ok = expect(!ShouldRankBefore(5, 20, 5, 10, false), "smallest tie later submit") && ok;
    // Equal score and equal submit: neither strictly before
    ok = expect(!ShouldRankBefore(7, 3, 7, 3, true), "equal pair not before (greatest)") && ok;
    ok = expect(!ShouldRankBefore(7, 3, 7, 3, false), "equal pair not before (smallest)") && ok;

    return ok;
}

// Pure rank-share assignment helpers.
auto testRankShareHelpers() -> bool
{
    using fishingcontestrankhelpers::AssignedContestRank;
    using fishingcontestrankhelpers::RankDatasetB;
    using fishingcontestrankhelpers::RankResultCount;
    using fishingcontestrankhelpers::ScoreMatchesShare;
    using fishingcontestrankhelpers::ShouldAdvanceRunningScore;

    bool ok = true;

    // First entry (running score starts at 0): nonzero score advances to rank 1
    ok = expect(AssignedContestRank(20, 0, 1, 1) == 1, "first nonzero rank 1") && ok;
    ok = expect(ShouldAdvanceRunningScore(20, 0), "first nonzero advances") && ok;

    // Zero-score first entry shares previous rank 1 without advancing
    ok = expect(AssignedContestRank(0, 0, 1, 1) == 1, "zero first uses previous rank") && ok;
    ok = expect(!ShouldAdvanceRunningScore(0, 0), "zero first does not advance") && ok;

    // Shared tie: same score keeps previous rank (1, 1, 3 pattern)
    ok = expect(AssignedContestRank(20, 20, 2, 1) == 1, "tie keeps previous rank") && ok;
    ok = expect(!ShouldAdvanceRunningScore(20, 20), "tie does not advance") && ok;

    // New score after ties: sequential position becomes rank
    ok = expect(AssignedContestRank(10, 20, 3, 1) == 3, "new score sequential rank") && ok;
    ok = expect(ShouldAdvanceRunningScore(10, 20), "new score advances") && ok;

    // Share predicate + packet fields
    ok = expect(ScoreMatchesShare(20, 20), "share match") && ok;
    ok = expect(!ScoreMatchesShare(10, 20), "share mismatch") && ok;
    ok = expect(RankDatasetB(2) == 2, "dataset_b mirrors share") && ok;
    ok = expect(RankResultCount(4) == 4, "result count from size") && ok;
    ok = expect(RankResultCount(0) == 0, "empty result count") && ok;

    return ok;
}

// Production RankContestEntries dual-wires pure helpers for sort + rank fill.
auto testRankContestEntriesDualWire() -> bool
{
    std::vector<FishingContestEntry> greatest{
        entry(10, 30), entry(20, 20), entry(20, 10), entry(5, 40),
    };
    fishingcontest::RankContestEntries(greatest, FISHING_CONTEST_MEASURE::GREATEST);

    bool ok = true;
    ok = expect(greatest[0].score == 20 && greatest[0].submitTime == 10, "greatest order first") && ok;
    ok = expect(greatest[1].score == 20 && greatest[1].submitTime == 20, "greatest order second") && ok;
    ok = expect(greatest[2].score == 10 && greatest[3].score == 5, "greatest tail order") && ok;
    ok = expect(greatest[0].contestRank == 1 && greatest[1].contestRank == 1, "greatest shared rank 1") && ok;
    ok = expect(greatest[2].contestRank == 3 && greatest[3].contestRank == 4, "greatest sequential after ties") && ok;
    for (const auto& value : greatest)
    {
        const auto expectedShare = value.score == 20 ? 2 : 1;
        ok = expect(value.share == expectedShare && value.dataset_b == expectedShare && value.resultCount == 4,
                    "greatest share fields") &&
             ok;
    }

    std::vector<FishingContestEntry> smallest{
        entry(10, 30), entry(5, 20), entry(5, 10), entry(20, 40),
    };
    fishingcontest::RankContestEntries(smallest, FISHING_CONTEST_MEASURE::SMALLEST);

    ok = expect(smallest[0].score == 5 && smallest[0].submitTime == 10, "smallest order first") && ok;
    ok = expect(smallest[1].score == 5 && smallest[1].submitTime == 20, "smallest order second") && ok;
    ok = expect(smallest[2].score == 10 && smallest[3].score == 20, "smallest tail order") && ok;
    ok = expect(smallest[0].contestRank == 1 && smallest[1].contestRank == 1 &&
                    smallest[2].contestRank == 3 && smallest[3].contestRank == 4,
                "smallest ranks") &&
         ok;

    // Zero-score running tally edge (initial score == 0)
    std::vector<FishingContestEntry> zeros{ entry(0, 20), entry(0, 10), entry(1, 30) };
    fishingcontest::RankContestEntries(zeros, FISHING_CONTEST_MEASURE::SMALLEST);
    ok = expect(zeros[0].score == 0 && zeros[0].submitTime == 10, "zero earliest first") && ok;
    ok = expect(zeros[0].contestRank == 1 && zeros[1].contestRank == 1 && zeros[2].contestRank == 3, "zero ranks") && ok;
    ok = expect(zeros[0].share == 2 && zeros[1].share == 2 && zeros[2].share == 1, "zero shares") && ok;
    ok = expect(zeros[0].dataset_b == 2 && zeros[2].dataset_b == 1, "zero dataset_b") && ok;
    ok = expect(zeros[0].resultCount == 3 && zeros[2].resultCount == 3, "zero resultCount") && ok;

    return ok;
}

// Inline dual-wire of pure comparator against production GREATEST/SMALLEST inject.
auto testInlineDualWireComparator() -> bool
{
    using fishingcontestrankhelpers::ShouldRankBefore;

    bool ok = true;
    const bool greatest = (FISHING_CONTEST_MEASURE::GREATEST == FISHING_CONTEST_MEASURE::GREATEST);
    const bool smallest = (FISHING_CONTEST_MEASURE::SMALLEST == FISHING_CONTEST_MEASURE::GREATEST);

    ok = expect(greatest && !smallest, "measure inject pins") && ok;
    ok = expect(ShouldRankBefore(20, 1, 10, 1, greatest) == true, "inline greatest") && ok;
    ok = expect(ShouldRankBefore(20, 1, 10, 1, smallest) == false, "inline smallest") && ok;
    return ok;
}

} // namespace

auto runFishingContestRank2847SelfTests() -> bool
{
    return testShouldRankBefore() && testRankShareHelpers() && testRankContestEntriesDualWire() &&
           testInlineDualWireComparator();
}
