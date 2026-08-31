#include "test_petskill_state_1386.h"

#include "map/ai/states/petskill_state_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "petskill state 1386 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "petskill state 1386 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runPetSkillState1386SelfTests() -> bool
{
    using namespace petskillstatehelpers;
    bool ok = true;

    ok = expect(ShouldRejectAmnesiaOrImpairment(true) && !ShouldRejectAmnesiaOrImpairment(false), "amnesia") && ok;
    ok = expectEq(SkillStartParam(100, 5), static_cast<uint16>(100), "param mob") && ok;
    ok = expectEq(SkillStartParam(0, 5), static_cast<uint16>(5), "param skill") && ok;
    ok = expect(SkillStartUsesWeaponskillMessage(10) && !SkillStartUsesWeaponskillMessage(0), "msg") && ok;
    ok = expect(ShouldEmitWyvernSkillReady(PetIDWyvern, 0) && !ShouldEmitWyvernSkillReady(PetIDWyvern, 1), "wyvern") && ok;
    ok = expect(!ShouldEmitWyvernSkillReady(1, 0), "not wyvern") && ok;

    ok = expect(ShouldSpendPetSkillTP(false) && !ShouldSpendPetSkillTP(true), "spend gate") && ok;
    {
        const auto [spent, rem] = EvaluatePetSkillSpendCost(false, 2000);
        ok = expectEq(spent, static_cast<int16>(2000), "spent") && ok;
        ok = expectEq(rem, static_cast<int16>(0), "rem") && ok;
    }
    {
        const auto [spent, rem] = EvaluatePetSkillSpendCost(true, 2000);
        ok = expectEq(spent, static_cast<int16>(0), "free spent") && ok;
        ok = expectEq(rem, static_cast<int16>(2000), "free rem") && ok;
    }

    ok = expect(ShouldFinishPetSkill(true, false) && !ShouldFinishPetSkill(true, true), "finish") && ok;
    ok = expect(!ShouldFinishPetSkill(false, false), "finish before cast") && ok;
    ok = expect(SkillSuccessFromAction(false) && !SkillSuccessFromAction(true), "success") && ok;
    ok = expect(ShouldExitPetSkill(true, true) && !ShouldExitPetSkill(false, true), "exit") && ok;
    ok = expect(!ShouldExitPetSkill(true, false), "exit before animation") && ok;

    ok = expect(ShouldUpdateExitEnmity(true, true, true, false, true), "enmity ok") && ok;
    ok = expect(!ShouldUpdateExitEnmity(true, true, true, true, true), "enmity self") && ok;
    ok = expect(!ShouldUpdateExitEnmity(true, true, true, false, false), "enmity same alleg") && ok;
    ok = expect(EnmityWithMaster(true) && !EnmityWithMaster(false), "with master") && ok;

    ok = expect(ShouldApplyAvatarsFavor(true, true, true, false, true), "favor") && ok;
    ok = expect(!ShouldApplyAvatarsFavor(true, true, true, false, false), "no favor") && ok;
    ok = expectEq(AvatarsFavorLevelGained(true), static_cast<int16>(3), "rage") && ok;
    ok = expectEq(AvatarsFavorLevelGained(false), static_cast<int16>(2), "ward") && ok;
    ok = expectEq(ApplyAvatarsFavorPower(5, 3), static_cast<int16>(11), "floor") && ok;
    ok = expectEq(ApplyAvatarsFavorPower(10, 3), static_cast<int16>(13), "grow") && ok;

    ok = expect(ShouldConsiderAvatarReengage(true, true, true, false), "consider reengage") && ok;
    ok = expect(ShouldReengageAfterBloodPact(true, PetTypeAvatar, 1, true, true, true), "reengage") && ok;
    ok = expect(!ShouldReengageAfterBloodPact(true, PetTypeAvatar, PetIDAlexander, true, true, true), "alex") && ok;
    ok = expect(!ShouldReengageAfterBloodPact(true, PetTypeAvatar, PetIDAtomos, true, true, true), "atomos") && ok;
    ok = expect(!ShouldReengageAfterBloodPact(true, 2, 1, true, true, true), "not avatar") && ok;

    ok = expect(ShouldApplyFinalAnimationSub(true, true) && !ShouldApplyFinalAnimationSub(false, true), "anim") && ok;
    ok = expect(ShouldInterruptOnCleanup(false) && !ShouldInterruptOnCleanup(true), "cleanup int") && ok;
    ok = expect(ShouldSendSkillStartPacket(true) && !ShouldSendSkillStartPacket(false), "start pkt") && ok;

    ok = expectEq(PetIDWyvern, static_cast<uint16>(48), "id wyvern") && ok;
    ok = expectEq(PetIDAlexander, static_cast<uint16>(17), "id alex") && ok;
    ok = expectEq(PetTypeAvatar, static_cast<uint8>(0), "type avatar") && ok;
    ok = expectEq(MsgReadiesWeaponskill, static_cast<uint16>(43), "msg ws") && ok;
    ok = expectEq(MsgReadiesSkill, static_cast<uint16>(326), "msg skill") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("petskill-state-1386", runPetSkillState1386SelfTests);
