#include "test_monstrosity_species_change_7486.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrositySpeciesChange7486SelfTests() -> bool
{
    using monstrosity::PlanSpeciesChange;
    using monstrosity::SpeciesChangeCandidate;

    const auto candidate = SpeciesChangeCandidate{
        .monstrosityId = 4,
        .mainJob       = JOB_WAR,
        .subJob        = JOB_MNK,
        .size          = 2,
        .look          = 0x1234,
    };

    if (PlanSpeciesChange(false, 4, candidate, 1, 99, true, false).changeSpecies)
    {
        std::cerr << "monstrosity species change: missing candidate failed\n";
        return false;
    }

    if (PlanSpeciesChange(true, 4, candidate, 1, 0, true, false).changeSpecies)
    {
        std::cerr << "monstrosity species change: locked species failed\n";
        return false;
    }

    if (PlanSpeciesChange(true, 256, candidate, 1, 99, false, false).changeSpecies)
    {
        std::cerr << "monstrosity species change: locked variant failed\n";
        return false;
    }

    const auto sameFamily = PlanSpeciesChange(true, 4, candidate, 4, 99, false, false);
    if (!sameFamily.changeSpecies || sameFamily.species != 4 || sameFamily.monstrosityId != 4 || sameFamily.mainJob != JOB_WAR || sameFamily.subJob != JOB_MNK || sameFamily.size != 2 || sameFamily.look != 0x1234 || sameFamily.clearInstincts || sameFamily.eraseStatusEffects)
    {
        std::cerr << "monstrosity species change: same family failed\n";
        return false;
    }

    const auto changedFamily = PlanSpeciesChange(true, 256, candidate, 1, 99, true, false);
    if (!changedFamily.changeSpecies || !changedFamily.clearInstincts || !changedFamily.eraseStatusEffects)
    {
        std::cerr << "monstrosity species change: family change failed\n";
        return false;
    }

    const auto preserveBuffs = PlanSpeciesChange(true, 256, candidate, 1, 99, true, true);
    if (!preserveBuffs.clearInstincts || preserveBuffs.eraseStatusEffects)
    {
        std::cerr << "monstrosity species change: preserve buffs failed\n";
        return false;
    }

    return true;
}
