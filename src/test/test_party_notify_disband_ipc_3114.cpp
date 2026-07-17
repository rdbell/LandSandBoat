#include "test_party_notify_disband_ipc_3114.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldNotifyPartyDisbandIPC 3114 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::DisbandParty PC-path IPC notify gate for dual-wire
// cross-check (slice 3114):
//   playerInitiated
auto inlineShouldNotifyPartyDisbandIPC(const bool playerInitiated) -> bool
{
    return playerInitiated;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldNotifyPartyDisbandIPC
// (DisbandParty PC-path IPC notify gate; slice 3114).
auto runPartyNotifyDisbandIPC3114SelfTests() -> bool
{
    using path = partyhelpers::disband_party_member_path;
    using partyhelpers::ClassifyDisbandPartyMemberPath;
    using partyhelpers::MsgLevelSyncRemoveLeftParty;
    using partyhelpers::ShouldDetachAllianceOnDisband;
    using partyhelpers::ShouldNotifyPartyDisbandIPC;
    using partyhelpers::ShouldReplaceSoloTreasurePool;

    bool ok = true;

    // Residual 1345 pins still hold under dual-wire.
    ok = expect(ShouldNotifyPartyDisbandIPC(true), "residual player-initiated notifies IPC") && ok;
    ok = expect(!ShouldNotifyPartyDisbandIPC(false), "residual non-player-initiated skips IPC") && ok;

    const struct
    {
        bool        playerInitiated;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "message-server disband skips IPC" },
        { true, true, "player-initiated sends PartyDisband IPC" },

        // Residual 1345 polarity pins.
        { false, false, "residual ShouldNotifyPartyDisbandIPC(false)" },
        { true, true, "residual ShouldNotifyPartyDisbandIPC(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldNotifyPartyDisbandIPC(c.playerInitiated);
        const bool inlineF = inlineShouldNotifyPartyDisbandIPC(c.playerInitiated);
        const bool wantPin = c.playerInitiated;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldNotifyPartyDisbandIPC dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldNotifyPartyDisbandIPC == pin formula playerInitiated") && ok;
    }

    // Pin composition: identity only.
    ok = expect(!ShouldNotifyPartyDisbandIPC(false), "false must not notify IPC") && ok;
    ok = expect(ShouldNotifyPartyDisbandIPC(true), "true must notify IPC") && ok;

    // Dense compose: full 2^1 boolean space.
    // free == inline identity == pin formula playerInitiated.
    for (const bool playerInitiated : { false, true })
    {
        const bool got  = ShouldNotifyPartyDisbandIPC(playerInitiated);
        const bool want = playerInitiated;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldNotifyPartyDisbandIPC(playerInitiated),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CParty::DisbandParty path semantics ---
    // Host injects:
    //   playerInitiated = DisbandParty argument
    // when true  → message::send(ipc::PartyDisband{ partyId = m_PartyID })
    // when false → skip IPC (disband already originated from message server)
    // Host only evaluates this gate on PC_FULL after member/DB cleanup.
    ok = expect(ShouldNotifyPartyDisbandIPC(true), "DisbandParty player-initiated → IPC path") && ok;
    ok = expect(!ShouldNotifyPartyDisbandIPC(false), "DisbandParty non-player-initiated → skip IPC") && ok;

    // Explicit dual-wire: free == identity inline == pin for host poles.
    const struct
    {
        bool        playerInitiated;
        const char* label;
    } hostPoles[] = {
        { true, "player-initiated PartyDisband IPC path" },
        { false, "message-server disband skip IPC path" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldNotifyPartyDisbandIPC(p.playerInitiated);
        const bool inlineF = inlineShouldNotifyPartyDisbandIPC(p.playerInitiated);
        const bool want    = p.playerInitiated;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Residual ClassifyDisbandPartyMemberPath coexistence (1345 suite).
    ok = expect(ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "residual Classify PC") && ok;
    ok = expect(ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "residual Classify MOB") && ok;
    ok = expect(ClassifyDisbandPartyMemberPath(false, false) == path::NONE, "residual Classify NONE") && ok;

    // Host compose: classify member path first; IPC only on PC_FULL.
    for (const bool playerInitiated : { false, true })
    {
        ok = expect(ShouldNotifyPartyDisbandIPC(playerInitiated) == playerInitiated, "IPC gate polarity") && ok;
        ok = expect(ShouldNotifyPartyDisbandIPC(playerInitiated) == inlineShouldNotifyPartyDisbandIPC(playerInitiated),
                    "IPC gate dual-wire") &&
             ok;
        ok = expect(ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "PC path before IPC inject") && ok;
        ok = expect(ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "MOB path no IPC host") && ok;
    }

    // Residual alliance detach / treasure / MsgStd still hold under dual-wire.
    ok = expect(ShouldDetachAllianceOnDisband(true), "residual alliance detach true") && ok;
    ok = expect(!ShouldDetachAllianceOnDisband(false), "residual alliance detach false") && ok;
    ok = expect(ShouldReplaceSoloTreasurePool(true, false), "residual replace solo") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(true, true), "residual keep zone pool") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(false, false), "residual no pool") && ok;
    ok = expect(MsgLevelSyncRemoveLeftParty == 553, "residual MsgStd 553") && ok;

    // Compose paths with gate polarity (mirror Go Test...Compose3114).
    const struct
    {
        bool        isPC;
        bool        isMob;
        path        want;
        const char* label;
    } paths[] = {
        { true, false, path::PC_FULL, "pc full" },
        { false, true, path::MOB_CLEAR, "mob clear" },
        { false, false, path::NONE, "none" },
        { true, true, path::PC_FULL, "pc over mob" },
    };
    for (const auto& p : paths)
    {
        for (const bool playerInitiated : { false, true })
        {
            ok = expect(ShouldNotifyPartyDisbandIPC(playerInitiated) == playerInitiated, "path IPC gate") && ok;
        }
        ok = expect(ClassifyDisbandPartyMemberPath(p.isPC, p.isMob) == p.want, p.label) && ok;
        if (p.want == path::PC_FULL)
        {
            ok = expect(ShouldNotifyPartyDisbandIPC(true), "PC_FULL player-initiated IPC") && ok;
            ok = expect(!ShouldNotifyPartyDisbandIPC(false), "PC_FULL non-player skip IPC") && ok;
        }
    }

    return ok;
}
