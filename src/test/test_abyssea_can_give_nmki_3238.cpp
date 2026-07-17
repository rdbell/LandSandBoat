#include "test_abyssea_can_give_nmki_3238.h"

#include "map/abyssea_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "abyssea CanGiveAtmaNMKI 3238 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua giveNMDrops atma-path formula for dual-wire checks (slice 3238):
//   CanGiveNMKI(roll1to100, AtmaNMKIDropChance /*10*/, redProc)
//     = roll1to100 <= 10 || redProc
auto inlineCanGiveAtmaNMKI(const int32 roll1to100, const bool redProc) -> bool
{
    return roll1to100 <= abysseahelpers::AtmaNMKIDropChance || redProc;
}

// Compact dual-wire pin matching Go pinCanGiveAtmaNMKI3238 / C++ capacity
// CanGiveAtmaNMKI direct-return form:
//   roll1to100 <= AtmaNMKIDropChance || redProc
auto pinCanGiveAtmaNMKI(const int32 roll1to100, const bool redProc) -> bool
{
    return roll1to100 <= abysseahelpers::AtmaNMKIDropChance || redProc;
}

} // namespace

// Pure dual-wire expansion for abysseahelpers::CanGiveAtmaNMKI
// (Lua giveNMDrops atma canGiveNMKI(mob, 10) path; OmegaXI
// internal/abyssea; dedicated slice 3238 expand residual 2861 /
// prior dedicated 3089 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == (roll <= AtmaNMKIDropChance || redProc)
//   - free == CanGiveNMKI(roll, AtmaNMKIDropChance, redProc) compose
//   - residual poles: roll at 10 boundary, redProc force, short roll
//   - dense rolls 1..100 edges (with redProc true/false)
//   - residual 1041 / 2861 / prior 3089 pins still hold
auto runAbysseaCanGiveNMKI3238SelfTests() -> bool
{
    using abysseahelpers::AtmaNMKIDropChance;
    using abysseahelpers::CanGiveAtmaNMKI;
    using abysseahelpers::CanGiveNMKI;
    using abysseahelpers::NormalNMKIDropChance;

    bool ok = true;

    // Residual constant / 1041 / 2861 / 3089 pins still hold under dual-wire.
    ok = expect(AtmaNMKIDropChance == 10, "AtmaNMKIDropChance == 10") && ok;
    ok = expect(NormalNMKIDropChance == 20, "NormalNMKIDropChance == 20") && ok;

    // Residual poles: roll at 10 boundary, short roll, redProc force.
    ok = expect(CanGiveAtmaNMKI(10, false), "residual: roll 10 boundary → true") && ok;
    ok = expect(!CanGiveAtmaNMKI(11, false), "residual: roll 11 short → false") && ok;
    ok = expect(CanGiveAtmaNMKI(1, false), "residual: roll 1 hit → true") && ok;
    ok = expect(!CanGiveAtmaNMKI(100, false), "residual: roll 100 short → false") && ok;
    ok = expect(CanGiveAtmaNMKI(100, true), "residual: red proc forces true") && ok;
    ok = expect(CanGiveAtmaNMKI(11, true), "residual: red proc + short → true") && ok;

    // --- Composition table: free == inline == pin + compose ---
    const struct
    {
        int32       roll;
        bool        redProc;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2861 / 3089 atma poles.
        { 10, false, true, "residual roll 10 boundary hit" },
        { 11, false, false, "residual roll 11 short" },
        { 1, false, true, "residual roll 1 hit" },
        { 100, false, false, "residual roll 100 short" },
        { 100, true, true, "residual red proc force miss roll" },
        { 11, true, true, "residual red proc + short" },
        { 9, false, true, "residual roll 9 under boundary" },
        { 10, true, true, "residual boundary + red proc" },

        // Residual poles: roll at 10 boundary, redProc force, short roll.
        { 10, false, true, "pole atma boundary equal" },
        { 11, false, false, "pole atma short" },
        { 9, false, true, "pole atma under" },
        { 100, true, true, "pole redProc force" },
        { 0, false, true, "pole roll 0 <= 10" },
        { -1, false, true, "pole negative roll <= 10" },
        { 101, false, false, "pole roll 101 short" },
        { 50, false, false, "pole mid short" },
        { 50, true, true, "pole mid + redProc" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanGiveAtmaNMKI(c.roll, c.redProc);
        const bool inlineGot = inlineCanGiveAtmaNMKI(c.roll, c.redProc);
        const bool pinGot    = pinCanGiveAtmaNMKI(c.roll, c.redProc);
        const bool compose   = CanGiveNMKI(c.roll, AtmaNMKIDropChance, c.redProc);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "dual-wire free == pin direct-return") && ok;
        ok = expect(got == compose, "dual-wire free == CanGiveNMKI(chance=10)") && ok;
        ok = expect(got == (c.roll <= AtmaNMKIDropChance || c.redProc),
                    "dual-wire free == (roll <= 10 || redProc)") &&
             ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       roll;
        bool        redProc;
        const char* label;
    } poles[] = {
        { 10, false, "boundary hit" },
        { 11, false, "short roll" },
        { 100, true, "redProc force" },
        { 1, false, "roll 1 hit" },
        { 100, false, "roll 100 short" },
        { 9, false, "under boundary" },
        { 10, true, "boundary + red" },
    };
    for (const auto& pole : poles)
    {
        const bool got       = CanGiveAtmaNMKI(pole.roll, pole.redProc);
        const bool inlineGot = inlineCanGiveAtmaNMKI(pole.roll, pole.redProc);
        const bool pinGot    = pinCanGiveAtmaNMKI(pole.roll, pole.redProc);
        const bool want      = pole.roll <= AtmaNMKIDropChance || pole.redProc;
        ok                   = expect(got == want, pole.label) && ok;
        ok                   = expect(got == inlineGot && got == pinGot, "pole free == inline == pin") && ok;
        ok                   = expect(got == CanGiveNMKI(pole.roll, AtmaNMKIDropChance, pole.redProc),
                                      "pole free == CanGiveNMKI compose") &&
             ok;
    }

    // Dense rolls 1..100 edges: free == inline == pin + compose for both redProc.
    for (int32 roll = 1; roll <= 100; ++roll)
    {
        for (const bool red : { false, true })
        {
            const bool got       = CanGiveAtmaNMKI(roll, red);
            const bool inlineGot = inlineCanGiveAtmaNMKI(roll, red);
            const bool pinGot    = pinCanGiveAtmaNMKI(roll, red);
            const bool compose   = CanGiveNMKI(roll, AtmaNMKIDropChance, red);
            const bool want      = roll <= AtmaNMKIDropChance || red;
            ok                   = expect(got == want, "dense free == formula") && ok;
            ok                   = expect(got == inlineGot && got == pinGot, "dense free == inline == pin") && ok;
            ok                   = expect(got == compose, "dense free == CanGiveNMKI compose") && ok;
        }
    }

    // Dense edge neighbors outside 1..100.
    for (const int32 roll : { 0, -1, 101, 200 })
    {
        for (const bool red : { false, true })
        {
            const bool got       = CanGiveAtmaNMKI(roll, red);
            const bool inlineGot = inlineCanGiveAtmaNMKI(roll, red);
            const bool pinGot    = pinCanGiveAtmaNMKI(roll, red);
            const bool want      = roll <= AtmaNMKIDropChance || red;
            ok                   = expect(got == want && got == inlineGot && got == pinGot,
                                          "edge free == inline == pin == formula") &&
                 ok;
        }
    }

    // Explicit composition pins matching C++ capacity direct-return form.
    ok = expect(CanGiveAtmaNMKI(10, false) == true, "compose atma boundary") && ok;
    ok = expect(CanGiveAtmaNMKI(11, false) == false, "compose atma miss") && ok;
    ok = expect(CanGiveAtmaNMKI(10, false) == pinCanGiveAtmaNMKI(10, false),
                "free == pin boundary") &&
         ok;
    ok = expect(CanGiveAtmaNMKI(11, false) == pinCanGiveAtmaNMKI(11, false),
                "free == pin short") &&
         ok;
    ok = expect(CanGiveAtmaNMKI(100, true) == pinCanGiveAtmaNMKI(100, true),
                "free == pin red-proc force") &&
         ok;
    ok = expect(CanGiveAtmaNMKI(10, false) == CanGiveNMKI(10, AtmaNMKIDropChance, false),
                "free == CanGiveNMKI(chance=10) boundary compose") &&
         ok;
    ok = expect(CanGiveAtmaNMKI(11, false) == CanGiveNMKI(11, AtmaNMKIDropChance, false),
                "free == CanGiveNMKI(chance=10) short compose") &&
         ok;

    return ok;
}
