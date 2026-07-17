#include "test_party_reject_pc_add_full_3200.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRejectPCAddFull 3200 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember IsFull gate for dual-wire cross-check (dedicated 3200):
//   isPCEntity && isPCParty && partyFull
auto inlineShouldRejectPCAddFull3200(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// Compact dual-wire pin matching Go pinShouldRejectPCAddFull3200 / C++ capacity
// positive form (formula unchanged from 1327 / 1350 / 2928):
//   isPCEntity && isPCParty && partyFull
auto pinShouldRejectPCAddFull3200(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRejectPCAddFull
// (TYPE_PC + PARTY_PCS + partyFull three-bool AND;
// OmegaXI internal/party; dedicated slice 3200; residual expand 2928 /
// pure 1327 / 1350).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2928 / 1327 / 1350 pins still hold
//   - dense 2^3 boolean space free == inline == pin
//   - host inject poles + ClassifyAddMember integration
//   - residual independence (2937 trusts gate)
auto runPartyRejectPCAddFull3200SelfTests() -> bool
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

    // --- Core poles: free == inline == pin positive form ---
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
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectPCAddFull(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool inlineF = inlineShouldRejectPCAddFull3200(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool pin     = pinShouldRejectPCAddFull3200(c.isPCEntity, c.isPCParty, c.partyFull);
        // Positive form pin composition (explicit three-bool AND).
        const bool wantPin = c.isPCEntity && c.isPCParty && c.partyFull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRejectPCAddFull free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectPCAddFull(true, true, true) == pinShouldRejectPCAddFull3200(true, true, true),
                "free == pin all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, true, false) == pinShouldRejectPCAddFull3200(true, true, false),
                "free == pin not full") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(false, true, true) == pinShouldRejectPCAddFull3200(false, true, true),
                "free == pin non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddFull(true, false, true) == pinShouldRejectPCAddFull3200(true, false, true),
                "free == pin non-PC party") &&
         ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyFull : { false, true })
            {
                const bool got     = ShouldRejectPCAddFull(isPCEntity, isPCParty, partyFull);
                const bool inlineF = inlineShouldRejectPCAddFull3200(isPCEntity, isPCParty, partyFull);
                const bool pin     = pinShouldRejectPCAddFull3200(isPCEntity, isPCParty, partyFull);
                const bool want    = isPCEntity && isPCParty && partyFull;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
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
    ok = expect(ShouldRejectPCAddFull(true, true, true) && pinShouldRejectPCAddFull3200(true, true, true),
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
        const bool inlineF = inlineShouldRejectPCAddFull3200(p.isPCEntity, p.isPCParty, p.partyFull);
        const bool pin     = pinShouldRejectPCAddFull3200(p.isPCEntity, p.isPCParty, p.partyFull);
        const bool want    = p.isPCEntity && p.isPCParty && p.partyFull;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;

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

    // Residual independence (1327 / 1350 / 2928 / 2937):
    // full reject is distinct from trusts reject (same triple-AND shape).
    ok = expect(ShouldRejectPCAddFull(true, true, true), "full gate must reject via free gate") && ok;
    ok = expect(ShouldRejectPCAddTrusts(true, true, true), "trusts still rejects under residual (2937)") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "not full must pass even if trusts would reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, true, false), "no trusts must not reject via trusts gate") && ok;

    return ok;
}
