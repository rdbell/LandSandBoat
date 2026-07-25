#include "test_skill_up_1506.h"

#include "map/skill_up_capacity.h"
#include "map/skill_up_award_capacity.h"
#include "map/skill_up_cap_capacity.h"
#include "map/skill_up_extra_step_capacity.h"
#include "map/skill_up_key_items_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using skilluphelpers::ApplyRovSkillAmount;
using skilluphelpers::ApplySkillAmountMultiplier;
using skilluphelpers::ApplySkillUpRateMod;
using skilluphelpers::BaseSkillUpChance;
using skilluphelpers::CapSkillAmountToCeiling;
using skilluphelpers::CapSkillTenths;
using skilluphelpers::ClampSkillUpChance;
using skilluphelpers::CrossedSkillLevel;
using skilluphelpers::ExtraSkillUpTierChance;
using skilluphelpers::HitsSkillCap;
using skilluphelpers::IsCombatSkillUpSkill;
using skilluphelpers::IsMagicSkillUpSkill;
using skilluphelpers::IsSkillIDValid;
using skilluphelpers::ResolveCapSkill;
using skilluphelpers::ResolveMaxSkill;
using skilluphelpers::ShouldConsiderSkillUp;
using skilluphelpers::ShouldGainSkillUp;
using skilluphelpers::ShouldIncrementWorkingSkill;
using skilluphelpers::ShouldStopExtraSkillUp;
using skilluphelpers::SkillDiff;
using skilluphelpers::SkillUpTier;

auto nearly(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-9;
}

