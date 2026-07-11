#include "test_attack_damage_1377.h"

#include "map/attack_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack damage 1377 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attack damage 1377 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "attack damage 1377 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackDamage1377SelfTests() -> bool
{
    using namespace attackhelpers;
    bool ok = true;

    ok = expect(ShouldApplySneakAttack(true, true, true, true, false, false), "sa behind") && ok;
    ok = expect(ShouldApplySneakAttack(true, true, true, false, true, false), "sa hide") && ok;
    ok = expect(ShouldApplySneakAttack(true, true, true, false, false, true), "sa doubt") && ok;
    ok = expect(!ShouldApplySneakAttack(false, true, true, true, false, false), "sa not thf") && ok;
    ok = expect(!ShouldApplySneakAttack(true, false, true, true, false, false), "sa not first") && ok;
    ok = expect(!ShouldApplySneakAttack(true, true, false, true, false, false), "sa no effect") && ok;

    ok = expect(ShouldApplyTrickAttack(true, true, true) && !ShouldApplyTrickAttack(true, true, false), "ta") && ok;
    ok = expectNear(SneakAttackDexBonus(100, 20), 120.0f, "sa bonus") && ok;
    ok = expectNear(SneakAttackDexBonus(100, -10), 100.0f, "sa bonus floor") && ok;
    ok = expectNear(TrickAttackAgiBonus(50, 50), 75.0f, "ta bonus") && ok;

    ok = expect(ShouldPromoteNormalToSamba(AttackTypeNormal, true, false, false), "samba") && ok;
    ok = expect(!ShouldPromoteNormalToSamba(AttackTypeSamba, true, false, false), "samba not normal") && ok;
    ok = expect(ShouldTrySkillUp(1) && !ShouldTrySkillUp(0), "skillup dmg") && ok;
    ok = expect(ShouldSkillUpThrowing(AttackTypeDaken) && !ShouldSkillUpThrowing(0), "throw skill") && ok;
    ok = expect(ShouldSkillUpAutomaton(true, true, true) && !ShouldSkillUpAutomaton(true, true, false), "auto skill") && ok;
    ok = expectEq(NaturalH2HDamage(100), static_cast<int32>(14), "h2h natural") && ok; // floor(11)+3
    ok = expectNear(SelectMobH2HPenalty(true, true), 1.0f, "no pen") && ok;
    ok = expectNear(SelectMobH2HPenalty(false, true), MobH2HPenaltyPreToAU, "pre toau") && ok;
    ok = expectNear(SelectMobH2HPenalty(false, false), MobH2HPenaltyToAUOnward, "toau") && ok;
    ok = expect(ShouldApplyConsumeMana(true) && !ShouldApplyConsumeMana(false), "mana") && ok;

    return ok;
}
