/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_enmity_container.h"

#include "map/enmity_container.h"

#include <iostream>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "enmity container self-test failed: " << label << " got "
                  << static_cast<long long>(actual) << " expected " << static_cast<long long>(expected) << '\n';
        return false;
    }
    return true;
}

auto testClampAndCapAmount() -> bool
{
    constexpr int32 cap = 1000;
    bool          ok    = true;
    ok = expectEqual(enmitymath::ClampEnmity(-5, cap), 0, "clamp floors at 0") && ok;
    ok = expectEqual(enmitymath::ClampEnmity(500, cap), 500, "clamp mid") && ok;
    ok = expectEqual(enmitymath::ClampEnmity(1500, cap), cap, "clamp ceilings at cap") && ok;
    // SetCE/SetVE path: min(amount, cap) only — negative allowed
    ok = expectEqual(enmitymath::CapAmount(1500, cap), cap, "cap amount above") && ok;
    ok = expectEqual(enmitymath::CapAmount(-50, cap), -50, "cap amount negative allowed") && ok;
    ok = expectEqual(enmitymath::CapAmount(cap, cap), cap, "cap amount exact") && ok;
    return ok;
}

auto testApplyDeltaBonusOnlyOnPositive() -> bool
{
    bool ok = true;
    // positive: current + delta * bonus
    ok = expectEqual(enmitymath::ApplyDelta(1000, 100, 1.5f), 1150, "positive delta * bonus") && ok;
    // zero and negative: raw add, no bonus
    ok = expectEqual(enmitymath::ApplyDelta(1000, 0, 2.0f), 1000, "zero delta ignores bonus") && ok;
    ok = expectEqual(enmitymath::ApplyDelta(1000, -100, 2.0f), 900, "negative delta ignores bonus") && ok;
    return ok;
}

auto testApplyNewEntryAxisInitialBoostAndBonus() -> bool
{
    constexpr int32 cap = 30000;
    bool            ok  = true;

    // Mirrors new-entry path: optional +200/+900 then value*bonus clamp
    int32 ce = 100 + 200;
    int32 ve = 50 + 900;
    ok       = expectEqual(enmitymath::ApplyNewEntryAxis(ce, 1.0f, cap), 300, "initial CE boost") && ok;
    ok       = expectEqual(enmitymath::ApplyNewEntryAxis(ve, 1.0f, cap), 950, "initial VE boost") && ok;

    ce = 10 + 200;
    ve = 20 + 900;
    ok = expectEqual(enmitymath::ApplyNewEntryAxis(ce, 1.5f, cap), 315, "boosted CE * bonus") && ok;
    ok = expectEqual(enmitymath::ApplyNewEntryAxis(ve, 1.5f, cap), 1380, "boosted VE * bonus") && ok;

    ok = expectEqual(enmitymath::ApplyNewEntryAxis(200, 1.0f, 100), 100, "new entry clamp to cap") && ok;
    return ok;
}

auto testLowerByPercent() -> bool
{
    bool ok = true;
    ok      = expectEqual(enmitymath::LowerByPercent(1000, 25), 750, "lower CE 25%") && ok;
    ok      = expectEqual(enmitymath::LowerByPercent(500, 25), 375, "lower VE 25%") && ok;
    ok      = expectEqual(enmitymath::LowerByPercent(1000, 100), 0, "lower 100%") && ok;
    ok      = expectEqual(enmitymath::LowerByPercent(1000, 0), 1000, "lower 0%") && ok;
    return ok;
}

auto testDecayVE() -> bool
{
    bool ok = true;
    ok      = expectEqual(enmitymath::VEDecayAmount(), static_cast<int32>(24), "decay amount 60/2.5") && ok;
    ok      = expectEqual(enmitymath::DecayVE(100), 76, "decay above amount") && ok;
    ok      = expectEqual(enmitymath::DecayVE(10), 0, "decay below amount floors at 0") && ok;
    ok      = expectEqual(enmitymath::DecayVE(24), 0, "decay equal amount floors at 0") && ok;
    ok      = expectEqual(enmitymath::DecayVE(0), 0, "decay zero stays zero") && ok;
    return ok;
}

