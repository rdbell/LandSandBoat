#include "test_fishing_contest_placeholders_2855.h"

#include "map/fishingcontest.h"
#include "map/fishingcontest_placeholders_capacity.h"

#include <cstring>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest placeholders 2855 self-test failed: " << label << '\n';
    }
    return condition;
}

// Pure ShouldGeneratePlaceholderEntries truth table.
auto testShouldGenerate() -> bool
{
    using fishingcontestplaceholderhelpers::ShouldGeneratePlaceholderEntries;

    bool ok = true;

    // Presenting with room for fakes.
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 0), "presenting empty generates") && ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 14), "presenting one short generates") && ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 4, 2), "presenting half generates") && ok;

    // Presenting but full real count does not generate.
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 15, 15), "presenting full does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 4, 4), "presenting equal max does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 4, 5), "presenting over max does not") && ok;

    // Non-presenting never generates, even with room.
    ok = expect(!ShouldGeneratePlaceholderEntries(false, 15, 0), "non-presenting empty does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(false, 4, 2), "non-presenting partial does not") && ok;

    // Zero max: realEntries >= maxEntries when realEntries is 0.
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 0, 0), "zero max full does not") && ok;

    return ok;
}

// Pure PlaceholderEntryScore ladder + underflow.
auto testPlaceholderScore() -> bool
{
    using fishingcontestplaceholderhelpers::PlaceholderEntryScore;

    bool ok = true;

    // Greatest: (needed + 1) - number
    ok = expect(PlaceholderEntryScore(2, 1, false) == 2, "greatest n1 needed2") && ok;
    ok = expect(PlaceholderEntryScore(2, 2, false) == 1, "greatest n2 needed2") && ok;
    ok = expect(PlaceholderEntryScore(2, 3, false) == 0, "greatest n3 needed2 zero") && ok;
    ok = expect(PlaceholderEntryScore(2, 4, false) == std::numeric_limits<uint32>::max(),
                "greatest n4 needed2 underflow") &&
         ok;

    // Smallest: 9999 - needed + number
    ok = expect(PlaceholderEntryScore(2, 1, true) == 9998, "smallest n1 needed2") && ok;
    ok = expect(PlaceholderEntryScore(2, 2, true) == 9999, "smallest n2 needed2") && ok;
    ok = expect(PlaceholderEntryScore(2, 20, true) == 10017, "smallest n20 needed2") && ok;
    ok = expect(PlaceholderEntryScore(1, 1, true) == 9999, "smallest n1 needed1") && ok;

    // Dual-wire matches inline production ternary.
    const uint8 needed = 2;
    for (int n = 1; n <= 4; ++n)
    {
        const uint32 greatestInline = static_cast<uint32>((static_cast<int>(needed) + 1) - n);
        const uint32 smallestInline = static_cast<uint32>(9999 - needed + n);
        ok = expect(PlaceholderEntryScore(needed, n, false) == greatestInline, "inline greatest") && ok;
        ok = expect(PlaceholderEntryScore(needed, n, true) == smallestInline, "inline smallest") && ok;
    }

    return ok;
}

// Production GeneratePlaceholderEntries dual-wires pure gate + score.
auto testGenerateDualWire() -> bool
{
    using fishingcontestplaceholderhelpers::PlaceholderEntryScore;
    using fishingcontestplaceholderhelpers::ShouldGeneratePlaceholderEntries;

    bool ok = true;

    // Gate dual-wire: non-presenting / full real count → empty.
    const bool presentingAccepting =
        (FISHING_CONTEST_STATUS::ACCEPTING == FISHING_CONTEST_STATUS::PRESENTING);
    ok = expect(!presentingAccepting, "accepting is not presenting") && ok;
    ok = expect(
             !ShouldGeneratePlaceholderEntries(presentingAccepting, 4, 0) &&
                 fishingcontest::GeneratePlaceholderEntries(
                     FISHING_CONTEST_STATUS::ACCEPTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 0)
                     .empty(),
             "host ACCEPTING empty dual-wire") &&
         ok;
    ok = expect(
             !ShouldGeneratePlaceholderEntries(true, 4, 4) &&
                 fishingcontest::GeneratePlaceholderEntries(
                     FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 4)
                     .empty(),
             "host full real empty dual-wire") &&
         ok;

    // Greatest ladder + native max-entry loop quirk (emits maxEntries, not needed).
    const auto greatest = fishingcontest::GeneratePlaceholderEntries(
        FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 2);
    ok = expect(greatest.size() == 4, "native max-entry loop quirk") && ok;
    if (greatest.size() == 4)
    {
        const uint8 needed = 2;
        ok = expect(std::strcmp(greatest[0].name, " SmallFisher01 ") == 0 && greatest[0].mjob == 1 &&
                        greatest[0].mlvl == 75 && greatest[0].score == PlaceholderEntryScore(needed, 1, false) &&
                        greatest[0].contestRank == 3,
                    "first pure score dual-wire") &&
             ok;
        ok = expect(greatest[3].score == PlaceholderEntryScore(needed, 4, false) &&
                        greatest[3].score == std::numeric_limits<uint32>::max() &&
                        greatest[3].contestRank == 6,
                    "underflow pure score dual-wire") &&
             ok;
    }

    // Smallest ladder dual-wire.
    const auto smallest = fishingcontest::GeneratePlaceholderEntries(
        FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::SMALLEST, 20, 18);
    ok = expect(smallest.size() == 20, "smallest size") && ok;
    if (smallest.size() == 20)
    {
        const uint8 needed = 2;
        ok = expect(smallest.front().score == PlaceholderEntryScore(needed, 1, true) &&
                        smallest.front().score == 9998,
                    "smallest first score") &&
             ok;
        ok = expect(smallest.back().score == PlaceholderEntryScore(needed, 20, true) &&
                        smallest.back().score == 10017,
                    "smallest last score") &&
             ok;
    }

    return ok;
}

} // namespace

auto runFishingContestPlaceholders2855SelfTests() -> bool
{
    return testShouldGenerate() && testPlaceholderScore() && testGenerateDualWire();
}
