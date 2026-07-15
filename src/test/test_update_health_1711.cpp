#include "test_update_health_1711.h"

#include "map/update_health_capacity.h"

#include <iostream>

namespace
{
using namespace updatehealthhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "update health 1711 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runUpdateHealth1711SelfTests() -> bool
{
    bool ok = true;

    // --- cap pins ---
    ok = expect(PCMaxHP == 9999, "PCMaxHP pin") && ok;
    ok = expect(PCMaxMP == 9999, "PCMaxMP pin") && ok;

    // --- FloorFloat32 / FoodBonus helpers ---
    ok = expect(FloorFloat32(100.9f) == 100, "FloorFloat32(100.9)") && ok;
    ok = expect(FloorFloat32(static_cast<float>(16777217)) == 16777216, "FloorFloat32 large float32") && ok;
    ok = expect(FoodBonus(1100, 10, 50) == 50, "FoodBonus 10% cap 50") && ok;
    ok = expect(FoodBonus(1100, 10, 200) == 110, "FoodBonus 10% no cap") && ok;
    ok = expect(FoodBonus(7000, 1000, 30000) == 4464, "FoodBonus int16 narrow") && ok;
    ok = expect(FoodBonus(1000, 0, 50) == 0, "FoodBonus zero percent") && ok;

    // --- full modifier formula (host: MaxHP 1293 / MaxMP 753) ---
    {
        Params p{};
        p.BaseHP     = 1000;
        p.BaseMP     = 500;
        p.BaseHPMod  = 100;
        p.BaseMPMod  = 50;
        p.HPP        = 10;
        p.MPP        = 20;
        p.FoodHP     = 30;
        p.FoodMP     = 15;
        p.FoodHPP    = 10;
        p.FoodHPCap  = 50;
        p.FoodMPP    = 20;
        p.FoodMPCap  = 75;
        p.CurrentHP  = 1000;
        p.CurrentMP  = 500;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 1293, "formula EffectiveMaxHP 1293") && ok;
        ok           = expect(r.EffectiveMaxMP == 753, "formula EffectiveMaxMP 753") && ok;
        ok           = expect(r.ClampedHP == 1000, "formula ClampedHP 1000") && ok;
        ok           = expect(r.ClampedMP == 500, "formula ClampedMP 500") && ok;
    }

    // --- HP/MP conversion ---
    {
        Params p{};
        p.BaseHP     = 100;
        p.BaseMP     = 50;
        p.ConvMPToHP = 20;
        p.CurrentHP  = 100;
        p.CurrentMP  = 50;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 120, "MP-to-HP maxhp 120") && ok;
        ok           = expect(r.EffectiveMaxMP == 30, "MP-to-HP maxmp 30") && ok;
    }
    {
        Params p{};
        p.BaseHP     = 100;
        p.BaseMP     = 50;
        p.ConvHPToMP = 20;
        p.CurrentHP  = 100;
        p.CurrentMP  = 50;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 80, "HP-to-MP maxhp 80") && ok;
        ok           = expect(r.EffectiveMaxMP == 70, "HP-to-MP maxmp 70") && ok;
    }

    // --- weakness + curse ---
    {
        Params p{};
        p.BaseHP      = 100;
        p.BaseMP      = 50;
        p.WeaknessPct = -25;
        p.CursePct    = -20;
        const auto r  = Resolve(p);
        ok            = expect(r.EffectiveMaxHP == 60, "weak/curse maxhp 60") && ok;
        ok            = expect(r.EffectiveMaxMP == 29, "weak/curse maxmp 29") && ok;
    }

    // --- reduced maxima clamp currents ---
    {
        Params p{};
        p.BaseHP    = 1000;
        p.BaseMP    = 500;
        p.CursePct  = -50;
        p.CurrentHP = 1000;
        p.CurrentMP = 500;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 500, "reduced maxhp 500") && ok;
        ok           = expect(r.ClampedHP == 500, "reduced clamped hp 500") && ok;
        ok           = expect(r.EffectiveMaxMP == 250, "reduced maxmp 250") && ok;
        ok           = expect(r.ClampedMP == 250, "reduced clamped mp 250") && ok;
    }

    // --- PC ceiling ---
    {
        Params p{};
        p.BaseHP    = 20000;
        p.BaseHPMod = 1000;
        p.IsPC      = true;
        p.CurrentHP = 20000;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 9999, "PC maxhp 9999") && ok;
        ok           = expect(r.ClampedHP == 9999, "PC clamped hp 9999") && ok;
    }

    // --- float32 large + food int16 narrow ---
    {
        Params p{};
        p.BaseHP     = 16777217;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 16777216, "large float32 maxhp") && ok;
    }
    {
        Params p{};
        p.BaseHP    = 7000;
        p.FoodHPP   = 1000;
        p.FoodHPCap = 30000;
        const auto r = Resolve(p);
        ok           = expect(r.EffectiveMaxHP == 11464, "food int16 narrow maxhp 11464") && ok;
    }

    // --- fully weakened non-PC may be 0; PC floor is 1 ---
    {
        Params p{};
        p.BaseHP      = 100;
        p.CurrentHP   = 100;
        p.WeaknessPct = -100;
        const auto r  = Resolve(p);
        ok            = expect(r.EffectiveMaxHP == 0, "fully weakened non-PC maxhp 0") && ok;
        ok            = expect(r.ClampedHP == 0, "fully weakened non-PC clamped 0") && ok;
    }
    {
        Params p{};
        p.BaseHP      = 100;
        p.CurrentHP   = 100;
        p.WeaknessPct = -100;
        p.IsPC        = true;
        const auto r  = Resolve(p);
        ok            = expect(r.EffectiveMaxHP == 1, "fully weakened PC maxhp 1") && ok;
        ok            = expect(r.ClampedHP == 1, "fully weakened PC clamped 1") && ok;
    }

    return ok;
}
