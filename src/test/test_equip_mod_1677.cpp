#include "test_equip_mod_1677.h"

#include "map/equip_mod_capacity.h"

#include <iostream>

namespace
{
using namespace equipmodhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "equip mod 1677 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runEquipMod1677SelfTests() -> bool
{
    bool ok = true;

    // --- RemapModForSubSlot ---
    ok = expect(RemapModForSubSlot(Mod::MAIN_DMG_RANK) == Mod::SUB_DMG_RANK, "MAIN→SUB rank remap") && ok;
    ok = expect(RemapModForSubSlot(Mod::STR) == Mod::STR, "STR no remap") && ok;
    ok = expect(RemapModForSubSlot(Mod::SUB_DMG_RANK) == Mod::SUB_DMG_RANK, "SUB rank passthrough") && ok;
    ok = expect(RemapModForSubSlot(Mod::MAIN_DMG_RATING) == Mod::MAIN_DMG_RATING, "MAIN_DMG_RATING no remap") && ok;

    // --- Full-level SLOT_SUB MAIN_DMG_RANK remap ---
    {
        const auto plan = ApplyEquipModPlan(Mod::MAIN_DMG_RANK, 13, 75, 75, SlotSub);
        ok = expect(plan.targetMod == Mod::SUB_DMG_RANK && plan.amount == 13, "full sub MAIN_DMG_RANK") && ok;
    }
    {
        const auto plan = ApplyEquipModPlan(Mod::MAIN_DMG_RANK, 13, 75, 75, SlotMain);
        ok = expect(plan.targetMod == Mod::MAIN_DMG_RANK && plan.amount == 13, "full main MAIN_DMG_RANK") && ok;
    }
    {
        const auto plan = ApplyEquipModPlan(Mod::STR, 15, 99, 50, SlotSub);
        ok = expect(plan.targetMod == Mod::STR && plan.amount == 15, "full sub STR") && ok;
    }

    // --- Underlevel DEF family: mLevel*amount *3/4 / itemLevel ---
    // 50*100=5000, *3=15000, /4=3750, /75=50
    ok = expect(ScaleUnderlevelAmount(Mod::DEF, 100, 50, 75) == 50, "under DEF 100") && ok;
    // 50*40=2000, *3=6000, /4=1500, /75=20
    ok = expect(ScaleUnderlevelAmount(Mod::DEF, 40, 50, 75) == 20, "under DEF 40") && ok;
    // 1*20=20, *3=60, /4=15, /99=0
    ok = expect(ScaleUnderlevelAmount(Mod::MAIN_DMG_RATING, 20, 1, 99) == 0, "under MAIN_DMG_RATING") && ok;
    // 1*400=400, *3=1200, /4=300, /50=6
    ok = expect(ScaleUnderlevelAmount(Mod::SUB_DMG_RATING, 400, 1, 50) == 6, "under SUB_DMG_RATING") && ok;
    // 9*8=72, *3=216, /4=54, /10=5
    ok = expect(ScaleUnderlevelAmount(Mod::RANGED_DMG_RATING, 8, 9, 10) == 5, "under RANGED_DMG_RATING") && ok;

    // --- Underlevel HP/MP: mLevel*amount /2 / itemLevel ---
    // 50*100=5000, /2=2500, /75=33
    ok = expect(ScaleUnderlevelAmount(Mod::HP, 100, 50, 75) == 33, "under HP") && ok;
    // 74*200=14800, /2=7400, /75=98
    ok = expect(ScaleUnderlevelAmount(Mod::MP, 200, 74, 75) == 98, "under MP") && ok;
    // 1*99=99, /2=49, /50=0
    ok = expect(ScaleUnderlevelAmount(Mod::HP, 99, 1, 50) == 0, "under HP trunc") && ok;

    // --- Underlevel stats/combat: mLevel*amount /3 / itemLevel ---
    // 50*15=750, /3=250, /75=3
    ok = expect(ScaleUnderlevelAmount(Mod::STR, 15, 50, 75) == 3, "under STR") && ok;
    ok = expect(ScaleUnderlevelAmount(Mod::DEX, 15, 50, 75) == 3, "under DEX") && ok;
    ok = expect(ScaleUnderlevelAmount(Mod::ATT, 15, 50, 75) == 3, "under ATT") && ok;
    ok = expect(ScaleUnderlevelAmount(Mod::MACC, 15, 50, 75) == 3, "under MACC") && ok;
    // 1*300=300, /3=100, /50=2
    ok = expect(ScaleUnderlevelAmount(Mod::VIT, 300, 1, 50) == 2, "under VIT large") && ok;

    // --- Underlevel default → 0 ---
    ok = expect(ScaleUnderlevelAmount(Mod::MAIN_DMG_RANK, 13, 50, 75) == 0, "under MAIN_DMG_RANK zero") && ok;
    ok = expect(ScaleUnderlevelAmount(static_cast<Mod>(500), 250, 1, 99) == 0, "under default zero") && ok;

    // --- Integer division steps ---
    // 1*5=5, *3=15, /4=3, /10=0
    ok = expect(ScaleUnderlevelAmount(Mod::DEF, 5, 1, 10) == 0, "DEF step trunc") && ok;
    // 1*7=7, *3=21, /4=5, /5=1
    ok = expect(ScaleUnderlevelAmount(Mod::DEF, 7, 1, 5) == 1, "DEF step exact") && ok;
    // 1*8=8, /3=2, /2=1
    ok = expect(ScaleUnderlevelAmount(Mod::STR, 8, 1, 2) == 1, "STR step") && ok;

    // --- ApplyEquipModPlan underlevel families ---
    {
        const auto plan = ApplyEquipModPlan(Mod::DEF, 100, 50, 75, SlotMain);
        ok = expect(plan.targetMod == Mod::DEF && plan.amount == 50, "plan under DEF") && ok;
    }
    {
        const auto plan = ApplyEquipModPlan(Mod::HP, 100, 50, 75, SlotMain);
        ok = expect(plan.targetMod == Mod::HP && plan.amount == 33, "plan under HP") && ok;
    }
    {
        const auto plan = ApplyEquipModPlan(Mod::STR, 15, 50, 75, SlotMain);
        ok = expect(plan.targetMod == Mod::STR && plan.amount == 3, "plan under STR") && ok;
    }
    {
        const auto plan = ApplyEquipModPlan(Mod::MAIN_DMG_RANK, 13, 50, 75, SlotSub);
        ok = expect(plan.targetMod == Mod::SUB_DMG_RANK && plan.amount == 0, "plan under sub rank") && ok;
    }
    {
        const auto plan = ApplyEquipModPlan(Mod::STR, 15, 50, 75, SlotSub);
        ok = expect(plan.targetMod == Mod::STR && plan.amount == 3, "plan under sub STR") && ok;
    }

    // --- Boundary: mLevel == itemLevel uses full path ---
    {
        const auto plan = ApplyEquipModPlan(Mod::STR, 15, 75, 75, SlotMain);
        ok = expect(plan.amount == 15, "boundary full STR") && ok;
    }
    // mLevel == itemLevel-1 uses underlevel: 74*15=1110, /3=370, /75=4
    {
        const auto plan = ApplyEquipModPlan(Mod::STR, 15, 74, 75, SlotMain);
        ok = expect(plan.amount == 4, "boundary under STR") && ok;
    }

    // --- Distinct from GetScaledItemModifier (no mLevel mult there) ---
    // equip: 50; 0828-style without mLevel mult would be 100*3/4/75 = 1
    ok = expect(ScaleUnderlevelAmount(Mod::DEF, 100, 50, 75) == 50, "mLevel mult present") && ok;

    return ok;
}
