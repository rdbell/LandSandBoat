#include "test_weaponskill_state_1384.h"

#include "map/ai/states/weaponskill_state_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "weaponskill state 1384 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "weaponskill state 1384 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runWeaponSkillState1384SelfTests() -> bool
{
    using namespace weaponskillstatehelpers;
    bool ok = true;

    ok = expectEq(ResolveSpendCostPath(true, true), SpendCostPath::MeikyoShisui, "meikyo first") && ok;
    ok = expectEq(ResolveSpendCostPath(false, true), SpendCostPath::Sekkanoki, "sekkanoki") && ok;
    ok = expectEq(ResolveSpendCostPath(false, false), SpendCostPath::Normal, "normal") && ok;

    ok = expectEq(ResolveSpentTP(SpendCostPath::MeikyoShisui, 2500), static_cast<int16>(2500), "meikyo spent") && ok;
    ok = expectEq(ResolveSpentTP(SpendCostPath::Sekkanoki, 2500), static_cast<int16>(1000), "sek spent") && ok;
    ok = expectEq(ResolveSpentTP(SpendCostPath::Normal, 1800), static_cast<int16>(1800), "normal spent") && ok;

    ok = expectEq(ResolveTPDrain(SpendCostPath::MeikyoShisui, 2500, true), static_cast<int16>(1000), "meikyo drain") && ok;
    ok = expectEq(ResolveTPDrain(SpendCostPath::Sekkanoki, 1000, true), static_cast<int16>(1000), "sek drain") && ok;
    ok = expectEq(ResolveTPDrain(SpendCostPath::Normal, 1800, true), static_cast<int16>(1800), "normal drain") && ok;
    ok = expectEq(ResolveTPDrain(SpendCostPath::Normal, 1800, false), static_cast<int16>(0), "no deplete") && ok;

    // WS_NO_DEPLETE: mod <= roll → deplete. mod=0 always; mod=100 never for roll 0..99.
    ok = expect(ShouldDepleteNormalTP(0, 0) && ShouldDepleteNormalTP(0, 99), "deplete always mod0") && ok;
    ok = expect(!ShouldDepleteNormalTP(100, 0) && !ShouldDepleteNormalTP(100, 99), "never mod100") && ok;
    ok = expect(ShouldDepleteNormalTP(50, 50) && !ShouldDepleteNormalTP(50, 49), "deplete edge") && ok;

    ok = expect(ShouldDeleteSekkanoki(SpendCostPath::Sekkanoki) && !ShouldDeleteSekkanoki(SpendCostPath::MeikyoShisui), "del sek") && ok;

    ok = expect(ConserveTPProcs(25, 24) && !ConserveTPProcs(25, 25) && !ConserveTPProcs(0, 0), "conserve proc") && ok;
    ok = expectEq(ClampConserveTPRestore(5), static_cast<int16>(10), "clamp low") && ok;
    ok = expectEq(ClampConserveTPRestore(200), static_cast<int16>(199), "clamp high") && ok;
    ok = expectEq(ApplyConserveTPRestore(42), static_cast<int16>(42), "restore mid") && ok;

    // Full EvaluateSpendCost: Meikyo 3000 TP, drain 1000, conserve +50.
    {
        const auto r = EvaluateSpendCost(true, false, 3000, 0, 0, 100, 0, 50);
        ok = expectEq(r.spentTP, static_cast<int16>(3000), "eval meikyo spent") && ok;
        ok = expectEq(r.tpDrain, static_cast<int16>(1000), "eval meikyo drain") && ok;
        ok = expectEq(r.tpRestore, static_cast<int16>(50), "eval meikyo restore") && ok;
        ok = expect(!r.deleteSekkanoki, "eval meikyo no del") && ok;
    }
    // Sekkanoki: spent 1000, drain 1000, delete, no conserve (roll miss).
    {
        const auto r = EvaluateSpendCost(false, true, 2500, 0, 0, 50, 50, 10);
        ok = expectEq(r.spentTP, static_cast<int16>(1000), "eval sek spent") && ok;
        ok = expectEq(r.tpDrain, static_cast<int16>(1000), "eval sek drain") && ok;
        ok = expectEq(r.tpRestore, static_cast<int16>(0), "eval sek no conserve") && ok;
        ok = expect(r.deleteSekkanoki, "eval sek del") && ok;
    }
    // Normal no-deplete (mod 100), conserve restore 15.
    {
        const auto r = EvaluateSpendCost(false, false, 2000, 100, 0, 100, 0, 15);
        ok = expectEq(r.spentTP, static_cast<int16>(2000), "eval nd spent") && ok;
        ok = expectEq(r.tpDrain, static_cast<int16>(0), "eval nd drain") && ok;
        ok = expectEq(r.tpRestore, static_cast<int16>(15), "eval nd restore") && ok;
    }
    // Normal deplete full.
    {
        const auto r = EvaluateSpendCost(false, false, 1500, 0, 50, 0, 0, 10);
        ok = expectEq(r.spentTP, static_cast<int16>(1500), "eval dep spent") && ok;
        ok = expectEq(r.tpDrain, static_cast<int16>(1500), "eval dep drain") && ok;
        ok = expectEq(r.tpRestore, static_cast<int16>(0), "eval dep no restore") && ok;
    }

    ok = expect(IsSkillFinishCategory(3, 3) && !IsSkillFinishCategory(7, 3), "skill finish") && ok;
    ok = expectEq(ExtractWeaponskillDamage(0x12ABCDEF), static_cast<uint32>(0x00ABCDEF), "ws dmg mask") && ok;
    ok = expectEq(ExtractWeaponskillDamage(0xFFFFFFFF), static_cast<uint32>(0x00FFFFFF), "ws dmg full") && ok;

    ok = expect(ShouldExecuteWeaponskill(true, false) && !ShouldExecuteWeaponskill(true, true), "execute") && ok;
    ok = expect(!ShouldExecuteWeaponskill(false, false), "dead") && ok;
    ok = expect(ShouldExitWeaponskill(true) && !ShouldExitWeaponskill(false), "exit") && ok;
    ok = expect(ShouldResetRestraint(true) && !ShouldResetRestraint(false), "restraint") && ok;
    ok = expect(ShouldCountPCHistoryWS(true) && !ShouldCountPCHistoryWS(false), "pc hist") && ok;

    ok = expectEq(ResolveWeaponskillTargetFlags(true, 0x01, 0x20), static_cast<uint16>(0x01), "self flags") && ok;
    ok = expectEq(ResolveWeaponskillTargetFlags(false, 0x01, 0x20), static_cast<uint16>(0x20), "enemy flags") && ok;

    ok = expectEq(SekkanokiSpentTP, static_cast<int16>(1000), "pin sek") && ok;
    ok = expectEq(WeaponskillDamageMask, static_cast<uint32>(0x00FFFFFFu), "pin mask") && ok;

    return ok;
}
