#include "test_pet_mod_save_1678.h"

#include "map/pet_mod_save_capacity.h"

#include <iostream>
#include <unordered_map>

namespace
{
using namespace petmodsavehelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet_mod_save 1678 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runPetModSave1678SelfTests() -> bool
{
    bool ok = true;

    // --- List length and ordered pins ---
    const auto mods = PetModsToUpdate();
    ok = expect(mods.size() == PetModsToUpdateCount, "list size 29") && ok;
    ok = expect(mods.size() == 29, "list size literal 29") && ok;

    // Physical SDT (HTH before IMPACT)
    ok = expect(mods[0] == ModSlashSDT && ModSlashSDT == 49, "SlashSDT pin") && ok;
    ok = expect(mods[1] == ModPierceSDT && ModPierceSDT == 50, "PierceSDT pin") && ok;
    ok = expect(mods[2] == ModHTHSDT && ModHTHSDT == 52, "HTHSDT pin") && ok;
    ok = expect(mods[3] == ModImpactSDT && ModImpactSDT == 51, "ImpactSDT pin") && ok;
    ok = expect(mods[2] == 52 && mods[3] == 51, "HTH before IMPACT") && ok;

    // Uncapped Magic
    ok = expect(mods[4] == ModUDMGMagic && ModUDMGMagic == 389, "UDMGMagic pin") && ok;

    // Element SDT 54–61
    ok = expect(mods[5] == 54 && mods[6] == 55 && mods[7] == 56 && mods[8] == 57, "elem SDT 0-3") && ok;
    ok = expect(mods[9] == 58 && mods[10] == 59 && mods[11] == 60 && mods[12] == 61, "elem SDT 4-7") && ok;

    // Element RES_RANK 192–199
    ok = expect(mods[13] == 192 && mods[14] == 193 && mods[20] == 199, "elem RES_RANK span") && ok;

    // Status RES_RANK 1160–1167
    ok = expect(mods[21] == 1160 && mods[28] == 1167, "status RES_RANK span") && ok;
    ok = expect(mods[21] == ModParalyzeResRank && mods[28] == ModBlindResRank, "status RES_RANK pins") && ok;

    // --- ShouldUpdateSaved ---
    ok = expect(ShouldUpdateSaved(false, 0, 0), "missing key zero") && ok;
    ok = expect(ShouldUpdateSaved(false, 0, 100), "missing key nonzero") && ok;
    ok = expect(ShouldUpdateSaved(false, 99, -5), "missing ignores savedVal") && ok;
    ok = expect(!ShouldUpdateSaved(true, 10, 10), "same value") && ok;
    ok = expect(!ShouldUpdateSaved(true, 0, 0), "same zero") && ok;
    ok = expect(ShouldUpdateSaved(true, 10, 11), "different positive") && ok;
    ok = expect(ShouldUpdateSaved(true, -3, 3), "different negative") && ok;
    ok = expect(ShouldUpdateSaved(true, 0, 1), "saved zero current nonzero") && ok;
    ok = expect(ShouldUpdateSaved(true, 5, 0), "saved nonzero current zero") && ok;

    // --- PlanSavePetModifiers: only diffs ---
    {
        // Seed every list mod so missing-key noise does not dominate the plan;
        // then override the few cases under test.
        std::unordered_map<std::uint16_t, std::int16_t> current;
        std::unordered_map<std::uint16_t, std::int16_t> saved;
        for (const auto mod : PetModsToUpdate())
        {
            current[mod] = 0;
            saved[mod]   = 0;
        }
        current[ModSlashSDT]  = 100;
        saved[ModSlashSDT]    = 100; // same → skip
        current[ModPierceSDT] = 200;
        saved[ModPierceSDT]   = 199; // different → write 200
        current[ModUDMGMagic] = -50;
        saved.erase(ModUDMGMagic); // missing → write -50
        current[ModFireSDT] = 0;
        saved[ModFireSDT]   = 0; // same zero → skip
        current[1]          = 999; // stray Mod::DEF — must not appear

        const auto updates = PlanSavePetModifiers(current, saved);
        ok = expect(updates.find(ModSlashSDT) == updates.end(), "plan skip slash same") && ok;
        ok = expect(updates.count(ModPierceSDT) == 1 && updates.at(ModPierceSDT) == 200, "plan pierce diff") && ok;
        ok = expect(updates.count(ModUDMGMagic) == 1 && updates.at(ModUDMGMagic) == -50, "plan udmg missing") && ok;
        ok = expect(updates.find(ModFireSDT) == updates.end(), "plan skip fire same zero") && ok;
        ok = expect(updates.find(1) == updates.end(), "plan ignore stray") && ok;
        ok = expect(updates.size() == 2, "plan only-diffs size 2") && ok;
    }

    // --- Missing current resolves to 0 ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{};
        std::unordered_map<std::uint16_t, std::int16_t> saved{ { ModSlashSDT, 5 } };
        const auto updates = PlanSavePetModifiers(current, saved);
        ok = expect(updates.count(ModSlashSDT) == 1 && updates.at(ModSlashSDT) == 0, "missing current → 0") && ok;
        ok = expect(updates.size() == PetModsToUpdateCount, "empty current full list writes") && ok;
    }

    // --- Empty saved writes all present current ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current;
        std::uint16_t                                   i = 0;
        for (const auto mod : PetModsToUpdate())
        {
            current[mod] = static_cast<std::int16_t>(++i);
        }
        const auto updates = PlanSavePetModifiers(current, {});
        ok = expect(updates.size() == PetModsToUpdateCount, "empty saved full list") && ok;
        i = 0;
        for (const auto mod : PetModsToUpdate())
        {
            ++i;
            ok = expect(updates.count(mod) == 1 && updates.at(mod) == static_cast<std::int16_t>(i), "empty saved value") && ok;
        }
    }

    // --- Identical maps → no updates ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current;
        std::unordered_map<std::uint16_t, std::int16_t> saved;
        for (const auto mod : PetModsToUpdate())
        {
            current[mod] = 42;
            saved[mod]   = 42;
        }
        const auto updates = PlanSavePetModifiers(current, saved);
        ok = expect(updates.empty(), "identical no updates") && ok;
    }

    // --- Both empty → write 0 for every list mod ---
    {
        const auto updates = PlanSavePetModifiers({}, {});
        ok = expect(updates.size() == PetModsToUpdateCount, "both empty full list") && ok;
        for (const auto mod : PetModsToUpdate())
        {
            ok = expect(updates.count(mod) == 1 && updates.at(mod) == 0, "both empty zero write") && ok;
        }
    }

    return ok;
}
