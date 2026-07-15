#include "test_pet_mod_apply_1688.h"

#include "map/pet_mod_apply_capacity.h"

#include <iostream>
#include <vector>

namespace
{
using namespace petmodapplyhelpers;

// PetModType / Mod sentinels as raw keys (parity pins, not full enums).
constexpr std::uint8_t  PetAll       = 0;
constexpr std::uint8_t  PetAvatar    = 1;
constexpr std::uint8_t  PetWyvern    = 2;
constexpr std::uint8_t  PetAutomaton = 3;
constexpr std::uint16_t ModNone      = 0;
constexpr std::uint16_t ModDEF       = 1;
constexpr std::uint16_t ModHP        = 2;
constexpr std::uint16_t ModMP        = 5;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet_mod_apply 1688 self-test failed: " << label << '\n';
    }
    return condition;
}

auto deltasEqual(const std::vector<ModDelta>& got, const std::vector<ModDelta>& want) -> bool
{
    if (got.size() != want.size())
    {
        return false;
    }
    for (std::size_t i = 0; i < got.size(); ++i)
    {
        if (got[i].ModID != want[i].ModID || got[i].Amount != want[i].Amount)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runPetModApply1688SelfTests() -> bool
{
    bool ok = true;

    // --- empty store ---
    {
        ok = expect(PlanApplyPetModifiers(Store{}, std::vector<std::uint8_t>{ PetAll }).empty(), "empty store apply") && ok;
        ok = expect(PlanRemovePetModifiers(Store{}, std::vector<std::uint8_t>{ PetAvatar }).empty(), "empty store remove") && ok;
    }

    // --- empty eligible ---
    {
        Store store;
        store[PetAvatar][ModDEF] = 10;
        ok = expect(PlanApplyPetModifiers(store, std::vector<std::uint8_t>{}).empty(), "empty eligible apply") && ok;
        ok = expect(PlanRemovePetModifiers(store, std::vector<std::uint8_t>{}).empty(), "empty eligible remove") && ok;
    }

    // --- filter by eligible list ---
    {
        Store store;
        store[PetAll][ModDEF]    = 1;
        store[PetAvatar][ModHP]  = 20;
        store[PetAvatar][ModDEF] = 10;
        store[PetWyvern][ModMP]  = 5;

        const auto avatarOnly = PlanApplyPetModifiers(store, std::vector<std::uint8_t>{ PetAvatar });
        const std::vector<ModDelta> wantAvatar{
            { ModDEF, 10 },
            { ModHP, 20 },
        };
        ok = expect(deltasEqual(avatarOnly, wantAvatar), "avatar only filter") && ok;

        const auto allAndAvatar = PlanApplyPetModifiers(store, std::vector<std::uint8_t>{ PetAll, PetAvatar });
        const std::vector<ModDelta> wantAllAvatar{
            { ModDEF, 1 },  // PetAll first
            { ModDEF, 10 }, // then PetAvatar
            { ModHP, 20 },
        };
        ok = expect(deltasEqual(allAndAvatar, wantAllAvatar), "all+avatar filter") && ok;

        const auto absent = PlanApplyPetModifiers(store, std::vector<std::uint8_t>{ PetAutomaton });
        ok = expect(absent.empty(), "absent eligible empty") && ok;
    }

    // --- order stable by petmod then modID ---
    {
        Store store;
        store[PetWyvern][ModMP]  = 3;
        store[PetWyvern][ModNone] = 1;
        store[PetWyvern][ModHP]  = 2;
        store[PetAvatar][ModHP]  = 20;
        store[PetAvatar][ModDEF] = 10;
        store[PetAll][ModDEF]    = 5;

        // Eligible list intentionally unsorted.
        const std::vector<std::uint8_t> eligible{ PetWyvern, PetAll, PetAvatar };
        const auto got = PlanApplyPetModifiers(store, eligible);
        const std::vector<ModDelta> want{
            // PetAll (0)
            { ModDEF, 5 },
            // PetAvatar (1)
            { ModDEF, 10 },
            { ModHP, 20 },
            // PetWyvern (2)
            { ModNone, 1 },
            { ModHP, 2 },
            { ModMP, 3 },
        };
        ok = expect(deltasEqual(got, want), "stable order apply") && ok;

        // Remove: same amounts as-is (not negated).
        const auto gotRemove = PlanRemovePetModifiers(store, eligible);
        ok = expect(deltasEqual(gotRemove, want), "stable order remove as-is") && ok;
    }

    // --- amounts as stored (negative / zero) ---
    {
        Store store;
        store[PetAvatar][ModDEF] = -7;
        store[PetAvatar][ModHP]  = 0;
        store[PetAvatar][ModMP]  = 42;
        const std::vector<std::uint8_t> eligible{ PetAvatar };
        const std::vector<ModDelta> want{
            { ModDEF, -7 },
            { ModHP, 0 },
            { ModMP, 42 },
        };
        ok = expect(deltasEqual(PlanApplyPetModifiers(store, eligible), want), "apply amounts as stored") && ok;
        ok = expect(deltasEqual(PlanRemovePetModifiers(store, eligible), want), "remove amounts as stored") && ok;
    }

    // --- duplicate eligible ids do not double-emit ---
    {
        Store store;
        store[PetAvatar][ModDEF] = 9;
        const auto got = PlanApplyPetModifiers(store, std::vector<std::uint8_t>{ PetAvatar, PetAvatar, PetAvatar });
        const std::vector<ModDelta> want{ { ModDEF, 9 } };
        ok = expect(deltasEqual(got, want), "dup eligible single emit") && ok;
    }

    // --- empty inner map skipped ---
    {
        Store store;
        store[PetAvatar] = {}; // empty inner
        store[PetWyvern][ModDEF] = 4;
        const auto got = PlanApplyPetModifiers(store, std::vector<std::uint8_t>{ PetAvatar, PetWyvern });
        const std::vector<ModDelta> want{ { ModDEF, 4 } };
        ok = expect(deltasEqual(got, want), "empty inner skipped") && ok;
    }

    return ok;
}
