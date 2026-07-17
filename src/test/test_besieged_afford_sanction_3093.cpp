#include "test_besieged_afford_sanction_3093.h"

#include "map/besieged_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "besieged CanAffordSanction 3093 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinish sanctionCost for dual-wire cross-check (slice 3093):
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

// Compact dual-wire pin matching C++ capacity formula:
//   imperialStanding >= SanctionAffordCost(option)
auto pinCanAffordSanction(const int32 option, const int32 imperialStanding) -> bool
{
    return imperialStanding >= besiegedhelpers::SanctionAffordCost(option);
}

} // namespace

// Pure dual-wire expansion for besiegedhelpers::CanAffordSanction
// (Lua onEventFinish sanction imperial_standing afford gate; slice 3093).
// Standing at / under / over cost for known options; free == inline.
auto runBesiegedAffordSanction3093SelfTests() -> bool
{
    using besiegedhelpers::CanAffordSanction;
    using besiegedhelpers::SanctionAffordCost;
    using besiegedhelpers::SanctionOptionNone;
    using besiegedhelpers::SanctionStandingCost;

    bool ok = true;

    // Residual constant / 1120 / 2945 pins still hold under dual-wire.
    ok = expect(SanctionOptionNone == 0, "SanctionOptionNone pin 0") && ok;
    ok = expect(SanctionStandingCost == 100, "SanctionStandingCost pin 100") && ok;
    ok = expect(SanctionAffordCost(0) == 0, "residual option 0 afford cost free") && ok;
    ok = expect(SanctionAffordCost(16) == 100, "residual option 16 cost 100") && ok;
    ok = expect(SanctionAffordCost(32) == 100, "residual option 32 cost 100") && ok;
    ok = expect(SanctionAffordCost(48) == 100, "residual option 48 cost 100") && ok;
    ok = expect(CanAffordSanction(0, 0), "residual option0 free afford at 0") && ok;
    ok = expect(!CanAffordSanction(16, 99), "residual regen short must not afford") && ok;
    ok = expect(CanAffordSanction(16, 100), "residual regen exact must afford") && ok;

    // SanctionAffordCost dual-wire against inline Lua cost formula.
    for (const int32 opt : { 0, 16, 32, 48, 1, 64, 100, -1 })
    {
        ok = expect(SanctionAffordCost(opt) == inlineSanctionAffordCost(opt),
                    "SanctionAffordCost dual-wire == inline Lua cost") &&
             ok;
    }

    // Standing at / under / over cost for known options.
    const struct
    {
        int32       option;
        int32       standing;
        bool        want;
        const char* label;
    } cases[] = {
        // Option 0 free (cost 0): at / under / over boundary of 0.
        { 0, 0, true, "option0 free standing0 (at cost)" },
        { 0, -1, false, "option0 free standing-1 (under cost)" },
        { 0, 1, true, "option0 free standing1 (over cost)" },
        { 0, 99, true, "option0 free standing99" },
        { 0, 100, true, "option0 free standing100" },

        // Power1 regen (option 16): cost 100 — under / at / over.
        { 16, 99, false, "regen under cost (99)" },
        { 16, 100, true, "regen at cost (100)" },
        { 16, 101, true, "regen over cost (101)" },
        { 16, 0, false, "regen empty standing" },
        { 16, -1, false, "regen negative standing" },

        // Power2 refresh (option 32)
        { 32, 99, false, "refresh under" },
        { 32, 100, true, "refresh at" },
        { 32, 200, true, "refresh over" },

        // Power3 food (option 48)
        { 48, 99, false, "food under" },
        { 48, 100, true, "food at" },
        { 48, 1000, true, "food over large" },

        // Named constant poles under / at / over.
        { 16, SanctionStandingCost - 1, false, "const regen under" },
        { 16, SanctionStandingCost, true, "const regen at" },
        { 16, SanctionStandingCost + 1, true, "const regen over" },
        { 32, SanctionStandingCost - 1, false, "const refresh under" },
        { 32, SanctionStandingCost, true, "const refresh at" },
        { 48, SanctionStandingCost - 1, false, "const food under" },
        { 48, SanctionStandingCost, true, "const food at" },

        // Non-sanction still cost 100 (host classifies first)
        { 1, 99, false, "non-sanction under" },
        { 1, 100, true, "non-sanction at" },
        { 1, 101, true, "non-sanction over" },
        { 64, 100, true, "option64 at cost100" },
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

    // --- Production onEventFinish path semantics (host inject compose) ---
    // Host classifies option ∈ {0,16,32,48}, then injects standing into
    // CanAffordSanction. Reject when short; proceed to delCurrency/effect.
    ok = expect(CanAffordSanction(0, 0), "finish option0 free → proceed path") && ok;
    ok = expect(!CanAffordSanction(16, 99), "finish regen short → return early") && ok;
    ok = expect(CanAffordSanction(16, 100), "finish regen exact → proceed path") && ok;
    ok = expect(CanAffordSanction(16, 101), "finish regen over → proceed path") && ok;
    ok = expect(CanAffordSanction(48, 100), "finish food exact → proceed path") && ok;

    // Compose: free == inline == pin for at / under / over poles per option.
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

    // Dense cost dual-wire over sanction options × standing poles.
    const int32 options[]   = { 0, 16, 32, 48 };
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
