#include "test_action_effect_flags_1631.h"

#include "map/action_effect_flags_capacity.h"

#include <iostream>

auto runActionEffectFlags1631SelfTests() -> bool
{
    using namespace actioneffectflagshelpers;

    bool ok = true;

    ok = ok && ActionCategoryBasicAttack == 1;
    ok = ok && ActionCategorySkillFinish == 3;
    ok = ok && ActionCategoryMobSkillFinish == 11;
    ok = ok && ActionCategoryPetSkillFinish == 13;

    ok = ok && InitialIsMainTarget();
    ok = ok && !NextIsMainTarget();

    {
        const auto p = ResolveTargetEffectPlan(false, true, true, true, true, true);
        ok           = ok && !p.skipEvent && !p.countAsHostileEmit;
    }
    {
        // PC in event, same allegiance → skip only
        const auto p = ResolveTargetEffectPlan(true, true, true, false, false, true);
        ok           = ok && p.skipEvent && !p.countAsHostileEmit && !p.delOnAttack;
    }
    {
        // Main hostile PC fishing
        const auto p = ResolveTargetEffectPlan(true, true, false, true, true, true);
        ok           = ok && p.countAsHostileEmit && p.delDetectable && p.delOnAttack && p.interruptFishing;
    }
    {
        // Secondary hostile non-PC
        const auto p = ResolveTargetEffectPlan(true, false, false, false, true, false);
        ok           = ok && p.countAsHostileEmit && !p.delDetectable && p.delOnAttack && !p.interruptFishing;
    }

    ok = ok && ShouldDelActorOnAttack(true);
    ok = ok && !ShouldDelActorOnAttack(false);

    ok = ok && ShouldDelActorAttackFlag(true, ActionCategoryBasicAttack);
    ok = ok && ShouldDelActorAttackFlag(true, ActionCategorySkillFinish);
    ok = ok && ShouldDelActorAttackFlag(true, ActionCategoryMobSkillFinish);
    ok = ok && ShouldDelActorAttackFlag(true, ActionCategoryPetSkillFinish);
    ok = ok && !ShouldDelActorAttackFlag(true, 4); // MagicFinish
    ok = ok && !ShouldDelActorAttackFlag(false, ActionCategoryBasicAttack);

    ok = ok && IsPhysicalHostileActionType(ActionCategoryBasicAttack);
    ok = ok && !IsPhysicalHostileActionType(6); // AbilityFinish

    if (!ok)
    {
        std::cerr << "action effect flags 1631 self-test failed\n";
    }
    return ok;
}
