#include "test_attackround_create_attacks_plan_2756.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround create attacks plan 2756 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* const label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attackround create attacks plan 2756 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackRoundCreateAttacksPlan2756SelfTests() -> bool
{
    using namespace attackroundhelpers;

    const auto mikage = ResolveCreateAttacksPlan(true, 3, MultiHitPreference::Quad, 2, true);
    const auto doubleHit = ResolveCreateAttacksPlan(false, 0, MultiHitPreference::Double, 1, false);
    const auto mythic = ResolveCreateAttacksPlan(false, 0, MultiHitPreference::MythicThrice, 1, false);
    const auto occasional = ResolveCreateAttacksPlan(false, 0, MultiHitPreference::OccasionalExtra, 3, true);
    const auto none = ResolveCreateAttacksPlan(false, 0, MultiHitPreference::None, 1, false);
    const auto zeroMikage = ResolveCreateAttacksPlan(true, 0, MultiHitPreference::None, 1, false);

    return expectEq(mikage.initialAttackType, AttackTypeNormal, "mikage type") &&
           expectEq(mikage.initialSwingCount, static_cast<uint8>(3), "mikage count") &&
           expect(mikage.addAdditionalSwing && mikage.addDefaultHit, "mikage extras") &&
           expectEq(doubleHit.initialAttackType, AttackTypeDouble, "double type") &&
           expectEq(doubleHit.initialSwingCount, static_cast<uint8>(2), "double count") &&
           expect(!doubleHit.addAdditionalSwing && !doubleHit.addDefaultHit, "double flags") &&
           expectEq(mythic.initialAttackType, AttackTypeNormal, "mythic type") &&
           expectEq(mythic.initialSwingCount, static_cast<uint8>(2), "mythic count") &&
           expect(!mythic.addAdditionalSwing && mythic.addDefaultHit, "mythic flags") &&
           expectEq(occasional.initialAttackType, AttackTypeNormal, "occasional type") &&
           expectEq(occasional.initialSwingCount, static_cast<uint8>(2), "occasional count") &&
           expect(occasional.addAdditionalSwing && occasional.addDefaultHit, "occasional flags") &&
           expectEq(none.initialAttackType, AttackTypeNormal, "none type") &&
           expectEq(none.initialSwingCount, static_cast<uint8>(0), "none count") &&
           expect(!none.addAdditionalSwing && none.addDefaultHit, "none flags") &&
           expectEq(zeroMikage.initialSwingCount, static_cast<uint8>(0), "zero mikage count") &&
           expect(zeroMikage.addDefaultHit && !zeroMikage.addAdditionalSwing, "zero mikage flags");
}
