#include "test_attack_hitrate_1379.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack hitrate 1379 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attack hitrate 1379 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackHitRate1379SelfTests() -> bool
{
    using namespace attackhelpers;
    bool ok = true;

    ok = expectEq(ResolveHitRatePath(AttackTypeKick, AttackDirectionRight), HitRatePath::KickMelee, "kick") && ok;
    ok = expectEq(ResolveHitRatePath(AttackTypeDaken, AttackDirectionLeft), HitRatePath::DakenRanged, "daken") && ok;
    ok = expectEq(ResolveHitRatePath(0, AttackDirectionRight), HitRatePath::RightMelee, "right") && ok;
    ok = expectEq(ResolveHitRatePath(0, AttackDirectionLeft), HitRatePath::LeftMelee, "left") && ok;
    // Kick takes priority over direction
    ok = expectEq(ResolveHitRatePath(AttackTypeKick, AttackDirectionLeft), HitRatePath::KickMelee, "kick over left") && ok;

    ok = expectEq(HitRateHandForPath(HitRatePath::KickMelee), HitRateHandKick, "hand kick") && ok;
    ok = expectEq(HitRateHandForPath(HitRatePath::RightMelee), HitRateHandRight, "hand right") && ok;
    ok = expectEq(HitRateHandForPath(HitRatePath::LeftMelee), HitRateHandLeft, "hand left") && ok;

    ok = expect(ShouldApplyZanshinAccBonus(AttackTypeZanshin) && !ShouldApplyZanshinAccBonus(0), "zanshin") && ok;
    ok = expectEq(ZanshinAccBonusOrZero(AttackTypeZanshin), ZanshinHitRateAccBonus, "zanshin bonus") && ok;
    ok = expectEq(ZanshinAccBonusOrZero(0), static_cast<uint8>(0), "no zanshin") && ok;

    ok = expectEq(ComputeSangeAccBonus(false, false, 0), SangeBaseAccBonus, "sange base") && ok;
    ok = expectEq(ComputeSangeAccBonus(true, true, 1), SangeBaseAccBonus, "sange 1 merit") && ok;
    ok = expectEq(ComputeSangeAccBonus(true, true, 3), static_cast<int16>(150), "sange 3 merits") && ok; // 100+(3-1)*25
    ok = expectEq(ComputeSangeAccBonus(true, false, 5), SangeBaseAccBonus, "sange no merit pts") && ok;

    ok = expect(ShouldStampSATAOnPerfectHit(HitRatePath::RightMelee, 100), "sata stamp") && ok;
    ok = expect(!ShouldStampSATAOnPerfectHit(HitRatePath::RightMelee, 99), "sata not 100") && ok;
    ok = expect(!ShouldStampSATAOnPerfectHit(HitRatePath::LeftMelee, 100), "sata left no") && ok;
    ok = expect(IsRightAttackDirection(AttackDirectionRight) && !IsRightAttackDirection(AttackDirectionLeft), "right dir") && ok;
    ok = expect(IsLeftAttackDirection(AttackDirectionLeft) && !IsLeftAttackDirection(AttackDirectionRight), "left dir") && ok;
    ok = expect(!IsRightAttackDirection(2) && !IsLeftAttackDirection(2), "invalid dirs") && ok;

    return ok;
}
