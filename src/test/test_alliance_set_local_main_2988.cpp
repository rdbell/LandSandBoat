#include "test_alliance_set_local_main_2988.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance ShouldSetLocalMainParty 2988 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline assignAllianceLeader local main-party formula for dual-wire
// cross-check (slice 2988):
//   memberFoundOnThisServer
auto inlineShouldSetLocalMainParty(const bool memberFoundOnThisServer) -> bool
{
    return memberFoundOnThisServer;
}

} // namespace

// Pure dual-wire expansion for alliancehelpers::ShouldSetLocalMainParty
// (memberFoundOnThisServer identity; slice 2988).
auto runAllianceSetLocalMain2988SelfTests() -> bool
{
    using alliancehelpers::ShouldSetLocalMainParty;

    bool ok = true;

    const struct
    {
        bool        memberFoundOnThisServer;
        bool        want;
        const char* label;
    } cases[] = {
        // Member found on this process → set local main party
        { true, true, "local member sets main party" },

        // Member not on this process → leave aLeader null for this party
        { false, false, "remote member skips local main" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSetLocalMainParty(c.memberFoundOnThisServer);
        const bool inlineF = inlineShouldSetLocalMainParty(c.memberFoundOnThisServer);
        const bool wantPin = c.memberFoundOnThisServer;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSetLocalMainParty dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSetLocalMainParty == pin formula identity") && ok;
    }

    // Pin composition: identity of memberFoundOnThisServer.
    ok = expect(ShouldSetLocalMainParty(true), "local member must set main") && ok;
    ok = expect(!ShouldSetLocalMainParty(false), "remote member must skip") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool memberFoundOnThisServer : { false, true })
    {
        const bool got  = ShouldSetLocalMainParty(memberFoundOnThisServer);
        const bool want = memberFoundOnThisServer;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSetLocalMainParty(memberFoundOnThisServer),
                    "compose free == inline") &&
             ok;
    }

    // --- Production assignAllianceLeader path semantics ---
    // Host injects:
    //   memberFoundOnThisServer = PParty->GetMemberByName(name) != nullptr
    // true  → this->aLeader = PParty; break
    // false → continue partyList scan (aLeader stays nullptr until a local hit)
    ok = expect(ShouldSetLocalMainParty(true), "assignAllianceLeader local GetMemberByName → set aLeader") && ok;
    ok = expect(!ShouldSetLocalMainParty(false), "assignAllianceLeader missing local member → skip party") && ok;

    // Residual 1346 pins still hold under dual-wire.
    ok = expect(ShouldSetLocalMainParty(true), "residual local main") && ok;
    ok = expect(!ShouldSetLocalMainParty(false), "residual remote main") && ok;

    // assignAllianceLeader pure residual stays orthogonal (local main only
    // gates aLeader assignment after a successful promote row).
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, true) ==
                    alliancehelpers::assign_alliance_leader_gate::FOUND,
                "assignAllianceLeader found residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(false, true) ==
                    alliancehelpers::assign_alliance_leader_gate::NOT_FOUND,
                "assignAllianceLeader query-fail residual") &&
         ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, false) ==
                    alliancehelpers::assign_alliance_leader_gate::NOT_FOUND,
                "assignAllianceLeader no-row residual") &&
         ok;
    ok = expect(alliancehelpers::AllianceLeaderFlag == 0x0008, "AllianceLeaderFlag residual") && ok;
    ok = expect(alliancehelpers::NewAllianceIDFromLeaderChar(42) == 42, "NewAllianceIDFromLeaderChar residual") && ok;

    return ok;
}
