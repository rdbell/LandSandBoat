#include "test_add_mod_list_1692.h"

#include "map/add_mod_list_capacity.h"

#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

namespace
{
using namespace addmodlisthelpers;

// Non-NONE sentinel (Mod::DEF = 1).
constexpr std::uint16_t ModDEF = 1;
constexpr std::uint16_t ModHP  = 2;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "add_mod_list 1692 self-test failed: " << label << '\n';
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

auto runAddModList1692SelfTests() -> bool
{
    bool ok = true;

    // --- ApplyAddToValue ---
    ok = expect(ApplyAddToValue(0, 0) == 0, "AddToValue zero") && ok;
    ok = expect(ApplyAddToValue(15, 5) == 20, "AddToValue positive") && ok;
    ok = expect(ApplyAddToValue(10, -3) == 7, "AddToValue neg amount") && ok;
    ok = expect(ApplyAddToValue(-5, 8) == 3, "AddToValue neg current") && ok;
    ok = expect(ApplyAddToValue(-10, -4) == -14, "AddToValue both neg") && ok;
    ok = expect(ApplyAddToValue(-20, 20) == 0, "AddToValue to zero") && ok;
    ok = expect(ApplyAddToValue(0, 7) == 7, "AddToValue from zero") && ok;
    ok = expect(ApplyAddToValue(std::numeric_limits<std::int16_t>::max(), 1) == std::numeric_limits<std::int16_t>::min(),
                "AddToValue positive int16 wrap") &&
         ok;
    ok = expect(ApplyAddToValue(std::numeric_limits<std::int16_t>::min(), -1) == std::numeric_limits<std::int16_t>::max(),
                "AddToValue negative int16 wrap") &&
         ok;

    // --- Empty list is a pure copy ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 }, { ModHP, 5 } };
        const auto                                      empty = ApplyAddModifiers(current, std::vector<ModEntry>{});
        ok = expect(empty.size() == 2 && mapAt(empty, ModDEF) == 10 && mapAt(empty, ModHP) == 5, "empty list copy") && ok;
        ok = expect(mapAt(current, ModDEF) == 10 && mapAt(current, ModHP) == 5, "empty list input intact") && ok;

        const auto nilList = ApplyAddModifiers(current, std::span<const ModEntry>{});
        ok = expect(nilList.size() == 2 && mapAt(nilList, ModDEF) == 10, "nil span empty list") && ok;

        const auto emptyCurrent = ApplyAddModifiers({}, std::vector<ModEntry>{});
        ok = expect(emptyCurrent.empty(), "empty current empty list") && ok;
    }

    // --- NONE skipped (asymmetry with delModifiers) ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModNone, 100 }, { ModDEF, 50 } };
        const std::vector<ModEntry>                     list{
            { .id = ModNone, .amount = 7 },
            { .id = ModDEF, .amount = 10 },
        };
        const auto got = ApplyAddModifiers(current, list);
        ok = expect(mapAt(got, ModNone) == 100, "NONE id skipped remains 100") && ok;
        ok = expect(mapAt(got, ModDEF) == 60, "DEF add 50+10") && ok;
        ok = expect(mapAt(current, ModNone) == 100 && mapAt(current, ModDEF) == 50, "NONE path input intact") && ok;
    }

    // --- NONE when missing from current: not introduced ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 1 } };
        const std::vector<ModEntry>                     list{ { .id = ModNone, .amount = 3 } };
        const auto                                      got = ApplyAddModifiers(current, list);
        ok = expect(!mapHas(got, ModNone), "missing NONE not introduced") && ok;
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
        const auto got = ApplyAddModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 135, "multi same id 100+10+20+5") && ok;
        ok = expect(mapAt(current, ModDEF) == 100, "multi same id input intact") && ok;
    }

    // --- Missing key treated as 0 → amount ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModHP, 9 } };
        const std::vector<ModEntry>                     list{ { .id = ModDEF, .amount = 4 } };
        const auto                                      got = ApplyAddModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 4, "missing DEF → 0+4") && ok;
        ok = expect(mapAt(got, ModHP) == 9, "untouched HP") && ok;

        const auto fromEmpty = ApplyAddModifiers({}, std::vector<ModEntry>{ { .id = ModDEF, .amount = 12 } });
        ok = expect(fromEmpty.size() == 1 && mapAt(fromEmpty, ModDEF) == 12, "empty current one entry") && ok;
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
            { .id = ModHP, .amount = -10 },
            { .id = ModDEF, .amount = 5 },
            { .id = 99, .amount = 2 },
        };
        const auto got = ApplyAddModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 40, "mixed DEF 30+5+5") && ok;
        ok = expect(mapAt(got, ModHP) == 0, "mixed HP to zero") && ok;
        ok = expect(mapAt(got, 5) == 1, "mixed stray preserved") && ok;
        ok = expect(!mapHas(got, ModNone), "mixed NONE not introduced") && ok;
        ok = expect(mapAt(got, 99) == 2, "mixed missing 99 → 2") && ok;
        ok = expect(mapAt(current, ModDEF) == 30 && mapAt(current, ModHP) == 10, "mixed input intact") && ok;
    }

    // --- Isolation: result mutation does not touch current ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 }, { ModHP, 20 } };
        const std::vector<ModEntry>                     list{ { .id = ModDEF, .amount = 3 } };
        auto                                            got = ApplyAddModifiers(current, list);
        got[ModDEF] = 0;
        got[ModHP]  = 0;
        got[99]     = 1;
        ok = expect(mapAt(current, ModDEF) == 10 && mapAt(current, ModHP) == 20, "isolation current intact") && ok;
        ok = expect(!mapHas(current, 99), "isolation new key not in current") && ok;
    }

    // --- Negative amount decreases stored value ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> current{ { ModDEF, 10 } };
        const std::vector<ModEntry>                     list{ { .id = ModDEF, .amount = -3 } };
        const auto                                      got = ApplyAddModifiers(current, list);
        ok = expect(mapAt(got, ModDEF) == 7, "neg amount add 10+(-3)") && ok;
    }

    return ok;
}