auto testExistingEntryClampComposition() -> bool
{
    constexpr int32 cap = 500;
    bool            ok  = true;
    // existing: apply delta then clamp — mirrors UpdateEnmity existing path
    auto ce = enmitymath::ClampEnmity(enmitymath::ApplyDelta(400, 200, 1.0f), cap);
    auto ve = enmitymath::ClampEnmity(enmitymath::ApplyDelta(400, 200, 1.0f), cap);
    ok      = expectEqual(ce, 500, "existing CE clamp after positive delta") && ok;
    ok      = expectEqual(ve, 500, "existing VE clamp after positive delta") && ok;

    ce = enmitymath::ClampEnmity(enmitymath::ApplyDelta(100, -1000, 1.0f), cap);
    ok = expectEqual(ce, 0, "existing CE clamp after large negative") && ok;
    return ok;
}

auto expectBool(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity container self-test failed: " << label << '\n';
    }
    return condition;
}

auto testUpdateEnmityAdmission1357() -> bool
{
    bool ok = true;

    ok = expectBool(enmitymath::ShouldRejectNonMobHolder(false), "reject non-mob") && ok;
    ok = expectBool(!enmitymath::ShouldRejectNonMobHolder(true), "accept mob") && ok;

    ok = expectEqual(static_cast<int>(enmitymath::EnmityRangeNormal * 10), 250, "range normal") && ok;
    ok = expectEqual(static_cast<int>(enmitymath::EnmityRangeNotorious * 10), 280, "range nm") && ok;
    ok = expectBool(enmitymath::EnmityRangeMax(true) == enmitymath::EnmityRangeNotorious, "max nm") && ok;
    ok = expectBool(enmitymath::EnmityRangeMax(false) == enmitymath::EnmityRangeNormal, "max normal") && ok;
    ok = expectBool(enmitymath::IsWithinEnmityRangePure(true, true), "in range") && ok;
    ok = expectBool(!enmitymath::IsWithinEnmityRangePure(false, true), "cross zone") && ok;
    ok = expectBool(enmitymath::ShouldZeroEnmityOutOfRange(false), "zero oor") && ok;

    ok = expectEqual(enmitymath::CapTreasureHunterLevel(10, true), static_cast<int16>(8), "th main cap") && ok;
    ok = expectEqual(enmitymath::CapTreasureHunterLevel(10, false), static_cast<int16>(4), "th non-main cap") && ok;
    ok = expectEqual(enmitymath::CapTreasureHunterLevel(3, false), static_cast<int16>(3), "th below cap") && ok;
    ok = expectBool(enmitymath::ShouldApplyDirectActionTH(true), "direct th") && ok;
    ok = expectBool(enmitymath::ShouldRaiseHolderTH(2, 4), "raise th") && ok;
    ok = expectBool(!enmitymath::ShouldRaiseHolderTH(4, 4), "no raise th") && ok;
    ok = expectBool(enmitymath::ShouldRaiseHolderGilfinder(1, 3), "raise gf") && ok;

    ok = expectBool(enmitymath::ShouldRebindEnmityOwner(false), "rebind") && ok;
    ok = expectBool(!enmitymath::ShouldRebindEnmityOwner(true), "no rebind") && ok;
    ok = expectBool(enmitymath::ShouldActivateEnmityEntry(0, 0), "activate zero") && ok;
    ok = expectBool(!enmitymath::ShouldActivateEnmityEntry(-1, 0), "no activate") && ok;
    ok = expectBool(enmitymath::ShouldCreateNewEnmityEntry(false, 1, 1), "create") && ok;
    ok = expectBool(!enmitymath::ShouldCreateNewEnmityEntry(true, 1, 1), "no create exists") && ok;
    ok = expectBool(enmitymath::ShouldApplyInitialEnmityBoost(false), "initial boost") && ok;
    ok = expectBool(!enmitymath::ShouldApplyInitialEnmityBoost(true), "no initial") && ok;
    ok = expectEqual(enmitymath::InitialCEBoost, static_cast<int32>(200), "ce boost") && ok;
    ok = expectEqual(enmitymath::InitialVEBoost, static_cast<int32>(900), "ve boost") && ok;

    ok = expectBool(enmitymath::ShouldAddMasterBaseEnmity(true, true, true, false), "master pet") && ok;
    ok = expectBool(enmitymath::ShouldAddMasterBaseEnmity(true, true, false, true), "master charm") && ok;
    ok = expectBool(!enmitymath::ShouldAddMasterBaseEnmity(true, true, false, false), "master other") && ok;
    ok = expectBool(enmitymath::ShouldMarkNotTameable(false), "not tameable") && ok;

    // bonus factor: 0 → 1.0, +50 → 1.5, -50 → 0.5, clamp +100 → 2.0, clamp -60 → 0.5
    ok = expectBool(enmitymath::CalculateEnmityBonusFactor(0) == 1.0f, "bonus 0") && ok;
    ok = expectBool(enmitymath::CalculateEnmityBonusFactor(50) == 1.5f, "bonus 50") && ok;
    ok = expectBool(enmitymath::CalculateEnmityBonusFactor(-50) == 0.5f, "bonus -50") && ok;
    ok = expectBool(enmitymath::CalculateEnmityBonusFactor(100) == 2.0f, "bonus 100") && ok;
    ok = expectBool(enmitymath::CalculateEnmityBonusFactor(200) == 2.0f, "bonus clamp high") && ok;
    ok = expectBool(enmitymath::CalculateEnmityBonusFactor(-60) == 0.5f, "bonus clamp low") && ok;

    ok = expectBool(enmitymath::ShouldSkipDamageEnmitySelf(true), "self dmg") && ok;
    ok = expectEqual(enmitymath::FloorDamageForEnmity(0), static_cast<int32>(1), "floor 0") && ok;
    ok = expectEqual(enmitymath::FloorDamageForEnmity(5), static_cast<int32>(5), "floor 5") && ok;

    ok = expectBool(enmitymath::AttackEnmityLossReduction(0) == 1.0f, "loss red 0") && ok;
    ok = expectBool(enmitymath::AttackEnmityLossReduction(50) == 0.5f, "loss red 50") && ok;
    ok = expectBool(enmitymath::AttackEnmityLossReduction(100) == 0.0f, "loss red 100") && ok;
    // -1800 * 100 / 1000 * 1.0 = -180
    ok = expectEqual(enmitymath::AttackEnmityCEDelta(100, 1000, 1.0f), static_cast<int32>(-180), "attack ce") && ok;
    ok = expectEqual(enmitymath::AttackEnmityCEDelta(100, 0, 1.0f), static_cast<int32>(0), "attack maxhp0") && ok;

    ok = expectBool(enmitymath::ShouldApplyCoverEnmity(true, true), "cover") && ok;
    ok = expectEqual(enmitymath::CoverUserNewCE(50), static_cast<int32>(250), "cover ce") && ok;
    ok = expectEqual(enmitymath::CoverEnmityLowerPercent, static_cast<uint8>(10), "cover pct") && ok;

    ok = expectBool(enmitymath::ShouldRaiseHiPCLvl(50, 75), "hi pc") && ok;
    ok = expectBool(enmitymath::ShouldSkipHighestEnmitySameAllegiance(true, true), "skip ally") && ok;
    ok = expectBool(!enmitymath::ShouldSkipHighestEnmitySameAllegiance(false, true), "null owner ok") && ok;
    ok = expectBool(enmitymath::ShouldPreferCurrentBattleTargetOnTie(true, true, true, true), "tie keep") && ok;
    ok = expectBool(enmitymath::ShouldPruneHighestEnmity(false, true, true, false), "prune null") && ok;
    ok = expectBool(enmitymath::ShouldPruneHighestEnmity(true, false, true, false), "prune zone") && ok;
    ok = expectBool(enmitymath::ShouldPruneHighestEnmity(true, true, true, true), "prune dead") && ok;
    ok = expectBool(!enmitymath::ShouldPruneHighestEnmity(true, true, true, false), "keep valid") && ok;
    ok = expectBool(enmitymath::ShouldAddBaseEnmitySameZone(true), "base zone") && ok;

    return ok;
}

} // namespace

auto runEnmityContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testClampAndCapAmount() && ok;
    ok      = testApplyDeltaBonusOnlyOnPositive() && ok;
    ok      = testApplyNewEntryAxisInitialBoostAndBonus() && ok;
    ok      = testLowerByPercent() && ok;
    ok      = testDecayVE() && ok;
    ok      = testExistingEntryClampComposition() && ok;
    ok      = testUpdateEnmityAdmission1357() && ok;
    return ok;
}
