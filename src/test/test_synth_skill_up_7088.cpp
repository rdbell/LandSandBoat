#include "test_synth_skill_up_7088.h"

#include "map/synth_skill_up.h"

#include <iostream>

auto runSynthSkillUp7088SelfTests() -> bool
{
    using synthskilluphelpers::IsEligible;

    const bool ok = !IsEligible(500, 500, 1, false, false) && !IsEligible(499, 500, 0, false, false) &&
                    IsEligible(499, 500, 1, false, false) && !IsEligible(499, 500, -11, true, false) &&
                    IsEligible(499, 500, -10, true, false) && IsEligible(499, 500, 5, true, true) &&
                    !IsEligible(499, 500, 6, true, true);
    if (!ok)
    {
        std::cerr << "synth skill-up 7088 self-test failed\n";
    }
    return ok;
}
