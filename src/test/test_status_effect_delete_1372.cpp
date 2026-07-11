#include "test_status_effect_delete_1372.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect delete 1372 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectDelete1372SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(MatchesActiveStatusID(3, 3, false) && !MatchesActiveStatusID(3, 3, true), "active id") && ok;
    ok = expect(MatchesDelBySubID(3, 3, 9, 9, false) && !MatchesDelBySubID(3, 3, 9, 8, false), "sub") && ok;
    ok = expect(MatchesDelBySource(3, 3, 1, 1, 42, 42, false) && !MatchesDelBySource(3, 3, 1, 1, 42, 7, false), "source") && ok;
    ok = expect(MatchesDelByTier(3, 3, 2, 2, false) && !MatchesDelByTier(3, 3, 2, 1, false), "tier") && ok;
    ok = expect(ShouldKillTimedEffect(true) && !ShouldKillTimedEffect(false), "kill timed") && ok;
    ok = expect(IsCharmStatusID(StatusIDCharmI) && IsCharmStatusID(StatusIDCharmIi), "charm ids") && ok;
    ok = expect(ShouldDespawnPetOnCharm(true, true, true) && !ShouldDespawnPetOnCharm(true, true, false), "despawn pet") && ok;
    ok = expect(ShouldRewriteSleepIcon(true, StatusIDSleepIi) && ShouldRewriteSleepIcon(true, StatusIDLullaby), "rewrite sleep") && ok;
    ok = expect(!ShouldRewriteSleepIcon(false, StatusIDSleepIi) && !ShouldRewriteSleepIcon(true, StatusIDStun), "no rewrite") && ok;
    ok = expect(CanClientCancelIcon(5, 5, false) && !CanClientCancelIcon(5, 5, true), "cancel icon") && ok;
    ok = expect(MatchesEffectType(7, 7) && !MatchesEffectType(0, 0), "type") && ok;
    ok = expect(MatchesFlagForDelete(true) && !MatchesFlagForDelete(false), "flag") && ok;
    ok = expect(ShouldSkipNightmareSleepOnDamageFlag(true, StatusIDSleepI, 5), "nm sleep") && ok;
    ok = expect(!ShouldSkipNightmareSleepOnDamageFlag(true, StatusIDSleepI, 4), "nm low tier") && ok;
    ok = expect(!ShouldSkipNightmareSleepOnDamageFlag(false, StatusIDSleepI, 5), "nm no damage") && ok;
    ok = expect(ShouldRejectZeroEffectType(0) && !ShouldRejectZeroEffectType(1), "zero type") && ok;
    ok = expect(ShouldRunGainSideEffects(true) && !ShouldRunGainSideEffects(false), "side alive") && ok;

    return ok;
}
