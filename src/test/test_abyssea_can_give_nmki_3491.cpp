#include "test_abyssea_can_give_nmki_3491.h"

#include "map/abyssea_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "abyssea CanGiveNMKI 3491 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua xi.abyssea.canGiveNMKI formula for dual-wire checks (slice 3491).
// Positive OR form only (QF1001-safe; no De Morgan rewrite):
//   math.random(1, 100) <= dropChance or redProcValue == 1
//     = roll1to100 <= dropChance || redProc
auto inlineCanGiveNMKI(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

// Compact dual-wire pin matching Go pinCanGiveNMKI3491 / C++ capacity
// CanGiveNMKI direct-return form (positive OR; QF1001-safe):
//   roll1to100 <= dropChance || redProc
auto pinCanGiveNMKI(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

// Compact dual-wire pin matching Go pinCanGiveNMKI3434 (prior dedicated
// expand residual 2861 independence pins).
auto pinCanGiveNMKI3434(const int32 roll1to100, const int32 dropChance, const bool redProc) -> bool
{
    return roll1to100 <= dropChance || redProc;
}

} // namespace

// Pure dual-wire expansion for abysseahelpers::CanGiveNMKI
// (Lua canGiveNMKI roll/red-proc gate; OmegaXI internal/abyssea;
// dedicated slice 3491 expand residual 2861 / prior 3434 / 3314 / 3284 /
// 3238 / 3089 — formula unchanged).
//
// Coverage (clones 3434 density on CanGiveNMKI primary + prior 3434 independence):
//   - free == inline == pin == (roll <= chance || redProc)  // positive OR
//   - residual poles: roll at/under/over chance, redProc force
//   - dense rolls 1..100 edges for normal/atma chances (with redProc true/false)
//   - residual 1041 / 2861 / prior 3089 / prior 3238 / 3284 / 3314 / 3434 pins still hold
//   - compose siblings CanGiveNormalNMKI / CanGiveAtmaNMKI lightly re-pinned
auto runAbysseaCanGiveNMKI3491SelfTests() -> bool
{
    using abysseahelpers::AtmaNMKIDropChance;
    using abysseahelpers::CanGiveAtmaNMKI;
    using abysseahelpers::CanGiveNMKI;
    using abysseahelpers::CanGiveNormalNMKI;
    using abysseahelpers::NormalNMKIDropChance;

    bool ok = true;

    // Residual constant / 1041 / 2861 / 3089 / 3238 / 3284 / 3314 / 3434 pins still hold.
    ok = expect(NormalNMKIDropChance == 20, "NormalNMKIDropChance == 20") && ok;
    ok = expect(AtmaNMKIDropChance == 10, "AtmaNMKIDropChance == 10") && ok;

    // Residual poles: roll at / under / over chance.
    ok = expect(CanGiveNMKI(1, 20, false), "residual: roll 1 / chance 20 → true") && ok;
    ok = expect(CanGiveNMKI(20, 20, false), "residual: roll 20 / chance 20 boundary → true") && ok;
    ok = expect(!CanGiveNMKI(21, 20, false), "residual: roll 21 / chance 20 → false") && ok;
    ok = expect(CanGiveNMKI(100, 0, true), "residual: red proc forces true") && ok;
    ok = expect(CanGiveNMKI(99, 10, true), "residual: red proc + miss roll → true") && ok;
    ok = expect(CanGiveNMKI(21, 20, true), "residual: red proc + over chance → true") && ok;

    // --- Composition table: free == inline == pin (positive OR form) ---
    const struct
    {
        int32       roll;
        int32       chance;
        bool        redProc;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2861 / 3089 / 3434 poles.
        { 1, 20, false, true, "residual roll 1 / chance 20" },
        { 20, 20, false, true, "residual roll 20 / chance 20 boundary" },
        { 21, 20, false, false, "residual roll 21 / chance 20 miss" },
        { 10, 10, false, true, "residual atma roll 10 / chance 10 boundary" },
        { 11, 10, false, false, "residual atma roll 11 / chance 10 miss" },
        { 100, 10, false, false, "residual roll 100 / chance 10" },
        { 1, 0, false, false, "residual chance 0 roll 1" },
        { 100, 0, true, true, "residual red proc force chance 0" },
        { 99, 10, true, true, "residual red proc + miss roll" },
        { 21, 20, true, true, "residual red proc + over chance" },
        { 100, 100, false, true, "residual chance 100 roll 100" },
        { 50, 50, false, true, "residual mid boundary equal" },
        { 51, 50, false, false, "residual mid boundary miss" },
        { 1, 1, false, true, "residual chance 1 hit" },
        { 2, 1, false, false, "residual chance 1 miss" },

        // Residual poles: roll at boundary, redProc force, short roll.
        { 20, 20, false, true, "pole normal boundary equal" },
        { 21, 20, false, false, "pole normal short" },
        { 19, 20, false, true, "pole normal under" },
        { 10, 10, false, true, "pole atma boundary equal" },
        { 11, 10, false, false, "pole atma short" },
        { 9, 10, false, true, "pole atma under" },
        { 100, 0, true, true, "pole redProc force" },
        { 0, 20, false, true, "pole roll 0 <= 20" },
        { -1, 20, false, true, "pole negative roll <= 20" },
        { 101, 20, false, false, "pole roll 101 short of 20" },
        { 0, 10, false, true, "pole roll 0 <= 10" },
        { 101, 10, false, false, "pole roll 101 short of 10" },
        { 50, 20, false, false, "pole mid short of normal" },
        { 50, 20, true, true, "pole mid + redProc" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanGiveNMKI(c.roll, c.chance, c.redProc);
        const bool inlineGot = inlineCanGiveNMKI(c.roll, c.chance, c.redProc);
        const bool pinGot    = pinCanGiveNMKI(c.roll, c.chance, c.redProc);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "dual-wire free == pin direct-return") && ok;
        ok = expect(got == (c.roll <= c.chance || c.redProc),
                    "dual-wire free == (roll <= chance || redProc)") &&
             ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       roll;
        int32       chance;
        bool        redProc;
        const char* label;
    } poles[] = {
        { 20, 20, false, "normal boundary hit" },
        { 21, 20, false, "normal short roll" },
        { 10, 10, false, "atma boundary hit" },
        { 11, 10, false, "atma short roll" },
        { 100, 0, true, "redProc force" },
        { 1, 20, false, "roll 1 hit" },
        { 100, 20, false, "roll 100 short of normal" },
        { 19, 20, false, "under normal boundary" },
        { 20, 20, true, "boundary + red" },
    };
    for (const auto& pole : poles)
    {
        const bool got       = CanGiveNMKI(pole.roll, pole.chance, pole.redProc);
        const bool inlineGot = inlineCanGiveNMKI(pole.roll, pole.chance, pole.redProc);
        const bool pinGot    = pinCanGiveNMKI(pole.roll, pole.chance, pole.redProc);
        const bool want      = pole.roll <= pole.chance || pole.redProc;
        ok                   = expect(got == want, pole.label) && ok;
        ok                   = expect(got == inlineGot && got == pinGot, "pole free == inline == pin") && ok;
    }

    // Dense rolls 1..100 edges for normal + atma chances: free == inline == pin.
    for (const int32 chance : { NormalNMKIDropChance, AtmaNMKIDropChance })
    {
        for (int32 roll = 1; roll <= 100; ++roll)
        {
            for (const bool red : { false, true })
            {
                const bool got       = CanGiveNMKI(roll, chance, red);
                const bool inlineGot = inlineCanGiveNMKI(roll, chance, red);
                const bool pinGot    = pinCanGiveNMKI(roll, chance, red);
                const bool want      = roll <= chance || red;
                ok                   = expect(got == want, "dense free == formula") && ok;
                ok                   = expect(got == inlineGot && got == pinGot, "dense free == inline == pin") && ok;
            }
        }
    }

    // Dense edge neighbors outside 1..100.
    for (const int32 chance : { NormalNMKIDropChance, AtmaNMKIDropChance, 0, 50, 100 })
    {
        for (const int32 roll : { 0, -1, 101, 200 })
        {
            for (const bool red : { false, true })
            {
                const bool got       = CanGiveNMKI(roll, chance, red);
                const bool inlineGot = inlineCanGiveNMKI(roll, chance, red);
                const bool pinGot    = pinCanGiveNMKI(roll, chance, red);
                const bool want      = roll <= chance || red;
                ok                   = expect(got == want && got == inlineGot && got == pinGot,
                                              "edge free == inline == pin == formula") &&
                     ok;
            }
        }
    }

    // Explicit composition pins matching C++ capacity direct-return form.
    ok = expect(CanGiveNMKI(20, 20, false) == true, "compose normal boundary") && ok;
    ok = expect(CanGiveNMKI(21, 20, false) == false, "compose normal miss") && ok;
    ok = expect(CanGiveNMKI(10, 10, false) == true, "compose atma boundary") && ok;
    ok = expect(CanGiveNMKI(11, 10, false) == false, "compose atma miss") && ok;
    ok = expect(CanGiveNMKI(20, 20, false) == pinCanGiveNMKI(20, 20, false),
                "free == pin boundary") &&
         ok;
    ok = expect(CanGiveNMKI(21, 20, false) == pinCanGiveNMKI(21, 20, false),
                "free == pin short") &&
         ok;
    ok = expect(CanGiveNMKI(100, 0, true) == pinCanGiveNMKI(100, 0, true),
                "free == pin red-proc force") &&
         ok;

    // Prior dedicated expand residual 3434 pin helpers still agree with free dual-wire.
    ok = expect(CanGiveNMKI(20, 20, false) == pinCanGiveNMKI3434(20, 20, false),
                "free == prior 3434 pin boundary") &&
         ok;
    ok = expect(CanGiveNMKI(21, 20, false) == pinCanGiveNMKI3434(21, 20, false),
                "free == prior 3434 pin short") &&
         ok;
    ok = expect(CanGiveNMKI(100, 0, true) == pinCanGiveNMKI3434(100, 0, true),
                "free == prior 3434 pin red-proc force") &&
         ok;
    ok = expect(pinCanGiveNMKI(20, 20, false) == pinCanGiveNMKI3434(20, 20, false),
                "3491 pin == prior 3434 pin boundary") &&
         ok;
    ok = expect(pinCanGiveNMKI(21, 20, false) == pinCanGiveNMKI3434(21, 20, false),
                "3491 pin == prior 3434 pin short") &&
         ok;
    ok = expect(pinCanGiveNMKI(100, 0, true) == pinCanGiveNMKI3434(100, 0, true),
                "3491 pin == prior 3434 pin red-proc force") &&
         ok;

    // Residual compose siblings (already dual-wired) still identity through free.
    ok = expect(CanGiveNormalNMKI(20, false) == CanGiveNMKI(20, NormalNMKIDropChance, false),
                "CanGiveNormalNMKI residual compose == free (boundary)") &&
         ok;
    ok = expect(CanGiveNormalNMKI(21, false) == CanGiveNMKI(21, NormalNMKIDropChance, false),
                "CanGiveNormalNMKI residual compose == free (short)") &&
         ok;
    ok = expect(CanGiveAtmaNMKI(10, false) == CanGiveNMKI(10, AtmaNMKIDropChance, false),
                "CanGiveAtmaNMKI residual compose == free (boundary)") &&
         ok;
    ok = expect(CanGiveAtmaNMKI(11, false) == CanGiveNMKI(11, AtmaNMKIDropChance, false),
                "CanGiveAtmaNMKI residual compose == free (short)") &&
         ok;

    return ok;
}
