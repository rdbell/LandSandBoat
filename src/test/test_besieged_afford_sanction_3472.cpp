#include "test_besieged_afford_sanction_3472.h"

#include "map/besieged_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "besieged CanAffordSanction 3472 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinish sanctionCost for dual-wire cross-check (slice 3472):
//   sanctionCost = 100
//   if option == 0 then sanctionCost = 0 end
auto inlineSanctionAffordCost(const int32 option) -> int32
{
    if (option == 0)
    {
        return 0;
    }
    return 100;
}

// Inline positive form of the Lua reject for dual-wire cross-check:
//   imperialStanding >= SanctionAffordCost(option)
//   // reject: imperialStanding < sanctionCost
auto inlineCanAffordSanction(const int32 option, const int32 imperialStanding) -> bool
{
    return imperialStanding >= inlineSanctionAffordCost(option);
}

// Compact dual-wire pin matching Go pinCanAffordSanction3472 / C++ capacity
// (direct return form):
//   imperialStanding >= SanctionAffordCost(option)
auto pinCanAffordSanction(const int32 option, const int32 imperialStanding) -> bool
{
    return imperialStanding >= besiegedhelpers::SanctionAffordCost(option);
}

} // namespace

// Pure dual-wire expansion for besiegedhelpers::CanAffordSanction
// (Lua onEventFinish sanction imperial_standing afford gate; OmegaXI
// internal/besieged; dedicated slice 3472 expand residual 2945 —
// formula unchanged; prior dedicated ~3418).
//
// Coverage:
//   - free == inline == pin == (standing >= SanctionAffordCost(option))
//   - residual 1120 / 2945 / 3093 / 3227 / 3277 / 3307 / 3418 pins still hold
//   - residual poles: known option costs vs standing short/exact/surplus
//   - invalid / non-sanction options still cost 100
auto runBesiegedAffordSanction3472SelfTests() -> bool
{
    using besiegedhelpers::CanAffordSanction;
    using besiegedhelpers::SanctionAffordCost;
    using besiegedhelpers::SanctionOptionNone;
    using besiegedhelpers::SanctionStandingCost;

    bool ok = true;

    // Residual constant / 1120 / 2945 / 3093 / 3227 / 3277 / 3307 / 3418 pins still hold under dual-wire.
    ok = expect(SanctionOptionNone == 0, "SanctionOptionNone pin 0") && ok;
    ok = expect(SanctionStandingCost == 100, "SanctionStandingCost pin 100") && ok;
    ok = expect(SanctionAffordCost(0) == 0, "residual option 0 afford cost free") && ok;
    ok = expect(SanctionAffordCost(16) == 100, "residual option 16 cost 100") && ok;
    ok = expect(SanctionAffordCost(32) == 100, "residual option 32 cost 100") && ok;
    ok = expect(SanctionAffordCost(48) == 100, "residual option 48 cost 100") && ok;
    ok = expect(CanAffordSanction(0, 0), "residual option0 free afford at 0") && ok;
    ok = expect(!CanAffordSanction(16, 99), "residual regen short must not afford") && ok;
    ok = expect(CanAffordSanction(16, 100), "residual regen exact must afford") && ok;
    ok = expect(CanAffordSanction(16, 101), "residual regen surplus must afford") && ok;

    // SanctionAffordCost dual-wire against inline Lua cost formula.
    for (const int32 opt : { 0, 16, 32, 48, 1, 64, 100, -1 })
    {
        ok = expect(SanctionAffordCost(opt) == inlineSanctionAffordCost(opt),
                    "SanctionAffordCost dual-wire == inline Lua cost") &&
             ok;
    }

    // Residual poles: known option costs vs standing short/exact/surplus;
    // invalid options still cost 100 (gate is cost-only).
    const struct
    {
        int32       option;
        int32       standing;
        bool        want;
        const char* label;
    } cases[] = {
        // Option 0 free (cost 0): short / exact / surplus around 0.
        { 0, 0, true, "option0 free standing0 exact" },
        { 0, -1, false, "option0 free standing-1 short" },
        { 0, 1, true, "option0 free standing1 surplus" },
        { 0, 99, true, "option0 free standing99" },
        { 0, 100, true, "option0 free standing100" },

        // Power1 regen (option 16): cost 100 — short / exact / surplus.
        { 16, 99, false, "regen short (99)" },
        { 16, 100, true, "regen exact (100)" },
        { 16, 101, true, "regen surplus (101)" },
        { 16, 0, false, "regen empty standing" },
        { 16, -1, false, "regen negative standing" },

        // Power2 refresh (option 32)
        { 32, 99, false, "refresh short" },
        { 32, 100, true, "refresh exact" },
        { 32, 200, true, "refresh surplus" },

        // Power3 food (option 48)
        { 48, 99, false, "food short" },
        { 48, 100, true, "food at" },
        { 48, 1000, true, "food surplus large" },

        // Named constant poles short / exact / surplus.
        { 16, SanctionStandingCost - 1, false, "const regen short" },
        { 16, SanctionStandingCost, true, "const regen exact" },
        { 16, SanctionStandingCost + 1, true, "const regen surplus" },
        { 32, SanctionStandingCost - 1, false, "const refresh short" },
        { 32, SanctionStandingCost, true, "const refresh exact" },
        { 32, SanctionStandingCost + 1, true, "const refresh surplus" },
        { 48, SanctionStandingCost - 1, false, "const food short" },
        { 48, SanctionStandingCost, true, "const food exact" },
        { 48, SanctionStandingCost + 1, true, "const food surplus" },

        // Invalid / non-sanction still cost 100 (host classifies first)
        { 1, 99, false, "invalid option1 short" },
        { 1, 100, true, "invalid option1 exact" },
        { 1, 101, true, "invalid option1 surplus" },
        { 64, 99, false, "invalid option64 short" },
        { 64, 100, true, "invalid option64 exact" },
        { 64, 101, true, "invalid option64 surplus" },
        { 100, 99, false, "invalid option100 short" },
        { 100, 100, true, "invalid option100 exact" },
        { -1, 99, false, "invalid option-1 short" },
        { -1, 100, true, "invalid option-1 exact" },
        { -1, 101, true, "invalid option-1 surplus" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordSanction(c.option, c.standing);
        const bool inlineF = inlineCanAffordSanction(c.option, c.standing);
        const bool pinGot  = pinCanAffordSanction(c.option, c.standing);
        const bool wantPin = c.standing >= SanctionAffordCost(c.option);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanAffordSanction dual-wire == inline Lua formula") && ok;
        ok = expect(got == pinGot, "CanAffordSanction dual-wire == C++ pin formula") && ok;
        ok = expect(got == wantPin, "CanAffordSanction == standing >= SanctionAffordCost") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       option;
        int32       standing;
        const char* label;
    } poles[] = {
        { 0, 0, "option0 exact" },
        { 0, -1, "option0 short" },
        { 0, 1, "option0 surplus" },
        { 16, 99, "regen short" },
        { 16, 100, "regen exact" },
        { 16, 101, "regen surplus" },
        { 32, 99, "refresh short" },
        { 32, 100, "refresh exact" },
        { 32, 101, "refresh surplus" },
        { 48, 99, "food short" },
        { 48, 100, "food exact" },
        { 48, 101, "food surplus" },
        { 1, 99, "invalid short" },
        { 1, 100, "invalid exact" },
        { 1, 101, "invalid surplus" },
    };
    for (const auto& pole : poles)
    {
        const bool got     = CanAffordSanction(pole.option, pole.standing);
        const bool inlineF = inlineCanAffordSanction(pole.option, pole.standing);
        const bool pinGot  = pinCanAffordSanction(pole.option, pole.standing);
        const bool want    = pole.standing >= SanctionAffordCost(pole.option);
        ok                 = expect(got == want, pole.label) && ok;
        ok                 = expect(got == inlineF, "pole free == inline") && ok;
        ok                 = expect(got == pinGot, "pole free == pin") && ok;
    }

    // --- Production onEventFinish path semantics (host inject compose) ---
    // Host classifies option ∈ {0,16,32,48}, then injects standing into
    // CanAffordSanction. Reject when short; proceed to delCurrency/effect.
    ok = expect(CanAffordSanction(0, 0), "finish option0 free → proceed path") && ok;
    ok = expect(!CanAffordSanction(16, 99), "finish regen short → return early") && ok;
    ok = expect(CanAffordSanction(16, 100), "finish regen exact → proceed path") && ok;
    ok = expect(CanAffordSanction(16, 101), "finish regen surplus → proceed path") && ok;
    ok = expect(CanAffordSanction(48, 100), "finish food exact → proceed path") && ok;

    // Compose: free == inline == pin for short / exact / surplus poles per option.
    const int32 sanctionOpts[] = { 0, 16, 32, 48 };
    for (const int32 opt : sanctionOpts)
    {
        const int32 cost = SanctionAffordCost(opt);
        ok               = expect(CanAffordSanction(opt, cost), "compose at cost must afford") && ok;
        if (cost > 0)
        {
            ok = expect(!CanAffordSanction(opt, cost - 1), "compose under cost must not afford") && ok;
        }
        ok = expect(CanAffordSanction(opt, cost + 1), "compose over cost must afford") && ok;

        for (const int32 delta : { -1, 0, 1 })
        {
            const int32 st       = cost + delta;
            const bool  got      = CanAffordSanction(opt, st);
            const bool  inlineF  = inlineCanAffordSanction(opt, st);
            const bool  pinGot   = pinCanAffordSanction(opt, st);
            ok                   = expect(got == inlineF, "compose free == inline") && ok;
            ok                   = expect(got == pinGot, "compose free == pin") && ok;
            ok                   = expect(got == (st >= cost), "compose free == (st >= cost)") && ok;
        }
    }

    // Dense cost dual-wire over sanction + invalid options × standing poles.
    const int32 options[]   = { 0, 16, 32, 48, 1, 64, -1 };
    const int32 standings[] = { -1, 0, 1, 99, 100, 101, 1000 };
    for (const int32 opt : options)
    {
        for (const int32 st : standings)
        {
            const bool got = CanAffordSanction(opt, st);
            ok             = expect(got == (st >= SanctionAffordCost(opt)), "dense free == pin formula") && ok;
            ok             = expect(got == inlineCanAffordSanction(opt, st), "dense free == inline") && ok;
            ok             = expect(got == pinCanAffordSanction(opt, st), "dense free == pin") && ok;
        }
    }

    return ok;
}
