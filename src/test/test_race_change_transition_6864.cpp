#include "test_race_change_transition_6864.h"
#include "map/char_race_change_transition.h"
#include <iostream>
auto runRaceChangeTransition6864SelfTests() -> bool
{
    bool ok = true;
    ok = racechangetransitionhelpers::MakeRaceChangeValidationPlan(false, true) == racechangetransitionhelpers::RaceChangeValidationPlan{} && ok;
    ok = racechangetransitionhelpers::MakeRaceChangeValidationPlan(true, false) == racechangetransitionhelpers::RaceChangeValidationPlan{ .reportInvalidArguments = true } && ok;
    ok = racechangetransitionhelpers::MakeRaceChangeValidationPlan(true, true) == racechangetransitionhelpers::RaceChangeValidationPlan{ .attemptLookUpdate = true } && ok;
    ok = racechangetransitionhelpers::MakeRaceChangeCompletionPlan(false) == racechangetransitionhelpers::RaceChangeCompletionPlan{ .reportLookUpdateFailure = true } && ok;
    ok = racechangetransitionhelpers::MakeRaceChangeCompletionPlan(true) == racechangetransitionhelpers::RaceChangeCompletionPlan{ .succeeded = true, .forceRezone = true } && ok;
    if (!ok) std::cerr << "race change transition 6864 self-test failed\n";
    return ok;
}
