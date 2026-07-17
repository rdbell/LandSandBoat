#include "test_besieged_afford_sanction_2945.h"

#include "map/besieged_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "besieged CanAffordSanction 2945 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinish sanctionCost for dual-wire cross-check (slice 2945):
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

} // namespace

// Pure dual-wire expansion for besiegedhelpers::CanAffordSanction
// (Lua onEventFinish sanction imperial_standing afford gate; slice 2945).
auto runBesiegedAffordSanction2945SelfTests() -> bool
{
    using besiegedhelpers::CanAffordSanction;
    using besiegedhelpers::SanctionAffordCost;
    using besiegedhelpers::SanctionOptionNone;
    using besiegedhelpers::SanctionStandingCost;

    bool ok = true;

    // Cost pins from Go / residual 1120 surface.
    ok = expect(SanctionOptionNone == 0, "SanctionOptionNone pin 0") && ok;
    ok = expect(SanctionStandingCost == 100, "SanctionStandingCost pin 100") && ok;
    ok = expect(SanctionAffordCost(0) == 0, "option 0 afford cost free") && ok;
    ok = expect(SanctionAffordCost(16) == 100, "option 16 cost 100") && ok;
    ok = expect(SanctionAffordCost(32) == 100, "option 32 cost 100") && ok;
    ok = expect(SanctionAffordCost(48) == 100, "option 48 cost 100") && ok;

    // SanctionAffordCost dual-wire against inline Lua cost formula.
    for (const int32 opt : { 0, 16, 32, 48, 1, 64, 100, -1 })
    {
        ok = expect(SanctionAffordCost(opt) == inlineSanctionAffordCost(opt),
                    "SanctionAffordCost dual-wire == inline Lua cost") &&
             ok;
    }

    const struct
    {
        int32       option;
        int32       standing;
        bool        want;
        const char* label;
    } cases[] = {
        // Option 0 is free (cost 0) — standing >= 0 affords; negative fails.
        { 0, 0, true, "option0 free standing0" },
        { 0, -1, false, "option0 free negative standing fails >=0" },
        { 0, 1, true, "option0 free standing1" },
        { 0, 99, true, "option0 free standing99" },
        { 0, 100, true, "option0 free standing100" },

        // Power1 (regen / option 16): cost 100
        { 16, 99, false, "regen short by one" },
        { 16, 100, true, "regen exact boundary" },
        { 16, 101, true, "regen above cost" },
        { 16, 0, false, "regen empty standing" },
        { 16, -1, false, "regen negative standing" },

        // Power2 (refresh / option 32)
        { 32, 99, false, "refresh short" },
        { 32, 100, true, "refresh exact" },
        { 32, 200, true, "refresh above" },

        // Power3 (food / option 48)
        { 48, 99, false, "food short" },
        { 48, 100, true, "food exact" },
        { 48, 1000, true, "food large standing" },

        // Non-sanction option still uses cost 100 (host classifies first)
        { 1, 99, false, "non-sanction cost100 short" },
        { 1, 100, true, "non-sanction cost100 exact" },
        { 64, 100, true, "option64 cost100 exact" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordSanction(c.option, c.standing);
        const bool inlineF = inlineCanAffordSanction(c.option, c.standing);
        const bool wantPin = c.standing >= SanctionAffordCost(c.option);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanAffordSanction dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "CanAffordSanction == standing >= SanctionAffordCost") && ok;
    }

    // Pin composition: free function uses SanctionAffordCost.
    ok = expect(CanAffordSanction(0, 0), "option0 free must afford at 0") && ok;
    ok = expect(!CanAffordSanction(16, SanctionStandingCost - 1), "regen cost-1 must not afford") && ok;
    ok = expect(CanAffordSanction(16, SanctionStandingCost), "regen exact must afford") && ok;
    ok = expect(CanAffordSanction(16, SanctionStandingCost + 1), "regen cost+1 must afford") && ok;

    // --- Production onEventFinish path semantics ---
    // Host classifies option ∈ {0,16,32,48}, then injects standing into
    // CanAffordSanction. Reject when short; proceed to delCurrency/effect.
    ok = expect(CanAffordSanction(0, 0), "finish option0 free → proceed path") && ok;
    ok = expect(!CanAffordSanction(16, 99), "finish regen short → return early") && ok;
    ok = expect(CanAffordSanction(16, 100), "finish regen exact → proceed path") && ok;
    ok = expect(CanAffordSanction(48, 100), "finish food exact → proceed path") && ok;

    // Dense cost dual-wire over sanction options × standing poles.
    const int32 options[]   = { 0, 16, 32, 48 };
    const int32 standings[] = { -1, 0, 1, 99, 100, 101, 1000 };
    for (const int32 opt : options)
    {
        for (const int32 st : standings)
        {
            const bool got  = CanAffordSanction(opt, st);
            const bool want = st >= SanctionAffordCost(opt);
            ok              = expect(got == want, "dense free == pin formula") && ok;
            ok              = expect(got == inlineCanAffordSanction(opt, st), "dense free == inline") && ok;
        }
    }

    return ok;
}
