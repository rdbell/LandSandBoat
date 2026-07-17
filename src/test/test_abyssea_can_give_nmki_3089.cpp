#include "test_abyssea_can_give_nmki_3089.h"

#include "map/abyssea_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "abyssea canGiveNMKI 3089 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua xi.abyssea.canGiveNMKI formula for dual-wire checks (slice 3089):
//   math.random(1, 100) <= dropChance or redProcValue == 1
auto inlineCanGiveNMKI(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

// Compact dual-wire pin matching C++ capacity formula:
//   roll1to100 <= dropChance || redProc
auto pinCanGiveNMKI(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

} // namespace

// Pure dual-wire expansion for abysseahelpers::CanGiveNMKI
// (Lua canGiveNMKI roll/red-proc gate; slice 3089).
auto runAbysseaCanGiveNMKI3089SelfTests() -> bool
{
    using abysseahelpers::AtmaNMKIDropChance;
    using abysseahelpers::CanGiveAtmaNMKI;
    using abysseahelpers::CanGiveNMKI;
    using abysseahelpers::CanGiveNormalNMKI;
    using abysseahelpers::NormalNMKIDropChance;

    bool ok = true;

    // Residual constant / 1041 / 2861 pins still hold under dual-wire.
    ok = expect(NormalNMKIDropChance == 20, "NormalNMKIDropChance == 20") && ok;
    ok = expect(AtmaNMKIDropChance == 10, "AtmaNMKIDropChance == 10") && ok;
    ok = expect(CanGiveNMKI(1, 20, false), "residual: roll 1 / chance 20 → true") && ok;
    ok = expect(CanGiveNMKI(20, 20, false), "residual: roll 20 / chance 20 boundary → true") && ok;
    ok = expect(!CanGiveNMKI(21, 20, false), "residual: roll 21 / chance 20 → false") && ok;
    ok = expect(CanGiveNMKI(100, 0, true), "residual: red proc forces true") && ok;
    ok = expect(CanGiveNMKI(99, 10, true), "residual: red proc + miss roll → true") && ok;

    // Truth table poles: roll at/under/over chance; redProc always admits.
    const struct
    {
        int32       roll;
        int32       chance;
        bool        redProc;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, 20, false, true, "roll 1 / chance 20" },
        { 20, 20, false, true, "roll 20 / chance 20 boundary" },
        { 21, 20, false, false, "roll 21 / chance 20 miss" },
        { 10, 10, false, true, "atma roll 10 / chance 10 boundary" },
        { 11, 10, false, false, "atma roll 11 / chance 10 miss" },
        { 100, 10, false, false, "roll 100 / chance 10" },
        { 1, 0, false, false, "chance 0 roll 1" },
        { 100, 0, true, true, "red proc force chance 0" },
        { 99, 10, true, true, "red proc + miss roll" },
        { 21, 20, true, true, "red proc + over chance" },
        { 100, 100, false, true, "chance 100 roll 100" },
        { 50, 50, false, true, "mid boundary equal" },
        { 51, 50, false, false, "mid boundary miss" },
        { 1, 1, false, true, "chance 1 hit" },
        { 2, 1, false, false, "chance 1 miss" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanGiveNMKI(c.roll, c.chance, c.redProc);
        const bool inlineGot = inlineCanGiveNMKI(c.roll, c.chance, c.redProc);
        const bool pinGot    = pinCanGiveNMKI(c.roll, c.chance, c.redProc);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == (c.roll <= c.chance || c.redProc),
                    "dual-wire free == (roll <= chance || redProc)") &&
             ok;
    }

    // Host compose: giveNMDrops normal chance 20 / atma chance 10 through free fn.
    ok = expect(CanGiveNormalNMKI(20, false), "normal compose hit boundary") && ok;
    ok = expect(!CanGiveNormalNMKI(21, false), "normal compose miss boundary") && ok;
    ok = expect(CanGiveAtmaNMKI(10, false), "atma compose hit boundary") && ok;
    ok = expect(!CanGiveAtmaNMKI(11, false), "atma compose miss boundary") && ok;
    ok = expect(!CanGiveNormalNMKI(100, false), "normal compose miss roll") && ok;
    ok = expect(CanGiveNormalNMKI(100, true), "normal compose red proc") && ok;
    ok = expect(!CanGiveAtmaNMKI(100, false), "atma compose miss roll") && ok;
    ok = expect(CanGiveAtmaNMKI(100, true), "atma compose red proc") && ok;

    // Dense representative poles: free == inline == pin + compose identity.
    const int32 rolls[] = { 1, 10, 11, 20, 21, 50, 51, 99, 100 };
    for (const int32 roll : rolls)
    {
        for (const bool red : { false, true })
        {
            const bool got = CanGiveNMKI(roll, NormalNMKIDropChance, red);
            ok             = expect(got == pinCanGiveNMKI(roll, NormalNMKIDropChance, red),
                                    "dense free == pin") &&
                 ok;
            ok = expect(got == inlineCanGiveNMKI(roll, NormalNMKIDropChance, red),
                        "dense free == inline") &&
                 ok;
            ok = expect(got == (roll <= NormalNMKIDropChance || red),
                        "dense free == (roll <= chance || red)") &&
                 ok;
            ok = expect(CanGiveNormalNMKI(roll, red) == CanGiveNMKI(roll, NormalNMKIDropChance, red),
                        "CanGiveNormalNMKI compose dual-wire") &&
                 ok;
            ok = expect(CanGiveAtmaNMKI(roll, red) == CanGiveNMKI(roll, AtmaNMKIDropChance, red),
                        "CanGiveAtmaNMKI compose dual-wire") &&
                 ok;
        }
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(CanGiveNormalNMKI(20, false) == true, "compose normal boundary") && ok;
    ok = expect(CanGiveNormalNMKI(21, false) == false, "compose normal miss") && ok;
    ok = expect(CanGiveAtmaNMKI(10, false) == true, "compose atma boundary") && ok;
    ok = expect(CanGiveAtmaNMKI(11, false) == false, "compose atma miss") && ok;

    return ok;
}
