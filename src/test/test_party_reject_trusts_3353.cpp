#include "test_party_reject_trusts_3353.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRejectPCAddTrusts 3353 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember HasTrusts gate for dual-wire cross-check (dedicated 3353):
//   isPCEntity && isPCParty && partyHasTrusts
auto inlineShouldRejectPCAddTrusts3353(const bool isPCEntity, const bool isPCParty, const bool partyHasTrusts) -> bool
{
    return isPCEntity && isPCParty && partyHasTrusts;
}

// Compact dual-wire pin matching Go pinShouldRejectPCAddTrusts3353 / C++ capacity
// positive form (formula unchanged from 1327 / 1350 / 2937):
//   isPCEntity && isPCParty && partyHasTrusts
auto pinShouldRejectPCAddTrusts3353(const bool isPCEntity, const bool isPCParty, const bool partyHasTrusts) -> bool
{
    return isPCEntity && isPCParty && partyHasTrusts;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRejectPCAddTrusts
// (TYPE_PC + PARTY_PCS + partyHasTrusts three-bool AND;
// OmegaXI internal/party; dedicated slice 3353; residual expand 2937 /
// pure 1327 / 1350).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2937 / 1327 / 1350 pins still hold
//   - dense 2^3 boolean space free == inline == pin
//   - host inject poles + ClassifyAddMember integration
//   - residual independence (2928 / 3200 full gate)
auto runPartyRejectTrusts3353SelfTests() -> bool
{
    using partyhelpers::ClassifyAddMember;
    using partyhelpers::ShouldRejectPCAddFull;
    using partyhelpers::ShouldRejectPCAddTrusts;
    using partyhelpers::add_member_gate;

    bool ok = true;

    // Residual 1327 / 1350 / 2937 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectPCAddTrusts(true, true, true), "residual: PC entity PC party with trusts rejects") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, true, false), "residual: PC entity PC party no trusts allows") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(false, true, true), "residual: non-PC entity must not reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, false, true), "residual: non-PC party must not reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(false, false, false), "residual: all false allows") && ok;
    // Residual 2937 combined-failure poles.
    ok = expect(!ShouldRejectPCAddTrusts(true, false, false), "residual 2937: PC entity mob party no trusts allows") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(false, true, false), "residual 2937: mob entity PC party no trusts allows") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(false, false, true), "residual 2937: mob entity mob party with trusts allows") && ok;

    // --- Core poles: free == inline == pin positive form ---
    const struct
    {
        bool        isPCEntity;
        bool        isPCParty;
        bool        partyHasTrusts;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic reject pole — all three true.
        { true, true, true, true, "PC entity PC party with trusts rejects" },

        // Residual 1327 / 1350 single-false poles.
        { true, true, false, false, "PC entity PC party no trusts allows" },
        { true, false, true, false, "PC entity mob party with trusts allows" },
        { false, true, true, false, "mob entity PC party with trusts allows" },

        // Combined failures (residual 2937 dense space).
        { true, false, false, false, "PC entity mob party no trusts allows" },
        { false, true, false, false, "mob entity PC party no trusts allows" },
        { false, false, true, false, "mob entity mob party with trusts allows" },
        { false, false, false, false, "mob entity mob party no trusts allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectPCAddTrusts(c.isPCEntity, c.isPCParty, c.partyHasTrusts);
        const bool inlineF = inlineShouldRejectPCAddTrusts3353(c.isPCEntity, c.isPCParty, c.partyHasTrusts);
        const bool pin     = pinShouldRejectPCAddTrusts3353(c.isPCEntity, c.isPCParty, c.partyHasTrusts);
        // Positive form pin composition (explicit three-bool AND).
        const bool wantPin = c.isPCEntity && c.isPCParty && c.partyHasTrusts;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRejectPCAddTrusts free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectPCAddTrusts(true, true, true) == pinShouldRejectPCAddTrusts3353(true, true, true),
                "free == pin all-true reject") &&
         ok;
    ok = expect(ShouldRejectPCAddTrusts(true, true, false) == pinShouldRejectPCAddTrusts3353(true, true, false),
                "free == pin no trusts") &&
         ok;
    ok = expect(ShouldRejectPCAddTrusts(false, true, true) == pinShouldRejectPCAddTrusts3353(false, true, true),
                "free == pin non-PC entity") &&
         ok;
    ok = expect(ShouldRejectPCAddTrusts(true, false, true) == pinShouldRejectPCAddTrusts3353(true, false, true),
                "free == pin non-PC party") &&
         ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyHasTrusts : { false, true })
            {
                const bool got     = ShouldRejectPCAddTrusts(isPCEntity, isPCParty, partyHasTrusts);
                const bool inlineF = inlineShouldRejectPCAddTrusts3353(isPCEntity, isPCParty, partyHasTrusts);
                const bool pin     = pinShouldRejectPCAddTrusts3353(isPCEntity, isPCParty, partyHasTrusts);
                const bool want    = isPCEntity && isPCParty && partyHasTrusts;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
            }
        }
    }

    // --- Production CParty::AddMember / ClassifyAddMember path semantics ---
    // Host injects:
    //   isPCEntity     = objtype == TYPE_PC
    //   isPCParty      = m_PartyType == PARTY_PCS
    //   partyHasTrusts = HasTrusts() (any PC member has non-empty trusts)
    // when true  → reject AddMember (REJECT_TRUSTS)
    // when false → trusts gate passes
    ok = expect(ShouldRejectPCAddTrusts(true, true, true) && pinShouldRejectPCAddTrusts3353(true, true, true),
                "AddMember PC+PC party+trusts → reject path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, true, false), "AddMember no trusts → trusts gate passes") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(false, true, true), "AddMember non-PC entity → trusts gate passes") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, false, true), "AddMember mob party → trusts gate passes") && ok;

    // Host inject poles + ClassifyAddMember integration.
    // partyFull held false so full gate does not short-circuit.
    const struct
    {
        bool        isPCEntity;
        bool        isPCParty;
        bool        partyHasTrusts;
        const char* label;
    } hostPoles[] = {
        { true, true, true, "PC trusts reject" },
        { true, true, false, "PC no trusts proceed" },
        { false, true, true, "mob entity trusts proceed" },
        { true, false, true, "mob party trusts proceed" },
        { false, false, false, "all false proceed" },
        // Combined pure poles:
        { true, false, false, "PC entity mob party no trusts" },
        { false, true, false, "mob entity PC party no trusts" },
        { false, false, true, "mob entity mob party with trusts" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRejectPCAddTrusts(p.isPCEntity, p.isPCParty, p.partyHasTrusts);
        const bool inlineF = inlineShouldRejectPCAddTrusts3353(p.isPCEntity, p.isPCParty, p.partyHasTrusts);
        const bool pin     = pinShouldRejectPCAddTrusts3353(p.isPCEntity, p.isPCParty, p.partyHasTrusts);
        const bool want    = p.isPCEntity && p.isPCParty && p.partyHasTrusts;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;

        const auto gate = ClassifyAddMember(false, false, false, p.isPCEntity, p.isPCParty, false, p.partyHasTrusts);
        if (want)
        {
            ok = expect(gate == add_member_gate::REJECT_TRUSTS, "compose reject → REJECT_TRUSTS") && ok;
        }
        else
        {
            ok = expect(gate == add_member_gate::PROCEED, "compose allow → PROCEED") && ok;
        }
    }

    // Residual independence (1327 / 1350 / 2937 / 2928 / 3200):
    // trusts reject is distinct from full reject (same triple-AND shape).
    ok = expect(ShouldRejectPCAddTrusts(true, true, true), "trusts gate must reject via free gate") && ok;
    ok = expect(ShouldRejectPCAddFull(true, true, true), "full still rejects under residual (2928 / 3200)") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, true, false), "no trusts must pass even if full would reject") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "not full must not reject via full gate") && ok;

    return ok;
}
