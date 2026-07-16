#include "test_fishingcontest_state_2623.h"

#include <iostream>

#include "map/fishingcontest.h"

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest state self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runFishingContestState2623SelfTests() -> bool
{
    auto       defaults = FishingContest{};
    bool       ok       = true;
    ok                  = expect(defaults.status == FISHING_CONTEST_STATUS::CONTESTING && defaults.criteria == FISHING_CONTEST_CRITERIA::SIZE && defaults.measure == FISHING_CONTEST_MEASURE::GREATEST, "struct defaults") && ok;
    ok                  = expect(defaults.fishId == 0 && defaults.startTime == 0 && defaults.changeTime == 0xFFFFFFFF, "default values") && ok;
    ok                  = expect(!defaults.isValid(), "default requires fish and start time") && ok;
    fishingcontest::SetContestStatus(FISHING_CONTEST_STATUS::ACCEPTING);
    fishingcontest::SetContestCriteria(FISHING_CONTEST_CRITERIA::BOTH);
    fishingcontest::SetContestMeasure(FISHING_CONTEST_MEASURE::SMALLEST);
    fishingcontest::SetContestFish(4304);
    fishingcontest::SetContestStartTime(1234);
    ok = expect(fishingcontest::GetContestStatus() == FISHING_CONTEST_STATUS::ACCEPTING && fishingcontest::GetContestCriteria() == FISHING_CONTEST_CRITERIA::BOTH && fishingcontest::GetContestMeasure() == FISHING_CONTEST_MEASURE::SMALLEST, "bounded setters") && ok;
    ok = expect(fishingcontest::GetContestFish() == 4304 && fishingcontest::GetContestStartTime() == 1234, "fish and start setters") && ok;
    fishingcontest::SetContestStatus(static_cast<FISHING_CONTEST_STATUS>(7));
    return expect(fishingcontest::GetContestStatus() == FISHING_CONTEST_STATUS::ACCEPTING, "invalid status ignored") && ok;
}
