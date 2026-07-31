#include "test_status_effect_can_gain_1364.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect can gain 1364 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectCanGain1364SelfTests() -> bool
{
    bool ok = true;

    // Overwrite modes
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteAlways, 0, 0, 1, 100), "always") && ok;
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteIgnoreDuplicate, 0, 0, 1, 100), "ignore") && ok;
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteNever, 2, 200, 1, 100), "never") && ok;

    // EqualHigher: tier then power
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteEqualHigher, 2, 0, 2, 0), "eq higher tier eq") && ok;
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteEqualHigher, 3, 0, 2, 0), "eq higher tier gt") && ok;
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteEqualHigher, 1, 0, 2, 0), "eq higher tier lt") && ok;
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteEqualHigher, 0, 50, 0, 50), "eq higher power eq") && ok;
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteEqualHigher, 0, 49, 0, 50), "eq higher power lt") && ok;

    // Higher strict
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteHigher, 2, 0, 2, 0), "higher tier eq") && ok;
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteHigher, 3, 0, 2, 0), "higher tier gt") && ok;
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteHigher, 0, 51, 0, 50), "higher power gt") && ok;
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteHigher, 0, 50, 0, 50), "higher power eq") && ok;

    // TierHigher only when both tiers set
    ok = expect(statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteTierHigher, 3, 1, 2, 99), "tier higher") && ok;
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteTierHigher, 2, 99, 2, 1), "tier higher eq") && ok;
    ok = expect(!statuseffecthelpers::CanGainOverwrite(statuseffecthelpers::OverwriteTierHigher, 0, 99, 0, 1), "tier higher no tier") && ok;

    // Negative competition
    ok = expect(statuseffecthelpers::CanGainVsNegative(0, 100, 0, 50, false, false), "neg power") && ok;
    ok = expect(!statuseffecthelpers::CanGainVsNegative(0, 40, 0, 50, false, false), "neg power fail") && ok;
    ok = expect(statuseffecthelpers::CanGainVsNegative(3, 0, 2, 0, true, false), "neg tier gt") && ok;
    ok = expect(statuseffecthelpers::CanGainVsNegative(2, 0, 2, 0, true, true), "neg tier eq status gt") && ok;
    ok = expect(!statuseffecthelpers::CanGainVsNegative(2, 0, 2, 0, true, false), "neg tier eq status le") && ok;

    // Gates
    ok = expect(statuseffecthelpers::ShouldBlockCharmOnPet(true, true), "charm pet") && ok;
    ok = expect(!statuseffecthelpers::ShouldBlockCharmOnPet(true, false), "charm no master") && ok;
    ok = expect(statuseffecthelpers::ShouldBlockByBlockId(5, true), "block id") && ok;
    ok = expect(!statuseffecthelpers::ShouldBlockByBlockId(5, false), "block id absent") && ok;
    ok = expect(!statuseffecthelpers::ShouldBlockByBlockId(0, true), "block id zero") && ok;
    ok = expect(statuseffecthelpers::ShouldBlockSpikesDueToAftermath(true, 8), "aftermath 8") && ok;
    ok = expect(statuseffecthelpers::ShouldBlockSpikesDueToAftermath(true, 22), "aftermath 22") && ok;
    ok = expect(!statuseffecthelpers::ShouldBlockSpikesDueToAftermath(true, 7), "aftermath other") && ok;
    ok = expect(statuseffecthelpers::IsHasteVsSlowRemote(true, true, 1), "haste remote") && ok;
    ok = expect(!statuseffecthelpers::IsHasteVsSlowRemote(true, true, 0), "not remote") && ok;
    ok = expect(statuseffecthelpers::ShouldDeleteOnOverwrite(statuseffecthelpers::OverwriteAlways), "delete overwrite") && ok;
    ok = expect(!statuseffecthelpers::ShouldDeleteOnOverwrite(statuseffecthelpers::OverwriteIgnoreDuplicate), "keep ignore") && ok;
    ok = expect(statuseffecthelpers::ShouldRemoveLinkedId(10, 0), "remove linked") && ok;
    ok = expect(!statuseffecthelpers::ShouldRemoveLinkedId(9, 10), "linked below ko") && ok;
    ok = expect(!statuseffecthelpers::ShouldRemoveLinkedId(10, 10), "linked equal ko") && ok;
    ok = expect(!statuseffecthelpers::ShouldRemoveLinkedId(0, 0), "no remove ko") && ok;
    ok = expect(statuseffecthelpers::ShouldRejectSleepImmunity(true, true, false, true, false), "sleep light") && ok;
    ok = expect(statuseffecthelpers::ShouldRejectSleepImmunity(true, false, true, false, true), "sleep dark") && ok;
    ok = expect(!statuseffecthelpers::ShouldRejectSleepImmunity(true, true, false, false, false), "sleep no immune") && ok;
    ok = expect(statuseffecthelpers::CanGainWhenNoExisting(), "no existing") && ok;

    return ok;
}
