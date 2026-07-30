#include "test_latent_condition_eval_1359.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "latent condition eval 1359 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLatentConditionEval1359SelfTests() -> bool
{
    bool ok = true;

    // Resource percent
    ok = expect(latenthelpers::ResourcePercent(50, 100) == 50.0f, "pct 50") && ok;
    ok = expect(latenthelpers::ResourcePercent(0, 0) == 0.0f, "pct max0") && ok;

    // HP
    ok = expect(latenthelpers::EvaluateHpUnderPercent(25, 100, 50), "hp under") && ok;
    ok = expect(!latenthelpers::EvaluateHpUnderPercent(75, 100, 50), "hp under fail") && ok;
    ok = expect(latenthelpers::EvaluateHpOverPercent(75, 100, 50), "hp over") && ok;
    ok = expect(latenthelpers::EvaluateHpUnderTpUnder100(25, 100, 500, 50), "hp under tp under") && ok;
    ok = expect(!latenthelpers::EvaluateHpUnderTpUnder100(25, 100, 1000, 50), "hp under tp full") && ok;
    ok = expect(latenthelpers::EvaluateHpOverTpUnder100(75, 100, 500, 50), "hp over tp under") && ok;

    // MP / TP
    ok = expect(latenthelpers::EvaluateMpUnderPercent(20, 100, 25), "mp under %") && ok;
    ok = expect(!latenthelpers::EvaluateMpUnderPercent(20, 0, 25), "mp under % max0") && ok;
    ok = expect(latenthelpers::EvaluateMpUnder(10, 10), "mp under") && ok;
    ok = expect(latenthelpers::EvaluateMpOver(10, 10), "mp over gte") && ok;
    ok = expect(!latenthelpers::EvaluateMpStrictlyOver(10, 10), "mp strict over") && ok;
    ok = expect(latenthelpers::EvaluateTpUnder(999, 1000), "tp under") && ok;
    ok = expect(latenthelpers::EvaluateTpOver(1001, 1000), "tp over") && ok;

    // Jobs / weapon
    ok = expect(latenthelpers::EvaluateSubjob(5, 5), "subjob") && ok;
    ok = expect(latenthelpers::EvaluateMainjob(1, 1), "mainjob") && ok;
    ok = expect(latenthelpers::EvaluateWeaponDrawn(true), "drawn") && ok;
    ok = expect(latenthelpers::EvaluateWeaponSheathed(false), "sheathed") && ok;
    ok = expect(latenthelpers::EvaluateJobLevelBelow(49, 50), "lvl below") && ok;
    ok = expect(latenthelpers::EvaluateJobLevelAbove(50, 50), "lvl above") && ok;
    ok = expect(latenthelpers::EvaluateDuringWS(true), "during ws") && ok;
    ok = expect(!latenthelpers::EvaluateDuringWS(false), "outside ws") && ok;

    // Pet / status / food / party
    ok = expect(latenthelpers::EvaluatePetID(true, true, 7, 7), "pet id") && ok;
    ok = expect(latenthelpers::EvaluateStatusEffectActive(true), "status") && ok;
    ok = expect(latenthelpers::EvaluateNoFoodActive(false), "no food") && ok;
    ok = expect(latenthelpers::EvaluateFoodActive(true, true), "food") && ok;
    ok = expect(latenthelpers::EvaluatePartyMembers(3, 2, 1), "party") && ok;
    ok = expect(!latenthelpers::EvaluatePartyMembers(4, 2, 1), "party short") && ok;
    ok = expect(latenthelpers::EvaluatePartyMembersInZone(2, 2), "in zone") && ok;

    // Slot / drawn combos
    ok = expect(latenthelpers::EvaluateEquippedInSlot(4, 4), "slot") && ok;
    ok = expect(latenthelpers::EvaluateWeaponDrawnHPUnder(true, 25, 100, 50), "drawn hp") && ok;
    ok = expect(latenthelpers::EvaluateWeaponDrawnMPOver(true, 51, 50), "drawn mp") && ok;
    ok = expect(!latenthelpers::EvaluateWeaponDrawnMPOver(true, 50, 50), "drawn mp eq") && ok;

    // Process gates
    ok = expect(latenthelpers::ShouldRejectProcessLatent(true, false), "null owner") && ok;
    ok = expect(latenthelpers::ShouldRejectProcessLatent(false, true), "zone 0") && ok;
    ok = expect(!latenthelpers::ShouldRejectProcessLatent(false, false), "ok process") && ok;
    ok = expect(latenthelpers::ShouldApplyLatentExpression(true), "found") && ok;
    ok = expect(latenthelpers::ApplyLatentWantsActivate(true), "activate") && ok;
    ok = expect(!latenthelpers::ApplyLatentWantsActivate(false), "deactivate") && ok;
    ok = expect(latenthelpers::ProcessLatentListWantsHealthUpdate(true), "health") && ok;

    // Region bonuses / signet / avatar
    ok = expect(latenthelpers::EvaluateSanctionRegionHPUnder(true, 40, 100, 50), "sanction hp") && ok;
    ok = expect(latenthelpers::EvaluateSigilRegionMPUnder(true, 40, 100, 50), "sigil mp") && ok;
    ok = expect(latenthelpers::EvaluateSignetBonus(true, true, true), "signet") && ok;
    ok = expect(latenthelpers::EvaluateAvatarMatch(true, 5, 5), "avatar") && ok;
    ok = expect(latenthelpers::EvaluateAvatarMatch(true, 5, 21), "avatar any") && ok;
    ok = expect(!latenthelpers::EvaluateAvatarMatch(true, 25, 21), "avatar high id") && ok;

    return ok;
}
