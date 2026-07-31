#include "test_attack_assembly_1378.h"

#include "map/attack_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack assembly 1378 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attack assembly 1378 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "attack assembly 1378 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackAssembly1378SelfTests() -> bool
{
    using namespace attackhelpers;
    bool ok = true;

    ok = expectEq(FloorAtZero(-3), static_cast<int32>(0), "floor zero") && ok;
    ok = expectEq(FloorAtZero(5), static_cast<int32>(5), "floor pos") && ok;
    ok = expectEq(ApplyDamageRatio(100, 1.5f), static_cast<int32>(150), "ratio") && ok;
    ok = expectEq(ApplyDamageRatio(0, 2.0f), static_cast<int32>(0), "ratio zero") && ok;
    ok = expectEq(ApplyDamageRatio(-1, 2.0f), static_cast<int32>(0), "ratio neg") && ok;

    // Mob non-kick: (50+10)*0.425 = 25.5
    ok = expectNear(AssembleMobH2HDamagePreRatio(50, false, 0, 10, 0.425f), 25.5f, "mob punch") && ok;
    ok = expectEq(AssembleMobH2HDamage(50, false, 0, 10, 0.425f), static_cast<int32>(25), "mob punch integer cast") && ok;
    // Mob kick: (50+5)*0.425*(2/3) + 10
    {
        float expected = (50.0f + 5.0f) * 0.425f * MobKickPenalty + 10.0f;
        ok             = expectNear(AssembleMobH2HDamagePreRatio(50, true, 5, 10, 0.425f), expected, "mob kick") && ok;
        ok             = expectEq(AssembleMobH2HDamage(50, true, 5, 10, 0.425f), static_cast<int32>(25), "mob kick integer cast") && ok;
    }
    ok = expectEq(AssembleMobH2HDamage(-100, false, 0, 0, 1.0f), static_cast<int32>(0), "mob damage clamp") && ok;

    ok = expectEq(AssemblePlayerH2HKickPreRatio(14, 10, 5, 3), static_cast<int32>(32), "player kick") && ok;
    ok = expectEq(AssemblePlayerH2HPunchPreRatio(20, 14, 5, 3), static_cast<int32>(42), "player punch") && ok;
    ok = expectEq(AssembleMainHandPreRatio(30, 5, -40), static_cast<int32>(0), "main floor") && ok;
    ok = expectEq(AssembleSubHandPreRatio(12, 2, 1), static_cast<int32>(15), "sub") && ok;
    ok = expectEq(AssembleRangedAmmoPreRatio(25, 2), static_cast<int32>(27), "ammo") && ok;

    ok = expectNear(ScarletDeliriumMultiplier(250), 1.25f, "scarlet") && ok;
    ok = expect(ShouldApplyScarletDelirium(true) && !ShouldApplyScarletDelirium(false), "scarlet gate") && ok;
    ok = expectEq(FloorProduct(100, 1.25f), static_cast<int32>(125), "floor product") && ok;

    // DA quirk: floor(100 * 1.0 + 0.20) = 100
    ok = expectEq(ApplyDoubleTripleAttackDamage(100, 20), static_cast<int32>(100), "da additive quirk") && ok;
    ok = expectEq(ApplyDoubleTripleAttackDamage(100, 150), static_cast<int32>(101), "da large") && ok;
    ok = expect(ShouldApplyDoubleAttackDamage(AttackTypeDouble, true), "da gate") && ok;
    ok = expect(ShouldApplyTripleAttackDamage(AttackTypeTriple, true), "ta gate") && ok;
    ok = expect(!ShouldApplyDoubleAttackDamage(AttackTypeDouble, false), "da not pc") && ok;

    ok = expect(ShouldApplySoulEater(true) && !ShouldApplySoulEater(false), "soul") && ok;
    ok = expect(ShouldApplySAAugment(10, true, true) && !ShouldApplySAAugment(0, true, true), "sa aug") && ok;
    ok = expect(ShouldApplyTAAugment(10, true, true) && !ShouldApplyTAAugment(10, false, true), "ta aug") && ok;
    ok = expectNear(AugmentDamageMultiplier(25), 1.25f, "aug mult") && ok;
    ok = expectEq(ClampNonNegativeDamage(-5), static_cast<int32>(0), "clamp") && ok;
    ok = expect(IsKickAttackType(AttackTypeKick) && !IsKickAttackType(0), "kick type") && ok;

    return ok;
}