auto Check() -> bool
{
    if (!IsSkillIDValid(0) || !IsSkillIDValid(63) || IsSkillIDValid(64))
    {
        return false;
    }
    if (!ShouldConsiderSkillUp(1, 0, false) || ShouldConsiderSkillUp(0, 0, false) || !ShouldConsiderSkillUp(0, 0, true))
    {
        return false;
    }
    if (ShouldConsiderSkillUp(1, 0x8000, false) || !ShouldConsiderSkillUp(1, 0x8000, true))
    {
        return false;
    }
    if (ResolveCapSkill(300, 200, true) != 300 || ResolveCapSkill(200, 300, true) != 300 || ResolveCapSkill(200, 300, false) != 200)
    {
        return false;
    }
    if (ResolveMaxSkill(300, 200, 250, 180, true) != 250 || ResolveMaxSkill(200, 300, 150, 280, true) != 280)
    {
        return false;
    }
    if (SkillDiff(100, 500) != 50 || SkillDiff(100, 999) != 1)
    {
        return false;
    }
    // Diff=50, cur=500 → 500/100=5 → log10(6); mult=1
    // 50/5 + 1*(2 - log10(6)) = 10 + 2 - log10(6)
    {
        const auto got  = BaseSkillUpChance(50, 500, 1.0);
        const auto want = 10.0 + 1.0 * (2.0 - std::log10(6.0));
        if (!nearly(got, want))
        {
            return false;
        }
    }
    if (!nearly(ClampSkillUpChance(0.9), 0.5) || !nearly(ClampSkillUpChance(0.3), 0.3))
    {
        return false;
    }
    if (!IsCombatSkillUpSkill(1) || !IsCombatSkillUpSkill(12) || !IsCombatSkillUpSkill(25) || !IsCombatSkillUpSkill(31))
    {
        return false;
    }
    if (IsCombatSkillUpSkill(13) || IsCombatSkillUpSkill(32) || IsCombatSkillUpSkill(0))
    {
        return false;
    }
    if (!IsMagicSkillUpSkill(32) || !IsMagicSkillUpSkill(44) || IsMagicSkillUpSkill(31) || IsMagicSkillUpSkill(45))
    {
        return false;
    }
    if (!nearly(ApplySkillUpRateMod(0.4, 50), 0.6))
    {
        return false;
    }
    if (!ShouldGainSkillUp(1, 0.1, 0.5, false) || ShouldGainSkillUp(0, 0.1, 0.5, false) || !ShouldGainSkillUp(1, 0.9, 0.1, true))
    {
        return false;
    }
    if (SkillUpTier(0) != 1 || SkillUpTier(4) != 1 || SkillUpTier(5) != 2 || SkillUpTier(20) != 5 || SkillUpTier(100) != 5)
    {
        return false;
    }
    if (!nearly(ExtraSkillUpTierChance(5), 0.9) || !nearly(ExtraSkillUpTierChance(1), 0.2) || !nearly(ExtraSkillUpTierChance(0), 0.0))
    {
        return false;
    }
    if (!ShouldStopExtraSkillUp(0.3, 0.5, 1) || ShouldStopExtraSkillUp(0.5, 0.3, 1) || !ShouldStopExtraSkillUp(0.9, 0.1, 5))
    {
        return false;
    }
    const auto continuedExtraStep = skillupextrastephelpers::PlanFor({
        .tier        = 5,
        .skillAmount = 1,
        .random      = 0.3,
    });
    const auto stoppedExtraStep = skillupextrastephelpers::PlanFor({
        .tier        = 1,
        .skillAmount = 1,
        .random      = 0.3,
    });
    const auto maxedExtraStep = skillupextrastephelpers::PlanFor({
        .tier        = 5,
        .skillAmount = 5,
        .random      = 0.1,
    });
    if (continuedExtraStep.stop || continuedExtraStep.chance != 0.9 || continuedExtraStep.nextTier != 4 ||
        continuedExtraStep.nextSkillAmount != 2 || !stoppedExtraStep.stop || stoppedExtraStep.chance != 0.2 ||
        stoppedExtraStep.nextTier != 1 || stoppedExtraStep.nextSkillAmount != 1 || !maxedExtraStep.stop ||
        maxedExtraStep.nextTier != 5 || maxedExtraStep.nextSkillAmount != 5)
    {
        return false;
    }
    const auto noRovKeyItems = skilluprovhelpers::PlanFor({});
    const auto heldRovKeyItems = skilluprovhelpers::PlanFor({
        .held = { true, false, true },
    });
    if (skilluprovhelpers::SkillUpIncreaseKeyItems[0] != KeyItem::RHAPSODY_IN_WHITE ||
        skilluprovhelpers::SkillUpIncreaseKeyItems[1] != KeyItem::RHAPSODY_IN_CRIMSON ||
        skilluprovhelpers::SkillUpIncreaseKeyItems[2] != KeyItem::RHAPSODY_IN_FUCHSIA || noRovKeyItems.heldCount != 0 ||
        heldRovKeyItems.heldCount != 2)
    {
        return false;
    }
    if (CapSkillTenths(100) != 1000)
    {
        return false;
    }
    if (ApplyRovSkillAmount(2, 0) != 2 || ApplyRovSkillAmount(2, 2) != 6 || ApplyRovSkillAmount(5, 3) != 9)
    {
        return false;
    }
    if (ApplySkillAmountMultiplier(2, 1) != 2 || ApplySkillAmountMultiplier(2, 2) != 6 || ApplySkillAmountMultiplier(5, 3) != 9)
    {
        return false;
    }
    if (CapSkillAmountToCeiling(5, 990, 1000) != 5 || CapSkillAmountToCeiling(20, 990, 1000) != 10)
    {
        return false;
    }
    if (!HitsSkillCap(20, 990, 1000) || HitsSkillCap(3, 100, 1000))
    {
        return false;
    }
    const auto belowCapAward = skillupcaphelpers::PlanFor({
        .currentSkill = 900,
        .skillAmount  = 5,
        .capSkill     = 1000,
    });
    const auto exactCapAward = skillupcaphelpers::PlanFor({
        .currentSkill = 990,
        .skillAmount  = 10,
        .capSkill     = 1000,
    });
    const auto overCapAward = skillupcaphelpers::PlanFor({
        .currentSkill = 990,
        .skillAmount  = 20,
        .capSkill     = 1000,
    });
    if (belowCapAward.skillAmount != 5 || belowCapAward.markSkillCapped || exactCapAward.skillAmount != 10 ||
        !exactCapAward.markSkillCapped || overCapAward.skillAmount != 10 || !overCapAward.markSkillCapped)
    {
        return false;
    }
    // (99/10=9) < ((99+2)/10=10); (90/10=9) < ((90+5)/10=9) is false; (100/10=10) < ((100+10)/10=11)
    if (!CrossedSkillLevel(99, 2) || CrossedSkillLevel(90, 5) || !CrossedSkillLevel(100, 10))
    {
        return false;
    }
    if (!ShouldIncrementWorkingSkill(false, 10, 20) || !ShouldIncrementWorkingSkill(true, 10, 10) || ShouldIncrementWorkingSkill(true, 10, 20))
    {
        return false;
    }
    const auto noLevelAward = skillupawardhelpers::PlanFor({
        .currentSkill = 90,
        .skillAmount  = 5,
    });
    const auto levelAward = skillupawardhelpers::PlanFor({
        .currentSkill = 99,
        .skillAmount  = 2,
    });
    const auto artsLevelAward = skillupawardhelpers::PlanFor({
        .currentSkill     = 99,
        .skillAmount      = 2,
        .artsActive       = true,
        .skillBonusBefore = 10,
        .skillBonusAfter  = 20,
    });
    if (!noLevelAward.sendSkillGain || noLevelAward.incrementWorkingSkill || noLevelAward.sendStatus ||
        noLevelAward.sendSkillLevelUp || noLevelAward.checkWeaponSkill || !noLevelAward.saveSkill || noLevelAward.skillLevel != 0 ||
        !levelAward.incrementWorkingSkill || !levelAward.sendStatus || !levelAward.sendSkillLevelUp ||
        !levelAward.checkWeaponSkill || levelAward.skillLevel != 10 || artsLevelAward.incrementWorkingSkill ||
        !artsLevelAward.sendStatus || !artsLevelAward.sendSkillLevelUp || !artsLevelAward.checkWeaponSkill)
    {
        return false;
    }
    return true;
}
} // namespace

auto runSkillUp1506SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "skill up 1506 self-test failed\n";
    }
    return ok;
}
