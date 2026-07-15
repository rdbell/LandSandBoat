#include "test_max_gear_mod_1675.h"

#include "map/max_gear_mod_capacity.h"

#include <array>
#include <iostream>
#include <span>

namespace
{
using namespace maxgearmodhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "max gear mod 1675 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runMaxGearMod1675SelfTests() -> bool
{
    bool ok = true;

    // --- pins ---
    ok = expect(ModNone == 0, "ModNone") && ok;
    ok = expect(SlotBack == 15, "SlotBack") && ok;
    ok = expect(GearModSlotCount == 15, "GearModSlotCount") && ok;
    ok = expect(GearModSlotCount == static_cast<std::size_t>(SlotBack), "count == SlotBack bound") && ok;

    // --- NONE short-circuit ---
    {
        const std::array<std::int16_t, 3> mods{ 10, 20, 5 };
        ok = expect(ResolveMaxGearMod(ModNone, true, mods) == 0, "NONE with gear") && ok;
        ok = expect(ResolveMaxGearMod(0, true, mods) == 0, "modID 0 with gear") && ok;
        ok = expect(ResolveMaxGearMod(ModNone, false, {}) == 0, "NONE non-PC") && ok;
    }

    // --- non-PC short-circuit ---
    {
        const std::array<std::int16_t, 3> mods{ 10, 20, 5 };
        ok = expect(ResolveMaxGearMod(1, false, mods) == 0, "non-PC with gear") && ok;
        ok = expect(ResolveMaxGearMod(42, false, {}) == 0, "non-PC empty") && ok;
    }

    // --- empty slots ---
    ok = expect(ResolveMaxGearMod(1, true, {}) == 0, "empty span") && ok;
    {
        const std::array<std::int16_t, 3> zeros{ 0, 0, 0 };
        ok = expect(ResolveMaxGearMod(1, true, zeros) == 0, "all-zero slots") && ok;
    }

    // --- max selection ---
    {
        const std::array<std::int16_t, 1> single{ 12 };
        ok = expect(ResolveMaxGearMod(1, true, single) == 12, "single") && ok;
    }
    {
        const std::array<std::int16_t, 3> first{ 50, 10, 20 };
        ok = expect(ResolveMaxGearMod(2, true, first) == 50, "first wins") && ok;
    }
    {
        const std::array<std::int16_t, 3> mid{ 10, 75, 20 };
        ok = expect(ResolveMaxGearMod(3, true, mid) == 75, "middle wins") && ok;
    }
    {
        const std::array<std::int16_t, 3> last{ 10, 20, 99 };
        ok = expect(ResolveMaxGearMod(4, true, last) == 99, "last wins") && ok;
    }
    {
        const std::array<std::int16_t, 3> ties{ 30, 30, 10 };
        ok = expect(ResolveMaxGearMod(5, true, ties) == 30, "ties") && ok;
    }
    {
        const std::array<std::int16_t, 4> mixed{ 0, 0, 8, 0 };
        ok = expect(ResolveMaxGearMod(6, true, mixed) == 8, "zeros and value") && ok;
    }
    {
        const std::array<std::int16_t, 15> window{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
        ok = expect(ResolveMaxGearMod(7, true, window) == 15, "fifteen-slot window") && ok;
    }

    // --- uint16 comparison parity (negative int16 widens high) ---
    {
        const std::array<std::int16_t, 2> negWins{ 10, -1 };
        ok = expect(ResolveMaxGearMod(8, true, negWins) == -1, "negative wins as uint16") && ok;
    }
    {
        const std::array<std::int16_t, 3> allNeg{ -5, -1, -100 };
        ok = expect(ResolveMaxGearMod(9, true, allNeg) == -1, "all negative") && ok;
    }

    // --- gate order ---
    {
        const std::array<std::int16_t, 3> mods{ 1, 2, 3 };
        ok = expect(ResolveMaxGearMod(0, true, mods) == 0, "NONE ignores max") && ok;
        ok = expect(ResolveMaxGearMod(100, false, mods) == 0, "non-PC ignores max") && ok;
    }

    return ok;
}
