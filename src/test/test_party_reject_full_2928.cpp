#include "test_party_reject_full_2928.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRejectPCAddFull 2928 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember IsFull gate for dual-wire cross-check (slice 2928):
//   isPCEntity && isPCParty && partyFull
auto inlineShouldRejectPCAddFull(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRejectPCAddFull
// (TYPE_PC + PARTY_PCS + partyFull; slice 2928).
auto runPartyRejectFull2928SelfTests() -> bool
{
    using partyhelpers::ClassifyAddMember;
    using partyhelpers::ShouldRejectPCAddFull;
    using partyhelpers::add_member_gate;

    bool ok = true;

    const struct
    {
        bool        isPCEntity;
        bool        isPCParty;
        bool        partyFull;
        bool        want;
        const char* label;
    } cases[] = {
        // All three true → reject
        { true, true, true, true, "PC entity PC party full rejects" },

        // Any false → allow
        { true, true, false, false, "PC entity PC party not full allows" },
        { true, false, true, false, "PC entity mob party full allows" },
        { false, true, true, false, "mob entity PC party full allows" },
        { true, false, false, false, "PC entity mob party not full allows" },
        { false, true, false, false, "mob entity PC party not full allows" },
        { false, false, true, false, "mob entity mob party full allows" },
        { false, false, false, false, "mob entity mob party not full allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectPCAddFull(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool inlineF = inlineShouldRejectPCAddFull(c.isPCEntity, c.isPCParty, c.partyFull);
        const bool wantPin = c.isPCEntity && c.isPCParty && c.partyFull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectPCAddFull dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectPCAddFull == pin formula") && ok;
    }

    // Pin composition: all three inputs required.
    ok = expect(ShouldRejectPCAddFull(true, true, true), "all-true must reject") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "not full must not reject") && ok;
    ok = expect(!ShouldRejectPCAddFull(false, true, true), "non-PC entity must not reject") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, false, true), "non-PC party must not reject") && ok;

    // Dense compose: full 2^3 boolean space.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyFull : { false, true })
            {
                const bool got  = ShouldRejectPCAddFull(isPCEntity, isPCParty, partyFull);
                const bool want = isPCEntity && isPCParty && partyFull;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldRejectPCAddFull(isPCEntity, isPCParty, partyFull),
                           "compose free == inline") &&
                     ok;
            }
        }
    }

    // ClassifyAddMember integration: REJECT_FULL only when free function rejects.
    ok = expect(ClassifyAddMember(false, false, false, true, true, true, false) == add_member_gate::REJECT_FULL,
                "ClassifyAddMember full reject") &&
         ok;
    ok = expect(ClassifyAddMember(false, false, false, true, true, false, false) == add_member_gate::PROCEED,
                "ClassifyAddMember not full proceed") &&
         ok;
    ok = expect(ClassifyAddMember(false, false, false, false, true, true, false) == add_member_gate::PROCEED,
                "ClassifyAddMember mob entity proceed") &&
         ok;
    ok = expect(ClassifyAddMember(false, false, false, true, false, true, false) == add_member_gate::PROCEED,
                "ClassifyAddMember mob party proceed") &&
         ok;

    // Explicit dual-wire: REJECT_FULL iff free function rejects when prior gates pass.
    for (const bool isPCEntity : { false, true })
    {
        for (const bool isPCParty : { false, true })
        {
            for (const bool partyFull : { false, true })
            {
                const auto gate   = ClassifyAddMember(false, false, false, isPCEntity, isPCParty, partyFull, false);
                const bool reject = ShouldRejectPCAddFull(isPCEntity, isPCParty, partyFull);
                if (reject)
                {
                    ok = expect(gate == add_member_gate::REJECT_FULL, "compose reject → REJECT_FULL") && ok;
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
