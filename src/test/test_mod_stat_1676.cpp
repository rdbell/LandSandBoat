#include "test_mod_stat_1676.h"

#include "map/mod_stat_capacity.h"

#include <iostream>

namespace
{
using namespace modstathelpers;

// Non-NONE sentinel (Mod::DEF = 1).
constexpr std::uint16_t ModDEF = 1;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "mod_stat 1676 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runModStat1676SelfTests() -> bool
{
    bool ok = true;

    // --- ModNone pin ---
    ok = expect(ModNone == 0, "ModNone == 0") && ok;

    // --- ShouldApplyMod ---
    ok = expect(!ShouldApplyMod(ModNone), "ShouldApply NONE false") && ok;
    ok = expect(ShouldApplyMod(ModDEF), "ShouldApply DEF true") && ok;
    ok = expect(ShouldApplyMod(2), "ShouldApply 2 true") && ok;

    // --- ApplyAdd NONE no-op ---
    ok = expect(ApplyAdd(10, 5, ModNone) == 10, "Add NONE no-op") && ok;
    ok = expect(ApplyAdd(10, -5, ModNone) == 10, "Add NONE negative amount no-op") && ok;
    ok = expect(ApplyAdd(-3, 7, ModNone) == -3, "Add NONE negative current no-op") && ok;

    // --- ApplyAdd arithmetic ---
    ok = expect(ApplyAdd(0, 0, ModDEF) == 0, "Add zero") && ok;
    ok = expect(ApplyAdd(10, 5, ModDEF) == 15, "Add positive") && ok;
    ok = expect(ApplyAdd(10, -3, ModDEF) == 7, "Add negative amount") && ok;
    ok = expect(ApplyAdd(-5, 8, ModDEF) == 3, "Add negative current") && ok;
    ok = expect(ApplyAdd(-10, -4, ModDEF) == -14, "Add both negative") && ok;
    ok = expect(ApplyAdd(50, -50, ModDEF) == 0, "Add cancel to zero") && ok;

    // --- ApplySet NONE no-op ---
    ok = expect(ApplySet(10, 99, ModNone) == 10, "Set NONE no-op") && ok;
    ok = expect(ApplySet(-3, 7, ModNone) == -3, "Set NONE negative current no-op") && ok;

    // --- ApplySet arithmetic ---
    ok = expect(ApplySet(10, 0, ModDEF) == 0, "Set zero") && ok;
    ok = expect(ApplySet(10, 42, ModDEF) == 42, "Set positive") && ok;
    ok = expect(ApplySet(10, -7, ModDEF) == -7, "Set negative") && ok;
    ok = expect(ApplySet(-99, 5, ModDEF) == 5, "Set overwrite") && ok;
    ok = expect(ApplySet(999, 1, ModDEF) == 1, "Set ignores current") && ok;

    // --- ApplyDel NONE no-op ---
    ok = expect(ApplyDel(10, 5, ModNone) == 10, "Del NONE no-op") && ok;
    ok = expect(ApplyDel(10, -5, ModNone) == 10, "Del NONE negative amount no-op") && ok;
    ok = expect(ApplyDel(-3, 7, ModNone) == -3, "Del NONE negative current no-op") && ok;

    // --- ApplyDel arithmetic ---
    ok = expect(ApplyDel(0, 0, ModDEF) == 0, "Del zero") && ok;
    ok = expect(ApplyDel(15, 5, ModDEF) == 10, "Del positive") && ok;
    ok = expect(ApplyDel(10, -3, ModDEF) == 13, "Del negative amount") && ok;
    ok = expect(ApplyDel(-5, 8, ModDEF) == -13, "Del negative current") && ok;
    ok = expect(ApplyDel(-10, -4, ModDEF) == -6, "Del both negative") && ok;
    ok = expect(ApplyDel(20, 20, ModDEF) == 0, "Del to zero") && ok;
    ok = expect(ApplyDel(5, 12, ModDEF) == -7, "Del past zero") && ok;

    // --- add/del round-trip ---
    {
        const std::int16_t cur = 100;
        const std::int16_t amt = 17;
        const auto         afterAdd = ApplyAdd(cur, amt, ModDEF);
        ok = expect(afterAdd == 117, "round-trip add") && ok;
        ok = expect(ApplyDel(afterAdd, amt, ModDEF) == cur, "round-trip del") && ok;
    }

    // --- set vs add distinction ---
    ok = expect(ApplyAdd(999, 1, ModDEF) == 1000, "Add accumulates") && ok;

    return ok;
}
