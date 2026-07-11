#include "test_status_effect_tick_1366.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect tick 1366 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "status effect tick 1366 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runStatusEffectTick1366SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    // Expiry / tick due
    ok = expect(ShouldExpireEffect(true, 100, 100), "expire at") && ok;
    ok = expect(ShouldExpireEffect(true, 99, 100), "expire past") && ok;
    ok = expect(!ShouldExpireEffect(true, 101, 100), "not expire") && ok;
    ok = expect(!ShouldExpireEffect(false, 0, 100), "perm no expire") && ok;
    ok = expect(ShouldTickEffect(true, 0, 1), "tick due") && ok;
    ok = expect(!ShouldTickEffect(true, 1, 1), "tick not due") && ok;
    ok = expect(!ShouldTickEffect(false, 0, 5), "no period") && ok;

    // Aura
    ok = expectEq(ComputeAuraRange(0), 6.0f, "aura base") && ok;
    ok = expectEq(ComputeAuraRange(100), 7.0f, "aura +1") && ok;
    ok = expect(IsWithinAuraRange(6.5f, 6.0f, 1.0f), "in range") && ok;
    ok = expect(!IsWithinAuraRange(7.1f, 6.0f, 1.0f), "out range") && ok;
    ok = expect(ShouldUseMasterForAura(true, false) && ShouldUseMasterForAura(false, true), "master") && ok;
    ok = expect(!ShouldUseMasterForAura(false, false), "no master") && ok;
    ok = expectEq(ResolveAuraEffectIcon(0, 42), static_cast<uint16>(42), "icon fallback") && ok;
    ok = expectEq(ResolveAuraEffectIcon(9, 42), static_cast<uint16>(9), "icon sub") && ok;
    ok = expect(ShouldRefreshAlwaysExpiringAura(true, true), "refresh") && ok;
    ok = expect(!ShouldRefreshAlwaysExpiringAura(true, false), "no refresh") && ok;
    ok = expect(ShouldUpdateAuraPower(1, 2) && !ShouldUpdateAuraPower(2, 2), "power") && ok;

    // Eleven roll
    ok = expect(IsElevenRollEffect(ElevenRollIDFirst, 11, ElevenRollIDFirst, ElevenRollIDLast, RuneistsRollID), "fighters 11") && ok;
    ok = expect(IsElevenRollEffect(ElevenRollIDLast, 11, ElevenRollIDFirst, ElevenRollIDLast, RuneistsRollID), "naturalists 11") && ok;
    ok = expect(IsElevenRollEffect(RuneistsRollID, 11, ElevenRollIDFirst, ElevenRollIDLast, RuneistsRollID), "runeists 11") && ok;
    ok = expect(!IsElevenRollEffect(ElevenRollIDFirst, 10, ElevenRollIDFirst, ElevenRollIDLast, RuneistsRollID), "not 11") && ok;
    ok = expect(!IsElevenRollEffect(400, 11, ElevenRollIDFirst, ElevenRollIDLast, RuneistsRollID), "not roll") && ok;

    // Sleep / regain / despawn
    ok = expect(ShouldBreakSleepFromRegenDown(false, 0), "no sleep") && ok;
    ok = expect(ShouldBreakSleepFromRegenDown(true, 3), "sleep low tier") && ok;
    ok = expect(!ShouldBreakSleepFromRegenDown(true, 4), "nightmare") && ok;
    ok = expect(ShouldApplyRegainTP(false, false) && ShouldApplyRegainTP(true, true), "regain") && ok;
    ok = expect(!ShouldApplyRegainTP(true, false), "mob idle no regain") && ok;
    ok = expect(ShouldDespawnAvatarOnZeroMP(true, true, true), "despawn") && ok;
    ok = expect(!ShouldDespawnAvatarOnZeroMP(false, true, true), "mp left") && ok;

    // Perpetuation
    ok = expect(ShouldApplyAvatarPerpetuationPath(5, true) && !ShouldApplyAvatarPerpetuationPath(0, true), "perp path") && ok;
    ok = expectEq(ApplyHalfPerpetuation(5, true), static_cast<int16>(2), "half floor") && ok;
    ok = expectEq(ApplyHalfPerpetuation(5, false), static_cast<int16>(5), "no half") && ok;
    ok = expectEq(AdjustPerpetuationAfterHalf(10, 2, 1, 1, 1), static_cast<int16>(5), "adjust") && ok;
    ok = expectEq(ApplyAvatarFavorPerpetuation(10, true), static_cast<int16>(12), "favor 1.2") && ok;
    ok = expectEq(ApplyAvatarFavorPerpetuation(5, true), static_cast<int16>(6), "favor floor") && ok; // 5*1.2=6.0
    ok = expectEq(FinalizePerpetuationCost(5, true), static_cast<int16>(0), "astral") && ok;
    ok = expectEq(FinalizePerpetuationCost(0, false), static_cast<int16>(1), "min 1") && ok;
    ok = expectEq(FinalizePerpetuationCost(3, false), static_cast<int16>(3), "keep") && ok;
    ok = expect(IsPetElementValid(1, 1, 8) && !IsPetElementValid(0, 1, 8), "element") && ok;
    ok = expect(WeatherMatchesPetStrong(4, 4) && WeatherMatchesPetStrong(5, 4), "weather") && ok;
    ok = expect(!WeatherMatchesPetStrong(6, 4), "weather miss") && ok;

    return ok;
}
