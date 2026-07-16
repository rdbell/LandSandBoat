#include "test_mobskill_2627.h"

#include <iostream>

#include "map/mobskill.h"

auto runMobSkill2627SelfTests() -> bool
{
    CMobSkill skill{ 42 };
    bool      ok = skill.getID() == 42 && skill.getTotalTargets() == 1 && skill.getRadius() == 0 && skill.isSingle() && !skill.isAoE() && !skill.isConal() &&
              !skill.isTpFreeSkill() && !skill.isAstralFlow() && !skill.isBloodPactRage() && !skill.isBloodPactWard();

    skill.setDistance(12.5f);
    skill.setAoe(1);
    ok = skill.getRadius() == 12.5f && skill.isAoE() && !skill.isSingle() && !skill.isConal() && ok;
    skill.setAoe(2);
    ok = skill.getRadius() == 8.0f && skill.isAoE() && ok;
    skill.setAoeRadius(3.25f);
    ok = skill.getRadius() == 3.25f && ok;
    skill.setAoe(4);
    ok = skill.isConal() && !skill.isAoE() && !skill.isSingle() && ok;
    skill.setAoe(8);
    ok = skill.isConal() && !skill.isAoE() && !skill.isSingle() && ok;

    skill.setFlag(SKILLFLAG_NO_TP_COST | SKILLFLAG_ASTRAL_FLOW | SKILLFLAG_BLOODPACT_RAGE | SKILLFLAG_BLOODPACT_WARD);
    ok = skill.isTpFreeSkill() && skill.isAstralFlow() && skill.isBloodPactRage() && skill.isBloodPactWard() && ok;
    if (!ok)
    {
        std::cerr << "mob skill self-test failed\n";
    }
    return ok;
}
