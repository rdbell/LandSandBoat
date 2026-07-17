#include "test_party_reject_trusts_2937.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRejectPCAddTrusts 2937 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember HasTrusts gate for dual-wire cross-check (slice 2937):
//   isPCEntity && isPCParty && partyHasTrusts
auto inlineShouldRejectPCAddTrusts(const bool isPCEntity, const bool isPCParty, const bool partyHasTrusts) -> bool
{
    return isPCEntity && isPCParty && partyHasTrusts;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRejectPCAddTrusts
// (TYPE_PC + PARTY_PCS + partyHasTrusts; slice 2937).
auto runPartyRejectTrusts2937SelfTests() -> bool
{
    using partyhelpers::ClassifyAddMember;
    using partyhelpers::ShouldRejectPCAddTrusts;
    using partyhelpers::add_member_gate;

    bool ok = true;

    const struct
    {
        bool        isPCEntity;
        bool        isPCParty;
        bool        partyHasTrusts;
        bool        want;
        const char* label;
    } cases[] = {
        // All three true → reject
        { true, true, true, true, "PC entity PC party with trusts rejects" },

        // Any false → allow
        { true, true, false, false, "PC entity PC party no trusts allows" },
        { true, false, true, false, "PC entity mob party with trusts allows" },
        { false, true, true, false, "mob entity PC party with trusts allows" },
        { true, false, false, false, "PC entity mob party no trusts allows" },
        { false, true, false, false, "mob entity PC party no trusts allows" },
        { false, false, true, false, "mob entity mob party with trusts allows" },
        { false, false, false, false, "mob entity mob party no trusts allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectPCAddTrusts(c.isPCEntity, c.isPCParty, c.partyHasTrusts);
        const bool inlineF = inlineShouldRejectPCAddTrusts(c.isPCEntity, c.isPCParty, c.partyHasTrusts);
        const bool wantPin = c.isPCEntity && c.isPCParty && c.partyHasTrusts;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectPCAddTrusts dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectPCAddTrusts == pin formula") && ok;
    }

    // Pin composition: all three inputs required.
    ok = expect(ShouldRejectPCAddTrusts(true, true, true), "all-true must reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, true, false), "no trusts must not reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(false, true, true), "non-PC entity must not reject") && ok;
    ok = expect(!ShouldRejectPCAddTrusts(true, false, true), "non-PC party must not reject") && ok;

    // Dense compose: full 2^3 boolean space.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyHasTrusts : { false, true })
            {
                const bool got  = ShouldRejectPCAddTrusts(isPCEntity, isPCParty, partyHasTrusts);
                const bool want = isPCEntity && isPCParty && partyHasTrusts;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldRejectPCAddTrusts(isPCEntity, isPCParty, partyHasTrusts),
                           "compose free == inline") &&
                     ok;
            }
        }
    }

    // ClassifyAddMember integration: REJECT_TRUSTS only when free function rejects.
    // partyFull held false so full gate does not short-circuit.
    ok = expect(ClassifyAddMember(false, false, false, true, true, false, true) == add_member_gate::REJECT_TRUSTS,
                "ClassifyAddMember trusts reject") &&
         ok;
    ok = expect(ClassifyAddMember(false, false, false, true, true, false, false) == add_member_gate::PROCEED,
                "ClassifyAddMember no trusts proceed") &&
         ok;
    ok = expect(ClassifyAddMember(false, false, false, false, true, false, true) == add_member_gate::PROCEED,
                "ClassifyAddMember mob entity proceed") &&
         ok;
    ok = expect(ClassifyAddMember(false, false, false, true, false, false, true) == add_member_gate::PROCEED,
                "ClassifyAddMember mob party proceed") &&
         ok;

    // Explicit dual-wire: REJECT_TRUSTS iff free function rejects when prior gates pass.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyHasTrusts : { false, true })
            {
                const auto gate   = ClassifyAddMember(false, false, false, isPCEntity, isPCParty, false, partyHasTrusts);
                const bool reject = ShouldRejectPCAddTrusts(isPCEntity, isPCParty, partyHasTrusts);
                if (reject)
                {
                    ok = expect(gate == add_member_gate::REJECT_TRUSTS, "compose reject → REJECT_TRUSTS") && ok;
                }
                else
                {
                    ok = expect(gate == add_member_gate::PROCEED, "compose allow → PROCEED") && ok;
                }
            }
        }
    }

    return ok;
}
