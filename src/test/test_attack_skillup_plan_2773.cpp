#include "test_attack_skillup_plan_2773.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack skillup plan 2773 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const attackhelpers::AttackSkillUpPlan& plan,
    const attackhelpers::AttackSkillUpTarget target,
    const bool                               useWeaponAtSlot,
    const char* const                        label) -> bool
{
    return expect(plan.target == target && plan.useWeaponAtSlot == useWeaponAtSlot, label);
}

} // namespace

auto runAttackSkillUpPlan2773SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // damage <= 0 → None regardless of actor flags
    ok = expectPlan(
             PlanAttackSkillUp(0, true, false, true, false, false, false),
             AttackSkillUpTarget::None,
             false,
             "zero damage pc weapon") &&
         ok;
    ok = expectPlan(
             PlanAttackSkillUp(-1, true, true, true, false, false, false),
             AttackSkillUpTarget::None,
             false,
             "negative damage daken") &&
         ok;
    ok = expectPlan(
             PlanAttackSkillUp(0, false, false, false, true, true, true),
             AttackSkillUpTarget::None,
             false,
             "zero damage automaton") &&
         ok;

    // PC Daken → Throwing (weapon slot irrelevant)
    ok = expectPlan(
             PlanAttackSkillUp(1, true, true, false, false, false, false),
             AttackSkillUpTarget::Throwing,
             false,
             "pc daken no weapon") &&
         ok;
    ok = expectPlan(
             PlanAttackSkillUp(10, true, true, true, false, false, false),
             AttackSkillUpTarget::Throwing,
             false,
             "pc daken with weapon") &&
         ok;

    // PC non-Daken with weapon → WeaponSkillType + useWeaponAtSlot
    ok = expectPlan(
             PlanAttackSkillUp(5, true, false, true, false, false, false),
             AttackSkillUpTarget::WeaponSkillType,
             true,
             "pc weapon skill") &&
         ok;

    // PC non-Daken without weapon → None
    ok = expectPlan(
             PlanAttackSkillUp(5, true, false, false, false, false, false),
             AttackSkillUpTarget::None,
             false,
             "pc no weapon") &&
         ok;

    // PC path wins over pet/automaton flags (production if/else if)
    ok = expectPlan(
             PlanAttackSkillUp(5, true, false, true, true, true, true),
             AttackSkillUpTarget::WeaponSkillType,
             true,
             "pc preferred over automaton") &&
         ok;

    // Automaton: pet + master PC + automaton
    ok = expectPlan(
             PlanAttackSkillUp(3, false, false, false, true, true, true),
             AttackSkillUpTarget::AutomatonMelee,
             false,
             "automaton melee") &&
         ok;

    // Incomplete automaton gates → None
    ok = expectPlan(
             PlanAttackSkillUp(3, false, false, false, false, true, true),
             AttackSkillUpTarget::None,
             false,
             "not pet") &&
         ok;
    ok = expectPlan(
             PlanAttackSkillUp(3, false, false, false, true, false, true),
             AttackSkillUpTarget::None,
             false,
             "master not pc") &&
         ok;
    ok = expectPlan(
             PlanAttackSkillUp(3, false, false, false, true, true, false),
             AttackSkillUpTarget::None,
             false,
             "not automaton") &&
         ok;

    // Mob / other non-PC non-automaton → None
    ok = expectPlan(
             PlanAttackSkillUp(100, false, false, true, false, false, false),
             AttackSkillUpTarget::None,
             false,
             "mob") &&
         ok;

    // Component helpers still pin the same gates
    ok = expect(ShouldTrySkillUp(1) && !ShouldTrySkillUp(0), "ShouldTrySkillUp") && ok;
    ok = expect(ShouldSkillUpThrowing(AttackTypeDaken) && !ShouldSkillUpThrowing(0), "ShouldSkillUpThrowing") && ok;
    ok = expect(ShouldSkillUpAutomaton(true, true, true) && !ShouldSkillUpAutomaton(true, true, false), "ShouldSkillUpAutomaton") && ok;

    return ok;
}
