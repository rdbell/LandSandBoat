#include "test_attack_checks_1376.h"

#include "map/attack_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack checks 1376 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attack checks 1376 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "attack checks 1376 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackChecks1376SelfTests() -> bool
{
    using namespace attackhelpers;
    bool ok = true;

    ok = expect(IsDakenAttack(AttackTypeDaken) && !IsDakenAttack(0), "daken") && ok;
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken), "skip parry") && ok;
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken), "skip counter") && ok;
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken), "skip anticipate") && ok;

    ok = expectNear(ApplyGuardDamageRatio(1.5f), 0.5f, "guard ratio") && ok;
    ok = expectNear(ApplyGuardDamageRatio(0.5f), 0.0f, "guard floor") && ok;

    ok = expect(IsDeflected(true, 64, true) && !IsDeflected(true, 0, true), "deflect") && ok;
    ok = expect(!IsDeflected(false, 64, true) && !IsDeflected(true, 64, false), "deflect false") && ok;

    ok = expect(ShouldBlockCounterForState(false, false), "not engaged") && ok;
    ok = expect(ShouldBlockCounterForState(true, true), "prevent action") && ok;
    ok = expect(!ShouldBlockCounterForState(true, false), "can counter state") && ok;

    ok = expect(ShouldAddMNKCounterMerit(true, true) && !ShouldAddMNKCounterMerit(true, false), "mnk merit") && ok;
    ok = expectEq(ClampCounterRate(50, 10), static_cast<uint16>(60), "counter clamp") && ok;
    ok = expectEq(ClampCounterRate(100, 20), CounterRateHardCap, "counter cap") && ok;
    ok = expectEq(ClampCounterRate(-5, 0), static_cast<uint16>(0), "counter floor") && ok;

    ok = expect(IsValidSeiganForCounter(true, true) && !IsValidSeiganForCounter(false, true), "seigan valid") && ok;
    ok = expectEq(ComputeSeiganCounterChance(true, 40, 8), static_cast<uint16>(12), "seigan chance") && ok; // 48/4
    ok = expectEq(ComputeSeiganCounterChance(true, 200, 0), static_cast<uint16>(25), "seigan clamp") && ok; // 100/4
    ok = expectEq(ComputeSeiganCounterChance(false, 100, 0), static_cast<uint16>(0), "seigan off") && ok;

    ok = expect(ShouldSkipCounterForPerfectDodge(true) && !ShouldSkipCounterForPerfectDodge(false), "pd") && ok;
    ok = expect(ShouldAttemptCounterRate(true, false) || ShouldAttemptCounterRate(false, true), "attempt rate") && ok;
    ok = expect(ShouldLandCounter(true, true, true) && !ShouldLandCounter(true, false, true), "land counter") && ok;
    ok = expect(ShouldPerfectCounter(true) && !ShouldPerfectCounter(false), "perfect counter") && ok;
    ok = expect(IsCoverActive(true, true) && !IsCoverActive(true, false), "cover") && ok;
    ok = expect(HasThirdEyeForAnticipate(true) && !HasThirdEyeForAnticipate(false), "third eye") && ok;

    return ok;
}
