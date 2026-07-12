#include "test_attackround_multihit_1374.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround multihit 1374 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attackround multihit 1374 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackRoundMultihit1374SelfTests() -> bool
{
    using namespace attackroundhelpers;
    bool ok = true;

    ok = expectEq(ClampMaxSwings(10), static_cast<uint8>(8), "clamp max") && ok;
    ok = expectEq(ClampMaxSwings(3), static_cast<uint8>(3), "clamp under") && ok;
    ok = expect(ShouldUseWeaponHitCount(50, 75) && !ShouldUseWeaponHitCount(80, 75), "weapon lvl") && ok;
    ok = expect(ShouldApplyMaxSwingsMod(true, 2) && !ShouldApplyMaxSwingsMod(true, 0), "max swings") && ok;
    ok = expect(ShouldApplyMobMultiHit(3) && !ShouldApplyMobMultiHit(0), "mob multi") && ok;
    ok = expectEq(MobMultiHitSwingCount(2), static_cast<uint8>(3), "mob count") && ok;
    ok = expectEq(ClampAttackRate(150), static_cast<int16>(100), "rate high") && ok;
    ok = expectEq(ClampAttackRate(-5), static_cast<int16>(0), "rate low") && ok;
    ok = expect(IsMainHandForMultiHit(false, true, true), "main right") && ok;
    ok = expect(IsMainHandForMultiHit(true, false, false), "main h2h") && ok;
    ok = expect(!IsMainHandForMultiHit(false, true, false), "offhand") && ok;
    ok = expect(ShouldAddMikageSwings(true, true) && !ShouldAddMikageSwings(true, false), "mikage") && ok;

    ok = expectEq(ResolveExclusiveMultiHitPreference(true, true, true, true, true, true, 3), MultiHitPreference::Quad, "pref quad") && ok;
    ok = expectEq(ResolveExclusiveMultiHitPreference(false, true, true, true, true, true, 3), MultiHitPreference::Triple, "pref triple") && ok;
    ok = expectEq(ResolveExclusiveMultiHitPreference(false, false, true, true, true, true, 3), MultiHitPreference::Double, "pref double") && ok;
    ok = expectEq(ResolveExclusiveMultiHitPreference(false, false, false, true, true, true, 3), MultiHitPreference::MythicThrice, "pref myth thrice") && ok;
    ok = expectEq(ResolveExclusiveMultiHitPreference(false, false, false, true, false, true, 3), MultiHitPreference::MythicTwice, "pref myth twice") && ok;
    ok = expectEq(ResolveExclusiveMultiHitPreference(false, false, false, false, true, true, 3), MultiHitPreference::OccasionalExtra, "pref occ") && ok;
    ok = expectEq(ResolveExclusiveMultiHitPreference(false, false, false, true, false, false, 1), MultiHitPreference::None, "pref none") && ok;

    ok = expectEq(ExclusiveMultiHitSwingCount(MultiHitPreference::Quad, 1), static_cast<uint8>(4), "swings quad") && ok;
    ok = expectEq(ExclusiveMultiHitSwingCount(MultiHitPreference::OccasionalExtra, 3), static_cast<uint8>(2), "swings occ") && ok;
    ok = expect(MultiHitOccurred(MultiHitPreference::Double) && !MultiHitOccurred(MultiHitPreference::MythicTwice), "occurred") && ok;
    ok = expect(ShouldAddDefaultHit(false) && !ShouldAddDefaultHit(true), "default hit") && ok;
    ok = expect(ShouldAddAdditionalSwing(true, true) && !ShouldAddAdditionalSwing(false, true), "add swing") && ok;
    ok = expect(ShouldAddOffhandExtraDualWield(false) && !ShouldAddOffhandExtraDualWield(true), "extra dw") && ok;
    ok = expect(AmbushRotationInWindow(22) && !AmbushRotationInWindow(23), "ambush rot") && ok;
    ok = expect(ShouldApplyAmbushTripleBonus(true, true, true) && !ShouldApplyAmbushTripleBonus(true, false, true), "ambush bonus") && ok;

    // Slice 1579: exclusive preference → attack type + apply gate
    ok = expectEq(ExclusiveMultiHitAttackType(MultiHitPreference::Quad), AttackTypeQuad, "type quad") && ok;
    ok = expectEq(ExclusiveMultiHitAttackType(MultiHitPreference::Triple), AttackTypeTriple, "type triple") && ok;
    ok = expectEq(ExclusiveMultiHitAttackType(MultiHitPreference::Double), AttackTypeDouble, "type double") && ok;
    ok = expectEq(ExclusiveMultiHitAttackType(MultiHitPreference::MythicThrice), AttackTypeNormal, "type myth") && ok;
    ok = expect(ShouldApplyExclusiveMultiHitSwings(MultiHitPreference::OccasionalExtra) &&
                    !ShouldApplyExclusiveMultiHitSwings(MultiHitPreference::None),
                "apply exclusive") &&
         ok;

    return ok;
}
