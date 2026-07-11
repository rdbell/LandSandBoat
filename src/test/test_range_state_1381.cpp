#include "test_range_state_1381.h"

#include "map/ai/states/range_state_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "range state 1381 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "range state 1381 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runRangeState1381SelfTests() -> bool
{
    using namespace rangestatehelpers;
    bool ok = true;

    // Rapid Shot eligibility and proc.
    ok = expect(ShouldTryRapidShot(true, false) && !ShouldTryRapidShot(true, true), "try rapid") && ok;
    ok = expect(!ShouldTryRapidShot(false, false), "not pc/trust") && ok;
    ok = expect(RapidShotChanceEligible(1) && !RapidShotChanceEligible(0), "chance eligible") && ok;
    ok = expect(RapidShotProcs(25, 0) && RapidShotProcs(25, 24), "proc hit") && ok;
    ok = expect(!RapidShotProcs(25, 25) && !RapidShotProcs(0, 0), "proc miss") && ok;

    // Delay reduction: 100 * (1 - 0.10) = 90; band clamp.
    ok = expectEq(ApplyRapidShotDelayReduction(100, 10), static_cast<int16>(90), "reduce 10%") && ok;
    ok = expectEq(ApplyRapidShotDelayReduction(100, 2), static_cast<int16>(98), "reduce 2%") && ok;
    ok = expectEq(ApplyRapidShotDelayReduction(100, 49), static_cast<int16>(51), "reduce 49%") && ok;
    ok = expectEq(ClampRapidShotReductionPercent(1), static_cast<uint16>(2), "clamp low") && ok;
    ok = expectEq(ClampRapidShotReductionPercent(50), static_cast<uint16>(49), "clamp high") && ok;
    ok = expectEq(ClampRapidShotReductionPercent(25), static_cast<uint16>(25), "clamp mid") && ok;

    // Mob free-phase / return weapon.
    ok = expectEq(MobFreePhaseTimeMs(0), static_cast<int32>(6500), "mob free 0") && ok;
    ok = expectEq(MobFreePhaseTimeMs(1499), static_cast<int32>(7999), "mob free max") && ok;
    ok = expectEq(MobReturnWeaponDelayMs, static_cast<int32>(2850), "mob return") && ok;
    ok = expect(ShouldApplyMobReturnWeaponDelay(true) && !ShouldApplyMobReturnWeaponDelay(false), "mob return gate") && ok;
    ok = expect(ShouldApplyMobMeleeFreePhase(true, true) && !ShouldApplyMobMeleeFreePhase(true, false), "mob melee") && ok;
    ok = expect(!ShouldApplyMobMeleeFreePhase(false, true), "not mob melee") && ok;

    // Equip skill gates.
    ok = expect(HasInitialRangedEquip(true, false) && HasInitialRangedEquip(false, true), "initial equip") && ok;
    ok = expect(!HasInitialRangedEquip(false, false), "no initial equip") && ok;
    ok = expectEq(ResolveRangedSkillType(true, SkillArchery, SkillThrowing), SkillArchery, "skill from ranged") && ok;
    ok = expectEq(ResolveRangedSkillType(false, SkillArchery, SkillThrowing), SkillThrowing, "skill from ammo") && ok;
    ok = expect(PCRangedSkillEquipOK(SkillThrowing, false), "throwing ok") && ok;
    ok = expect(PCRangedSkillEquipOK(SkillArchery, true) && !PCRangedSkillEquipOK(SkillArchery, false), "archery ammo") && ok;
    ok = expect(PCRangedSkillEquipOK(SkillMarksmanship, true) && !PCRangedSkillEquipOK(SkillMarksmanship, false), "mark ammo") && ok;
    ok = expect(!PCRangedSkillEquipOK(0, true), "default skill fail") && ok;

    // Animation / LOS / free-phase / range / moved.
    ok = expect(IsAllowedRangedAnimation(AnimationNone) && IsAllowedRangedAnimation(AnimationAttack), "anim ok") && ok;
    ok = expect(!IsAllowedRangedAnimation(2), "anim bad") && ok;
    ok = expect(ShouldCheckLineOfSight(false) && !ShouldCheckLineOfSight(true), "los gate") && ok;
    ok = expect(FreePhaseBusy(1099, 1100) && !FreePhaseBusy(1100, 1100), "free phase") && ok;
    ok = expect(IsOutOfRangedAttackRange(25.1f, 25.0f) && !IsOutOfRangedAttackRange(25.0f, 25.0f), "oor") && ok;
    ok = expect(RangeHasMoved(true, 0.31f) && !RangeHasMoved(true, 0.3f), "moved thr") && ok;
    ok = expect(!RangeHasMoved(false, 10.0f), "non-pc moved") && ok;

    // Interrupt on error: CannotSee is suppressed; other msgs interrupt.
    ok = expect(ShouldInterruptOnError(true, true, static_cast<uint16>(RangedUseFail::NoRangedWeapon)), "interrupt weapon") && ok;
    ok = expect(!ShouldInterruptOnError(true, true, static_cast<uint16>(RangedUseFail::CannotSee)), "suppress cannot see") && ok;
    ok = expect(ShouldInterruptOnError(true, false, 0), "non-battle interrupt") && ok;
    ok = expect(!ShouldInterruptOnError(false, true, 0), "no error") && ok;

    // Full CanUse ladder.
    ok = expectEq(
        EvaluateCanUseRangedAttack(false, true, true, false, SkillArchery, true, true, false, true, false, AnimationNone),
        RangedUseFail::CannotAttackTarget,
        "no target") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, false, false, 0, false, true, false, true, false, AnimationNone),
        RangedUseFail::NoRangedWeapon,
        "no weapon") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, false, true, false, true, false, AnimationNone),
        RangedUseFail::NoRangedWeapon,
        "no ammo") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, true, false, false, true, false, AnimationNone),
        RangedUseFail::CannotSee,
        "facing") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, true, true, false, false, false, AnimationNone),
        RangedUseFail::CannotPerformAction,
        "los") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, true, true, true, false, false, AnimationNone),
        RangedUseFail::None,
        "los skip end") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, true, true, false, true, true, AnimationNone),
        RangedUseFail::WaitLonger,
        "wait") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, true, true, false, true, false, 5),
        RangedUseFail::CannotPerformAction,
        "anim") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, true, false, SkillArchery, true, true, false, true, false, AnimationAttack),
        RangedUseFail::None,
        "ok attack anim") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, false, false, false, 0, false, true, false, true, false, AnimationNone),
        RangedUseFail::None,
        "non-pc ok") && ok;
    ok = expectEq(
        EvaluateCanUseRangedAttack(true, true, false, true, SkillThrowing, false, true, false, true, false, AnimationNone),
        RangedUseFail::None,
        "throwing ok") && ok;

    // MsgBasic pin values.
    ok = expectEq(static_cast<uint16>(RangedUseFail::CannotAttackTarget), static_cast<uint16>(446), "msg target") && ok;
    ok = expectEq(static_cast<uint16>(RangedUseFail::NoRangedWeapon), static_cast<uint16>(216), "msg weapon") && ok;
    ok = expectEq(static_cast<uint16>(RangedUseFail::CannotSee), static_cast<uint16>(217), "msg see") && ok;
    ok = expectEq(static_cast<uint16>(RangedUseFail::CannotPerformAction), static_cast<uint16>(71), "msg action") && ok;
    ok = expectEq(static_cast<uint16>(RangedUseFail::WaitLonger), static_cast<uint16>(94), "msg wait") && ok;

    return ok;
}
