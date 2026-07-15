#include "test_del_mod_list_1681.h"

#include "map/del_mod_list_capacity.h"

#include <iostream>
#include <unordered_map>
#include <vector>

namespace
{
using namespace delmodlisthelpers;

// Non-NONE sentinel (Mod::DEF = 1).
constexpr std::uint16_t ModDEF = 1;
constexpr std::uint16_t ModHP  = 2;
constexpr std::uint16_t ModNone = 0;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "del_mod_list 1681 self-test failed: " << label << '\n';
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
} // namespace

auto runDelModList1681SelfTests() -> bool
{
    bool ok = true;

    // --- ApplyDelToValue ---
    ok = expect(ApplyDelToValue(0, 0) == 0, "DelToValue zero") && ok;
    ok = expect(ApplyDelToValue(15, 5) == 10, "DelToValue positive") && ok;
    ok = expect(ApplyDelToValue(10, -3) == 13, "DelToValue neg amount") && ok;
    ok = expect(ApplyDelToValue(-5, 8) == -13, "DelToValue neg current") && ok;
    ok = expect(ApplyDelToValue(-10, -4) == -6, "DelToValue both neg") && ok;
    ok = expect(ApplyDelToValue(20, 20) == 0, "DelToValue to zero") && ok;
    ok = expect(ApplyDelToValue(5, 12) == -7, "DelToValue past zero") && ok;
    ok = expect(ApplyDelToValue(0, 7) == -7, "DelToValue from zero") && ok;

    // --- Empty list is a pure copy ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 }, { ModHP, 5 } };
        const auto                                      empty  = ApplyDelModifiers(current, std::vector<ModEntry>{});
        ok = expect(empty.size() == 2 && mapAt(empty, ModDEF) == 10 && mapAt(empty, ModHP) == 5, "empty list copy") && ok;
        ok = expect(mapAt(current, ModDEF) == 10 && mapAt(current, ModHP) == 5, "empty list input intact") && ok;

        const auto nilList = ApplyDelModifiers(current, std::span<const ModEntry>{});
        ok = expect(nilList.size() == 2 && mapAt(nilList, ModDEF) == 10, "nil span empty list") && ok;

        const auto emptyCurrent = ApplyDelModifiers({}, std::vector<ModEntry>{});
        ok = expect(emptyCurrent.empty(), "empty current empty list") && ok;
    }

    // --- Subtract including NONE (no skip) ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModNone, 100 }, { ModDEF, 50 } };
        const std::vector<ModEntry>                     list{
            { .id = ModNone, .amount = 7 },
            { .id = ModDEF, .amount = 10 },
        };
        const auto got = ApplyDelModifiers(current, list);
        ok = expect(mapAt(got, ModNone) == 93, "NONE id subtract 100-7") && ok;
        ok = expect(mapAt(got, ModDEF) == 40, "DEF subtract 50-10") && ok;
        ok = expect(mapAt(current, ModNone) == 100 && mapAt(current, ModDEF) == 50, "NONE path input intact") && ok;
    }

    // --- NONE when missing from current ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 1 } };
        const std::vector<ModEntry>                     list{ { .id = ModNone, .amount = 3 } };
        const auto                                      got = ApplyDelModifiers(current, list);
        ok = expect(mapAt(got, ModNone) == -3, "missing NONE → 0-3") && ok;
        ok = expect(mapAt(got, ModDEF) == 1, "untouched DEF with missing NONE") && ok;
    }

    // --- Multiple same ID accumulate ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 100 } };
        const std::vector<ModEntry>                     list{
            { .id = ModDEF, .amount = 10 },
            { .id = ModDEF, .amount = 20 },
            { .id = ModDEF, .amount = 5 },
        };
        const auto got = ApplyDelModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 65, "multi same id 100-10-20-5") && ok;
        ok = expect(mapAt(current, ModDEF) == 100, "multi same id input intact") && ok;
    }

    // --- Missing key treated as 0 ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModHP, 9 } };
        const std::vector<ModEntry>                     list{ { .id = ModDEF, .amount = 4 } };
        const auto                                      got = ApplyDelModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == -4, "missing DEF → 0-4") && ok;
        ok = expect(mapAt(got, ModHP) == 9, "untouched HP") && ok;

        const auto fromEmpty = ApplyDelModifiers({}, std::vector<ModEntry>{ { .id = ModDEF, .amount = 12 } });
        ok = expect(fromEmpty.size() == 1 && mapAt(fromEmpty, ModDEF) == -12, "empty current one entry") && ok;
    }

    // --- Mixed list ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{
            { ModDEF, 30 },
            { ModHP, 10 },
            { 5, 1 },
        };
        const std::vector<ModEntry> list{
            { .id = ModDEF, .amount = 5 },
            { .id = ModNone, .amount = 1 },
            { .id = ModHP, .amount = 10 },
            { .id = ModDEF, .amount = 5 },
            { .id = 99, .amount = 2 },
        };
        const auto got = ApplyDelModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 20, "mixed DEF 30-5-5") && ok;
        ok = expect(mapAt(got, ModHP) == 0, "mixed HP to zero") && ok;
        ok = expect(mapAt(got, 5) == 1, "mixed stray preserved") && ok;
        ok = expect(mapAt(got, ModNone) == -1, "mixed missing NONE") && ok;
        ok = expect(mapAt(got, 99) == -2, "mixed missing 99") && ok;
        ok = expect(mapAt(current, ModDEF) == 30 && mapAt(current, ModHP) == 10, "mixed input intact") && ok;
    }

    // --- Negative amount increases stored value ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 } };
        const std::vector<ModEntry>                     list{ { .id = ModDEF, .amount = -3 } };
        const auto                                      got = ApplyDelModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 13, "neg amount del 10-(-3)") && ok;
    }

    return ok;
}
