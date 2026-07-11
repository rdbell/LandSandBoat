#include "test_ability_state_1383.h"

#include "map/ai/states/ability_state_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "ability state 1383 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ability state 1383 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAbilityState1383SelfTests() -> bool
{
    using namespace abilitystatehelpers;
    bool ok = true;

    ok = expect(HasAbilityRecast(true) && !HasAbilityRecast(false), "recast") && ok;
    ok = expect(ImpairmentBlocksAbility(true, 0x01) && ImpairmentBlocksAbility(true, 0x03), "impair block") && ok;
    ok = expect(!ImpairmentBlocksAbility(true, 0x02) && !ImpairmentBlocksAbility(false, 0x01), "impair ok") && ok;
    ok = expect(StatusBlocksAbility(true, false, 0) && StatusBlocksAbility(false, true, 0x01), "status block") && ok;

    ok = expect(IsLuopanAbility(AbilityConcentricPulse) && IsLuopanAbility(AbilityRadialArcana), "luopan") && ok;
    ok = expect(!IsLuopanAbility(352) && !IsLuopanAbility(356), "not luopan") && ok;

    ok = expect(NeedsPetSkillDistanceCheck(true, true, true, false, false, 600), "need bp") && ok;
    ok = expect(!NeedsPetSkillDistanceCheck(true, true, true, false, false, AbilityConcentricPulse), "skip luopan") && ok;
    ok = expect(!NeedsPetSkillDistanceCheck(false, true, true, false, false, 600), "not pet abil") && ok;

    ok = expect(OutOfAbilityRange(15.f, 10.f, 1.f, 1.f) && !OutOfAbilityRange(12.f, 10.f, 1.f, 1.f), "abil range") && ok;
    ok = expect(BloodPactPCOutOfRange(false, 25.f, 1.f, 1.f) && !BloodPactPCOutOfRange(true, 100.f, 1.f, 1.f), "bp pc") && ok;
    ok = expect(BloodPactPetOutOfRange(20.f, 15.f, 1.f, 1.f), "bp pet") && ok;
    ok = expect(ReadyPCToPetOutOfRange(10.f, 1.f, 1.f) && !ReadyPCToPetOutOfRange(5.f, 1.f, 1.f), "ready pc-pet") && ok;
    ok = expect(ReadyPetToEnemyOutOfRange(true, true, 20.f, 10.f, 1.f, 1.f), "ready pet-enemy") && ok;
    ok = expect(!ReadyPetToEnemyOutOfRange(false, true, 20.f, 10.f, 1.f, 1.f), "ready no enemy flag") && ok;

    ok = expect(EvaluatePetSkillDistance(false, true, false, 0, 0, 0, 0, 0, 0, false, 0, false, false, 0, 0) == false, "no pet") && ok;
    ok = expect(EvaluatePetSkillDistance(true, true, false, 5.f, 1.f, 1.f, 5.f, 15.f, 1.f, false, 0, false, false, 0, 0), "bp ok") && ok;
    ok = expect(!EvaluatePetSkillDistance(true, true, false, 30.f, 1.f, 1.f, 5.f, 15.f, 1.f, false, 0, false, false, 0, 0), "bp pc far") && ok;
    ok = expect(EvaluatePetSkillDistance(true, false, false, 0, 1.f, 1.f, 0, 15.f, 1.f, true, 3.f, true, true, 5.f, 1.f), "ready ok") && ok;
    ok = expect(!EvaluatePetSkillDistance(true, false, false, 0, 1.f, 1.f, 0, 15.f, 1.f, true, 10.f, true, true, 5.f, 1.f), "ready pc-pet far") && ok;

    ok = expect(PCLacksAbilityAccess(false, 1, false, false) && !PCLacksAbilityAccess(false, 1, true, false), "pc abil") && ok;
    ok = expect(PCLacksAbilityAccess(true, AbilityHealingRuby, false, false) && !PCLacksAbilityAccess(true, AbilityHealingRuby, false, true), "pet abil") && ok;
    ok = expect(!PCLacksAbilityAccess(true, 100, false, false), "pet below ruby") && ok;

    ok = expect(ShouldCheckPlayerAbilityLOS(true) && !ShouldCheckPlayerAbilityLOS(false), "los") && ok;

    ok = expect(NonPCCancelAbility(false, false, false, 0, false, false, 0, 0, 0, 0), "no target") && ok;
    ok = expect(NonPCCancelAbility(true, true, false, 0, true, false, 0, 10, 1, 1), "amnesia") && ok;
    ok = expect(NonPCCancelAbility(true, false, false, 0, true, false, 20, 10, 1, 1), "range") && ok;
    ok = expect(!NonPCCancelAbility(true, false, false, 0, true, true, 100, 10, 1, 1), "self ok") && ok;
    ok = expect(!NonPCCancelAbility(true, false, false, 0, true, false, 5, 10, 1, 1), "in range") && ok;

    ok = expect(ShouldUpdateHostileEnmity(true, true, true, 10, 10) && !ShouldUpdateHostileEnmity(true, true, true, 0, 0), "hostile enmity") && ok;
    ok = expect(ShouldGenerateAllyEnmity(true) && !ShouldGenerateAllyEnmity(false), "ally enmity") && ok;
    ok = expect(IsCharmAbility(AbilityCharm) && !IsCharmAbility(1), "charm") && ok;

    ok = expectEq(static_cast<uint16>(AbilityUseFail::TargetOutOfRange), static_cast<uint16>(4), "msg oor") && ok;
    ok = expectEq(static_cast<uint16>(AbilityUseFail::UnableToSeeTarget), static_cast<uint16>(5), "msg see") && ok;
    ok = expectEq(static_cast<uint16>(AbilityUseFail::UnableToUseJobAbility2), static_cast<uint16>(88), "msg abil") && ok;
    ok = expectEq(AbilityHealingRuby, static_cast<uint16>(512), "id ruby") && ok;
    ok = expectEq(AbilityConcentricPulse, static_cast<uint16>(353), "id pulse") && ok;
    ok = expectEq(AbilityCharm, static_cast<uint16>(52), "id charm") && ok;

    return ok;
}
