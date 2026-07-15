#include "test_get_mod_1679.h"

#include "map/get_mod_capacity.h"

#include <iostream>
#include <unordered_map>

namespace
{
using namespace getmodhelpers;

// Non-NONE sentinel (Mod::DEF = 1).
constexpr std::uint16_t ModDEF = 1;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "get_mod 1679 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runGetMod1679SelfTests() -> bool
{
    bool ok = true;

    // --- ModNone pin ---
    ok = expect(ModNone == 0, "ModNone == 0") && ok;

    // --- GetMod NONE short-circuit ---
    ok = expect(GetMod(ModNone, 0) == 0, "GetMod NONE zero store") && ok;
    ok = expect(GetMod(ModNone, 42) == 0, "GetMod NONE ignores store") && ok;
    ok = expect(GetMod(ModNone, -99) == 0, "GetMod NONE negative store") && ok;
    ok = expect(GetMod(0, 15) == 0, "GetMod modID 0") && ok;

    // --- GetMod present (non-NONE returns stored) ---
    ok = expect(GetMod(ModDEF, 0) == 0, "GetMod DEF zero") && ok;
    ok = expect(GetMod(ModDEF, 15) == 15, "GetMod DEF positive") && ok;
    ok = expect(GetMod(ModDEF, -7) == -7, "GetMod DEF negative") && ok;
    ok = expect(GetMod(2, 1000) == 1000, "GetMod large") && ok;
    ok = expect(GetMod(3, -32768) == -32768, "GetMod min int16") && ok;
    ok = expect(GetMod(4, 32767) == 32767, "GetMod max int16") && ok;

    // --- GetMod missing-as-zero (host injects 0) ---
    ok = expect(GetMod(99, 0) == 0, "GetMod missing inject 0") && ok;

    // --- GetModFromMap NONE ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> mods{
            { ModNone, 99 },
            { ModDEF, 15 },
        };
        ok = expect(GetModFromMap(ModNone, mods) == 0, "FromMap NONE short-circuit") && ok;
        ok = expect(GetModFromMap(0, mods) == 0, "FromMap modID 0") && ok;
    }
    {
        std::unordered_map<std::uint16_t, std::int16_t> empty;
        ok = expect(GetModFromMap(ModNone, empty) == 0, "FromMap NONE empty") && ok;
    }

    // --- GetModFromMap present ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> mods{
            { ModDEF, 15 },
            { 2, -3 },
            { 100, 42 },
            { 3, 0 },
        };
        ok = expect(GetModFromMap(ModDEF, mods) == 15, "FromMap DEF") && ok;
        ok = expect(GetModFromMap(2, mods) == -3, "FromMap negative") && ok;
        ok = expect(GetModFromMap(100, mods) == 42, "FromMap 100") && ok;
        ok = expect(GetModFromMap(3, mods) == 0, "FromMap explicit zero") && ok;
    }

    // --- GetModFromMap missing ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> mods{ { ModDEF, 15 } };
        ok = expect(GetModFromMap(2, mods) == 0, "FromMap missing 2") && ok;
        ok = expect(GetModFromMap(99, mods) == 0, "FromMap missing 99") && ok;
    }
    {
        std::unordered_map<std::uint16_t, std::int16_t> empty;
        ok = expect(GetModFromMap(ModDEF, empty) == 0, "FromMap empty DEF") && ok;
    }

    // --- GetMod agrees with FromMap for shared cases ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> mods{
            { ModDEF, 20 },
            { 5, -1 },
        };
        for (const std::uint16_t id : { ModNone, ModDEF, static_cast<std::uint16_t>(5), static_cast<std::uint16_t>(7) })
        {
            std::int16_t stored = 0;
            if (const auto it = mods.find(id); it != mods.end())
            {
                stored = it->second;
            }
            const auto fromStored = GetMod(id, stored);
            const auto fromMap    = GetModFromMap(id, mods);
            ok = expect(fromStored == fromMap, "GetMod matches FromMap") && ok;
        }
    }

    return ok;
}
