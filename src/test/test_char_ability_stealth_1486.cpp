#include "test_char_ability_stealth_1486.h"

#include "map/char_ability_stealth_capacity.h"

#include <iostream>

namespace
{
using charabilitystealthhelpers::PlanCleanup;

auto Check() -> bool
{
    // Exempt abilities do nothing.
    for (const auto id : { charabilitystealthhelpers::AbilityTame,
                           charabilitystealthhelpers::AbilityFight,
                           charabilitystealthhelpers::AbilityDeploy,
                           charabilitystealthhelpers::AbilityGauge })
    {
        const auto plan = PlanCleanup(id, charabilitystealthhelpers::TargetEnemy);
        if (plan.removeInvisible || plan.removeDetectable || plan.removeIllusion)
        {
            return false;
        }
    }

    // Enemy Assault: invisible + illusion only.
    {
        const auto plan = PlanCleanup(charabilitystealthhelpers::AbilityAssault, charabilitystealthhelpers::TargetEnemy);
        if (!plan.removeInvisible || plan.removeDetectable || !plan.removeIllusion)
        {
            return false;
        }
    }

    // Enemy non-Assault: detectable + illusion.
    {
        const auto plan = PlanCleanup(1, charabilitystealthhelpers::TargetEnemy);
        if (plan.removeInvisible || !plan.removeDetectable || !plan.removeIllusion)
        {
            return false;
        }
    }

    // Friendly Trick Attack: no cleanup.
    {
        const auto plan = PlanCleanup(charabilitystealthhelpers::AbilityTrickAttack, 0x0001);
        if (plan.removeInvisible || plan.removeDetectable || plan.removeIllusion)
        {
            return false;
        }
    }

    // Friendly other: invisible + illusion.
    {
        const auto plan = PlanCleanup(1, 0x0001);
        if (!plan.removeInvisible || plan.removeDetectable || !plan.removeIllusion)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runCharAbilityStealth1486SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char ability stealth 1486 self-test failed\n";
    }
    return ok;
}
