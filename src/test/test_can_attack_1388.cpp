#include "test_can_attack_1388.h"

#include "map/can_attack_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "can attack 1388 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "can attack 1388 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "can attack 1388 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runCanAttack1388SelfTests() -> bool
{
    using namespace canattackhelpers;
    bool ok = true;

    ok = expectNear(GetMeleeRange(1.0f, 2.0f), 5.0f, "melee range") && ok; // 1+2+2
    ok = expectNear(GetMeleeRange(0.0f, 0.0f), 2.0f, "melee pad only") && ok;
    ok = expectEq(RangedAttackRangeDefault, 25.0f, "ranged range") && ok;

    ok = expect(TooFarForMelee(5.1f, 5.0f) && !TooFarForMelee(5.0f, 5.0f), "too far") && ok;

    ok = expect(BattleCanAttack(false, 3.0f, 5.0f, true), "battle ok") && ok;
    ok = expect(!BattleCanAttack(true, 3.0f, 5.0f, true), "untargetable") && ok;
    ok = expect(!BattleCanAttack(false, 6.0f, 5.0f, true), "too far battle") && ok;
    ok = expect(!BattleCanAttack(false, 3.0f, 5.0f, false), "aa off") && ok;

    ok = expectNear(MobSkillListAttackRange(1.0f, 10.0f, 1.0f), 12.0f, "mob skill range") && ok;
    ok = expect(ShouldUseMobSkillListRange(5) && !ShouldUseMobSkillListRange(0), "skill list mod") && ok;
    ok = expect(MobCanAttackWithSkillListRange(5.0f, 10.0f, true) && !MobCanAttackWithSkillListRange(11.0f, 10.0f, true), "mob skill can") && ok;

    ok = expectEq(
        EvaluateCharCanAttack(true, true, false, 5.0f, true, 10.0f),
        CanAttackFail::Untargetable,
        "char untargetable") && ok;
    ok = expectEq(
        EvaluateCharCanAttack(false, false, false, 5.0f, true, 10.0f),
        CanAttackFail::AlreadyClaimed,
        "claimed") && ok;
    ok = expectEq(
        EvaluateCharCanAttack(false, true, false, 31.0f, true, 10.0f),
        CanAttackFail::LoseSight,
        "lose sight") && ok;
    ok = expectEq(
        EvaluateCharCanAttack(false, true, true, 31.0f, true, 10.0f),
        CanAttackFail::TargetOutOfRange,
        "charmed far melee") && ok; // charm skips lose-sight; still fails melee 10
    ok = expectEq(
        EvaluateCharCanAttack(false, true, false, 5.0f, false, 10.0f),
        CanAttackFail::UnableToSeeTarget,
        "facing") && ok;
    ok = expectEq(
        EvaluateCharCanAttack(false, true, false, 15.0f, true, 10.0f),
        CanAttackFail::TargetOutOfRange,
        "oor") && ok;
    ok = expectEq(
        EvaluateCharCanAttack(false, true, false, 5.0f, true, 10.0f),
        CanAttackFail::None,
        "char ok") && ok;

    ok = expect(ShouldDisengageOnCharCanAttackFail(CanAttackFail::AlreadyClaimed), "disengage claim") && ok;
    ok = expect(ShouldDisengageOnCharCanAttackFail(CanAttackFail::LoseSight), "disengage sight") && ok;
    ok = expect(!ShouldDisengageOnCharCanAttackFail(CanAttackFail::UnableToSeeTarget), "no disengage face") && ok;
    ok = expect(!ShouldDisengageOnCharCanAttackFail(CanAttackFail::TargetOutOfRange), "no disengage oor") && ok;

    ok = expect(PetCanAttackClaimFail(true, false) && !PetCanAttackClaimFail(true, true), "pet claim") && ok;
    ok = expect(!PetCanAttackClaimFail(false, false), "pet no master") && ok;

    ok = expect(CharCanAttackSucceeded(CanAttackFail::None) && !CharCanAttackSucceeded(CanAttackFail::AlreadyClaimed), "succeeded") && ok;

    ok = expectEq(static_cast<uint16>(CanAttackFail::AlreadyClaimed), static_cast<uint16>(12), "msg claim") && ok;
    ok = expectEq(static_cast<uint16>(CanAttackFail::LoseSight), static_cast<uint16>(36), "msg sight") && ok;
    ok = expectEq(static_cast<uint16>(CanAttackFail::UnableToSeeTarget), static_cast<uint16>(5), "msg see") && ok;
    ok = expectEq(static_cast<uint16>(CanAttackFail::TargetOutOfRange), static_cast<uint16>(4), "msg oor") && ok;
    ok = expectEq(CharFacingCone, static_cast<uint8>(64), "facing cone") && ok;
    ok = expectEq(CharLoseSightDistance, 30.0f, "lose sight dist") && ok;

    return ok;
}
