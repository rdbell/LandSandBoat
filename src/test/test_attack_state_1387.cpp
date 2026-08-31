#include "test_attack_state_1387.h"

#include "map/ai/states/attack_state_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack state 1387 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attack state 1387 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackState1387SelfTests() -> bool
{
    using namespace attackstatehelpers;
    bool ok = true;

    ok = expect(ShouldExitNoTarget(false, false) && ShouldExitNoTarget(true, true), "exit no target") && ok;
    ok = expect(!ShouldExitNoTarget(true, false), "has live target") && ok;

    ok = expect(CanAttackSucceeded(true, false) && !CanAttackSucceeded(true, true), "can attack") && ok;
    ok = expect(!CanAttackSucceeded(false, false), "can attack fail") && ok;
    ok = expect(ShouldAddWeaponDelayAfterCanAttack(true, false), "add delay") && ok;

    ok = expect(ShouldSendAttackPacket(3) && !ShouldSendAttackPacket(ActionCategoryNone), "send pkt") && ok;
    ok = expect(ShouldHandleAttackError(false, true) && !ShouldHandleAttackError(true, true), "handle err") && ok;
    ok = expect(ShouldSubtractAttackElapsed(false) && !ShouldSubtractAttackElapsed(true), "subtract") && ok;
    ok = expect(ShouldCleanupDisengage(false) && !ShouldCleanupDisengage(true), "cleanup") && ok;
    ok = expect(ShouldClearPathOnEnter(true) && !ShouldClearPathOnEnter(false), "path") && ok;
    ok = expect(ShouldThrowInitNoTarget(false, false) && ShouldThrowInitNoTarget(true, true), "init throw") && ok;

    ok = expect(AutoTargetCandidate(AnimationAttack, true, 10.0f), "at edge") && ok;
    ok = expect(AutoTargetCandidate(AnimationAttack, true, 9.9f), "at in") && ok;
    ok = expect(!AutoTargetCandidate(AnimationAttack, true, 10.1f), "at far") && ok;
    ok = expect(!AutoTargetCandidate(0, true, 5.0f), "at anim") && ok;
    ok = expect(!AutoTargetCandidate(AnimationAttack, false, 5.0f), "at face") && ok;

    ok = expect(ShouldTryAutoTarget(true, true) && !ShouldTryAutoTarget(true, false), "try at") && ok;
    ok = expect(ShouldChangeTarget(1, 2) && !ShouldChangeTarget(1, 1), "change") && ok;
    ok = expect(ShouldNotifyChangeTarget(1, 2) && !ShouldNotifyChangeTarget(0, 2), "notify") && ok;
    ok = expect(ShouldClearErrorOnMissingNewTarget(false), "clear err") && ok;
    ok = expect(ShouldResolveBattleTarget(5) && !ShouldResolveBattleTarget(0), "resolve") && ok;
    ok = expect(ShouldExitAfterReadyPath(true) && !ShouldExitAfterReadyPath(false), "exit ready") && ok;

    ok = expectEq(ResolveAttackUpdate(false, false, false, false, false, false, false), AttackUpdateOutcome::ExitNoTarget, "ru no tgt") && ok;
    ok = expectEq(ResolveAttackUpdate(true, true, false, false, false, false, false), AttackUpdateOutcome::ExitNoTarget, "ru dead") && ok;
    ok = expectEq(ResolveAttackUpdate(true, false, false, false, false, false, false), AttackUpdateOutcome::Continue, "ru continue") && ok;
    ok = expectEq(ResolveAttackUpdate(true, false, true, true, true, false, false), AttackUpdateOutcome::ExitDisengaged, "ru disengage") && ok;
    ok = expectEq(ResolveAttackUpdate(true, false, true, true, false, false, false), AttackUpdateOutcome::PerformAttack, "ru attack") && ok;
    ok = expectEq(ResolveAttackUpdate(true, false, true, false, false, true, false), AttackUpdateOutcome::HandleError, "ru err") && ok;
    ok = expectEq(ResolveAttackUpdate(true, false, true, false, false, false, true), AttackUpdateOutcome::ExitDisengaged, "ru err zero") && ok;
    ok = expectEq(ResolveAttackUpdate(true, false, true, false, false, false, false), AttackUpdateOutcome::ReadyNoAction, "ru ready none") && ok;

    ok = expectEq(AutoTargetMaxDistance, 10.0f, "dist pin") && ok;
    ok = expectEq(AutoTargetFacingCone, static_cast<uint8>(64), "cone pin") && ok;
    ok = expectEq(AnimationAttack, static_cast<uint8>(1), "anim pin") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("attack-state-1387", runAttackState1387SelfTests);
