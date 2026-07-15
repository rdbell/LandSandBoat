#include "test_take_damage_gates_1697.h"

#include "map/take_damage_gates_capacity.h"

#include <iostream>

namespace
{
using namespace takedamagegateshelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "take damage gates 1697 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runTakeDamageGates1697SelfTests() -> bool
{
    bool ok = true;

    // --- tier threshold pins ---
    ok = expect(PlayerAvatarNightmareTierMin == 4, "tier pin 4") && ok;
    ok = expect(DiabolosNightmareTierMin == 11, "tier pin 11") && ok;

    // --- AbsorbDamageToMP concrete numbers ---
    ok = expect(AbsorbDamageToMP(200, 25) == 50, "200*25% = 50") && ok;
    ok = expect(AbsorbDamageToMP(100, 10) == 10, "100*10% = 10") && ok;
    ok = expect(AbsorbDamageToMP(80, 100) == 80, "80*100% = 80") && ok;
    ok = expect(AbsorbDamageToMP(50, 25) == 12, "50*25% truncates to 12") && ok;
    ok = expect(AbsorbDamageToMP(1, 25) == 0, "1*25% floors to 0") && ok;
    ok = expect(AbsorbDamageToMP(200, 0) == 0, "zero percent") && ok;
    ok = expect(AbsorbDamageToMP(200, -10) == 0, "negative percent") && ok;
    ok = expect(AbsorbDamageToMP(0, 25) == 0, "zero amount") && ok;
    ok = expect(AbsorbDamageToMP(-100, 25) == 0, "negative amount") && ok;
    ok = expect(AbsorbDamageToMP(1000, 50) == 500, "1000*50% = 500") && ok;
    ok = expect(AbsorbDamageToMP(7, 10) == 0, "7*10% floors to 0") && ok;
    ok = expect(AbsorbDamageToMP(15, 10) == 1, "15*10% = 1") && ok;

    // --- ResolveWakeUp: no / ordinary sleep ---
    ok = expect(ResolveWakeUp(true, 0, true), "default wake no sleep") && ok;
    ok = expect(!ResolveWakeUp(false, 0, true), "flag false no sleep") && ok;
    ok = expect(ResolveWakeUp(true, 1, true), "ordinary sleep tier 1") && ok;
    ok = expect(ResolveWakeUp(true, 3, true), "ordinary sleep tier 3") && ok;
    ok = expect(ResolveWakeUp(true, 1, false), "ordinary sleep breakBind false") && ok;
    ok = expect(!ResolveWakeUp(false, 2, true), "flag false ordinary sleep") && ok;

    // --- Player avatar Nightmare (tier >= 4) ---
    ok = expect(!ResolveWakeUp(true, 4, false), "nightmare tier4 breakBind false") && ok;
    ok = expect(!ResolveWakeUp(true, 5, false), "nightmare tier5 breakBind false") && ok;
    ok = expect(!ResolveWakeUp(true, 10, false), "nightmare tier10 breakBind false") && ok;
    ok = expect(ResolveWakeUp(true, 4, true), "nightmare tier4 breakBind true") && ok;
    ok = expect(ResolveWakeUp(true, 10, true), "nightmare tier10 breakBind true") && ok;
    ok = expect(!ResolveWakeUp(false, 4, true), "flag false nightmare tier4 breakBind true") && ok;
    ok = expect(!ResolveWakeUp(false, 4, false), "flag false nightmare tier4 breakBind false") && ok;

    // --- Diabolos NM Nightmare (tier >= 11) ---
    ok = expect(!ResolveWakeUp(true, 11, true), "diabolos tier11 breakBind true") && ok;
    ok = expect(!ResolveWakeUp(true, 11, false), "diabolos tier11 breakBind false") && ok;
    ok = expect(!ResolveWakeUp(true, 12, true), "diabolos tier12 breakBind true") && ok;
    ok = expect(!ResolveWakeUp(false, 11, true), "diabolos tier11 flag false") && ok;

    // --- Boundary pins: 3 / 4 / 10 / 11 ---
    ok = expect(ResolveWakeUp(true, 3, false), "boundary tier3 wakes") && ok;
    ok = expect(!ResolveWakeUp(true, 4, false), "boundary tier4 no break") && ok;
    ok = expect(ResolveWakeUp(true, 4, true), "boundary tier4 with break") && ok;
    ok = expect(ResolveWakeUp(true, 10, true), "boundary tier10 with break") && ok;
    ok = expect(!ResolveWakeUp(true, 11, true), "boundary tier11 no wake") && ok;

    // --- Compact-form equivalence over flag/bind/tier grid ---
    for (const bool wake : { true, false })
    {
        for (const bool bind : { true, false })
        {
            for (const std::uint16_t tier : { std::uint16_t{ 0 }, std::uint16_t{ 1 }, std::uint16_t{ 3 },
                                              std::uint16_t{ 4 }, std::uint16_t{ 5 }, std::uint16_t{ 10 },
                                              std::uint16_t{ 11 }, std::uint16_t{ 12 }, std::uint16_t{ 20 } })
            {
                bool compact = wake;
                if (tier >= 4 && (!bind || tier >= 11))
                {
                    compact = false;
                }
                if (ResolveWakeUp(wake, tier, bind) != compact)
                {
                    ok = expect(false, "compact-form equivalence") && ok;
                }
            }
        }
    }

    return ok;
}
