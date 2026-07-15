#include "test_battle_life_1659.h"

#include "map/battle_life_capacity.h"

#include <iostream>

namespace
{
using namespace battlelifehelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle life 1659 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleLife1659SelfTests() -> bool
{
    bool ok = true;

    // --- pin ---
    ok = expect(StatusDisappear == 2, "StatusDisappear pin") && ok;

    // --- each OR arm alone → dead ---
    ok = expect(IsDead(0, 0, false, false), "hp zero alone") && ok;
    ok = expect(IsDead(-1, 0, false, false), "hp negative alone") && ok;
    ok = expect(IsDead(100, StatusDisappear, false, false), "disappear alone") && ok;
    ok = expect(IsDead(100, 0, true, false), "death state alone") && ok;
    ok = expect(IsDead(100, 0, false, true), "despawn state alone") && ok;

    // --- complements of those arms ---
    ok = expect(!IsAlive(0, 0, false, false), "hp zero not alive") && ok;
    ok = expect(!IsAlive(-1, 0, false, false), "hp negative not alive") && ok;
    ok = expect(!IsAlive(100, StatusDisappear, false, false), "disappear not alive") && ok;
    ok = expect(!IsAlive(100, 0, true, false), "death state not alive") && ok;
    ok = expect(!IsAlive(100, 0, false, true), "despawn state not alive") && ok;

    // --- all arms false → alive ---
    ok = expect(!IsDead(1, 0, false, false), "all false not dead") && ok;
    ok = expect(IsAlive(1, 0, false, false), "all false alive") && ok;
    ok = expect(!IsDead(9999, 1, false, false), "high hp not dead") && ok;
    ok = expect(IsAlive(9999, 1, false, false), "high hp alive") && ok;

    // --- non-disappear statuses stay alive with positive HP ---
    ok = expect(!IsDead(100, 0, false, false), "status 0 alive") && ok;
    ok = expect(!IsDead(100, 1, false, false), "status 1 alive") && ok;
    ok = expect(!IsDead(100, 3, false, false), "status 3 alive") && ok;
    ok = expect(!IsDead(100, 255, false, false), "status 255 alive") && ok;

    // --- both states ---
    ok = expect(IsDead(100, 0, true, true), "both states dead") && ok;
    ok = expect(!IsAlive(100, 0, true, true), "both states not alive") && ok;

    // --- IsAlive is exact complement ---
    ok = expect(IsAlive(50, 0, false, false) == !IsDead(50, 0, false, false), "complement alive") && ok;
    ok = expect(IsAlive(0, StatusDisappear, true, true) == !IsDead(0, StatusDisappear, true, true), "complement all arms") && ok;

    return ok;
}
