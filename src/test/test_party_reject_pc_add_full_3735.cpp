#include "test_party_reject_pc_add_full_3735.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRejectPCAddFull 3735 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember IsFull gate for dual-wire cross-check (dedicated 3690
// expand residual 2928; prior dedicated 3645 / 3600 / 3555 / 3498 / 3200):
//   isPCEntity && isPCParty && partyFull
auto inlineShouldRejectPCAddFull3735(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Compact dual-wire pin matching Go pinShouldRejectPCAddFull3735 / C++ capacity
// positive form (formula unchanged from 1327 / 1350 / 2928 / 3200 / 3498 / 3555 / 3600 / 3645 / 3690):
//   isPCEntity && isPCParty && partyFull
auto pinShouldRejectPCAddFull3735(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Prior dedicated 3200 inline/pin for cross-suite independence pins.
auto inlineShouldRejectPCAddFull3200(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

auto pinShouldRejectPCAddFull3200(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Prior dedicated 3498 inline/pin for free==prior independence.
auto inlineShouldRejectPCAddFull3498(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

auto pinShouldRejectPCAddFull3498(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Prior dedicated 3555 inline/pin for free==prior independence.
auto inlineShouldRejectPCAddFull3555(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

auto pinShouldRejectPCAddFull3555(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Prior dedicated 3600 inline/pin for free==prior independence.
auto inlineShouldRejectPCAddFull3600(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

auto pinShouldRejectPCAddFull3600(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Prior dedicated 3645 inline/pin for free==prior independence.
auto inlineShouldRejectPCAddFull3645(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

auto pinShouldRejectPCAddFull3645(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Prior dedicated 3690 inline/pin for free==inline==pin==pin3690 independence.
auto inlineShouldRejectPCAddFull3690(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

auto pinShouldRejectPCAddFull3690(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRejectPCAddFull
// (TYPE_PC + PARTY_PCS + partyFull three-bool AND;
// OmegaXI internal/party; dedicated slice 3735; residual expand 2928 /
// prior dedicated 3690 / 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2928 / 1327 / 1350 pins still hold
//   - prior 3200 / 3498 / 3555 / 3600 / 3645 / 3690 dedicated poles still hold
//   - dense 2^3 boolean space free == inline == pin
//   - host inject poles + ClassifyAddMember integration
//   - residual independence (2937 trusts gate)
//   - free == inline == pin == pin3690 independence
//   - prior 3200 / 3498 / 3555 / 3600 / 3645 / 3690 independence (free == prior inline/pin)
auto runPartyRejectPCAddFull3735SelfTests() -> bool
{
    using partyhelpers::ClassifyAddMember;
    using partyhelpers::ShouldRejectPCAddFull;
    using partyhelpers::ShouldRejectPCAddTrusts;
    using partyhelpers::add_member_gate;

    bool ok = true;

    // Residual 1327 / 1350 / 2928 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectPCAddFull(true, true, true), "residual: PC entity PC party full rejects") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "residual: PC entity PC party not full allows") && ok;
    ok = expect(!ShouldRejectPCAddFull(false, true, true), "residual: non-PC entity must not reject") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, false, true), "residual: non-PC party must not reject") && ok;
    ok = expect(!ShouldRejectPCAddFull(false, false, false), "residual: all false allows") && ok;
    // Residual 2928 combined-failure poles.
    ok = expect(!ShouldRejectPCAddFull(true, false, false), "residual 2928: PC entity mob party not full allows") && ok;
    ok = expect(!ShouldRejectPCAddFull(false, true, false), "residual 2928: mob entity PC party not full allows") && ok;
    ok = expect(!ShouldRejectPCAddFull(false, false, true), "residual 2928: mob entity mob party full allows") && ok;

    // --- Core poles: free == inline == pin positive form (full 8-row) ---
    const struct
    {
        bool        isPCEntity;
        bool        isPCParty;
        bool        partyFull;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic reject pole — all three true.
        { true, true, true, true, "PC entity PC party full rejects" },

        // Residual 1327 / 1350 single-false poles.
        { true, true, false, false, "PC entity PC party not full allows" },
        { true, false, true, false, "PC entity mob party full allows" },
        { false, true, true, false, "mob entity PC party full allows" },

        // Combined failures (residual 2928 dense space).
        { true, false, false, false, "PC entity mob party not full allows" },
        { false, true, false, false, "mob entity PC party not full allows" },
        { false, false, true, false, "mob entity mob party full allows" },
        { false, false, false, false, "mob entity mob party not full allows" },

        // Prior dedicated 3200 poles still hold.
        { true, true, true, true, "prior 3200 PC full rejects" },
        { true, true, false, false, "prior 3200 not full allows" },
        { true, false, true, false, "prior 3200 mob party full allows" },
        { false, true, true, false, "prior 3200 mob entity full allows" },
        { false, false, false, false, "prior 3200 all false allows" },

        // Prior dedicated 3498 poles still hold.
        { true, true, true, true, "prior 3498 PC full rejects" },
        { true, true, false, false, "prior 3498 not full allows" },
        { true, false, true, false, "prior 3498 mob party full allows" },
        { false, true, true, false, "prior 3498 mob entity full allows" },
        { false, false, false, false, "prior 3498 all false allows" },

        // Prior dedicated 3555 poles still hold.
        { true, true, true, true, "prior 3555 PC full rejects" },
        { true, true, false, false, "prior 3555 not full allows" },
        { true, false, true, false, "prior 3555 mob party full allows" },
        { false, true, true, false, "prior 3555 mob entity full allows" },
        { false, false, false, false, "prior 3555 all false allows" },

        // Prior dedicated 3600 poles still hold.
        { true, true, true, true, "prior 3600 PC full rejects" },
        { true, true, false, false, "prior 3600 not full allows" },
        { true, false, true, false, "prior 3600 mob party full allows" },
        { false, true, true, false, "prior 3600 mob entity full allows" },
        { false, false, false, false, "prior 3600 all false allows" },

        // Prior dedicated 3645 poles still hold.
        { true, true, true, true, "prior 3645 PC full rejects" },
        { true, true, false, false, "prior 3645 not full allows" },
        { true, false, true, false, "prior 3645 mob party full allows" },
        { false, true, true, false, "prior 3645 mob entity full allows" },
        { false, false, false, false, "prior 3645 all false allows" },

        // Prior dedicated 3690 poles still hold.
        { true, true, true, true, "prior 3690 PC full rejects" },
        { true, true, false, false, "prior 3690 not full allows" },
        { true, false, true, false, "prior 3690 mob party full allows" },
        { false, true, true, false, "prior 3690 mob entity full allows" },
        { false, false, false, false, "prior 3690 all false allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectPCAddFull(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool inlineF = inlineShouldRejectPCAddFull3735(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool pin     = pinShouldRejectPCAddFull3735(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool pin3690 = pinShouldRejectPCAddFull3690(c.isPCEntity, c.isPCParty, c.partyFull);
        // Positive form pin composition (explicit three-bool AND).
        const bool wantPin = c.isPCEntity && c.isPCParty && c.partyFull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin && got == pin3690,
                    "ShouldRejectPCAddFull free == inline == pin == pin3690 positive form") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3735(true, true, true),
                "free == pin all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3735(true, true, false),
                "free == pin not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3735(false, true, true),
                "free == pin non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3735(true, false, true),
                "free == pin non-PC party") &&
         ok;

    // free == inline == pin == pin3690 independence poles.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3735(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3735(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3690(true, true, true),
                "free == inline == pin == pin3690 all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3735(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3735(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3690(true, true, false),
                "free == inline == pin == pin3690 not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3735(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3735(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3690(false, true, true),
                "free == inline == pin == pin3690 non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3735(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3735(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3690(true, false, true),
                "free == inline == pin == pin3690 non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3735(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3735(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3690(false, false, false),
                "free == inline == pin == pin3690 all false") &&
         ok;

    // Prior dedicated 3200 independence: free still matches prior inline/pin.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3200(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3200(true, true, true),
                "prior 3200 independence all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3200(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3200(true, true, false),
                "prior 3200 independence not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3200(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3200(false, true, true),
                "prior 3200 independence non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3200(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3200(true, false, true),
                "prior 3200 independence non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3200(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3200(false, false, false),
                "prior 3200 independence all false") &&
         ok;

    // Prior dedicated 3498 independence: free still matches prior inline/pin.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3498(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3498(true, true, true),
                "prior 3498 independence all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3498(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3498(true, true, false),
                "prior 3498 independence not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3498(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3498(false, true, true),
                "prior 3498 independence non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3498(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3498(true, false, true),
                "prior 3498 independence non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3498(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3498(false, false, false),
                "prior 3498 independence all false") &&
         ok;

    // Prior dedicated 3555 independence: free still matches prior inline/pin.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3555(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3555(true, true, true),
                "prior 3555 independence all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3555(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3555(true, true, false),
                "prior 3555 independence not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3555(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3555(false, true, true),
                "prior 3555 independence non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3555(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3555(true, false, true),
                "prior 3555 independence non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3555(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3555(false, false, false),
                "prior 3555 independence all false") &&
         ok;

    // Prior dedicated 3600 independence: free still matches prior inline/pin.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3600(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3600(true, true, true),
                "prior 3600 independence all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3600(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3600(true, true, false),
                "prior 3600 independence not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3600(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3600(false, true, true),
                "prior 3600 independence non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3600(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3600(true, false, true),
                "prior 3600 independence non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3600(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3600(false, false, false),
                "prior 3600 independence all false") &&
         ok;

    // Prior dedicated 3645 independence: free still matches prior inline/pin.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3645(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3645(true, true, true),
                "prior 3645 independence all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3645(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3645(true, true, false),
                "prior 3645 independence not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3645(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3645(false, true, true),
                "prior 3645 independence non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3645(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3645(true, false, true),
                "prior 3645 independence non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3645(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3645(false, false, false),
                "prior 3645 independence all false") &&
         ok;

    // Prior dedicated 3690 independence: free still matches prior inline/pin.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == inlineShouldRejectPCAddFull3690(true, true, true) &&
                    ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3690(true, true, true),
                "prior 3690 independence all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == inlineShouldRejectPCAddFull3690(true, true, false) &&
                    ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3690(true, true, false),
                "prior 3690 independence not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == inlineShouldRejectPCAddFull3690(false, true, true) &&
                    ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3690(false, true, true),
                "prior 3690 independence non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == inlineShouldRejectPCAddFull3690(true, false, true) &&
                    ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3690(true, false, true),
                "prior 3690 independence non-PC party") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, false, false) == inlineShouldRejectPCAddFull3690(false, false, false) &&
                    ShouldRejectPCAddFull(false, false, false) == pinShouldRejectPCAddFull3690(false, false, false),
                "prior 3690 independence all false") &&
         ok;

    // Dense compose: full 2^3 boolean space free == inline == pin == pin3690.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyFull : { false, true })
            {
                const bool got     = ShouldRejectPCAddFull(isPCEntity, isPCParty, partyFull);
                const bool inlineF = inlineShouldRejectPCAddFull3735(isPCEntity, isPCParty, partyFull);
                const bool pin     = pinShouldRejectPCAddFull3735(isPCEntity, isPCParty, partyFull);
                const bool pin3690 = pinShouldRejectPCAddFull3690(isPCEntity, isPCParty, partyFull);
                const bool want    = isPCEntity && isPCParty && partyFull;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin && got == pin3690,
                                           "compose free == inline == pin == pin3690") &&
                     ok;
                ok = expect(got == inlineShouldRejectPCAddFull3200(isPCEntity, isPCParty, partyFull) &&
                                got == pinShouldRejectPCAddFull3200(isPCEntity, isPCParty, partyFull),
                            "compose free == prior 3200 inline/pin") &&
                     ok;
                ok = expect(got == inlineShouldRejectPCAddFull3498(isPCEntity, isPCParty, partyFull) &&
                                got == pinShouldRejectPCAddFull3498(isPCEntity, isPCParty, partyFull),
                            "compose free == prior 3498 inline/pin") &&
                     ok;
                ok = expect(got == inlineShouldRejectPCAddFull3555(isPCEntity, isPCParty, partyFull) &&
                                got == pinShouldRejectPCAddFull3555(isPCEntity, isPCParty, partyFull),
                            "compose free == prior 3555 inline/pin") &&
                     ok;
                ok = expect(got == inlineShouldRejectPCAddFull3600(isPCEntity, isPCParty, partyFull) &&
                                got == pinShouldRejectPCAddFull3600(isPCEntity, isPCParty, partyFull),
                            "compose free == prior 3600 inline/pin") &&
                     ok;
                ok = expect(got == inlineShouldRejectPCAddFull3645(isPCEntity, isPCParty, partyFull) &&
                                got == pinShouldRejectPCAddFull3645(isPCEntity, isPCParty, partyFull),
                            "compose free == prior 3645 inline/pin") &&
                     ok;
                ok = expect(got == inlineShouldRejectPCAddFull3690(isPCEntity, isPCParty, partyFull) &&
                                got == pinShouldRejectPCAddFull3690(isPCEntity, isPCParty, partyFull),
                            "compose free == prior 3690 inline/pin") &&
                     ok;
            }
        }
    }

    // --- Production CParty::AddMember / ClassifyAddMember path semantics ---
    // Host injects:
    //   isPCEntity = objtype == TYPE_PC
    //   isPCParty  = m_PartyType == PARTY_PCS
    //   partyFull  = IsFull() (local/remote; see IsPartyFull)
    // when true  → reject AddMember (REJECT_FULL)
    // when false → full gate passes
    ok = expect(ShouldRejectPCAddFull(true, true, true) && pinShouldRejectPCAddFull3735(true, true, true),
                "AddMember PC+PC party+full → reject path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "AddMember not full → full gate passes") && ok;
    ok = expect(!ShouldRejectPCAddFull(false, true, true), "AddMember non-PC entity → full gate passes") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, false, true), "AddMember mob party → full gate passes") && ok;

    // Host inject poles + ClassifyAddMember integration.
    const struct
    {
        bool        isPCEntity;
        bool        isPCParty;
        bool        partyFull;
        const char* label;
    } hostPoles[] = {
        { true, true, true, "PC full reject" },
        { true, true, false, "PC not full proceed" },
        { false, true, true, "mob entity full proceed" },
        { true, false, true, "mob party full proceed" },
        { false, false, false, "all false proceed" },
        // Combined pure poles:
        { true, false, false, "PC entity mob party not full" },
        { false, true, false, "mob entity PC party not full" },
        { false, false, true, "mob entity mob party full" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRejectPCAddFull(p.isPCEntity, p.isPCParty, p.partyFull);
        const bool inlineF = inlineShouldRejectPCAddFull3735(p.isPCEntity, p.isPCParty, p.partyFull);
        const bool pin     = pinShouldRejectPCAddFull3735(p.isPCEntity, p.isPCParty, p.partyFull);
        const bool pin3690 = pinShouldRejectPCAddFull3690(p.isPCEntity, p.isPCParty, p.partyFull);
        const bool want    = p.isPCEntity && p.isPCParty && p.partyFull;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3690,
                                    "host inject free == inline == pin == pin3690") &&
             ok;

        const auto gate = ClassifyAddMember(false, false, false, p.isPCEntity, p.isPCParty, p.partyFull, false);
        if (want)
        {
            ok = expect(gate == add_member_gate::REJECT_FULL, "compose reject → REJECT_FULL") && ok;
        }
        else
        {
            ok = expect(gate == add_member_gate::PROCEED, "compose allow → PROCEED") && ok;
        }
    }

    // Residual independence (1327 / 1350 / 2928 / 2937 / 3200 / 3498 / 3555 / 3600 / 3645 / 3690):
    // full reject is distinct from trusts reject (same triple-AND shape).
    ok = expect(ShouldRejectPCAddFull(true, true, true), "full gate must reject via free gate") && ok;
    ok = expect(ShouldRejectPCAddTrusts(true, true, true), "trusts still rejects under residual (2937)") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "not full must pass even if trusts would reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, true, false), "no trusts must not reject via trusts gate") && ok;

    return ok;
}
