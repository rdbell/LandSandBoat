#include "test_mobskill_state_1385.h"

#include "map/ai/states/mobskill_state_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobskill state 1385 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mobskill state 1385 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMobSkillState1385SelfTests() -> bool
{
    using namespace mobskillstatehelpers;
    bool ok = true;

    ok = expectEq(ResolveMobSkillSpendPath(true, true, true, 3), MobSkillSpendPath::TpFree, "free first") && ok;
    ok = expectEq(ResolveMobSkillSpendPath(false, true, true, 3), MobSkillSpendPath::Sekkanoki, "sek") && ok;
    ok = expectEq(ResolveMobSkillSpendPath(false, false, true, 2), MobSkillSpendPath::MeikyoShisui, "meikyo") && ok;
    ok = expectEq(ResolveMobSkillSpendPath(false, false, true, 0), MobSkillSpendPath::Normal, "meikyo count0") && ok;
    ok = expectEq(ResolveMobSkillSpendPath(false, false, false, 0), MobSkillSpendPath::Normal, "normal") && ok;

    ok = expectEq(SekkanokiActualDrain(2500), static_cast<int16>(1000), "sek drain full") && ok;
    ok = expectEq(SekkanokiActualDrain(500), static_cast<int16>(500), "sek drain partial") && ok;
    ok = expectEq(SekkanokiActualDrain(0), static_cast<int16>(0), "sek drain zero") && ok;

    {
        const auto r = EvaluateMobSkillSpendCost(true, false, false, 0, 2000);
        ok = expectEq(r.spentTP, static_cast<int16>(0), "free spent") && ok;
        ok = expectEq(r.newTP, static_cast<int16>(2000), "free tp") && ok;
    }
    {
        const auto r = EvaluateMobSkillSpendCost(false, true, false, 0, 2500);
        ok = expectEq(r.spentTP, static_cast<int16>(1000), "sek spent") && ok;
        ok = expectEq(r.newTP, static_cast<int16>(1500), "sek new") && ok;
        ok = expect(r.deleteSekkanoki, "sek del") && ok;
    }
    {
        const auto r = EvaluateMobSkillSpendCost(false, true, false, 0, 400);
        ok = expectEq(r.spentTP, static_cast<int16>(400), "sek low spent") && ok;
        ok = expectEq(r.newTP, static_cast<int16>(0), "sek low new") && ok;
    }
    {
        const auto r = EvaluateMobSkillSpendCost(false, false, true, 2, 3000);
        ok = expectEq(r.spentTP, static_cast<int16>(3000), "meikyo spent") && ok;
        ok = expectEq(r.meikyoCountAfter, static_cast<uint32>(1), "meikyo count") && ok;
        ok = expect(!r.zeroTPOnMeikyoEnd, "meikyo mid") && ok;
        ok = expectEq(r.newTP, static_cast<int16>(3000), "meikyo mid tp") && ok;
    }
    {
        const auto r = EvaluateMobSkillSpendCost(false, false, true, 1, 3000);
        ok = expect(r.zeroTPOnMeikyoEnd, "meikyo end") && ok;
        ok = expectEq(r.newTP, static_cast<int16>(0), "meikyo end tp") && ok;
        ok = expectEq(r.meikyoCountAfter, static_cast<uint32>(0), "meikyo end count") && ok;
    }
    {
        const auto r = EvaluateMobSkillSpendCost(false, false, false, 0, 1800);
        ok = expectEq(r.spentTP, static_cast<int16>(1800), "normal spent") && ok;
        ok = expectEq(r.newTP, static_cast<int16>(0), "normal zero") && ok;
    }

    ok = expect(ShouldProcessInstantMobSkill(true) && !ShouldProcessInstantMobSkill(false), "instant") && ok;
    ok = expect(ShouldTurnDuringMobSkillCast(true, true) && !ShouldTurnDuringMobSkillCast(true, false), "turn") && ok;
    ok = expect(ShouldFinishMobSkill(true, false) && !ShouldFinishMobSkill(true, true), "finish >= ") && ok;
    ok = expect(ShouldInterruptMobSkillFinish(true, false) || ShouldInterruptMobSkillFinish(false, true), "interrupt") && ok;
    ok = expect(SkillSuccessFromAction(false) && !SkillSuccessFromAction(true), "success") && ok;

    ok = expect(ShouldUpdateExitEnmity(true, true, true, false, true) && !ShouldUpdateExitEnmity(true, true, true, true, true), "enmity") && ok;
    ok = expect(EnmityWithMaster(true, false, false) && EnmityWithMaster(false, true, true) && !EnmityWithMaster(false, true, false), "with master") && ok;

    ok = expect(ShouldApplyAvatarsFavor(true, true, true, false, true), "favor rage") && ok;
    ok = expect(!ShouldApplyAvatarsFavor(true, true, true, false, false), "no favor") && ok;
    ok = expectEq(AvatarsFavorLevelGained(true), static_cast<int16>(3), "rage gain") && ok;
    ok = expectEq(AvatarsFavorLevelGained(false), static_cast<int16>(2), "ward gain") && ok;
    ok = expectEq(ApplyAvatarsFavorPower(5, 3), static_cast<int16>(11), "favor floor") && ok;
    ok = expectEq(ApplyAvatarsFavorPower(10, 3), static_cast<int16>(13), "favor grow") && ok;

    ok = expect(ShouldExitMobSkill(true, true) && !ShouldExitMobSkill(true, false), "exit") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("mobskill-state-1385", runMobSkillState1385SelfTests);
