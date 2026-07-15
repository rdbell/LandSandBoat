#include "test_set_mod_list_1693.h"

#include "map/set_mod_list_capacity.h"

#include <iostream>
#include <unordered_map>
#include <vector>

namespace
{
using namespace setmodlisthelpers;

// Non-NONE sentinel (Mod::DEF = 1). ModNone comes from setmodlisthelpers.
constexpr std::uint16_t ModDEF = 1;
constexpr std::uint16_t ModHP  = 2;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "set_mod_list 1693 self-test failed: " << label << '\n';
    }
    return condition;
}

auto mapAt(const std::unordered_map<std::uint16_t, std::int16_t>& m, const std::uint16_t key, const std::int16_t fallback = 0)
    -> std::int16_t
{
    if (const auto it = m.find(key); it != m.end())
    {
        return it->second;
    }
    return fallback;
}

auto mapHas(const std::unordered_map<std::uint16_t, std::int16_t>& m, const std::uint16_t key) -> bool
{
    return m.find(key) != m.end();
}
} // namespace

auto runSetModList1693SelfTests() -> bool
{
    bool ok = true;

    // --- Empty list is a pure deep copy ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 }, { ModHP, 5 } };
        const auto                                      empty = ApplySetModifiers(current, std::vector<ModEntry>{});
        ok = expect(empty.size() == 2 && mapAt(empty, ModDEF) == 10 && mapAt(empty, ModHP) == 5, "empty list copy") && ok;
        ok = expect(mapAt(current, ModDEF) == 10 && mapAt(current, ModHP) == 5, "empty list input intact") && ok;

        const auto nilList = ApplySetModifiers(current, std::span<const ModEntry>{});
        ok = expect(nilList.size() == 2 && mapAt(nilList, ModDEF) == 10, "nil span empty list") && ok;

        const auto emptyCurrent = ApplySetModifiers({}, std::vector<ModEntry>{});
        ok = expect(emptyCurrent.empty(), "empty current empty list") && ok;
    }

    // --- Set not add ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 50 }, { ModHP, 20 } };
        const std::vector<ModEntry>                     list{
            { .id = ModDEF, .amount = 10 },
            { .id = ModHP, .amount = 7 },
        };
        const auto got = ApplySetModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 10, "DEF set not add") && ok;
        ok = expect(mapAt(got, ModHP) == 7, "HP set not add") && ok;
        ok = expect(mapAt(current, ModDEF) == 50 && mapAt(current, ModHP) == 20, "set not add input intact") && ok;
    }

    // --- Skips NONE ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModNone, 100 }, { ModDEF, 50 } };
        const std::vector<ModEntry>                     list{
            { .id = ModNone, .amount = 7 },
            { .id = ModDEF, .amount = 10 },
        };
        const auto got = ApplySetModifiers(current, list);
        ok = expect(mapAt(got, ModNone) == 100, "NONE id skipped preserves 100") && ok;
        ok = expect(mapAt(got, ModDEF) == 10, "DEF set with NONE sibling") && ok;
        ok = expect(mapAt(current, ModNone) == 100 && mapAt(current, ModDEF) == 50, "NONE path input intact") && ok;
    }

    // --- NONE when missing from current stays absent ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 1 } };
        const std::vector<ModEntry>                     list{ { .id = ModNone, .amount = 3 } };
        const auto                                      got = ApplySetModifiers(current, list);
        ok = expect(!mapHas(got, ModNone), "missing NONE stays absent") && ok;
        ok = expect(mapAt(got, ModDEF) == 1, "untouched DEF with skipped NONE") && ok;
    }

    // --- Multiple same ID: last write wins ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 100 } };
        const std::vector<ModEntry>                     list{
            { .id = ModDEF, .amount = 10 },
            { .id = ModDEF, .amount = 20 },
            { .id = ModDEF, .amount = 5 },
        };
        const auto got = ApplySetModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 5, "multi same id last set") && ok;
        ok = expect(mapAt(current, ModDEF) == 100, "multi same id input intact") && ok;
    }

    // --- Missing key introduced ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModHP, 9 } };
        const std::vector<ModEntry>                     list{ { .id = ModDEF, .amount = 4 } };
        const auto                                      got = ApplySetModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 4, "missing DEF introduced") && ok;
        ok = expect(mapAt(got, ModHP) == 9, "untouched HP") && ok;

        const auto fromEmpty = ApplySetModifiers({}, std::vector<ModEntry>{ { .id = ModDEF, .amount = 12 } });
        ok = expect(fromEmpty.size() == 1 && mapAt(fromEmpty, ModDEF) == 12, "empty current one entry") && ok;
    }

    // --- Zero and negative amounts ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 } };
        const auto                                      toZero = ApplySetModifiers(current, std::vector<ModEntry>{ { .id = ModDEF, .amount = 0 } });
        ok = expect(mapHas(toZero, ModDEF) && mapAt(toZero, ModDEF) == 0, "zero amount keeps key") && ok;

        const auto toNeg = ApplySetModifiers(current, std::vector<ModEntry>{ { .id = ModDEF, .amount = -3 } });
        ok = expect(mapAt(toNeg, ModDEF) == -3, "neg amount set") && ok;
    }

    // --- Mixed list ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{
            { ModDEF, 30 },
            { ModHP, 10 },
            { 5, 1 },
            { ModNone, 42 },
        };
        const std::vector<ModEntry> list{
            { .id = ModDEF, .amount = 5 },
            { .id = ModNone, .amount = 1 },
            { .id = ModHP, .amount = 0 },
            { .id = ModDEF, .amount = 8 },
            { .id = 99, .amount = 2 },
        };
        const auto got = ApplySetModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 8, "mixed DEF last set 8") && ok;
        ok = expect(mapAt(got, ModHP) == 0, "mixed HP to zero") && ok;
        ok = expect(mapAt(got, 5) == 1, "mixed stray preserved") && ok;
        ok = expect(mapAt(got, ModNone) == 42, "mixed NONE skipped keeps 42") && ok;
        ok = expect(mapAt(got, 99) == 2, "mixed missing 99 introduced") && ok;
        ok = expect(mapAt(current, ModDEF) == 30 && mapAt(current, ModHP) == 10, "mixed input intact") && ok;
    }

    // --- Deep copy: mutating result does not touch input ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 3 } };
        auto                                            got = ApplySetModifiers(current, std::vector<ModEntry>{ { .id = ModHP, .amount = 1 } });
        got[ModDEF] = 99;
        got[ModHP]  = 88;
        ok = expect(mapAt(current, ModDEF) == 3, "deep copy input DEF intact") && ok;
        ok = expect(!mapHas(current, ModHP), "deep copy input no HP key") && ok;
    }

    return ok;
}
