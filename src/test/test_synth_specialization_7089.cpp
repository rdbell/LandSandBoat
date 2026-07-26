#include "test_synth_specialization_7089.h"

#include "map/synth_skill_up.h"

#include <iostream>

auto runSynthSpecialization7089SelfTests() -> bool
{
    using synthskilluphelpers::MakeSpecializationPlan;

    const auto otherHighest = MakeSpecializationPlan(49, 49, 510, 2, 500, 41, { 510, 530, 500, 500, 500, 500, 500, 500 });
    const auto selfFallback = MakeSpecializationPlan(49, 49, 500, 1, 500, 0, { 500, 500, 500, 500, 500, 500, 500, 500 });
    const bool ok = otherHighest.skillCumulation == 42 && otherHighest.skillHighest == 50 && otherHighest.removeSkill &&
                    selfFallback.skillHighest == 49 && selfFallback.skillCumulation == 1 && selfFallback.removeSkill;
    if (!ok)
    {
        std::cerr << "synth specialization 7089 self-test failed\n";
    }
    return ok;
}
