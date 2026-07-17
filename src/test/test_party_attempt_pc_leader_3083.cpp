#include "test_party_attempt_pc_leader_3083.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldAttemptPCLeaderPromote 3083 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::RemovePartyLeader PC promote gate for dual-wire
// cross-check (slice 3083):
//   !isMobParty
auto inlineShouldAttemptPCLeaderPromote(const bool isMobParty) -> bool
{
    return !isMobParty;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldAttemptPCLeaderPromote
// (RemovePartyLeader PC promote gate; slice 3083).
auto runPartyAttemptPCLeader3083SelfTests() -> bool
{
    using plan = partyhelpers::remove_party_leader_plan;
    using partyhelpers::ClassifyRemovePartyLeader;
    using partyhelpers::FormatRemovePartyLeaderEmptyWarning;
    using partyhelpers::RemovePartyLeaderReturnValue;
    using partyhelpers::ShouldAttemptPCLeaderPromote;

    bool ok = true;

    // Residual 1340 pins still hold under dual-wire.
    ok = expect(ShouldAttemptPCLeaderPromote(false), "residual PC party attempts promote") && ok;
    ok = expect(!ShouldAttemptPCLeaderPromote(true), "residual mob party skips PC promote") && ok;

    const struct
    {
        bool        isMobParty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "PC party attempts promote" },
        { true, false, "mob party skips PC promote" },

        // Residual 1340 polarity pins.
        { false, true, "residual ShouldAttemptPCLeaderPromote(false)" },
        { true, false, "residual ShouldAttemptPCLeaderPromote(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAttemptPCLeaderPromote(c.isMobParty);
        const bool inlineF = inlineShouldAttemptPCLeaderPromote(c.isMobParty);
        const bool wantPin = !c.isMobParty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAttemptPCLeaderPromote dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAttemptPCLeaderPromote == pin formula !isMobParty") && ok;
    }

    // Pin composition: negation only.
    ok = expect(ShouldAttemptPCLeaderPromote(false), "false (not mob) must attempt promote") && ok;
    ok = expect(!ShouldAttemptPCLeaderPromote(true), "true (mob) must not attempt PC promote") && ok;

    // Dense compose: full 2^1 boolean space.
    // free == inline negation == pin formula !isMobParty.
    for (const bool isMobParty : { false, true })
    {
        const bool got  = ShouldAttemptPCLeaderPromote(isMobParty);
        const bool want = !isMobParty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldAttemptPCLeaderPromote(isMobParty),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CParty::RemovePartyLeader path semantics ---
    // Host injects:
    //   isMobParty = m_PartyType == PARTY_MOBS
    // when false → SELECT oldest non-leader from accounts_sessions JOIN; SetLeader
    // when true  → skip SQL promote; later Classify may MOB_PROMOTE_AND_DEL
    // Host empty-list short-circuit sits before this gate.
    ok = expect(ShouldAttemptPCLeaderPromote(false), "RemovePartyLeader PC party → promote path") && ok;
    ok = expect(!ShouldAttemptPCLeaderPromote(true), "RemovePartyLeader mob party → skip SQL promote") && ok;

    // Explicit dual-wire: free == negation inline == pin for host poles.
    const struct
    {
        bool        isMobParty;
        const char* label;
    } hostPoles[] = {
        { false, "PC promote SQL path" },
        { true, "mob skip SQL promote path" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldAttemptPCLeaderPromote(p.isMobParty);
        const bool inlineF = inlineShouldAttemptPCLeaderPromote(p.isMobParty);
        const bool want    = !p.isMobParty;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Residual ClassifyRemovePartyLeader coexistence (1340 suite).
    ok = expect(ClassifyRemovePartyLeader(false, false, true, true) == plan::DISBAND,
                "residual Classify PC still leader") &&
         ok;
    ok = expect(ClassifyRemovePartyLeader(false, false, true, false) == plan::REMOVE_MEMBER,
                "residual Classify PC after promote") &&
         ok;
    ok = expect(ClassifyRemovePartyLeader(false, true, true, true) == plan::MOB_PROMOTE_AND_DEL,
                "residual Classify mob promote") &&
         ok;
    ok = expect(ClassifyRemovePartyLeader(true, false, false, true) == plan::EMPTY_LIST,
                "residual Classify empty") &&
         ok;

    // Host compose: empty check first, then promote gate, then classify.
    for (const bool isEmpty : { false, true })
    {
        for (const bool isMobParty : { false, true })
        {
            if (isEmpty)
            {
                ok = expect(ClassifyRemovePartyLeader(true, isMobParty, false, true) == plan::EMPTY_LIST,
                            "empty short-circuit") &&
                     ok;
                continue;
            }
            ok = expect(ShouldAttemptPCLeaderPromote(isMobParty) == !isMobParty, "promote gate polarity") && ok;
            ok = expect(ShouldAttemptPCLeaderPromote(isMobParty) == inlineShouldAttemptPCLeaderPromote(isMobParty),
                        "promote gate dual-wire") &&
                 ok;
        }
    }

    // Return-value residual polarity still holds under dual-wire.
    ok = expect(!RemovePartyLeaderReturnValue(plan::EMPTY_LIST), "residual empty return false") && ok;
    ok = expect(RemovePartyLeaderReturnValue(plan::MOB_PROMOTE_AND_DEL), "residual mob promote return true") && ok;
    ok = expect(!RemovePartyLeaderReturnValue(plan::DISBAND), "residual disband return false") && ok;
    ok = expect(RemovePartyLeaderReturnValue(plan::REMOVE_MEMBER), "residual remove return true") && ok;

    // Compose plans with gate polarity (mirror Go Test...Compose3083).
    const struct
    {
        bool        isEmpty;
        bool        isMob;
        bool        hasOther;
        bool        stillLeader;
        plan        want;
        const char* label;
    } plans[] = {
        { true, false, false, true, plan::EMPTY_LIST, "empty" },
        { false, true, true, true, plan::MOB_PROMOTE_AND_DEL, "mob other" },
        { false, true, false, true, plan::DISBAND, "mob alone" },
        { false, false, true, true, plan::DISBAND, "pc still leader" },
        { false, false, true, false, plan::REMOVE_MEMBER, "pc after promote" },
    };
    for (const auto& p : plans)
    {
        if (!p.isEmpty)
        {
            ok = expect(ShouldAttemptPCLeaderPromote(p.isMob) == !p.isMob, "plan promote gate") && ok;
        }
        ok = expect(ClassifyRemovePartyLeader(p.isEmpty, p.isMob, p.hasOther, p.stillLeader) == p.want, p.label) && ok;
    }

    ok = expect(FormatRemovePartyLeaderEmptyWarning() ==
                    "CParty::RemovePartyLeader - called when \"member\" list was empty",
                "residual empty warning") &&
         ok;

    return ok;
}
