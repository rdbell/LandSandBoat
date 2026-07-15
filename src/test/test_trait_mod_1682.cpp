#include "test_trait_mod_1682.h"

#include "map/trait_mod_capacity.h"

#include <iostream>
#include <tuple>
#include <vector>

namespace
{
using namespace traitmodhelpers;

// Non-NONE sentinel (Mod::DEF = 1).
constexpr std::uint16_t ModDEF = 1;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "trait_mod 1682 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runTraitMod1682SelfTests() -> bool
{
    bool ok = true;

    // --- PlanAddTrait projects fields ---
    {
        const TraitMod t{ 15, ModDEF, 10 };
        const auto [id, mod, amt] = PlanAddTrait(t);
        ok = expect(id == 15 && mod == ModDEF && amt == 10, "PlanAddTrait double attack") && ok;
    }
    {
        const TraitMod t{ 20, modstathelpers::ModNone, 99 };
        const auto [id, mod, amt] = PlanAddTrait(t);
        ok = expect(id == 20 && mod == 0 && amt == 99, "PlanAddTrait NONE mod still plans") && ok;
    }
    {
        const TraitMod t{ 0, 0, 0 };
        const auto [id, mod, amt] = PlanAddTrait(t);
        ok = expect(id == 0 && mod == 0 && amt == 0, "PlanAddTrait zeros") && ok;
    }

    // --- PlanDelTrait projects mod + amount (not trait ID) ---
    {
        const TraitMod t{ 15, ModDEF, 10 };
        const auto [mod, amt] = PlanDelTrait(t);
        ok = expect(mod == ModDEF && amt == 10, "PlanDelTrait") && ok;
    }
    {
        const TraitMod t{ 7, 3, -5 };
        const auto [mod, amt] = PlanDelTrait(t);
        ok = expect(mod == 3 && amt == -5, "PlanDelTrait negative") && ok;
    }

    // --- ApplyAddTraitMod matches modstathelpers::ApplyAdd ---
    ok = expect(ApplyAddTraitMod(10, 5, ModDEF) == 15, "Add positive") && ok;
    ok = expect(ApplyAddTraitMod(10, -3, ModDEF) == 7, "Add negative amount") && ok;
    ok = expect(ApplyAddTraitMod(-5, 8, ModDEF) == 3, "Add negative current") && ok;
    ok = expect(ApplyAddTraitMod(10, 5, modstathelpers::ModNone) == 10, "Add NONE no-op") && ok;
    ok = expect(ApplyAddTraitMod(10, 5, ModDEF) == modstathelpers::ApplyAdd(10, 5, ModDEF),
                "Add matches modstat") &&
         ok;

    // --- ApplyDelTraitMod matches modstathelpers::ApplyDel ---
    ok = expect(ApplyDelTraitMod(15, 5, ModDEF) == 10, "Del positive") && ok;
    ok = expect(ApplyDelTraitMod(10, -3, ModDEF) == 13, "Del negative amount") && ok;
    ok = expect(ApplyDelTraitMod(5, 12, ModDEF) == -7, "Del past zero") && ok;
    ok = expect(ApplyDelTraitMod(10, 5, modstathelpers::ModNone) == 10, "Del NONE no-op") && ok;
    ok = expect(ApplyDelTraitMod(15, 5, ModDEF) == modstathelpers::ApplyDel(15, 5, ModDEF),
                "Del matches modstat") &&
         ok;

    // --- add/del mod round-trip ---
    {
        const std::int16_t cur = 100;
        const std::int16_t amt = 17;
        const auto         afterAdd = ApplyAddTraitMod(cur, amt, ModDEF);
        ok = expect(afterAdd == 117, "round-trip add") && ok;
        ok = expect(ApplyDelTraitMod(afterAdd, amt, ModDEF) == cur, "round-trip del") && ok;
    }

    // --- End-to-end plan + apply + ID list approx ---
    {
        const TraitMod t{ 15, ModDEF, 12 };
        const auto [appendID, modID, addAmt] = PlanAddTrait(t);
        auto       ids                       = AppendTraitID({}, appendID);
        ok = expect(ids.size() == 1 && ids[0] == 15, "append after plan") && ok;

        const std::int16_t base = 40;
        auto               mod  = ApplyAddTraitMod(base, addAmt, modID);
        ok                      = expect(mod == 52, "e2e mod after add") && ok;

        const auto [delMod, delAmt] = PlanDelTrait(t);
        mod                         = ApplyDelTraitMod(mod, delAmt, delMod);
        ok                          = expect(mod == base, "e2e mod after del") && ok;
        ids                         = RemoveTraitID(ids, appendID);
        ok                          = expect(ids.empty(), "e2e remove id") && ok;
    }

    // --- AppendTraitID ---
    {
        auto ids = AppendTraitID({}, 15);
        ok       = expect(ids.size() == 1 && ids[0] == 15, "append to empty") && ok;
        ids      = AppendTraitID(ids, 3);
        ok       = expect(ids.size() == 2 && ids[1] == 3, "append second") && ok;
        // Duplicates allowed.
        ids = AppendTraitID(ids, 15);
        ok  = expect(ids.size() == 3 && ids[2] == 15, "append dup") && ok;
    }

    // --- RemoveTraitID first match only ---
    {
        std::vector<std::uint16_t> ids{ 1, 15, 15, 3 };
        ids = RemoveTraitID(ids, 15);
        ok  = expect(ids.size() == 3 && ids[0] == 1 && ids[1] == 15 && ids[2] == 3,
                    "remove first 15") &&
             ok;
        const std::vector<std::uint16_t> miss{ 1, 2, 3 };
        const auto                       after = RemoveTraitID(miss, 99);
        ok = expect(after.size() == 3 && after[0] == 1 && after[1] == 2 && after[2] == 3,
                    "remove miss") &&
             ok;
        ok = expect(RemoveTraitID({}, 1).empty(), "remove from empty") && ok;
        ok = expect(RemoveTraitID(std::vector<std::uint16_t>{ 7 }, 7).empty(), "remove sole") && ok;
    }

    return ok;
}
