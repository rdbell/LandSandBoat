#include "test_pet_mod_map_1684.h"

#include "map/pet_mod_map_capacity.h"

#include <iostream>

namespace
{
using namespace petmodmaphelpers;

// PetModType / Mod sentinels as raw keys (parity pins, not full enums).
constexpr std::uint8_t  PetAll       = 0;
constexpr std::uint8_t  PetAvatar    = 1;
constexpr std::uint8_t  PetWyvern    = 2;
constexpr std::uint8_t  PetAutomaton = 3;
constexpr std::uint16_t ModNone      = 0;
constexpr std::uint16_t ModDEF       = 1;
constexpr std::uint16_t ModHP        = 2;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet_mod_map 1684 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runPetModMap1684SelfTests() -> bool
{
    bool ok = true;

    // --- ApplyAdd accumulate ---
    {
        Store store;
        store = ApplyAdd(store, PetAvatar, ModDEF, 10);
        ok    = expect(Get(store, PetAvatar, ModDEF) == 10, "add first → 10") && ok;
        store = ApplyAdd(store, PetAvatar, ModDEF, 5);
        ok    = expect(Get(store, PetAvatar, ModDEF) == 15, "add second → 15") && ok;
        store = ApplyAdd(store, PetAvatar, ModDEF, -3);
        ok    = expect(Get(store, PetAvatar, ModDEF) == 12, "add negative → 12") && ok;
    }

    // --- ApplyAdd missing keys auto-create ---
    {
        Store empty;
        const auto fromEmpty = ApplyAdd(empty, PetAll, ModDEF, 4);
        ok = expect(Get(fromEmpty, PetAll, ModDEF) == 4, "add empty store") && ok;
        ok = expect(fromEmpty.size() == 1, "add empty outer size 1") && ok;

        Store base;
        base[PetAvatar][ModDEF] = 9;
        const auto got = ApplyAdd(base, PetAvatar, ModHP, 3);
        ok = expect(Get(got, PetAvatar, ModHP) == 3, "add missing modID → 3") && ok;
        ok = expect(Get(got, PetAvatar, ModDEF) == 9, "add missing modID leaves DEF") && ok;
        ok = expect(Get(base, PetAvatar, ModDEF) == 9 && Get(base, PetAvatar, ModHP) == 0, "add input intact") && ok;
    }

    // --- ApplySet overwrite ---
    {
        Store store = ApplyAdd({}, PetAvatar, ModDEF, 50);
        store       = ApplySet(store, PetAvatar, ModDEF, 7);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 7, "set overwrite → 7") && ok;
        store       = ApplySet(store, PetAvatar, ModDEF, -99);
        ok          = expect(Get(store, PetAvatar, ModDEF) == -99, "set negative → -99") && ok;
        store       = ApplySet(store, PetAvatar, ModDEF, 1);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 1, "set ignores current → 1") && ok;
    }

    // --- ApplySet missing keys ---
    {
        const auto got = ApplySet({}, PetAutomaton, ModHP, 42);
        ok = expect(Get(got, PetAutomaton, ModHP) == 42, "set empty store") && ok;
        const auto zero = ApplySet({}, PetAll, ModDEF, 0);
        ok = expect(zero.count(PetAll) == 1, "set zero creates outer") && ok;
        ok = expect(Get(zero, PetAll, ModDEF) == 0, "set zero amount") && ok;
    }

    // --- ApplyDel subtract ---
    {
        Store store = ApplyAdd({}, PetAvatar, ModDEF, 20);
        store       = ApplyDel(store, PetAvatar, ModDEF, 5);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 15, "del → 15") && ok;
        store       = ApplyDel(store, PetAvatar, ModDEF, -3);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 18, "del neg amount → 18") && ok;
        store       = ApplyDel(store, PetAvatar, ModDEF, 30);
        ok          = expect(Get(store, PetAvatar, ModDEF) == -12, "del past zero → -12") && ok;
    }

    // --- ApplyDel missing keys ---
    {
        const auto missingOuter = ApplyDel({}, PetWyvern, ModDEF, 4);
        ok = expect(Get(missingOuter, PetWyvern, ModDEF) == -4, "del missing outer → -4") && ok;

        Store base;
        base[PetAvatar][ModHP] = 9;
        const auto got = ApplyDel(base, PetAvatar, ModDEF, 12);
        ok = expect(Get(got, PetAvatar, ModDEF) == -12, "del missing modID → -12") && ok;
        ok = expect(Get(got, PetAvatar, ModHP) == 9, "del missing modID leaves HP") && ok;
        ok = expect(Get(base, PetAvatar, ModHP) == 9 && Get(base, PetAvatar, ModDEF) == 0, "del input intact") && ok;
    }

    // --- add/del round-trip ---
    {
        Store store = ApplyAdd({}, PetAvatar, ModDEF, 100);
        store       = ApplyAdd(store, PetAvatar, ModDEF, 17);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 117, "round-trip add") && ok;
        store       = ApplyDel(store, PetAvatar, ModDEF, 17);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 100, "round-trip del") && ok;
    }

    // --- isolation: result nested independent of input ---
    {
        Store base;
        base[PetAvatar][ModDEF] = 10;
        base[PetWyvern][ModDEF] = 3;
        auto got                = ApplyAdd(base, PetAvatar, ModDEF, 1);
        got[PetAvatar][ModDEF]  = 999;
        ok = expect(Get(base, PetAvatar, ModDEF) == 10, "mutate result leaves base") && ok;
        ok = expect(Get(base, PetWyvern, ModDEF) == 3, "untouched petmod intact") && ok;
    }

    // --- separate petmods independent ---
    {
        Store store = ApplyAdd({}, PetAvatar, ModDEF, 10);
        store       = ApplyAdd(store, PetWyvern, ModDEF, 20);
        store       = ApplySet(store, PetAvatar, ModDEF, 1);
        ok          = expect(Get(store, PetAvatar, ModDEF) == 1, "avatar after set") && ok;
        ok          = expect(Get(store, PetWyvern, ModDEF) == 20, "wyvern untouched") && ok;
    }

    // --- no Mod::NONE skip (unlike modstat) ---
    {
        Store store = ApplyAdd({}, PetAll, ModNone, 5);
        ok          = expect(Get(store, PetAll, ModNone) == 5, "add NONE id") && ok;
        store       = ApplySet(store, PetAll, ModNone, 9);
        ok          = expect(Get(store, PetAll, ModNone) == 9, "set NONE id") && ok;
        store       = ApplyDel(store, PetAll, ModNone, 2);
        ok          = expect(Get(store, PetAll, ModNone) == 7, "del NONE id") && ok;
    }

    // --- Get missing ---
    {
        ok = expect(Get({}, PetAvatar, ModDEF) == 0, "Get empty → 0") && ok;
        Store store;
        store[PetAvatar][ModDEF] = 3;
        ok = expect(Get(store, PetWyvern, ModDEF) == 0, "Get missing petmod → 0") && ok;
        ok = expect(Get(store, PetAvatar, ModHP) == 0, "Get missing modID → 0") && ok;
        ok = expect(Get(store, PetAvatar, ModDEF) == 3, "Get present → 3") && ok;
    }

    return ok;
}
