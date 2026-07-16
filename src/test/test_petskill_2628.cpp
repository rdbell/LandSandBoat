#include "test_petskill_2628.h"

#include <iostream>

#include "map/petskill.h"
#include "map/mobskill.h"

auto runPetSkill2628SelfTests() -> bool
{
    CPetSkill skill{ 42 };
    bool      ok = skill.getID() == 42 && skill.getTotalTargets() == 1 && skill.isSingle() && !skill.isAoE() && !skill.isConal() &&
              !skill.isTpFreeSkill() && !skill.isAstralFlow() && !skill.isBloodPactRage() && !skill.isBloodPactWard();

    skill.setAoe(1);
    ok = skill.isAoE() && !skill.isSingle() && !skill.isConal() && ok;
    skill.setAoe(4);
    ok = skill.isConal() && !skill.isAoE() && !skill.isSingle() && ok;
    skill.setAoe(5);
    ok = !skill.isConal() && !skill.isAoE() && !skill.isSingle() && ok;

    skill.setFlag(SKILLFLAG_NO_TP_COST | SKILLFLAG_ASTRAL_FLOW | SKILLFLAG_BLOODPACT_RAGE | SKILLFLAG_BLOODPACT_WARD);
    ok = skill.isTpFreeSkill() && skill.isAstralFlow() && skill.isBloodPactRage() && skill.isBloodPactWard() && ok;
    if (!ok)
    {
        std::cerr << "pet skill self-test failed\n";
    }
    return ok;
}
