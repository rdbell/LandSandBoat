#include "test_magic_state_1382.h"

#include "map/ai/states/magic_state_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "magic state 1382 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "magic state 1382 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectNear(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "magic state 1382 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMagicState1382SelfTests() -> bool
{
    using namespace magicstatehelpers;
    bool ok = true;

    ok = expect(MagicHasMoved(true, 0.31f) && !MagicHasMoved(true, 0.3f), "moved thr") && ok;
    ok = expect(!MagicHasMoved(false, 10.0f), "non-pc moved") && ok;

    ok = expect(OmertaBlocksSpell(1, 1) && !OmertaBlocksSpell(1, 2), "omerta song") && ok;
    ok = expect(OmertaBlocksSpell(2, 2) && !OmertaBlocksSpell(0, 1), "omerta black/zero") && ok;

    ok = expect(IsIndiSpellFamily(SpellFamilyIndiBuff) && IsIndiSpellFamily(SpellFamilyIndiDebuff), "indi") && ok;
    ok = expect(!IsIndiSpellFamily(SpellFamilyUtsusemi), "not indi") && ok;

    ok = expectNear(ResolvePCSpellRange(10.f, 1.f, 1.f, false, 0), 12.f, "range base") && ok;
    ok = expectNear(ResolvePCSpellRange(10.f, 1.f, 1.f, true, SpellFamilyIndiBuff), 25.f, "entrust indi") && ok;
    ok = expectNear(ResolvePCSpellRange(10.f, 1.f, 1.f, true, 0), 12.f, "entrust non-indi") && ok;

    ok = expect(TooFarAbsolute(40.1f) && !TooFarAbsolute(40.f), "abs far") && ok;
    ok = expect(OutOfPCSpellRange(12.1f, 12.f) && !OutOfPCSpellRange(12.f, 12.f), "pc range") && ok;
    ok = expect(OutOfMobSpellRange(28.6f) && !OutOfMobSpellRange(28.5f), "mob range") && ok;
    ok = expect(ShouldCheckPlayerLOS(false, true, true) && !ShouldCheckPlayerLOS(true, true, true), "los") && ok;

    ok = expectEq(EvaluateHasCost(SpellGroupNinjutsu, true, MagicFlagsNone, false, true), MagicUseFail::NoNinjaTools, "no tools") && ok;
    ok = expectEq(EvaluateHasCost(SpellGroupNinjutsu, true, MagicFlagsIgnoreTools, false, true), MagicUseFail::None, "ignore tools") && ok;
    ok = expectEq(EvaluateHasCost(SpellGroupWhite, true, MagicFlagsNone, true, false), MagicUseFail::NotEnoughMP, "no mp") && ok;
    ok = expectEq(EvaluateHasCost(SpellGroupWhite, true, MagicFlagsNone, true, true), MagicUseFail::None, "cost ok") && ok;

    ok = expectEq(
        EvaluateCanCastSpell(false, true, false, false, 0, 0, MagicUseFail::None, true, false, false, 0, true, 10, false, false, false, true),
        MagicUseFail::CannotCastSpell,
        "cannot use") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, false, false, false, 0, 0, MagicUseFail::None, true, false, false, 0, true, 10, false, false, false, true),
        MagicUseFail::CannotUseInArea,
        "zone") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, true, false, 0, 0, MagicUseFail::None, true, false, false, 0, true, 10, false, false, false, true),
        MagicUseFail::UnableToCastSpells,
        "silence") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, true, 1, 1, MagicUseFail::None, true, false, false, 0, true, 10, false, false, false, true),
        MagicUseFail::UnableToCastSpells,
        "omerta") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::NoNinjaTools, true, false, false, 0, true, 10, false, false, false, true),
        MagicUseFail::NoNinjaTools,
        "cost fail") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, false, false, false, 0, true, 10, false, false, false, true),
        MagicUseFail::CannotOnThatTarget,
        "no target") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, true, false, 0, true, 10, false, false, false, true),
        MagicUseFail::SilentReject,
        "hidden") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, false, true, 50, true, 10, false, false, false, true),
        MagicUseFail::None,
        "self skip dist") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, false, false, 41, true, 10, false, false, false, true),
        MagicUseFail::TooFarAway,
        "too far") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, false, false, 15, true, 12, false, false, false, true),
        MagicUseFail::OutOfRangeUnableCast,
        "pc range fail") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, false, false, 29, false, 10, true, false, false, true),
        MagicUseFail::SilentReject,
        "mob range") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, false, false, 5, true, 10, false, false, true, false),
        MagicUseFail::CannotPerformAction,
        "los fail") && ok;
    ok = expectEq(
        EvaluateCanCastSpell(true, true, false, false, 0, 0, MagicUseFail::None, true, false, false, 5, true, 10, false, true, true, false),
        MagicUseFail::None,
        "los skip end") && ok;

    ok = expect(ShouldSpendNinjaTools(SpellGroupNinjutsu, MagicFlagsNone) && !ShouldSpendNinjaTools(SpellGroupNinjutsu, MagicFlagsIgnoreTools), "spend tools") && ok;
    ok = expect(ShouldSpendMP(true, false, MagicFlagsNone) && !ShouldSpendMP(true, true, MagicFlagsNone), "spend mp manafont") && ok;
    ok = expect(!ShouldSpendMP(true, false, MagicFlagsIgnoreMP), "spend mp ignore") && ok;

    ok = expectEq(ApplyQuickMagicCost(100, 0), static_cast<int16>(100), "qm 0") && ok;
    // jp=50 → (50*2)/100 = 1 → cost * 0
    ok = expectEq(ApplyQuickMagicCost(100, 50), static_cast<int16>(0), "qm 50") && ok;
    // jp=25 → (50)/100 = 0 integer → no reduction
    ok = expectEq(ApplyQuickMagicCost(100, 25), static_cast<int16>(100), "qm 25 int div") && ok;

    ok = expect(ShouldApplyConserveMP(20, 19) && !ShouldApplyConserveMP(20, 20), "conserve roll") && ok;
    ok = expectEq(ApplyConserveMP(160, 8.0f), static_cast<int16>(80), "conserve half") && ok;

    ok = expect(RecastIsZero(true, false, false) && RecastIsZero(false, true, false) && RecastIsZero(false, false, true), "recast zero") && ok;
    ok = expect(!RecastIsZero(false, false, false), "recast nonzero") && ok;

    ok = expectEq(ApplySubtleSorceryCE(40, true), 0, "subtle") && ok;
    ok = expectEq(ApplySubtleSorceryCE(40, false), 40, "no subtle") && ok;
    ok = expect(ShouldApplyYoninUtsusemiEnmity(true, SpellFamilyUtsusemi, 1) && !ShouldApplyYoninUtsusemiEnmity(true, SpellFamilyUtsusemi, 0), "yonin") && ok;
    ok = expectEq(ApplyDivineEmblemScale(100, 50), 150, "emblem") && ok;
    ok = expect(ShouldApplyDivineEmblem(true, SkillDivineMagic) && !ShouldApplyDivineEmblem(true, 0), "emblem skill") && ok;
    ok = expectEq(DivineBenisonEnmityDelta(10), static_cast<int16>(-5), "benison") && ok;

    ok = expect(ShouldSkipSummoningEnmity(SpellGroupSummoning) && !ShouldSkipSummoningEnmity(SpellGroupWhite), "summon skip") && ok;
    ok = expect(ShouldGenerateMobEnmity(false, false) && ShouldGenerateMobEnmity(true, true) && !ShouldGenerateMobEnmity(true, false), "gen enmity") && ok;
    ok = expect(ShouldSkipUncharmedMobCaster(true, false) && !ShouldSkipUncharmedMobCaster(true, true), "uncharmed") && ok;
    ok = expect(ShouldClaimOnDeath(true, false, false) && ShouldClaimOnDeath(true, true, true) && !ShouldClaimOnDeath(true, true, false), "claim") && ok;
    ok = expect(ShouldApplyTranquility(true, SpellGroupWhite) && !ShouldApplyTranquility(true, SpellGroupBlack), "tranq") && ok;
    ok = expect(ShouldApplyEquanimity(true, SpellGroupBlack) && !ShouldApplyEquanimity(true, SpellGroupWhite), "equa") && ok;

    ok = expectEq(static_cast<uint16>(MagicUseFail::NotEnoughMP), static_cast<uint16>(34), "msg mp") && ok;
    ok = expectEq(static_cast<uint16>(MagicUseFail::NoNinjaTools), static_cast<uint16>(35), "msg tools") && ok;
    ok = expectEq(static_cast<uint16>(MagicUseFail::CannotCastSpell), static_cast<uint16>(47), "msg cast") && ok;
    ok = expectEq(static_cast<uint16>(MagicUseFail::OutOfRangeUnableCast), static_cast<uint16>(313), "msg oor") && ok;

    return ok;
}
