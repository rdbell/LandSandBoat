#include "test_attackutils_multiplier_1380.h"

#include "map/utils/attackutils_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils multiplier 1380 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attackutils multiplier 1380 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "attackutils multiplier 1380 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackutilsMultiplier1380SelfTests() -> bool
{
    using namespace attackutilshelpers;
    bool ok = true;

    ok = expect(ShouldRejectNullWeapon(true) && !ShouldRejectNullWeapon(false), "null weapon") && ok;
    ok = expect(ShouldUseRangedRemOcc(AttackTypeRanged) && ShouldUseRangedRemOcc(AttackTypeRapidShot), "ranged rem") && ok;
    ok = expect(!ShouldUseRangedRemOcc(AttackTypeNormal), "not ranged rem") && ok;
    ok = expect(ShouldUseMainHandRemOcc(AttackTypeNormal, SlotMain), "main rem") && ok;
    ok = expect(!ShouldUseMainHandRemOcc(AttackTypeNormal, 1), "not main rem") && ok;

    ok = expectNear(OccExtraDmgMultiplier(250), 2.5f, "extra mult") && ok;
    ok = expectEq(OccExtraDmgChance(50), static_cast<int16>(5), "extra chance") && ok;
    ok = expectEq(RemOccChance(30), static_cast<int16>(3), "rem chance") && ok;

    ok = expect(RollChancePercent(10, 0), "roll hit") && ok; // 1+0=1 <= 10
    ok = expect(RollChancePercent(10, 9), "roll edge") && ok; // 1+9=10 <= 10
    ok = expect(!RollChancePercent(10, 10), "roll miss") && ok; // 1+10=11 > 10
    ok = expect(!RollChancePercent(0, 0), "roll zero chance") && ok;

    ok = expectEq(ResolveAllowProcLadder(false, 4.0f, true, true, true, true, true), OccProcResult::None, "no allow") && ok;
    ok = expectEq(ResolveAllowProcLadder(true, 4.0f, true, false, false, false, false), OccProcResult::ExtraDamage, "extra gt3") && ok;
    ok = expectEq(ResolveAllowProcLadder(true, 4.0f, false, true, false, false, false), OccProcResult::RemTriple, "rem triple") && ok;
    ok = expectEq(ResolveAllowProcLadder(true, 2.5f, false, false, true, false, false), OccProcResult::ExtraDamage, "extra gt2") && ok;
    ok = expectEq(ResolveAllowProcLadder(true, 2.5f, false, false, false, true, false), OccProcResult::RemDouble, "rem double") && ok;
    ok = expectEq(ResolveAllowProcLadder(true, 1.5f, false, false, false, false, true), OccProcResult::ExtraDamage, "extra any") && ok;
    ok = expectEq(ResolveAllowProcLadder(true, 1.5f, false, false, false, false, false), OccProcResult::None, "none") && ok;

    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::RemTriple, 1.0f), static_cast<uint32>(300), "apply triple") && ok;
    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::RemDouble, 1.0f), static_cast<uint32>(200), "apply double") && ok;
    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::ExtraDamage, 2.5f), static_cast<uint32>(250), "apply extra") && ok;
    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::None, 2.5f), static_cast<uint32>(100), "apply none") && ok;

    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true), "zanshin") && ok;
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true), "ta triple") && ok;
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true), "da double") && ok;
    ok = expect(ShouldApplyRapidShotDoubleDamage(AttackTypeRapidShot, true), "rapid") && ok;
    ok = expect(ShouldApplySambaDoubleDamage(AttackTypeSamba, true), "samba") && ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, false), "zanshin fail") && ok;
    ok = expectEq(ApplyTypeDoubleDamage(50, 2), static_cast<uint32>(100), "type dmg") && ok;
    ok = expect(RollRatePercent(25, 10) && !RollRatePercent(25, 25), "rate pct") && ok;

    return ok;
}
